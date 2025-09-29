---@diagnostic disable: need-check-nil

local DocGen = {}

local start = os.clock() * 1000

local Markdown = require("mdgen")

local docwords = {
    "@brief",
    "@param",
    "@return",
    "@note",
    "@example",
    "@since",
    "@deprecated",
    "@threadsafety",
    "@pre",
    "@post",
    "@throws",
    "@section",
    "@internal",
    "@noexcept",
    "@block",
    "@nodiscard",
    "@see",

    "enum",
    "struct"
}

local function formatDocTable(doc, signature, insideClass, className)
    local formatted = {
        brief = "",
        params = {
            type = 0
        },
        ["return"] = "",
        note = "",
        example = "",
        since = "",
        deprecated = false,
        threadsafety = "",
        pre = "",
        post = "",
        throws = "",
        section = "",
        internal = false,
        noexcept = false,
        block = false,
        nodiscard = false,
        see = {},
        signature = {
            type = "",
            full = ""
        },
        title = "",
        insideClass = insideClass or false,
        className = className or ""
    }

    local docstrings = {}
    for _, line in ipairs(doc) do
        -- Check if line starts with any doc word
        local startsWithDocword = false
        for _, docword in ipairs(docwords) do
            if line:match("^%s*" .. docword) then
                startsWithDocword = true
                break
            end
        end

        if startsWithDocword then
            table.insert(docstrings, line)
        else
            if #docstrings == 0 then
                -- No docstring yet, ignore
                goto continue
            end
            -- If line doesn't start with doc word, it's part of the previous one
            docstrings[#docstrings] = docstrings[#docstrings] .. " " .. line
        end
        ::continue::
    end

    -- Get the type by using the word right *before* the title
    local i, j = signature:find("[%w_:]+%s*[{=<(]")
    local typeWord = ""
    if not i then
        -- Must be struct or enum, check first word in "one of the lines"
        for _, line in ipairs(docstrings) do
            local firstWord = line:match("^%s*([%w_@]+)")
            if firstWord == "enum" or firstWord == "struct" then
                typeWord = firstWord:sub(1)
                break
            end
        end
    else
        typeWord = signature:match("([%w_:]+)%s*")
        if typeWord == "inline" then
            typeWord = signature:match("%g+%s*%g+%s*(%g+)")
        end
    end
    formatted.signature.type = typeWord or ""

    -- Or, for enums and structs, the type is the first word in the full signature
    if formatted.signature.type == "" or formatted.signature.type == ":" then
        formatted.signature.type = signature:match("^%s*([%w_]+)")
    end

    -- Replaced all multiple spaces with one space
    formatted.signature.full = signature:gsub("%s+", " ")

    -- Get title (word right before "{", "=", "(", or "<")
    if j then
        if signature:match("^%s*inline") then
            formatted.title = signature:match("%g+%s*%g+%s*([%w_]+)")
        else
            formatted.title = signature:match("%g+%s*([%w_]+)")
        end
        if formatted.signature.type == "static" or formatted.signature.type:match("inline") then
            formatted.signature.type = formatted.title
            formatted.title = signature:match("%g+%s*%g+%s*([%w_]+)")
            if formatted.title:match("std") or formatted.title:match("size_t") then
                formatted.title = signature:match("%g+%s*%g+%s*%g+%s*(%w+)")
            end
        end
        --("Title:" .. formatted.title, "Type:" .. formatted.signature.type)
    else
        formatted.title = "No title found"
    end
    -- For enums/structs, the title is the word right after the type
    if formatted.signature.type and (formatted.signature.type:match("enum") or formatted.signature.type:match("struct")) then
        -- find line with actual enum/struct
        local sig = ""
        for _, line in ipairs(docstrings) do
            if line:match("^(enum)") or line:match("^(struct)") then
                sig = line
                formatted.signature.full = sig:match("^(.-){") or sig
                break
            end
        end

        -- find the word immediately after enum/struct
        if formatted.signature.type:match("enum") then
            local _, _, _, typename = sig:find("^%s*(enum)%s+([%w_]+)")
            formatted.title = typename
        else
            local _, _, _, typename = sig:find("^%s*(struct)%s+([%w_]+)")
            formatted.title = typename
        end
        if formatted.title == "class" then
            -- Enum class
            local _, _, _, typename = sig:find("^%s*(enum class)%s+([%w_]+)")
            formatted.title = typename
        end
    elseif not formatted.signature.type then
        -- Discard docstring, end of struct
        if formatted.signature.full:match("};") then
            print("Discarding docstring due to end-of-struct")
        end
        formatted = nil
        return formatted
    end

    -- Some special cases
    if signature:match("delete;$") then
        --return nil
        formatted.title = signature:match("^%s*([%w_:]+)")
        print("Destructor detected: " .. formatted.title)
    end
    if signature:match("operator") then
        formatted.title = "Operator " .. signature:match("operator%s*([%g]+)[(]") .. " overload"
        formatted.signature.type = 4
    end
    if signature:match("%s*[%w_:][(]") then
        formatted.title = signature:match("%s*([%w_:]+)[(]")
    end
    if signature:match("%s*[%w_:<()>]+%s*[%w_:]+%s*=%s*%S+%s*->%s*%w+%s*{};") then
        formatted.title = signature:match("%s*[%w_:<()>]+%s*([%w_:]+)")
    end

    if formatted.signature.type == "enum" then
        formatted.params.type = 1
        print("Enum detected: " .. formatted.title)
    elseif formatted.signature.type == "struct" then
        formatted.params.type = 2
        if not formatted.title or formatted.title == "" then
            formatted.title = signature:match("^%s*struct%s*([%w_]+)")
        end
    end

    -- For every docstring in the doc, find the first symbol and add it to the formatted table
    for _, line in ipairs(docstrings) do
        -- get first word
        local firstWord = line:match("^%s*(@%w+)")
        if firstWord then
            firstWord = firstWord:sub(2) -- remove @
            -- God please I need a switch
            if firstWord == "param" then
                -- Get parameter information based on member type
                local paramName, paramDesc, paramType = "", "", ""
                if type(formatted.params.type) == "number" and formatted.params.type ~= 2 then
                    paramName, paramDesc = line:match("^%s*@param%s*(%g+)%s+(.*)")
                else
                    paramType, paramName, paramDesc = line:match("^%s*@param%s*(%g+) %s*(%g+)%s+(.*)")
                end

                if paramName and paramDesc then
                    if paramType == "unsigned" then
                        paramType = paramType .. " " .. paramName
                        paramName = paramDesc:match("^(%g+)")
                        paramDesc = paramDesc:sub(#paramName + 2)
                    end

                    local paramTable = {
                        name = paramName,
                        description = paramDesc,
                        type = paramType
                    }

                    table.insert(formatted.params, paramTable)
                end
                goto continue
            elseif firstWord == "see" then
                local seeRef = line:sub(#firstWord + 2)
                table.insert(formatted.see, seeRef)
                --print("Matched see reference: " .. seeRef)
                goto continue
            elseif firstWord == "internal" then
                formatted.internal = true
                goto continue
            elseif firstWord == "deprecated" then
                formatted.deprecated = true
                goto continue
            elseif firstWord == "noexcept" then
                formatted.noexcept = true
                goto continue
            elseif firstWord == "nodiscard" then
                formatted.nodiscard = true
                goto continue
            elseif firstWord == "block" then
                formatted.block = true
                goto continue
            end

            -- Not anything that needs special case
            formatted[firstWord] = line:sub(#firstWord + 2)
            --print("Matched word: " .. firstWord .. "| with content: " .. formatted[firstWord])
        end
        ::continue::
    end

    if formatted.title == formatted.signature.type or formatted.brief:match("Constructor") then
        formatted.title = formatted.signature.type
        formatted.signature.type = 3
        --return formatted
    end
    if not formatted.title or formatted.title == "" then
        print("No title found for signature: " .. signature)
        formatted.title = ""
    end
    if formatted.title == "No title found" then
        print("Discarding docstring due to no title")
        formatted = nil
    end

    return formatted
end

function DocGen:ReadFile(path, name)
    local f = io.open(path, "r")

    local docs = {}
    local memberVars = {}
    local currentDoc = {}
    local collecting = false
    local insideClass = false
    local className = ""
    local insideStruct = false
    local insideEnum = false

    if f then
        for line in f:lines() do
            -- Check for doc comment start
            if line:match("^%s*///") then
                collecting = true
                local sub = line:gsub("^%s*///%s*", "")
                table.insert(currentDoc, sub)
            elseif line:find("//*", 1, true) then
                -- Match member variables
                local comment = line:match("//*%s*(.*)")
                -- Try to match enum/struct member with optional assignment and trailing comma
                local type, name = "", ""
                if insideEnum then
                    name = line:match("^%s*([%w_]+)")
                    if not name then
                        -- Part of previous comment, append to last comment
                        if comment and #comment > 0 and #currentDoc > 0 then
                            currentDoc[#currentDoc] = currentDoc[#currentDoc] .. " " .. comment:sub(3)
                        end
                    end
                elseif insideStruct then
                    type, name = line:match("^%s*(.-)%s+([%w_]+)%s*=")
                    if not type then
                        type, name = line:match("^%s*(.-)%s+([%w_]+)%s*;")
                    end
                    if not type then
                        type, name = line:match("^%s*(.-)%s+([%w_]+)$")
                    end
                else
                    -- Match optional 'static' or 'const', then type and name
                    type, name = line:match("^%s*static%s+([%w_:<>%*%&]+)%s+([%w_]+)")
                    if not type then
                        type, name = line:match("^%s*([%w_:<>%*%&]+)%s+([%w_]+)")
                    end
                end
                if type and name and comment and not (insideEnum or insideStruct) then
                    table.insert(memberVars, {
                        type = type,
                        name = name,
                        description = comment:sub(3)
                    })
                end
                if insideEnum and name and comment then
                    table.insert(currentDoc, string.format("@param %s %s", name, comment:sub(3)))
                end
                if insideStruct and name and comment then
                    table.insert(currentDoc, string.format("@param %s %s %s", type, name, comment:sub(3)))
                end
            elseif (line:match("^%s*enum") or line:match("^%s*struct")) and collecting then
                if line:match("^%s*enum") then
                    insideEnum = true
                else
                    insideStruct = true
                end
                table.insert(currentDoc, line)
            elseif line:match("^%s*class%s+[%w_]+%s*{") then
                insideClass = true
                className = line:match("^%s*class%s+([%w_]+)%s*{")
            elseif collecting then
                -- End of doc block, next line is function signature
                nline = line
                if not line:find(";") then
                    -- Append following line
                    local nextLine = f:read("*l")
                    if nextLine then
                        nline = nline .. nextLine
                    end
                end

                if (insideEnum or insideStruct) and line:match("^%s*};") then
                    insideEnum = false
                    insideStruct = false
                elseif insideClass and line:match("^%s*};") then
                    insideClass = false
                    className = ""
                end

                --Utils.PrintTable(currentDoc)
                local doctable = formatDocTable(currentDoc, nline, insideClass, className)
                table.insert(docs, doctable)
                currentDoc = {}
                collecting = false
            end
        end
        f:close()
    end


    -- holy markdown batman
    local filename = path:match("([^/]+)$")
    local m = Markdown.CreateFile(name)

    if #docs == 0 then
        return
    end

    -- Search for constructor and move to the top
    for i, doc in ipairs(docs) do
        if doc.signature.type == 3 then
            table.remove(docs, i)
            table.insert(docs, 1, doc)
            break
        end
    end

    m:addHeader("Syngine API Documentation")
    m:addHeader(filename .. " header", 2)
    m:addLink("<- Back", "../index.md")
    m:addLink("See source", path)
    m:addLineBreak()

    -- Jump table
    m:addHeader("Goto: ", 2)
    m:startList()
    if #memberVars > 0 then
        m:addListItem("[Member Variables](#member-variables)")
    end
    for _, doc in ipairs(docs) do
        if doc.signature.type == "class" then
            goto continue
        end
        if doc.signature.type == 3 then
            m:addListItem(string.format("[Constructor](#class-constructor)"))
        else
            local className = "syngine"
            if doc.className and doc.className ~= "" then
                className = doc.className
            end
            m:addListItem(string.format("[%s](#%s)", doc.title, (className .. doc.title):lower():gsub("%s+", "")))
        end
        ::continue::
    end
    m:endList()
    m:addLineBreak()

    for _, doc in ipairs(docs) do
        if doc.signature.type == "class" then
            goto continue
        end

        if doc.signature.type == 3 then
            -- Constructor
            m:addHeader("Class Constructor", 2)
        end

        if doc.signature.type == 4 then
            -- Operator overload
            m:addHeader("**" .. m:inlCode(doc.title) .. "**", 4)
        else
            if doc.insideClass then
                local className = ""
                if doc.className and doc.className ~= "" then
                    className = doc.className .. "::"
                end
                m:addHeader("**" .. m:inlCode(className .. doc.title) .. "**", 4)
            else
                m:addHeader("**" .. m:inlCode("Syngine::" .. doc.title) .. "**", 4)
            end
        end

        if doc.deprecated then
            m = m + m:inlStrikethrough(doc.brief:gsub("^%s*", ""))
            m = m + m:inlBold("DEPRECATED")
        else
            m:addParagraph(doc.brief)
        end

        if doc.internal then
            m:addHeader("This function is internal use only and not intended for public use!", 4)
        end

        if doc.note and #doc.note > 0 then
            m = m + (m:inlBold("Note:") .. doc.note)
        end

        if doc.pre and #doc.pre > 0 then
            m = m + (m:inlBold("Preconditions:") .. doc.pre)
        end

        if doc.post and #doc.post > 0 then
            m = m + (m:inlBold("Postconditions:") .. doc.post)
        end

        if doc.example and #doc.example > 0 then
            m = m + (m:inlBold("Example:") .. doc.example)
        end

        --if doc.params.type == 0 then
            if doc.signature and #doc.signature.full > 0 then
                m:addParagraph("Signature:")
                local sig = doc.signature.full
                if sig:sub(-1) == "}" then
                    local i, j = sig:find("{[%g ]*}")
                    if i and j then
                        sig = sig:sub(1, i - 1) .. ";"
                    end
                end
                m:addCodeBlock(sig, "cpp")
            end
        --end

        if doc.params and #doc.params > 0 then
            -- We use md tables for enums and structs, lists for functions
            if doc.params.type == 1 or doc.params.type == 2 then
                m = m + (m:inlBold("Members:"))
                if doc.params.type == 1 then
                    -- Enum
                    m:startTable(2)
                    m:addTableRow({ "Name", "Description" }, true)
                else
                    -- Struct
                    m:startTable(3)
                    m:addTableRow({ "Type", "Name", "Description" }, true)
                end
                for _, param in ipairs(doc.params) do
                    if param.name then
                        if doc.params.type == 1 then
                            -- Enum
                            m:addTableRow({ m:inlCode(param.name), param.description })
                        else
                            -- Struct
                            m:addTableRow({ m:inlCode(param.type), m:inlCode(param.name), param.description })
                        end
                    end
                end
                m:endTable()
            else
                m = m + (m:inlBold("Parameters:"))
                m:startList()
                for _, param in ipairs(doc.params) do
                    if param.name then
                        m:addListItem(string.format("%s: %s", m:inlCode(param.name), param.description))
                    end
                end
                m:endList()
            end
        end

        if doc["return"] and #doc["return"] > 0 then
            m = m + (m:inlBold("Returns:") .. doc["return"])
        end

        if doc.threadsafety and #doc.threadsafety > 0 then
            m = m + (m:inlBold("Thread Safety:") .. doc.threadsafety)
        end

        if doc.since and #doc.since > 0 then
            m = m + (m:inlBold("This function has been available since:") .. doc.since)
        end

        if doc.throws and #doc.throws > 0 then
            m = m + (m:inlBold("Throws:") .. doc.throws)
        end

        if doc.block and #doc.block > 0 then
            m = m + m:inlBold("This is a blocking function.")
        end

        if doc.noexcept then
            m = m + m:inlBold("This function will not throw exceptions.")
        end

        if doc.nodiscard then
            m = m + m:inlBold("The return value of this function must not be discarded.")
        end

        if doc.see and next(doc.see) then
            m = m + m:inlBold("See also:")
            for _, see in ipairs(doc.see) do
                m = m + ("- " .. see)
            end
        end

        m:addLineBreak()
        if doc.signature.type == 3 then
            m:addHeader("Class & Related Members", 2)
        end

        ::continue::
    end

    if #memberVars > 0 then
        m:addHeader("Member Variables", 2)
        m:startTable(3)
        m:addTableRow({ "Type", "Name", "Description" }, true)
        for _, var in ipairs(memberVars) do
            m:addTableRow({ m:inlCode(var.type), m:inlCode(var.name), var.description })
        end
        m:endTable()
        m:addLineBreak()
    end

    Markdown.Export(m)

    local finish = os.clock() * 1000
    print("Time taken: " .. (finish - start) .. " ms")
end

return DocGen

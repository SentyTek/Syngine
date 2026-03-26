---@diagnostic disable: need-check-nil

local DocGen = {}

local start = os.clock() * 1000

local Markdown = require("mdgen")

local docwords = {
    "@brief",
    "@param",
    "@return",
    "@returns",
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
    "@tparam",

    "enum",
    "struct"
}

local function stripLineComment(line)
    return line:gsub("//.*", "")
end

local function parseStructMember(line)
    local decl = line:gsub("//%*.*$", ""):gsub("%s+$", "")
    if decl == "" then
        return nil, nil
    end

    local name, arrayPart = decl:match("([%w_]+)%s*(%b[])%s*[=;]")
    if not name then
        name = decl:match("([%w_]+)%s*[=;]")
    end
    if not name then
        return nil, nil
    end

    local typeName = decl:match("^%s*(.-)%s+" .. name)
    if not typeName then
        return nil, nil
    end

    if arrayPart and #arrayPart > 0 then
        typeName = typeName .. arrayPart
    end

    return typeName, name
end

local function stripTemplatePrefix(signature)
    local s = signature:gsub("%s+", " ")

    while s:match("^%s*template%s*<") do
        local i = s:find("<")
        local depth = 0
        local j = i

        while j <= #s do
            local ch = s:sub(j, j)
            if ch == "<" then
                depth = depth + 1
            elseif ch == ">" then
                depth = depth - 1
                if depth == 0 then
                    s = s:sub(j + 1):gsub("^%s*", "")
                    break
                end
            end
            j = j + 1
        end

        if depth ~= 0 then
            break
        end
    end

    return s
end

local function collectSignature(f, firstLine)
    local parts = { firstLine }

    local parenDepth = 0
    local angleDepth = 0
    local initBraceDepth = 0
    local bodyBraceDepth = 0
    local sawParams = false
    local bodyStarted = false

    local function scan(line)
        local s = stripLineComment(line)
        local i = 1

        while i <= #s do
            local ch = s:sub(i, i)

            if bodyStarted then
                if ch == "{" then
                    bodyBraceDepth = bodyBraceDepth + 1
                elseif ch == "}" then
                    bodyBraceDepth = bodyBraceDepth - 1
                end
            else
                if ch == "(" then
                    parenDepth = parenDepth + 1
                    sawParams = true
                elseif ch == ")" then
                    if parenDepth > 0 then
                        parenDepth = parenDepth - 1
                    end
                elseif ch == "<" then
                    angleDepth = angleDepth + 1
                elseif ch == ">" then
                    if angleDepth > 0 then
                        angleDepth = angleDepth - 1
                    end
                elseif ch == "{" then
                    if parenDepth > 0 then
                        initBraceDepth = initBraceDepth + 1
                    elseif sawParams and parenDepth == 0 and initBraceDepth == 0 then
                        bodyStarted = true
                        bodyBraceDepth = 1
                    end
                elseif ch == "}" then
                    if initBraceDepth > 0 then
                        initBraceDepth = initBraceDepth - 1
                    end
                elseif ch == ";" then
                    if parenDepth == 0 and angleDepth == 0 and initBraceDepth == 0 and not bodyStarted then
                        return true
                    end
                end
            end

            i = i + 1
        end

        if bodyStarted and bodyBraceDepth == 0 then
            return true
        end

        return false
    end

    if scan(firstLine) then
        return table.concat(parts, "\n")
    end

    while true do
        local nextLine = f:read("*l")
        if not nextLine then
            break
        end

        table.insert(parts, nextLine)

        if scan(nextLine) then
            break
        end
    end

    return table.concat(parts, "\n")
end

local function stripFunctionBody(signature)
    local out = {}

    local parenDepth = 0
    local angleDepth = 0
    local initBraceDepth = 0
    local sawParams = false

    local s = signature:gsub("%s+", " ")

    for i = 1, #s do
        local ch = s:sub(i, i)

        if ch == "(" then
            parenDepth = parenDepth + 1
            sawParams = true
        elseif ch == ")" then
            if parenDepth > 0 then
                parenDepth = parenDepth - 1
            end
        elseif ch == "<" then
            angleDepth = angleDepth + 1
        elseif ch == ">" then
            if angleDepth > 0 then
                angleDepth = angleDepth - 1
            end
        elseif ch == "{" then
            if parenDepth > 0 then
                initBraceDepth = initBraceDepth + 1
            elseif sawParams and parenDepth == 0 and initBraceDepth == 0 then
                return table.concat(out):gsub("%s+$", "") .. ";"
            end
        elseif ch == "}" then
            if initBraceDepth > 0 then
                initBraceDepth = initBraceDepth - 1
            end
        end

        table.insert(out, ch)
    end

    return table.concat(out):gsub("%s+$", "")
end

local function formatDocTable(doc, signature, insideClass, className)
    local formatted = {
        brief = "",
        params = {
            type = 0
        },
        tparams = {

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

     -- Replaced all multiple spaces with one space
    formatted.signature.full = signature:gsub("%s+", " ")
    local parseSignature = stripTemplatePrefix(formatted.signature.full)

    -- Skip aliases and forward declarations in generated docs
    if parseSignature:match("^%s*using%s+") or parseSignature:match("^%s*using%s+namespace%s+") then
        return nil
    end
    if parseSignature:match("^%s*class%s+[%w_]+%s*;%s*$") or parseSignature:match("^%s*struct%s+[%w_]+%s*;%s*$") then
        return nil
    end

    -- Get the type by using the word right *before* the title
    local i, j = parseSignature:find("[%w_:]+%s*[{=<(]")
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
        typeWord = parseSignature:match("([%w_:]+)%s*")
        if typeWord == "inline" then
            typeWord = parseSignature:match("%g+%s*%g+%s*(%g+)")
        end
    end
    formatted.signature.type = typeWord or ""

    -- For enums and structs, fall back to first word in full signature
    if formatted.signature.type == "" or formatted.signature.type == ":" then
        formatted.signature.type = parseSignature:match("^%s*([%w_]+)")
    end

    -- Get title (word right before "{", "=", "(", or "<")
    if j then
        if parseSignature:match("^%s*inline") then
            formatted.title = parseSignature:match("%g+%s*%g+%s*([%w_]+)")
        else
            formatted.title = parseSignature:match("%g+%s*([%w_]+)")
        end
        if formatted.signature.type == "static" or formatted.signature.type:match("inline") then
            formatted.signature.type = formatted.title
            formatted.title = parseSignature:match("%g+%s*%g+%s*([%w_]+)")
            if formatted.title:match("std") or formatted.title:match("size_t") then
                formatted.title = parseSignature:match("%g+%s*%g+%s*%g+%s*(%w+)")
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
            if line:match("^%s*enum") or line:match("^%s*struct") then
                sig = line
                formatted.signature.full = sig:match("^(.-){") or sig
                break
            end
        end

        -- find the declared type name
        if formatted.signature.type:match("enum") then
            formatted.title = sig:match("^%s*enum%s+class%s+([%w_]+)")
                or sig:match("^%s*enum%s+([%w_]+)")
        else
            formatted.title = sig:match("^%s*struct%s+([%w_]+)")
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
    if parseSignature:match("delete;$") then
        --return nil
        formatted.title = parseSignature:match("^%s*([%w_:]+)")
        print("Destructor detected: " .. formatted.title)
    end
    if parseSignature:match("operator") then
        formatted.title = "Operator " .. parseSignature:match("operator%s*([%g]+)[(]") .. " overload"
        formatted.signature.type = 4
    end
    if parseSignature:match("%s*[%w_:][(]") then -- Free function or constructor
        formatted.title = parseSignature:match("%s*([%w_:]+)[(]")
    end
     if parseSignature:match("%s*[%w_:<()>]+%s*[%w_:]+%s*=%s*%S+%s*->%s*%w+%s*{};") then -- Lambda assigned to variables
        formatted.title = parseSignature:match("%s*[%w_:<()>]+%s*([%w_:]+)")
    end

    if formatted.signature.type == "enum" then
        formatted.params.type = 1
        print("Enum detected: " .. (formatted.title or "<unknown>"))
    elseif formatted.signature.type == "struct" then
        formatted.params.type = 2
        if not formatted.title or formatted.title == "" then
            formatted.title = parseSignature:match("^%s*struct%s*([%w_]+)")
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
            elseif firstWord == "tparam" then
                local tparamName, tparamDesc = line:match("^%s*@tparam%s*(%g+)%s+(.*)")
                if tparamName and tparamDesc then
                    local tparamTable = {
                        name = tparamName,
                        description = tparamDesc
                    }
                    table.insert(formatted.tparams, tparamTable)
                end
                goto continue
            elseif firstWord == "returns" then
                formatted["return"] = line:sub(#firstWord + 2)
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
                local type, name
                if insideEnum then
                    name = line:match("^%s*([%w_]+)")
                    if not name then
                        -- Part of previous comment, append to last comment
                        if comment and #comment > 0 and #currentDoc > 0 then
                            currentDoc[#currentDoc] = currentDoc[#currentDoc] .. " " .. comment:sub(3)
                        end
                    end
                elseif insideStruct then
                    type, name = parseStructMember(line)
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
            elseif line:match("^%s*class%s+[%w_]+[^{]*{") then
                insideClass = true
                className = line:match("^%s*class%s+([%w_]+)[^{]*{")
            elseif collecting then
                if insideEnum or insideStruct then
                    -- Keep consuming documented enum/struct body until it closes.
                    if line:match("^%s*};") then
                        local doctable = formatDocTable(currentDoc, line, insideClass, className)
                        if doctable then
                            table.insert(docs, doctable)
                        end
                        currentDoc = {}
                        collecting = false
                        insideEnum = false
                        insideStruct = false
                    end
                    goto continue
                end

                -- End of doc block, next line is function signature
                local nline = collectSignature(f, line)

                if (insideEnum or insideStruct) and line:match("^%s*};") then
                    insideEnum = false
                    insideStruct = false
                elseif insideClass and line:match("^%s*};") then
                    insideClass = false
                    className = ""
                end

                --Utils.PrintTable(currentDoc)
                local doctable = formatDocTable(currentDoc, nline, insideClass, className)
                if doctable then
                    table.insert(docs, doctable)
                end
                currentDoc = {}
                collecting = false
            end
            ::continue::
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
        if doc and doc.signature and doc.signature.type == 3 then
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
        if not doc or not doc.signature then
            goto continue
        end
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
        if not doc or not doc.signature then
            goto continue
        end
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
                local sig = stripFunctionBody(doc.signature.full)
                m:addCodeBlock(sig, "cpp")
            end
        --end

        if doc.tparams and #doc.tparams > 0 then
            m = m + (m:inlBold("Template Parameters:"))
            m:startList()
            for _, tparam in ipairs(doc.tparams) do
                m:addListItem(string.format("%s: %s", m:inlCode(tparam.name), tparam.description))
            end
            m:endList()
        end

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

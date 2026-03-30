local Markdown = {}

local MarkdownFile = {}
MarkdownFile.__index = MarkdownFile

function MarkdownFile:new(name)
    local obj = {
        name = name,
        content = {}
    }
    setmetatable(obj, self)
    return obj
end

function MarkdownFile:addHeader(text, level)
    level = level or 1
    local headerPrefix = string.rep("#", level)
    table.insert(self.content, headerPrefix .. " " .. text .. "\n")
end

function MarkdownFile:addParagraph(text)
    table.insert(self.content, text)
end

function MarkdownFile:addLineBreak()
    table.insert(self.content, "---")
end

function MarkdownFile:addLink(text, url)
    table.insert(self.content, "[" .. text .. "](" .. url .. ")")
end

function MarkdownFile:addImage(altText, url)
    table.insert(self.content, "![" .. altText .. "](" .. url .. ")")
end

function MarkdownFile:startList(ordered)
    self.listStack = self.listStack or {}
    table.insert(self.listStack, {ordered = ordered or false, items = {}})
end

function MarkdownFile:addListItem(text)
    if not self.listStack or #self.listStack == 0 then
        error("List not started. Call startList() first.")
    end
    local currentList = self.listStack[#self.listStack]
    table.insert(currentList.items, text)
end

function MarkdownFile:endList()
    if not self.listStack or #self.listStack == 0 then
        error("List not started. Call startList() first.")
    end
    local finishedList = table.remove(self.listStack)
    local function renderList(list, indent)
        local lines = {}
        for i, item in ipairs(list.items) do
            local prefix = list.ordered and (i .. ". ") or "- "
            if type(item) == "table" and item.items then
                -- Nested list
                local sublist = renderList(item, indent .. "  ")
                table.insert(lines, indent .. sublist)
            else
                table.insert(lines, indent .. prefix .. tostring(item))
            end
        end
        return table.concat(lines, "\n")
    end
    local rendered = renderList(finishedList, "")
    if #self.listStack > 0 then
        -- Add as sublist to parent
        table.insert(self.listStack[#self.listStack].items, finishedList)
    else
        table.insert(self.content, rendered)
    end
end

function MarkdownFile:startTable(numColumns)
    self.mdTable = { numColumns = numColumns, rows = {} }
end
function MarkdownFile:addTableRow(cells)
    if not self.mdTable then
        error("Table not started. Call startTable() first.")
    end
    if #cells ~= self.mdTable.numColumns then
        error("Number of cells does not match number of columns.")
    end
    table.insert(self.mdTable.rows, cells)
end
function MarkdownFile:endTable()
    if not self.mdTable then
        error("Table not started. Call startTable() first.")
    end
    local header = "| " .. table.concat(self.mdTable.rows[1], " | ") .. " |"
    local separator = "| " .. string.rep("--- | ", self.mdTable.numColumns)
    local rows = {header, separator}
    for i = 2, #self.mdTable.rows do
        table.insert(rows, "| " .. table.concat(self.mdTable.rows[i], " | ") .. " |")
    end
    table.insert(self.content, table.concat(rows, "\n"))
    self.mdTable = nil
end

function MarkdownFile:inlCode(code)
    return "`" .. code .. "`"
end

function MarkdownFile:inlItalics(text)
    return "*" .. text .. "*"
end

function MarkdownFile:inlBold(text)
    return "**" .. text .. "**"
end

function MarkdownFile:inlStrikethrough(text)
    return "~~" .. text .. "~~"
end

function MarkdownFile:endl()
    return "\n\n"
end

function MarkdownFile:addCodeBlock(code, language)
    language = language or ""
    table.insert(self.content, "```" .. language .. "\n" .. code .. "\n```")
end

function MarkdownFile:__add(other)
    table.insert(self.content, tostring(other))
    return self
end

function MarkdownFile:__tostring()
    local result = ""

    for i, line in ipairs(self.content) do
        local nextLine = self.content[i + 1]
        local isListItem = line:sub(1, 2) == "- " or line:find("^%d+%. ")

        if isListItem then
            local nextIsListItem = nextLine and (nextLine:sub(1, 2) == "- " or nextLine:find("^%d+%. "))
            result = result .. line .. (nextIsListItem and "\n" or "\n\n")
        else
            result = result .. line .. "\n\n"
        end
    end
    return result
end

Markdown.CreateFile = function(name)
    return MarkdownFile:new(name)
end

Markdown.Export = function(md, filename)
    filename = filename or (md.name .. ".md")
    local content = tostring(md)
    local file = io.open(filename, "w")
    if file then
        file:write(content)
        file:close()
    else
        error("Could not open file for writing: " .. filename)
    end
end

return Markdown

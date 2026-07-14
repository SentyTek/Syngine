local start = os.clock() * 1000

-- Add script's directory to package path to find modules
local scriptPath = debug.getinfo(1, "S").source:sub(2)
local scriptDir = scriptPath:match("(.*[/\\])")
if scriptDir then
    package.path = package.path .. ";" .. scriptDir .. "?.lua"
else
    -- If no path separator is found, the script is in the current directory.
    package.path = package.path .. ";./?.lua"
end

local scriptBaseDir = scriptDir or "./"

local DocGen = require("docgen")
local MdGen = require("mdgen")

local incPath = scriptBaseDir .. "../../include/Syngine"
local srcPath = scriptBaseDir .. "../../src/Syngine"
local names = {}
local files = {}

-- Load blacklist
local blacklist = {}
do
    local f = io.open("blacklist.txt", "r")
    if f then
        for line in f:lines() do
            blacklist[line] = true
        end
        f:close()
    end
end

local function loadFilesFromPath(path, isPrivate)
    for line in io.popen('ls "' .. path .. '"'):lines() do
        if line:sub(-2) == ".h" or line:sub(-4) == ".hpp" then
            goto continue
        else
            -- must be folder, keep going
            local subpath = path .. "/" .. line
            for subline in io.popen('ls "' .. subpath .. '"'):lines() do
                --print(subline)
                if subline:sub(-2) == ".h" or subline:sub(-4) == ".hpp" then
                    if blacklist[subline] then
                        print("Skipping blacklisted file: " .. subline)
                    else
                        table.insert(files, {dir = line, file = subline, pre = path, private = isPrivate})
                    end
                else
                    -- probably a folder, keep going
                    local subsubpath = subpath .. "/" .. subline
                    for subsubline in io.popen('ls "' .. subsubpath .. '"'):lines() do
                        if subsubline:sub(-2) == ".h" or subsubline:sub(-4) == ".hpp" then
                            if blacklist[subsubline] then
                                print("Skipping blacklisted file: " .. subsubline)
                            else
                                table.insert(files, {dir = line .. "/" .. subline, file = subsubline, pre = path, private = isPrivate})
                            end
                        end
                    end
                end
            end
        end
        ::continue::
    end
end

local systemOS = package.config:sub(1, 1) == "\\" and "win" or "unix"

if systemOS == "win" then
    error("Windows not supported")
else
    loadFilesFromPath(incPath, false)
    loadFilesFromPath(srcPath, true)
end

for _, filename in ipairs(files) do
    print("Found file: " .. filename.dir .. "/" .. filename.file)
end

-- Make ./api folder if it doesn't exist
os.execute("mkdir -p ../api")

-- Process files
for _, filename in ipairs(files) do
    local fullpath = filename.pre .. "/" .. filename.dir .. "/" .. filename.file
    print("\tProcessing " .. fullpath)
    local fileBase = filename.file:match("(.+)%.[^.]+$") or filename.file
    --local classmeta = DocGen.ReadFile(fullpath, "../api/" .. fileBase:lower() .. "_doc")
    --table.insert(names, { name= "../api/" .. fileBase:lower() .. "_doc.md", dir = filename.dir, private = filename.private, classMeta = classmeta })

    -- do not ask me why nil is supplied as first argument, it just works. yes, the function only has 2 args. dont question it
    local suc, res, classmeta = pcall(DocGen.ReadFile, fullpath, "../api/" .. fileBase:lower() .. "_doc")

    if suc then
        print("\tSuccessfully processed " .. fullpath)
        table.insert(names, { name= "../api/" .. fileBase:lower() .. "_doc.md", dir = filename.dir, private = filename.private, classMeta = classmeta })
    else
        print("\tFAILED to process: " .. res)
        -- record to file
        local f = io.open("failed.txt", "a")
        if f then
            local str = os.date("%Y-%m-%d %H:%M:%S") .. " | " .. fullpath .. " | " .. res
            f:write(str .. "\n")
            f:close()
        end
    end
end

-- Add index file
local m = MdGen.CreateFile("index")
m:addHeader("Syngine Documentation")

local f = io.open("start.txt", "r")
if f then
    for line in f:lines() do
        if line:sub(1, 2) ~= "--" then
            m = m + (line .. "\n")
        end
    end
    f:close()
end

-- Organize names by dir, and sort names alphabetically within each dir
local dir_map = {}
for _, entry in ipairs(names) do
    if not dir_map[entry.dir] then
        dir_map[entry.dir] = {}
    end
    table.insert(dir_map[entry.dir], {name = entry.name, private = entry.private, classMeta = entry.classMeta})
end

-- Sort dirs alphabetically
local sorted_dirs = {}
for dir in pairs(dir_map) do
    table.insert(sorted_dirs, dir)
end
table.sort(sorted_dirs)

m:addHeader("Table of Contents", 2)
m = m + "- [Code Format Policy](policy.md)"

-- Inject from toc.txt
local tocFile = io.open("toc.txt", "r")
if tocFile then
    for line in tocFile:lines() do
        if line:sub(1, 2) ~= "--" then
            m = m + (line)
        end
    end
    tocFile:close()
end

m:addHeader("API Reference", 3)
for _, dir in ipairs(sorted_dirs) do
    m:addHeader(dir, 4)

    -- Custom sort: Public first, then Private. Within those groups, sort alphabetically.
    table.sort(dir_map[dir], function(a, b)
        if a.private ~= b.private then
            return not a.private -- false (public) comes before true (private)
        else
            return a.name < b.name -- secondary sort alphabetically
        end
    end)

    m:startList()
    for _, entry in ipairs(dir_map[dir]) do
        local display_name = entry.name:sub(8, -8):gsub("^%l", string.upper)
        if entry.classMeta and entry.classMeta.nameoverride then
            display_name = entry.classMeta.nameoverride
        end
        m:addListItem(string.format("%s%s[%s](%s)",
            entry.private and "[Internal] " or "",
            entry.classMeta and entry.classMeta.deprecated and "**[DEPRECATED]** " or "",
            display_name,
            entry.name:sub(4)
        ))
    end
    m:endList()
end
MdGen.Export(m, "../index.md")

local finish = os.clock() * 1000
print("Total time taken: " .. (finish - start) .. " ms")

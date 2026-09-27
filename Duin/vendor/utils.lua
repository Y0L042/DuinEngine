local utils = {}

utils.prevDir = ""

utils.colors = {
  reset = "\27[0m",    -- reset to default
  red   = "\27[31m",   -- red text
  green = "\27[32m",   -- green text
  yellow= "\27[33m",   -- yellow text
  blue  = "\27[34m",   -- blue text
}

function utils.runCommand(command)
    print(utils.colors.yellow .. "Running: \n" .. utils.colors.green .. command .. utils.colors.reset)
    local result = os.execute(command)
    return result
end

function utils.printCurrentDir()
    local handle = io.popen("pwd")          
    local current_dir = handle:read("*a")   
    handle:close()                          
    print(utils.colors.yellow .. "Current dir: " ..  utils.colors.green .. current_dir .. utils.colors.reset)                      
end

function utils.print(message)
    print(message)
end

-- Depreciate, use pushDir for concistency
function utils.changeDir(newDir)
    utils.prevDir = os.getcwd()
    os.chdir(newDir)
    print(utils.colors.yellow .. "Changed dir to " ..  utils.colors.green .. newDir .. utils.colors.reset)
end

function utils.pushDir(newDir)
    utils.prevDir = os.getcwd()
    os.chdir(newDir)
    print(utils.colors.yellow .. "Pushed dir to " ..  utils.colors.green .. newDir .. utils.colors.reset)
end

function utils.popDir()
    if utils.prevDir then
        os.chdir(utils.prevDir)
        print(utils.colors.yellow .. "Popped dir to " ..  utils.colors.green .. utils.prevDir .. utils.colors.reset)
    end
end

function utils.isDir(folder)
    return os.isdir(folder)
end

function utils.fileExists(path)
    local file = io.open(path, "r")
    if file then file:close() end
    return file ~= nil
end

function utils.deleteFolder(path)
    if os.isdir(path) then  -- Changed: Check if the directory exists
        if os.target() == "windows" then  -- Changed: Use Windows-specific command
            os.execute('rd /s /q "' .. path .. '"')  -- Changed: Remove directory recursively on Windows
        else
            os.execute('rm -rf "' .. path .. '"')  -- Changed: Remove directory recursively on Unix-like systems
        end
        print("Deleted folder: " .. path)
    else
        print("Folder does not exist: " .. path)
    end
end

function utils.fixVsWherePath(batchFilePath)
    if not utils.fileExists(batchFilePath) then
        error("Batch file not found: " .. batchFilePath)
    end

    local inFile = io.open(batchFilePath, "r")
    local content = inFile:read("*all")
    inFile:close()

    -- Replace "\VsWhere.exe" with "/VsWhere.exe"
    local updatedContent = content:gsub("\\VsWhere%.exe", "/VsWhere.exe")

    local outFile = io.open(batchFilePath, "w")
    outFile:write(updatedContent)
    outFile:close()
    print("Updated batch file: Removed '\\' before 'VsWhere.exe'.")
end

function utils.runBatchScript(scriptPath, args)
    local winPath = scriptPath:gsub("/", "\\")
    local command = 'cmd /c "' .. winPath .. '"'
    if args then
        command = command .. " " .. args
    end
    utils.runCommand(command)
end

function utils.patchFile(filePath, pattern, replacement)
    local f = io.open(filePath, "r")
    if not f then return end
    local content = f:read("*all")
    f:close()
    local patched, count = content:gsub(pattern, replacement)
    if count > 0 then
        local out = io.open(filePath, "w")
        out:write(patched)
        out:close()
        print(utils.colors.yellow .. "Patched " .. count .. " occurrence(s) in " .. filePath .. utils.colors.reset)
    end
end

function utils.copyFiles(sourceDir, targetDir, patterns)
    if not os.isdir(targetDir) then
        os.mkdir(targetDir)
    end

    if os.target() == "windows" then
        for _, pattern in ipairs(patterns) do
            local winSource = sourceDir:gsub("/", "\\")
            local winTarget = targetDir:gsub("/", "\\")
            utils.runCommand("xcopy /Y /I \"" .. winSource .. "\\" .. pattern .. "\" \"" .. winTarget .. "\\\" >nul 2>&1")
        end
    else
        local findPatterns = ""
        for i, pattern in ipairs(patterns) do
            if i > 1 then
                findPatterns = findPatterns .. " -o "
            end
            findPatterns = findPatterns .. "-name '" .. pattern .. "'"
        end
        utils.runCommand("find \"" .. sourceDir .. "\" -maxdepth 1 \\( " .. findPatterns .. " \\) -exec cp -f {} \"" .. targetDir .. "/\" \\;")
    end
end

-- Build status log (TOML, one inline table per dependency). Absolute path, set by
-- dependencies.lua, since the dep scripts change directory while building.
utils.buildStatusFile = "build_status.log"

function utils.readBuildStatus()
    local entries = {}
    local f = io.open(utils.buildStatusFile, "r")
    if not f then return entries end
    for line in f:lines() do
        local name, status, date = line:match('^(%S+)%s*=%s*{%s*status%s*=%s*"(%u+)"%s*,%s*date%s*=%s*([%dT:%-]+)%s*}')
        if name then
            entries[name] = { status = status, date = date }
        end
    end
    f:close()
    return entries
end

function utils.reportBuildStatus(name, success)
    local entries = utils.readBuildStatus()
    local status = success and "BUILT" or "FAILED"
    entries[name] = { status = status, date = os.date("%Y-%m-%dT%H:%M:%S") }

    local names = {}
    for n in pairs(entries) do table.insert(names, n) end
    table.sort(names)

    local f = io.open(utils.buildStatusFile, "w")
    if not f then
        print(utils.colors.red .. "Could not write " .. utils.buildStatusFile .. utils.colors.reset)
        return
    end
    f:write("# Written by premake5 --deps. View with: premake5 --deps STATUS\n")
    for _, n in ipairs(names) do
        local e = entries[n]
        f:write(string.format('%s = { status = "%s", date = %s }\n', n, e.status, e.date))
    end
    f:close()

    local color = success and utils.colors.green or utils.colors.red
    print(color .. name .. " -> " .. status .. utils.colors.reset)
end

return utils

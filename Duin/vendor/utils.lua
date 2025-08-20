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

function utils.changeDir(newDir)
    utils.prevDir = os.getcwd()
    os.chdir(newDir)
    print(utils.colors.yellow .. "Changed dir to " ..  utils.colors.green .. newDir .. utils.colors.reset)
end

function utils.popDir()
    if utils.prevDir then
        os.chdir(utils.prevDir)
        print(utils.colors.yellow .. "Popped dir to " ..  utils.colors.green .. utils.prevDir .. utils.colors.reset)
    end
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
    if not fileExists(batchFilePath) then
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
    runCommand(command)
end

return utils
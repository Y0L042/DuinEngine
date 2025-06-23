local utils = {}

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
    print("Current dir: " .. current_dir)                      
end

return utils
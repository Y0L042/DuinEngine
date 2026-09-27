local utils = require "utils"
local dep_tomldaslang = {}
local name = "TOMLDASLANG"

local repo   = "https://github.com/Y0L042/tomlc17-daslang.git"
local folder = "toml-daslang"

function dep_tomldaslang.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.popDir()
    end
    print(name .. " cloned.")

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_tomldaslang

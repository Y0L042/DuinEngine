local utils = require "utils"
local dep_flecsdaslangbindings = {}
local name = "FLECSDASLANGBINDINGS"

local repo   = "https://github.com/Y0L042/flecs-daslang"
local folder = "flecs-daslang"

function dep_flecsdaslangbindings.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.popDir()
    end
    print(name .. " cloned.")

    print("END: " .. name)
end

return dep_flecsdaslangbindings

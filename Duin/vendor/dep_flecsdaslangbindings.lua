local utils = require "utils"
local dep_flecsdaslangbindings = {}
local name = "FLECSDASLANGBINDINGS"

-- DuinEngine branch: flecs_core module name, DLL build settings and flecs_boost that
-- Duin requires. main is the standalone version and does not load in Duin.
local repo   = "https://github.com/Y0L042/flecs-daslang"
local branch = "DuinEngine"
local folder = "flecs-daslang"

function dep_flecsdaslangbindings.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive -b " .. branch .. " " .. repo .. " " .. folder) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git fetch origin")
        ok = utils.runCommand("git checkout " .. branch) and ok
        utils.runCommand("git pull origin " .. branch)
        utils.popDir()
    end
    print(name .. " cloned.")

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_flecsdaslangbindings

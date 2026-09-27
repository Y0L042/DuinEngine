local utils = require "utils"
local dep_bx = {}
local name = "BX"

local repo   = "https://github.com/bkaradzic/bx"
local commit = "cac72f6"
local folder = "bx"

function dep_bx.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
        ok = utils.runCommand("cd " .. folder .. " && git checkout " .. commit) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        ok = utils.runCommand("git checkout " .. commit) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_bx

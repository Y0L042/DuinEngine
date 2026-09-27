local utils = require "utils"
local dep_bimg = {}
local name = "BIMG"

local repo   = "https://github.com/bkaradzic/bimg"
local commit = "9114b47"
local folder = "bimg"

function dep_bimg.build()
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

return dep_bimg

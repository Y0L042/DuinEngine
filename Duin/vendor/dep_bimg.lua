local utils = require "utils"
local dep_bimg = {}
local name = "BIMG"

local repo   = "https://github.com/bkaradzic/bimg"
local commit = "9114b47"
local folder = "bimg"

function dep_bimg.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. commit)
        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_bimg

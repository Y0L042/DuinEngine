local utils = require "utils"
local dep_bx = {}
local name = "BX"

local repo   = "https://github.com/bkaradzic/bx"
local commit = "cac72f6"
local folder = "bx"

function dep_bx.build()
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

return dep_bx

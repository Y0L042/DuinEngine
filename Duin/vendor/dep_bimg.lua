local utils = require "utils"
local dep_bimg = {}

local repo = "https://github.com/bkaradzic/bimg"
local branch = "master"
local folder = "bimg"

function dep_bimg.build()
    print("START: BIMG")

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. branch .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. branch .. "")

        utils.popDir()
    end
    print("BIMG downloaded.")

    print("END: BIMG")
end

return dep_bimg
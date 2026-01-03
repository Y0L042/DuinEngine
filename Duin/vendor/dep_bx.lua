local utils = require "utils"
local dep_bx = {}

local repo = "https://github.com/bkaradzic/bx"
local commit = "d3d4031"  -- Compatible with BGFX 93e6125 (March 2025)
local folder = "bx"

function dep_bx.build()
    print("START: BX")

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. commit .. "")

        utils.popDir()
    end
    print("BX downloaded.")

    print("END: BX")
end

return dep_bx
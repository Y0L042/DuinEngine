local utils = require "utils"
local dep_imgui = {}
local name = "IMGUI"

local repo = "https://github.com/ocornut/imgui"
local branch = "docking"
local folder = "imgui"

function dep_imgui.build()
    print("START: " .. name)

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
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_imgui
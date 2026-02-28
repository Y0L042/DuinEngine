local utils = require "utils"
local dep_imgui = {}
local name = "IMGUIZMO"

local repo = "https://github.com/CedricGuillemet/ImGuizmo.git"
local commit = "a15acd8"
local folder = "imguizmo"

function dep_imgui.build()
    print("START: " .. name)

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
    print(name .. " downloaded.")

    -- Copy files to external directory
    local targetDir = "../src/external/imguizmo"
    print("\t\tCopying files to " .. targetDir)
    utils.copyFiles(folder, targetDir, { "*.h", "*.hpp", "*.c", "*.cpp" })

    print("END: " .. name)
end

return dep_imgui
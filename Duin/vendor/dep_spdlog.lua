local utils = require "utils"
local dep_spdlog = {}
local name = "SPDLOG"

local repo = "https://github.com/gabime/spdlog"
local tag = "v1.15.0"
local folder = "spdlog"

function dep_spdlog.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout tags/" .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_spdlog
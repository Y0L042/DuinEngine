local utils = require "utils"
local dep_toml11 = {}
local name = "TOML11"

local repo = "https://github.com/ToruNiina/toml11"
local tag = "v4.4.0"
local folder = "toml11"

function dep_toml11.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tag/" .. tag .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout tag/" .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.runCommand('cd toml11 && cmake -B ./build/ -DTOML11_PRECOMPILE=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug')
    utils.runCommand("cd toml11 && cmake --build build --config Debug")

    print("END: " .. name)
end

return dep_toml11
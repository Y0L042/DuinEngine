local utils = require "utils"
local dep_sdl3 = {}

local repo = "https://github.com/libsdl-org/SDL"
-- local tag = "release-3.2.8"
local tag = "release-3.4.0"
local folder = "sdl"

function dep_sdl3.build()
    print("START: SDL3")

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
        utils.runCommand("git fetch --all --tags")
        utils.runCommand("git checkout tags/" .. tag .. "")

        utils.changeDir(currentDir)
    end
    print("SDL3 downloaded.")

    -- Build
    utils.runCommand("cd sdl && cmake -S . -B build -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug")
    utils.runCommand("cd sdl && cmake --build build")


    print("END: SDL3")
end

return dep_sdl3

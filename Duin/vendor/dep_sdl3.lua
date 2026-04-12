local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_sdl3 = {}
local name = "SDL3"

local repo   = "https://github.com/libsdl-org/SDL"
local tag    = "release-3.4.0"
local folder = "sdl"

function dep_sdl3.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git fetch --all --tags")
        utils.runCommand("git checkout tags/" .. tag)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.runCommand("cd " .. folder .. " && cmake -S . -B build -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=" .. Cfg.cmake_crt_debug)
    utils.runCommand("cd " .. folder .. " && cmake --build build")

    print("END: " .. name)
end

return dep_sdl3

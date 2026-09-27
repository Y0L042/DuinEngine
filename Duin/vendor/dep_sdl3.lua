local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_sdl3 = {}
local name = "SDL3"

local repo   = "https://github.com/libsdl-org/SDL"
local tag    = "release-3.4.0"
local folder = "sdl"

function dep_sdl3.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
        ok = utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git fetch --all --tags")
        ok = utils.runCommand("git checkout tags/" .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    ok = utils.runCommand("cd " .. folder .. " && cmake -S . -B build -DSDL_SHARED=OFF -DSDL_STATIC=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=" .. Cfg.cmake_crt_debug) and ok
    ok = utils.runCommand("cd " .. folder .. " && cmake --build build") and ok

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_sdl3

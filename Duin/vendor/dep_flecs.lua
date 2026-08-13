local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_flecs = {}
local name = "FLECS"

local repo   = "https://github.com/SanderMertens/flecs"
local tag    = "v4.1.6"
local folder = "flecs"

function dep_flecs.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout tags/" .. tag)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.deleteFolder(folder .. "/build_vs2026")
    -- Pin the generator: this is often invoked from an MSYS2 shell where CMake
    -- defaults to Ninja + gcc, which chokes on the MSVC runtime settings below.
    -- The VS generator is also what produces the Debug/ subdir that
    -- premake5.lua expects flecs_static.lib to live in.
    utils.runCommand('cmake -S ' .. folder .. ' -B ' .. folder .. '/build_vs2026 -G "' .. Cfg.cmake_generator .. '" -A ' .. Cfg.cmake_arch .. ' -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. Cfg.cmake_crt_debug)
    utils.runCommand("cmake --build " .. folder .. "/build_vs2026 --config Debug")

    print("END: " .. name)
end

return dep_flecs

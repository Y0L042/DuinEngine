local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_assimp = {}
local name = "ASSIMP"

local repo   = "https://github.com/assimp/assimp"
local tag    = "v6.0.5"
local folder = "assimp"

function dep_assimp.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git fetch --tags")
        utils.runCommand("git checkout tags/" .. tag)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.deleteFolder(folder .. "/build_vs2026")
    utils.runCommand('cmake -S ' .. folder .. ' -B ' .. folder .. '/build_vs2026'
        .. ' -G "' .. Cfg.cmake_generator .. '" -A ' .. Cfg.cmake_arch
        .. ' -DBUILD_SHARED_LIBS=OFF'
        .. ' -DASSIMP_BUILD_TESTS=OFF'
        .. ' -DASSIMP_BUILD_SAMPLES=OFF'
        .. ' -DASSIMP_BUILD_ASSIMP_TOOLS=OFF'
        .. ' -DASSIMP_INSTALL=OFF'
        .. ' -DASSIMP_WARNINGS_AS_ERRORS=OFF'
        .. ' -DASSIMP_INJECT_DEBUG_POSTFIX=OFF'
        .. ' -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. Cfg.cmake_crt_debug)
    utils.runCommand("cmake --build " .. folder .. "/build_vs2026 --config Debug")

    print("END: " .. name)
end

return dep_assimp

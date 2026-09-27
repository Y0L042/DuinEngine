local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_assimp = {}
local name = "ASSIMP"

local repo   = "https://github.com/assimp/assimp"
local tag    = "v6.0.5"
local folder = "assimp"

function dep_assimp.build()
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
        utils.runCommand("git fetch --tags")
        ok = utils.runCommand("git checkout tags/" .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.deleteFolder(folder .. "/build_vs2026")
    ok = utils.runCommand('cmake -S ' .. folder .. ' -B ' .. folder .. '/build_vs2026'
        .. ' -G "' .. Cfg.cmake_generator .. '" -A ' .. Cfg.cmake_arch
        .. ' -DBUILD_SHARED_LIBS=OFF'
        .. ' -DASSIMP_BUILD_TESTS=OFF'
        .. ' -DASSIMP_BUILD_SAMPLES=OFF'
        .. ' -DASSIMP_BUILD_ASSIMP_TOOLS=OFF'
        .. ' -DASSIMP_INSTALL=OFF'
        .. ' -DASSIMP_WARNINGS_AS_ERRORS=OFF'
        .. ' -DASSIMP_INJECT_DEBUG_POSTFIX=OFF'
        .. ' -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. Cfg.cmake_crt_debug) and ok
    ok = utils.runCommand("cmake --build " .. folder .. "/build_vs2026 --config Debug") and ok

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_assimp

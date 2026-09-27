local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_tracy = {}
local name = "TRACY"

local repo   = "https://github.com/wolfpld/tracy.git"
local tag    = "v0.13.1"
local folder = "tracy"

function dep_tracy.build()
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
        utils.runCommand("git pull")
        ok = utils.runCommand("git checkout tags/" .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.deleteFolder(folder .. "/build_vs2026")
    local crt_flag = (Cfg.CRT == "MT") and "/MTd" or "/MDd"
    ok = utils.runCommand('cmake -S ' .. folder .. ' -B ' .. folder .. '/build_vs2026'
        .. ' -DTRACY_ENABLE=ON'
        .. ' -DTRACY_ON_DEMAND=ON'
        .. ' -DBUILD_SHARED_LIBS=OFF'
        .. ' -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. Cfg.cmake_crt_debug
        .. ' -DCMAKE_C_FLAGS_DEBUG="' .. crt_flag .. '"'
        .. ' -DCMAKE_CXX_FLAGS_DEBUG="' .. crt_flag .. '"') and ok
    ok = utils.runCommand("cmake --build " .. folder .. "/build_vs2026 --config Debug") and ok

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_tracy

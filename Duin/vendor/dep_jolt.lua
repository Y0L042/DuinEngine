local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_jolt = {}
local name = "JOLT"

local repo   = "https://github.com/jrouwe/JoltPhysics"
local tag    = "v5.5.0"
local folder = "jolt"

function dep_jolt.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. tag)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. tag)
        utils.popDir()
    end
    print(name .. " downloaded.")

    local use_static_crt = (Cfg.CRT == "MT") and "ON" or "OFF"
    utils.deleteFolder(folder .. "/Build/vs2026")
    utils.runCommand('cmake -S ' .. folder .. '/Build -B ' .. folder .. '/Build/vs2026 -DUSE_STATIC_MSVC_RUNTIME_LIBRARY=' .. use_static_crt .. ' -DCMAKE_MSVC_RUNTIME_LIBRARY=' .. Cfg.cmake_crt_debug .. ' -DCMAKE_CXX_FLAGS="/DJPH_ENABLE_ASSERTS"')
    utils.runCommand('cmake --build ' .. folder .. '/Build/vs2026 --config Debug')

    print("END: " .. name)
end

return dep_jolt

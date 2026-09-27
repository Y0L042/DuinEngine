local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_toml11 = {}
local name = "TOML11"

local repo   = "https://github.com/ToruNiina/toml11"
local tag    = "v4.4.0"
local folder = "toml11"

function dep_toml11.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
        ok = utils.runCommand("cd " .. folder .. " && git checkout " .. tag) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        ok = utils.runCommand("git checkout " .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    ok = utils.runCommand("cd " .. folder .. " && cmake -B ./build/ -DTOML11_PRECOMPILE=ON -DCMAKE_MSVC_RUNTIME_LIBRARY=" .. Cfg.cmake_crt_debug) and ok
    ok = utils.runCommand("cd " .. folder .. " && cmake --build build --config Debug") and ok

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_toml11

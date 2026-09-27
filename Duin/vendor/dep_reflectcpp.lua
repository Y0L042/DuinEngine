local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_reflectcpp = {}
local name = "REFLECTCPP"

local repo   = "https://github.com/getml/reflect-cpp"
local tag    = "v0.22.0"
local folder = "reflectcpp"

function dep_reflectcpp.build()
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
        ok = utils.runCommand("git checkout tags/" .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.pushDir(folder)

    local cmake_flags = "-DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Debug"
    if os.target() == "windows" then
        cmake_flags = cmake_flags .. " -DCMAKE_MSVC_RUNTIME_LIBRARY=" .. Cfg.cmake_crt_debug
    end

    if os.isdir("build") then
        if os.target() == "windows" then
            utils.runCommand("rmdir /s /q build")
        else
            utils.runCommand("rm -rf build")
        end
    end

    print("\t\tConfiguring with: " .. cmake_flags)
    ok = utils.runCommand("cmake -S . -B build " .. cmake_flags) and ok
    ok = utils.runCommand("cmake --build build --config Debug -j 4") and ok

    utils.popDir()
    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_reflectcpp

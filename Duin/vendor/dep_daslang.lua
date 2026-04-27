local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_daslang = {}
local name = "DASLANG"

local repo   = "https://github.com/GaijinEntertainment/daScript"
local commit    = "6c1e28b"
local checkout = commit
local folder = "daslang"

function dep_daslang.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. checkout)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout " .. checkout)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.pushDir(folder)

    local build_type = "Debug"

    local cmake_flags = "-DCMAKE_BUILD_TYPE=" .. build_type
        .. " -DDAS_TUTORIAL_DISABLED=OFF"
        .. " -DDAS_TESTS_DISABLED=ON"
        .. " -DDAS_AOT_EXAMPLES_DISABLED=ON"
        .. " -DDAS_GLFW_DISABLED=ON"
        .. " -DDAS_IMGUI_DISABLED=OFF"
        .. " -DDAS_CLANG_BIND_DISABLED=OFF"

    cmake_flags = cmake_flags .. " -DCMAKE_CXX_FLAGS_DEBUG=\"/DDAS_SMART_PTR_DEBUG=1 /DDAS_ENABLE_EXCEPTIONS=1\""

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
    utils.runCommand("cmake -S . -B build " .. cmake_flags)
    utils.runCommand("cmake --build build --config " .. build_type .. " -j 1") -- -j 1 avoids build concurrency errors

    utils.popDir()
    print("END: " .. name)
end

return dep_daslang

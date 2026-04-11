local utils = require "utils"
local dep_daslang = {}
local name = "DASLANG"

local repo = "https://github.com/GaijinEntertainment/daScript"
local tag = "0.6.1-RC2"
local folder = "daslang"

function dep_daslang.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. tag .. "")
    else
        print("\t\tFetch")
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout " .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    -- Build static lib
    utils.pushDir(folder)

    -- Build daslang as RelWithDebInfo: retains usable debug info while cutting the
    -- library size dramatically (~908MB Debug → ~200MB RelWithDebInfo). The engine
    -- does not need to step into daslang interpreter internals during debugging.
    local build_type = "RelWithDebInfo"

    local cmake_flags = "-DCMAKE_BUILD_TYPE=" .. build_type
        -- .. " -DDAS_TOOLS_DISABLED=ON"
        .. " -DDAS_TUTORIAL_DISABLED=OFF"
        .. " -DDAS_TESTS_DISABLED=ON"
        .. " -DDAS_AOT_EXAMPLES_DISABLED=ON"
        .. " -DDAS_GLFW_DISABLED=ON"
        .. " -DDAS_IMGUI_DISABLED=OFF"

    if os.target() == "windows" then
        cmake_flags = cmake_flags .. " -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug"
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
    utils.runCommand("cmake --build build --config " .. build_type .. " -j 1") -- " -j 1" avoids build concurrency errors

    utils.popDir()
    print("END: " .. name)
end

return dep_daslang

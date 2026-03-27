local utils = require "utils"
local dep_daslang = {}
local name = "DASLANG"

local repo = "https://github.com/GaijinEntertainment/daScript"
local tag = "v0.6.0"
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

    local cmake_flags = "-DCMAKE_BUILD_TYPE=Debug"
        -- .. " -DDAS_TOOLS_DISABLED=ON"
        -- .. " -DDAS_TUTORIAL_DISABLED=ON"
        .. " -DDAS_TESTS_DISABLED=ON"
        .. " -DDAS_AOT_EXAMPLES_DISABLED=ON"
        .. " -DDAS_GLFW_DISABLED=ON"
        -- .. " -DDAS_STBIMAGE_DISABLED=ON"
        -- .. " -DDAS_STBTRUETYPE_DISABLED=ON"
        -- .. " -DDAS_STDDLG_DISABLED=ON"
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
    utils.runCommand("cmake --build build --config Debug -j 4")

    utils.popDir()
    print("END: " .. name)
end

return dep_daslang

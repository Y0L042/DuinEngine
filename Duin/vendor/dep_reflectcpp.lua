local utils = require "utils"
local dep_reflectcpp = {}
local name = "REFLECTCPP"

local repo = "https://github.com/getml/reflect-cpp"
local tag = "v0.22.0"
local folder = "reflectcpp"

function dep_reflectcpp.build()
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
        utils.runCommand("git checkout " .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    -- Build only for Debug
    utils.pushDir(folder)

    local cmake_flags = "-DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Debug"
    
    if os.target() == "windows" then
        cmake_flags = cmake_flags .. " -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug"
    end

    -- Clean previous build to avoid conflicts
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

return dep_reflectcpp
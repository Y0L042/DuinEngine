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
        utils.runCommand("cd " .. folder .. " && git checkout tag/" .. tag .. "")
    else
        print("\t\tFetch")
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout tag/" .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    -- Build
    utils.pushDir(folder)

    utils.runCommand("cmake -S . -B build -DCMAKE_CXX_STANDARD=20 -DCMAKE_BUILD_TYPE=Release")
    utils.runCommand("cmake --build build --config Release -j 4")

    utils.popDir()
    print("END: " .. name)
end

return dep_reflectcpp

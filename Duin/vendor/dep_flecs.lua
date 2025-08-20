local utils = require "utils"
local dep_flecs = {}
local name = "FLECS"

local repo = "https://github.com/SanderMertens/flecs"
local commit = "2c373c3"
local folder = "flecs"

function dep_flecs.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit .. "")
    else
        print("\t\tFetch")
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout " .. commit .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")


    utils.deleteFolder("flecs/build_vs2022")
    if not os.isdir("flecs/build_vs2022") then
        -- makeDir("flecs/build_vs2022")
        -- runCommand('cmake -S flecs -B flecs/build_vs2022 -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_FLAGS="/MT" -DCMAKE_C_FLAGS="/MT"')
        utils.runCommand('cmake -S flecs -B flecs/build_vs2022 -DBUILD_SHARED_LIBS=OFF -DCMAKE_MSVC_RUNTIME_LIBRARY=MultiThreadedDebug -DCMAKE_C_FLAGS_DEBUG="/MTd" -DCMAKE_CXX_FLAGS_DEBUG="/MTd"')
        -- runCommand("cmake --build flecs/build_vs2022 --config Release")
        utils.runCommand("cmake --build flecs/build_vs2022 --config Debug")
    end

    print("END: " .. name)
end

return dep_flecs

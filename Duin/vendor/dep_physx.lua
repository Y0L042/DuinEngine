local utils = require "utils"
local dep_physx = {}
local name = "PHYSX"

local repo = "https://github.com/NVIDIA-Omniverse/PhysX"
local tag = "106.5-physx-5.5.1-cy"
local folder = "physx"

function dep_physx.build()
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


    -- Update presets for static libraries and runtime
    local presetFile = "PhysX/physx/buildtools/presets/public/vc17win64.xml"
    if utils.fileExists(presetFile) then
        local presetData = io.open(presetFile, "r"):read("*all")
        presetData = presetData:gsub(
            '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="False"',
            '<cmakeSwitch name="PX_GENERATE_STATIC_LIBRARIES" value="True"'
        )
        presetData = presetData:gsub(
            '<cmakeSwitch name="PX_BUILDSNIPPETS" value="True" comment="Generate the snippets" />',
            '<cmakeSwitch name="PX_BUILDSNIPPETS" value="False" comment="Generate the snippets" />"'
        )

        presetData = presetData:gsub(
            '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="False"',
            '<cmakeSwitch name="NV_USE_STATIC_WINCRT" value="True"'
        )
        local outFile = io.open(presetFile, "w")
        outFile:write(presetData)
        outFile:close()
        print("Updated PhysX presets for static libraries and runtime.")
    end

    -- Generate projects
    local generateProjects = "PhysX/physx/generate_projects.bat"
    utils.fixVsWherePath(generateProjects)
    utils.runBatchScript(generateProjects, "vc17win64")
    utils.runCommand("cd PhysX/physx/buildtools && python3 cmake_generate_projects.py 1")


    -- Build projects automatically (if applicable)
    utils.deleteFolder("PhysX/physx/bin")
    if os.isdir("PhysX/physx/bin") == false then
        utils.runCommand("cd PhysX/physx && cmake -S compiler/public -B compiler/vc17win64")
        -- runCommand("cd PhysX/physx && cmake --build compiler/vc17win64 --config Release")
        utils.runCommand("cd PhysX/physx && cmake --build compiler/vc17win64 --config Debug")
        utils.printCurrentDir()
        -- os.execute('cd PhysX/physx && msbuild "compiler/vc17win64/PhysXSDK.sln" /p:Configuration=Debug /p:Platform=x64')
        -- os.execute('cd PhysX/physx && msbuild "compiler/vc17win64/PhysXSDK.sln" /t:PhysX_static_64 /p:Configuration=Debug /p:Platform=x64')
        -- os.execute('msbuild "PhysXSDK.sln" /p:Configuration=Debug /p:Platform=x64')
    end

    print("END: " .. name)
end

return dep_physx
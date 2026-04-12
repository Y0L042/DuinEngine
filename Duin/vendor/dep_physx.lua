local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_physx = {}
local name = "PHYSX"

local repo   = "https://github.com/NVIDIA-Omniverse/PhysX"
local tag    = "107.3-physx-5.6.1"
local folder = "physx"

function dep_physx.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git checkout tags/" .. tag)
        utils.popDir()
    end
    print(name .. " downloaded.")

    local presetDir  = "PhysX/physx/buildtools/presets/public"
    local presetFile = presetDir .. "/vc18win64.xml"
    local sourcePreset = presetDir .. "/vc17win64.xml"

    -- Create vc18win64.xml from vc17win64.xml if it doesn't exist
    if not utils.fileExists(presetFile) and utils.fileExists(sourcePreset) then
        local srcData = io.open(sourcePreset, "r"):read("*all")
        local presetData = srcData:gsub("vc17", "vc18")
        local outFile = io.open(presetFile, "w")
        outFile:write(presetData)
        outFile:close()
        print("Created vc18win64.xml preset from vc17win64.xml.")
    end

    -- Patch: remove PUBLIC_RELEASE from the external DLL copy guard so freeglut/PhysXDevice
    -- are not required when building without snippets (PX_BUILDSNIPPETS=False).
    local winCMake = "PhysX/physx/source/compiler/cmake/windows/CMakeLists.txt"
    if utils.fileExists(winCMake) then
        local data = io.open(winCMake, "r"):read("*all")
        local patched = data:gsub(
            "IF%(PX_COPY_EXTERNAL_DLL OR PUBLIC_RELEASE%)",
            "IF(PX_COPY_EXTERNAL_DLL)"
        )
        if patched ~= data then
            local out = io.open(winCMake, "w")
            out:write(patched)
            out:close()
            print("Patched windows CMakeLists.txt: freeglut copy now guarded by PX_COPY_EXTERNAL_DLL only.")
        end
    end

    utils.deleteFolder("PhysX/physx/bin")
    local physxRoot = os.getcwd() .. "/PhysX/physx"
    local cmakeFlags = table.concat({
        '-DPHYSX_ROOT_DIR="' .. physxRoot .. '"',
        '-DPX_OUTPUT_LIB_DIR="' .. physxRoot .. '"',
        '-DPX_OUTPUT_BIN_DIR="' .. physxRoot .. '"',
        "-DTARGET_BUILD_PLATFORM=windows",
        "-DPX_GENERATE_STATIC_LIBRARIES=True",
        "-DPX_BUILDSNIPPETS=False",
        "-DPX_BUILDPVDRUNTIME=True",
        "-DNV_USE_STATIC_WINCRT=" .. (Cfg.CRT == "MT" and "True" or "False"),
        "-DNV_USE_DEBUG_WINCRT=True",
        "-DPX_FLOAT_POINT_PRECISE_MATH=False",
    }, " ")
    utils.runCommand('cd PhysX/physx && cmake -S compiler/public -B compiler/vc18win64 ' .. cmakeFlags)
    utils.runCommand("cd PhysX/physx && cmake --build compiler/vc18win64 --config Debug")

    print("END: " .. name)
end

return dep_physx

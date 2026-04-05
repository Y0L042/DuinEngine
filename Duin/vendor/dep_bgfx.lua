local utils = require "utils"
local dep_bgfx = {}

local repo = "https://github.com/bkaradzic/bgfx"
local branch = "master"
local commit = "8532b2c"
local folder = "bgfx"

function dep_bgfx.build()
    print("START: BGFX")

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. commit .. "")

        utils.popDir()
    end
    print("BGFX downloaded.")

    utils.changeDir(folder)
    utils.runCommand("git submodule update --init --recursive")
    utils.runCommand("..\\bx\\tools\\bin\\windows\\genie vs2026")
    utils.runCommand('cd .build\\projects\\vs2026 && msbuild "bgfx.slnx" /p:Configuration=Debug /p:Platform=x64 /p:RuntimeLibrary=MultiThreadedDebug /p:CLCompileAdditionalOptions="/Zc:__cplusplus"')
    utils.popDir()

    -- Copy example helper files to external/
    local externalDir = "../src/external"
    print("\t\tCopying bgfx example helpers to " .. externalDir)
    utils.copyFiles(folder .. "/examples/common", externalDir, {
        "bgfx_utils.cpp", "bgfx_utils.h",
        "camera.cpp", "camera.h",
        "common.h",
        "example-glue.cpp",
    })

    -- Copy debugdraw sources to external/bgfx/
    local bgfxDir = "../src/external/bgfx"
    print("\t\tCopying debugdraw files to " .. bgfxDir)
    utils.copyFiles(folder .. "/examples/common/debugdraw", bgfxDir, {
        "*.cpp", "*.h", "*.sc",
    })

    -- Copy imgui shader binaries to external/backends/
    local backendsDir = "../src/external/backends"
    print("\t\tCopying imgui shader binaries to " .. backendsDir)
    utils.copyFiles(folder .. "/examples/common/imgui", backendsDir, {
        "fs_ocornut_imgui.bin.h", "vs_ocornut_imgui.bin.h",
        "fs_ocornut_imgui.sc", "vs_ocornut_imgui.sc",
    })

    print("END: BGFX")
end

return dep_bgfx
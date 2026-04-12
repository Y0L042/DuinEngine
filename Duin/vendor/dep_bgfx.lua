local Cfg = require "premakeCfg"
local utils = require "utils"
local dep_bgfx = {}
local name = "BGFX"

local repo   = "https://github.com/bkaradzic/bgfx"
local commit = "8532b2c"
local folder = "bgfx"

function dep_bgfx.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. commit)
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.pushDir(folder)
    utils.runCommand("git submodule update --init --recursive")
    utils.runCommand("..\\bx\\tools\\bin\\windows\\genie vs2026")

    -- Patch genie-generated vcxproj files to use the workspace CRT (MD/MDd).
    -- genie always emits static-CRT entries; /p:RuntimeLibrary= on msbuild is
    -- ignored for this property, so we rewrite the XML directly.
    local vcxprojDir = ".build\\projects\\vs2026"
    for _, proj in ipairs({ "bgfx", "bimg", "bimg_decode", "bx" }) do
        local path = vcxprojDir .. "\\" .. proj .. ".vcxproj"
        local f = io.open(path, "r")
        if f then
            local content = f:read("*a")
            f:close()
            content = content:gsub(
                "<RuntimeLibrary>MultiThreadedDebug</RuntimeLibrary>",
                "<RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>")
            content = content:gsub(
                "<RuntimeLibrary>MultiThreaded</RuntimeLibrary>",
                "<RuntimeLibrary>MultiThreadedDLL</RuntimeLibrary>")
            local out = io.open(path, "w")
            out:write(content)
            out:close()
            print("\t\tPatched CRT to DLL in " .. path)
        end
    end

    utils.runCommand('cd .build\\projects\\vs2026 && msbuild "bgfx.slnx" /p:Configuration=Debug /p:Platform=x64 /p:CLCompileAdditionalOptions="/Zc:__cplusplus"')
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

    print("END: " .. name)
end

return dep_bgfx

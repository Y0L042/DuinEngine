local utils = require "utils"
local dep_imgui = {}
local name = "IMGUI"

local repo   = "https://github.com/ocornut/imgui"
local branch = "docking"
local folder = "imgui"

function dep_imgui.build()
    print("START: " .. name)

    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. branch)
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. branch)
        utils.popDir()
    end
    print(name .. " downloaded.")

    -- Copy core ImGui source files to external/
    local targetDir = "../src/external"
    print("\t\tCopying core files to " .. targetDir)
    utils.copyFiles(folder, targetDir, {
        "imgui.cpp", "imgui.h",
        "imgui_demo.cpp", "imgui_draw.cpp",
        "imgui_internal.h", "imgui_tables.cpp", "imgui_widgets.cpp",
        "imconfig.h",
        "imstb_rectpack.h", "imstb_textedit.h", "imstb_truetype.h",
    })

    -- Copy SDL3 backend files to external/backends/
    local backendsDir = "../src/external/backends"
    print("\t\tCopying backend files to " .. backendsDir)
    utils.copyFiles(folder .. "/backends", backendsDir, {
        "imgui_impl_sdl3.cpp", "imgui_impl_sdl3.h",
    })

    -- Patch include paths: upstream uses "imgui.h" but we keep imgui in external/
    utils.patchFile(backendsDir .. "/imgui_impl_sdl3.h",   '"imgui%.h"',   '"../../external/imgui.h"')
    utils.patchFile(backendsDir .. "/imgui_impl_sdl3.cpp", '"imgui%.h"',   '"../../external/imgui.h"')

    print("END: " .. name)
end

return dep_imgui

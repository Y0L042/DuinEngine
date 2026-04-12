local utils = require "utils"
local dep_imguizmo = {}
local name = "IMGUIZMO"

local repo   = "https://github.com/CedricGuillemet/ImGuizmo.git"
local commit = "a15acd8"
local folder = "imguizmo"

function dep_imguizmo.build()
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

    -- Copy files to external directory
    local targetDir = "../src/external/imguizmo"
    print("\t\tCopying files to " .. targetDir)
    utils.copyFiles(folder, targetDir, { "*.h", "*.hpp", "*.c", "*.cpp" })

    -- Patch include paths: upstream uses "imgui.h"/"imgui_internal.h" but we keep imgui in external/
    local files = os.matchfiles(targetDir .. "/*.cpp")
    for _, f in ipairs(os.matchfiles(targetDir .. "/*.h")) do
        table.insert(files, f)
    end
    for _, f in ipairs(files) do
        utils.patchFile(f, '"imgui_internal%.h"', '"external/imgui_internal.h"')
        utils.patchFile(f, '"imgui%.h"',           '"external/imgui.h"')
    end

    print("END: " .. name)
end

return dep_imguizmo

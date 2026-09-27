local utils = require "utils"
local dep_imguizmo = {}
local name = "IMGUIZMO"

local repo   = "https://github.com/CedricGuillemet/ImGuizmo.git"
local commit = "a15acd8"
local folder = "imguizmo"

function dep_imguizmo.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
        ok = utils.runCommand("cd " .. folder .. " && git checkout " .. commit) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        ok = utils.runCommand("git checkout " .. commit) and ok
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

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_imguizmo

local utils = require "utils"
local dep_dasimgui = {}
local name = "DASIMGUI"

-- Fork of borisbat/dasImgui with a premake5.lua and bindings matched to our daslang.
local repo   = "https://github.com/Y0L042/dasImgui"
local branch = "DuinEngine"
local folder = "dasimgui"

function dep_dasimgui.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive -b " .. branch .. " " .. repo .. " " .. folder) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git fetch origin")
        ok = utils.runCommand("git checkout " .. branch) and ok
        utils.runCommand("git pull origin " .. branch)
        utils.popDir()
    end
    print(name .. " downloaded.")

    -- Patch imgui.h: add data() method to ImVector
    -- Required by daScript's ast_handle.h ManagedVectorAnnotation.
    local imguiHeader = "../src/external/imgui.h"
    if utils.fileExists(imguiHeader) then
        utils.patchFile(imguiHeader,
            "(inline T%*           end%(%)"
            .. "                               "
            .. "{ return Data %+ Size; })",
            "inline T*           data()"
            .. "                              "
            .. "{ return Data; }\n"
            .. "    inline const T*     data() const"
            .. "                        "
            .. "{ return Data; }\n"
            .. "    %1")
    else
        print("WARNING: imgui.h not found at " .. imguiHeader .. " — skipping data() patch")
    end

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_dasimgui

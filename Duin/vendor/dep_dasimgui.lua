local utils = require "utils"
local dep_dasimgui = {}
local name = "DASIMGUI"

function dep_dasimgui.build()
    print("START: " .. name)

    -- dasimgui now lives directly in vendor/dasimgui (no git clone needed).
    -- Only the imgui.h patch is required.

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

    print("END: " .. name)
end

return dep_dasimgui

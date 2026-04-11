local utils = require "utils"
local dep_dasimgui = {}
local name = "DASIMGUI"

local repo   = "https://github.com/borisbat/dasImgui"
local commit = "ebc957b38506617ff7cca694e3f1342a33be8574"
local folder = "dasimgui"

function dep_dasimgui.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit)
    else
        print("\t\tFetch")
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git fetch origin")
        utils.runCommand("git checkout " .. commit)

        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_dasimgui

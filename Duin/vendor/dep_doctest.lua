local utils = require "utils"
local dep_doctest = {}
local name = "DOCTEST"

local repo   = "https://github.com/doctest/doctest"
local tag    = "v2.4.12"
local folder = "doctest"

function dep_doctest.build()
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

    print("END: " .. name)
end

return dep_doctest

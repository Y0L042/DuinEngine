local utils = require "utils"
local dep_doctest = {}
local name = "DOCTEST"

local repo = "https://github.com/doctest/doctest"
local tag = "v2.4.12"
local folder = "doctest"

function dep_doctest.build()
    print("START: " .. name)

    -- Clone Repo
    if not utils.isDir(folder) then
        utils.print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. tag .. "")
    else
        utils.print("\t\tFetch")
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.runCommand("git checkout " .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_doctest

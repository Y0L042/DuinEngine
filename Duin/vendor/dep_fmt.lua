local utils = require "utils"
local dep_fmt = {}
local name = "FMT"

local repo = "https://github.com/fmtlib/fmt"
local tag = "11.0.2"
local folder = "fmt"

function dep_fmt.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout tag/" .. tag .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout tag/" .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_fmt
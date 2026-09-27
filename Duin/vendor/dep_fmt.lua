local utils = require "utils"
local dep_fmt = {}
local name = "FMT"

local repo   = "https://github.com/fmtlib/fmt"
local tag    = "11.0.2"
local folder = "fmt"

function dep_fmt.build()
    print("START: " .. name)
    local ok = true

    if not os.isdir(folder) then
        print("\t\tClone")
        ok = utils.runCommand("git clone --recursive " .. repo .. " " .. folder) and ok
        ok = utils.runCommand("cd " .. folder .. " && git checkout tags/" .. tag) and ok
    else
        print("\t\tFetch")
        utils.pushDir(folder)
        utils.runCommand("git stash")
        ok = utils.runCommand("git checkout tags/" .. tag) and ok
        utils.popDir()
    end
    print(name .. " downloaded.")

    utils.reportBuildStatus(name, ok)
    print("END: " .. name)
end

return dep_fmt

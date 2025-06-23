local utils = require "utils"
local dep_rapidjson = {}
local name = "RAPIDJSON"

local repo = "https://github.com/Tencent/rapidjson"
local tag = "v1.1.0"
local folder = "rapidjson"

function dep_rapidjson.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. tag .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. tag .. "")

        utils.popDir()
    end
    print(name .. " downloaded.")

    print("END: " .. name)
end

return dep_rapidjson
local utils = require "utils"
local dep_bgfx = {}

local repo = "https://github.com/bkaradzic/bgfx"
local branch = "master"
local commit = "93e6125"
local folder = "bgfx"

function dep_bgfx.build()
    print("START: BGFX")

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
        utils.runCommand("cd " .. folder .. " && git checkout " .. commit .. "")
    else
        print("\t\tFetch")
        local currentDir = os.getcwd()
        utils.changeDir(folder)

        utils.runCommand("git stash")
        utils.runCommand("git checkout " .. commit .. "")

        utils.popDir()
    end
    print("BGFX downloaded.")

    utils.changeDir(bgfx)
    utils.runCommand("git submodule update --init --recursive")
    utils.runCommand("..\\bx\\tools\\bin\\windows\\genie vs2022")
    utils.runCommand('.build\\projects\\vs2022 && msbuild "bgfx.sln" /p:Configuration=Debug /p:Platform=x64 /p:RuntimeLibrary=MultiThreadedDebug /p:CLCompileAdditionalOptions="/Zc:__cplusplus"')
    utils.popDir()

    print("END: BGFX")
end

return dep_bgfx
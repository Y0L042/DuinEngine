local utils = require "utils"
local dep_flecsdaslangbindings = {}
local name = "FLECSDASLANGBINDINGS"

local repo = "https://github.com/Y0L042/flecs-daslang"
local folder = "flecs-daslang"

-- On-disk sibling project path (relative to the vendor/ dir where this script runs)
local source_project = "../../flecs-daslang"
local source_headers = source_project .. "/flecs_das/src"
local source_lib     = source_project .. "/flecs_das/bin/Debug-windows-x86_64/flecs_das"

local dest_headers = folder .. "/flecs_das/src"
local dest_lib     = folder .. "/flecs_das/bin/Debug-windows-x86_64/flecs_das"

function dep_flecsdaslangbindings.build()
    print("START: " .. name)

    -- Clone Repo
    if not os.isdir(folder) then
        print("\t\tClone")
        utils.runCommand("git clone --recursive " .. repo .. " " .. folder)
    else
        print("\t\tFetch")
        utils.changeDir(folder)
        utils.runCommand("git stash")
        utils.runCommand("git pull")
        utils.popDir()
    end
    print(name .. " cloned.")

    -- Copy headers from on-disk project
    print("\t\tCopying headers from " .. source_headers)
    local win_dest_headers = dest_headers:gsub("/", "\\")
    local win_source_headers = source_headers:gsub("/", "\\")
    utils.runCommand("robocopy \"" .. win_source_headers .. "\" \"" .. win_dest_headers .. "\" *.h *.hpp")

    -- Copy pre-built lib from on-disk project
    print("\t\tCopying lib from " .. source_lib)
    local win_dest_lib = dest_lib:gsub("/", "\\")
    local win_source_lib = source_lib:gsub("/", "\\")
    utils.runCommand("robocopy \"" .. win_source_lib .. "\" \"" .. win_dest_lib .. "\" *.lib")

    print("END: " .. name)
end

return dep_flecsdaslangbindings

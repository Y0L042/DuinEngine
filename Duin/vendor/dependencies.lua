local vendorDeps = {}
local utils = require("utils")

local dep_sdl3 = require "dep_sdl3"

local dependencies = {
    SDL3 = dep_sdl3.build
}
-- Automatically register new options
for depName, dep in pairs(dependencies) do
    newoption {
        trigger     = depName,
        description = "Rebuild " .. depName
    }
end

-- Process all dependencies
function vendorDeps.processDependencies()
    if not _OPTIONS["deps"] then
        return
    end

    print("Building dependencies...")
    -- Store dir and go to /vendor
    local currentDir = os.getcwd()
    os.chdir("vendor")
    utils.printCurrentDir()

    -- gather any specific flags
    local flagged = {}
    for name, buildFn in pairs(dependencies) do
        if _OPTIONS[name] then
            table.insert(flagged, name)
        end
    end

    if #flagged > 0 then
        print("Processing only flagged dependencies:")
        for _, name in ipairs(flagged) do
            print(" -> " .. name)
            dependencies[name]()
        end
    else
        print("No specific flags; processing all dependencies:")
        for name, buildFn in pairs(dependencies) do
            print(" -> " .. name)
            buildFn()
        end
    end

    -- Return to prev dir
    os.chdir(currentDir)
end

return vendorDeps
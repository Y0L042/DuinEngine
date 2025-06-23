local vendorDeps = {}
local utils = require("utils")





local dep_sdl3 = require "dep_sdl3"
local dep_bx = require "dep_bx"
local dep_bimg = require "dep_bimg"
local dep_bgfx = require "dep_bgfx"
local dep_physx = require "dep_physx"
local dep_fmt = require "dep_fmt"
local dep_spdlog = require "dep_spdlog"
local dep_toml11 = require "dep_toml11"
local dep_imgui = require "dep_imgui"
local dep_rapidjson = require "dep_rapidjson"
local dep_flecs = require "dep_flecs"

local dependencies = {
    SDL3 = dep_sdl3.build,
    BX = dep_bx.build,
    BIMG = dep_bimg.build,
    BGFX = dep_bgfx.build,
    PHYSX = dep_physx.build,
    FMT = dep_fmt.build,
    SPDLOG = dep_spdlog.build,
    TOML11 = dep_toml11.build,
    IMGUI = dep_imgui.build,
    RAPIDJSON = dep_rapidjson.build,
    FLECS = dep_flecs.build
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
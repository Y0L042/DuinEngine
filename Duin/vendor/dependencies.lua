local vendorDeps = {}
local utils = require("utils")





local dep_sdl3 = require "dep_sdl3"
local dep_bx = require "dep_bx"
local dep_bimg = require "dep_bimg"
local dep_bgfx = require "dep_bgfx"
local dep_physx = require "dep_physx"
local dep_jolt = require "dep_jolt"
local dep_fmt = require "dep_fmt"
local dep_spdlog = require "dep_spdlog"
local dep_toml11 = require "dep_toml11"
local dep_imgui = require "dep_imgui"
local dep_imguizmo = require "dep_imguizmo"
local dep_rapidjson = require "dep_rapidjson"
local dep_flecs = require "dep_flecs"
local dep_doctest = require "dep_doctest"
local dep_reflectcpp = require "dep_reflectcpp"
local dep_daslang = require "dep_daslang"
local dep_dasimgui = require "dep_dasimgui"
local dep_flecsdaslangbindings = require "dep_flecsdaslangbindings"
local dep_tomldaslang = require "dep_dastoml"
local dep_tracy = require "dep_tracy"
local dep_assimp = require "dep_assimp"

-- Single source of truth for build order. Flagged builds follow this order too.
local dependencies_ordered = {
    -- Core utility libraries (header-only / standalone)
    {name = "FMT", buildFn = dep_fmt.build},
    {name = "SPDLOG", buildFn = dep_spdlog.build},
    {name = "TOML11", buildFn = dep_toml11.build},
    {name = "RAPIDJSON", buildFn = dep_rapidjson.build},
    {name = "REFLECTCPP", buildFn = dep_reflectcpp.build},
    {name = "DOCTEST", buildFn = dep_doctest.build},

    -- Platform & rendering
    {name = "SDL3", buildFn = dep_sdl3.build},
    {name = "BX", buildFn = dep_bx.build},     -- before BIMG and BGFX (BGFX uses bx's genie)
    {name = "BIMG", buildFn = dep_bimg.build}, -- before BGFX
    {name = "BGFX", buildFn = dep_bgfx.build},

    -- UI (copied into src/external)
    {name = "IMGUI", buildFn = dep_imgui.build},
    {name = "IMGUIZMO", buildFn = dep_imguizmo.build}, -- after IMGUI

    -- ECS, physics, assets, profiling
    {name = "FLECS", buildFn = dep_flecs.build},
    {name = "JOLT", buildFn = dep_jolt.build},
    {name = "PHYSX", buildFn = dep_physx.build},
    {name = "ASSIMP", buildFn = dep_assimp.build},
    {name = "TRACY", buildFn = dep_tracy.build},

    -- Scripting (daslang + bindings)
    {name = "DASLANG", buildFn = dep_daslang.build},
    {name = "DASIMGUI", buildFn = dep_dasimgui.build}, -- after IMGUI (patches the copied imgui.h) and DASLANG
    {name = "FLECSDASLANGBINDINGS", buildFn = dep_flecsdaslangbindings.build}, -- after FLECS and DASLANG
    {name = "TOMLDASLANG", buildFn = dep_tomldaslang.build},
}

newoption {
    trigger     = "deps",
    description = "Process dependencies (update or rebuild based on dependency-specific flags)"
}

for _, dep in ipairs(dependencies_ordered) do
    newoption {
        trigger     = dep.name,
        description = "Rebuild " .. dep.name
    }
end

-- Flagged deps in build order, or all of them when none are flagged.
local function selectedDependencies()
    local flagged = {}
    for _, dep in ipairs(dependencies_ordered) do
        if _OPTIONS[dep.name] then
            table.insert(flagged, dep)
        end
    end
    return flagged, #flagged == 0
end

local function confirm(selected, all)
    local answer
    repeat
        io.write("This will fetch and rebuild " .. (all and "ALL" or "the following") .. " dependencies:\n")
        for _, dep in ipairs(selected) do
            io.write("  - " .. dep.name .. "\n")
        end
        if all then
            io.write("\nThis may take a long time.\n")
        end
        io.write("\nContinue with this operation (yes/n)? ")
        io.flush()
        answer = io.read()
    until answer == "yes" or answer == "n" or answer == nil
    return answer == "yes"
end

-- Prompt, then build the selected deps from inside vendorDir (the folder holding the dep_*.lua scripts).
function vendorDeps.run(vendorDir)
    local selected, all = selectedDependencies()
    if all then
        selected = dependencies_ordered
    end

    if not confirm(selected, all) then
        print("Operation aborted.")
        return
    end
    print("Operation continued.")

    print("Building dependencies...")
    utils.buildStatusFile = path.join(vendorDir, "build_status.log")
    local currentDir = os.getcwd()
    os.chdir(vendorDir)
    utils.printCurrentDir()

    for _, dep in ipairs(selected) do
        print(" -> " .. dep.name)
        -- Deps report their own result; this catches the ones that abort via error().
        local ok, err = pcall(dep.buildFn)
        if not ok then
            utils.reportBuildStatus(dep.name, false)
            os.chdir(currentDir)
            error(err, 0)
        end
    end

    os.chdir(currentDir)
end

-- `premake5 --deps STATUS`: premake parses STATUS as the action.
function vendorDeps.isStatusRequest()
    return (_ACTION or ""):upper() == "STATUS"
end

-- Print the last recorded build result of every dependency, in build order.
function vendorDeps.status(vendorDir)
    utils.buildStatusFile = path.join(vendorDir, "build_status.log")
    local entries = utils.readBuildStatus()
    local c = utils.colors
    local built, failed, missing = 0, 0, 0

    print("Dependency build status (" .. utils.buildStatusFile .. "):")
    for _, dep in ipairs(dependencies_ordered) do
        local e = entries[dep.name]
        local status, color, date
        if not e then
            status, color, date = "NOT BUILT", c.yellow, ""
            missing = missing + 1
        elseif e.status == "BUILT" then
            status, color, date = e.status, c.green, e.date
            built = built + 1
        else
            status, color, date = e.status, c.red, e.date
            failed = failed + 1
        end
        print(string.format("  %-22s %s%-10s%s %s", dep.name, color, status, c.reset, date))
    end
    print(string.format("\n%d built, %d failed, %d not built", built, failed, missing))
end

return vendorDeps

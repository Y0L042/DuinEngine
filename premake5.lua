function prependRoot(root, dirs)
    local result = {}
    for _, dir in ipairs(dirs) do
        table.insert(result, root .. "/" .. dir)
    end
    return result
end

workspace "Duin"
    architecture "x64"
    startproject "DuinFPS"

    configurations { "Debug", "DebugCoverage", "Release", "Dist", "Archive" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    staticruntime "On"

    -- Include directories relative to the root folder (solution directory)
    IncludeDir = {}
    IncludeDir["sdl"] = "Duin/vendor/sdl/include"
    IncludeDir["bgfx"] = "Duin/vendor/bgfx/include"
    IncludeDir["bgfx_examples"] = "Duin/vendor/bgfx/examples/common"
    IncludeDir["bgfx_3p"] = "Duin/vendor/bgfx/3rdparty"
    IncludeDir["bx"] = "Duin/vendor/bx/include"
    IncludeDir["bimg"] = "Duin/vendor/bimg/include"
    IncludeDir["spdlog"] = "Duin/vendor/spdlog/include"
    IncludeDir["flecs"] = "Duin/vendor/flecs/include"
    IncludeDir["fmt"] = "Duin/vendor/fmt/include"
    IncludeDir["patches"] = "Duin/vendor/patches"
    IncludeDir["toml11"] = "Duin/vendor/toml11/include"
    IncludeDir["rapidjson"] = "Duin/vendor/rapidjson/include"
    IncludeDir["imguifilex"] = "Duin/vendor/ImGuiFileDialog"
    IncludeDir["imguizmo"] = "Duin/vendor/imguizmo"
    IncludeDir["physx"] = "Duin/vendor/PhysX/physx/include"
    IncludeDir["doctest"] = "Duin/vendor/doctest/doctest"
    IncludeDir["reflectcpp"] = "Duin/vendor/reflectcpp/include"
    IncludeDir["angelscript"] = "Duin/vendor/angelscript/sdk/angelscript/include"
    IncludeDir["angelscript_addon"] = "Duin/vendor/angelscript/sdk/add_on"
    IncludeDir["daslang"] = "Duin/vendor/daslang/include"

    global_files = {}
    global_includedirs = 
    { 
        "Duin/src" 
    }
    global_externalincludedirs = 
    {    
        "%{IncludeDir.sdl}",
        "%{IncludeDir.bx}",
        "%{IncludeDir.bimg}",
        "%{IncludeDir.bgfx}",
        "%{IncludeDir.bgfx_examples}",
        "%{IncludeDir.bgfx_3p}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.imguizmo}",
        "%{IncludeDir.imguifilex}",
        "%{IncludeDir.flecs}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.patches}",
        "%{IncludeDir.toml11}",
        "%{IncludeDir.rapidjson}",
        "%{IncludeDir.physx}",
        "%{IncludeDir.doctest}",
        "%{IncludeDir.reflectcpp}",
    }
    global_libdirs = 
    {
        "Duin/bin/" .. outputdir .. "/Duin",  -- Duin's output directory
        "Duin/vendor/sdl/build/Debug",
        "Duin/vendor/bgfx/.build/win64_vs2026/bin",
        "Duin/vendor/flecs/build_vs2026/Debug",
        "Duin/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
        "Duin/vendor/toml11/build/src/Debug",
        "Duin/vendor/reflectcpp/build/Debug",
        "Duin/vendor/daslang/lib/RelWithDebInfo",
    }
    global_defines = 
    {
        "DN_TESTING",
        "DN_PLATFORM_WINDOWS",
        "DN_BUILD_STATIC",
        "flecs_STATIC",
        "PX_PHYSX_STATIC_LIB",
        "BX_CONFIG_DEBUG=0",
    }
    global_links = 
    {
        "flecs_static.lib",
        "winmm.lib",
        "legacy_stdio_definitions.lib",
        "Setupapi.lib",
        "Version.lib",
        "Imm32.lib",
        "Cfgmgr32.lib",
        "SDL3-static.lib",
        "toml11.lib",
        "bxDebug.lib",
        "bimgDebug.lib",
        "bgfxDebug.lib",
        "Duin.lib",
        "PhysX_static.lib",
        "PhysXCooking_static.lib",
        "PhysXCommon_static.lib",
        "PhysXFoundation_static.lib",
        "PhysXPvdSDK_static.lib",
        "PhysXExtensions_static.lib",
        "PhysXCharacterKinematic_static.lib",
        "reflectcpp.lib",
    }

    -- Debug build size optimisations:
    --   FastLink PDB — references .obj files instead of copying all symbols in.
    --                  Cuts PDB size ~80%. Trade-off: keep bin-int/ to use the debugger.
    filter { "configurations:Debug or DebugCoverage", "system:windows" }
        symbols "FastLink"
    filter {}

    -- Include sub-Premake files
    include "Duin"
    include "DuinTests"
    include "DuinEditor"
    include "DuinEditorTests"
    include "ExampleProjects/DuinFPS"
    include "ExampleProjects/DuinFPSTests"
    include "ExampleProjects/Sandbox"
    include "ExampleProjects/DuinFPSDaslang"
    include "ScriptTests/AngelScriptTest"
    include "ScriptTests/DaslangTest"

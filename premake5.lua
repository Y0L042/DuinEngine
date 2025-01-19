function prependRoot(root, dirs)
    local result = {}
    for _, dir in ipairs(dirs) do
        table.insert(result, root .. "/" .. dir)
    end
    return result
end

workspace "Duin"
    architecture "x64"
    startproject "Sandbox"

    configurations { "Debug", "Release", "Dist", "Archive" }
    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    staticruntime "On"

    -- Include directories relative to the root folder (solution directory)
    IncludeDir = {}
    IncludeDir["spdlog"] = "Duin/vendor/spdlog/include"
    IncludeDir["rlimgui"] = "Duin/vendor/rlimgui"
    IncludeDir["raylib"] = "Duin/vendor/rlimgui/raylib-master/src"
    IncludeDir["raygui"] = "Duin/vendor/raygui/src"
    IncludeDir["imgui"] = "Duin/vendor/rlimgui/imgui-docking"
    IncludeDir["imguibackends"] = "Duin/vendor/rlimgui/imgui-docking/backends"
    IncludeDir["glfw"] = "Duin/vendor/rlimgui/raylib-master/src/external/glfw/include"
    IncludeDir["flecs"] = "Duin/vendor/flecs/include"
    IncludeDir["fmt"] = "Duin/vendor/fmt/include"
    IncludeDir["patches"] = "Duin/vendor/patches"
    IncludeDir["rapidjson"] = "Duin/vendor/rapidjson/include"
    IncludeDir["imguifilex"] = "Duin/vendor/ImGuiFileDialog/"
    IncludeDir["physx"] = "Duin/vendor/PhysX/physx/include"

    global_files = {}
    global_includedirs = 
    { 
        "Duin/src" 
    }
    global_externalincludedirs = 
    {    
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.raylib}",
        "%{IncludeDir.raygui}",
        "%{IncludeDir.imgui}",
        "%{IncludeDir.imguifilex}",
        "%{IncludeDir.rlimgui}",
        "%{IncludeDir.flecs}",
        "%{IncludeDir.fmt}",
        "%{IncludeDir.patches}",
        "%{IncludeDir.rapidjson}",
        "%{IncludeDir.physx}",
    }
    global_libdirs = 
    {
        "Duin/bin/" .. outputdir .. "/Duin",  -- Duin's output directory
        "Duin/vendor/rlimgui/bin/Debug",
        "Duin/vendor/flecs/build_vs2022/build/Debug",	
        "Duin/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
    }
    global_defines = 
    {
        "DN_PLATFORM_WINDOWS",
        "DN_BUILD_STATIC",
        "flecs_STATIC",
        "IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
        "PX_PHYSX_STATIC_LIB",
    }
    global_links = 
    {
        "raylib.lib", 
        "rlImGui.lib",
        "flecs_static.lib",
        "winmm.lib",
        "Duin.lib",
        "PhysX_static_64.lib",
        "PhysXCooking_static_64.lib",
        "PhysXCommon_static_64.lib",
        "PhysXFoundation_static_64.lib",
        "PhysXPvdSDK_static_64.lib",
        "PhysXExtensions_static_64.lib",
        "PhysXCharacterKinematic_static_64.lib",
    }

    -- Include sub-Premake files
    include "Duin"
    include "DuinEditor"
    include "ExampleProjects/DuinFPS"
    -- include "ExampleProjects/Sandbox"

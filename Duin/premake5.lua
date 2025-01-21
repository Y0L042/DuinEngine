SolutionRoot = ".."
ProjectRoot = "."

project "Duin"
    location ""
    kind "StaticLib"
    language "C++"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "dnpch.h"
    pchsource "./src/dnpch.cpp"

    -- Exclude precompiled headers for C files
    filter "files:**.c"
        flags { "NoPCH" }
        pchheader ""  -- Ensures .c files ignore PCH
    filter {} -- Clear the filter

    include "vendor"

    -- files(global_files)
	files 
	{
		ProjectRoot .. "/src/**.h",
		ProjectRoot .. "/src/**.hpp",
		ProjectRoot .. "/src/**.c",
		ProjectRoot .. "/src/**.cpp",
	}

    -- includedirs(global_includedirs) 
    includedirs 
    { 
        ProjectRoot .. "/src",
    }
    -- externalincludedirs(global_externalincludedirs) 
    externalincludedirs 
    { 
        SolutionRoot .. "/%{IncludeDir.spdlog}",
		SolutionRoot .. "/%{IncludeDir.raylib}",
		SolutionRoot .. "/%{IncludeDir.raygui}",
		SolutionRoot .. "/%{IncludeDir.imgui}",
		SolutionRoot .. "/%{IncludeDir.imguibackends}",
		SolutionRoot .. "/%{IncludeDir.glfw}",
		SolutionRoot .. "/%{IncludeDir.imguifilex}",
		SolutionRoot .. "/%{IncludeDir.rlimgui}",
		SolutionRoot .. "/%{IncludeDir.flecs}",
		SolutionRoot .. "/%{IncludeDir.fmt}",
		SolutionRoot .. "/%{IncludeDir.patches}",
        SolutionRoot .. "/%{IncludeDir.rapidjson}",
		SolutionRoot .. "/%{IncludeDir.physx}",
    }
    -- libdirs(global_libdirs) 
    libdirs 
    { 
		ProjectRoot .. "/vendor/rlimgui/bin/Debug",
		ProjectRoot .. "/vendor/flecs/build_vs2022/build/Debug",	
        ProjectRoot .. "/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
    }
    -- defines(global_defines)
    defines 
    { 
        -- TODO
        "flecs_STATIC",
        "DN_BUILD_STATIC",
		"DN_PLATFORM_WINDOWS",
        "PX_PHYSX_STATIC_LIB",

		"IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
		"SUPPORT_GIF_RECORDING",
    }
    -- links(global_links)
    links 
    { 
		"raylib.lib", 
		"rlImGui.lib",
        "flecs_static.lib",
		"winmm.lib",

        "PhysX_static_64.lib",
        "PhysXCooking_static_64.lib",
        "PhysXCommon_static_64.lib",
        "PhysXFoundation_static_64.lib",
        "PhysXPvdSDK_static_64.lib",
        "PhysXExtensions_static_64.lib",
        "PhysXCharacterKinematic_static_64.lib",
    }

    -- Enable multi-processor compilation
    filter "action:vs*"
      flags { "MultiProcessorCompile" }

    filter "system:windows"
        cppdialect "C++20"

    filter "configurations:Debug"
        defines { "DN_DEBUG", "_DEBUG" }
        symbols "On"

    filter "configurations:Release"
        defines { "DN_RELEASE", "NDEBUG" }
        optimize "On"

    filter "configurations:Dist"
        defines "DN_DIST"
        optimize "On"

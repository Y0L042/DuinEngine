workspace "Duin"
	architecture "x64"
	startproject "Sandbox"	

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    staticruntime "On"

    -- Include directories relative to the root folder (solution directory)
    IncludeDir = {}
    IncludeDir["spdlog"] = "Duin/vendor/spdlog/include"
	IncludeDir["rlimgui"] = "Duin/vendor/rlimgui"
	IncludeDir["raylib"] = "Duin/vendor/rlimgui/raylib-master/src"
	IncludeDir["imgui"] = "Duin/vendor/rlimgui/imgui-master"
	IncludeDir["flecs"] = "Duin/vendor/flecs/include"
	IncludeDir["fmt"] = "Duin/vendor/fmt/include"
	IncludeDir["patches"] = "Duin/vendor/patches"





project "Duin"
	location "Duin"
	-- kind "SharedLib"
    kind "StaticLib"
	language "C++"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	-- pchheader "dnpch.h"
	-- pchsource "Duin/src/dnpch.cpp"

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
        "%{prj.name}/src",
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.raylib}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.rlimgui}",
		"%{IncludeDir.flecs}",
		"%{IncludeDir.fmt}",
		"%{IncludeDir.patches}",
	}

	libdirs
	{
		"%{prj.name}/vendor/flecs/lib",	
		"%{prj.name}/vendor/rlimgui/bin/Debug",
	}

	links
	{
		"raylib.lib", 
		"rlImGui.lib",
        "flecs_static_debug.lib",
		"winmm.lib",
	}

	filter "system:windows"
		cppdialect "C++20"

	defines
	{
		"DN_PLATFORM_WINDOWS",
        "DN_BUILD_STATIC",
		"IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
		"SUPPORT_GIF_RECORDING",
	}
	
    -- Define DN_STATIC if building as a Static Library
    filter "kind:StaticLib"
        defines "DN_BUILD_STATIC"

    -- Define DN_BUILD_DLL if building as a Shared Library
    filter "kind:SharedLib"
        defines "DN_BUILD_DLL"

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/DuinEditor"),
	}
	
	filter "configurations:Debug"
	defines "DN_DEBUG"
	symbols "On"
	
	filter "configurations:Release"
	defines "DN_RELEASE"
	optimize "On"
	
	filter "configurations:Dist"
	defines "DN_DIST"
	optimize "On"


-- MODIFY THESE WHEN ADDING LIBS AND NEW PROJECTS


global_includedirs = 
{
	"Duin/src",
	"%{IncludeDir.spdlog}",
    "%{IncludeDir.raylib}",
    "%{IncludeDir.imgui}",
	"%{IncludeDir.rlimgui}",
    "%{IncludeDir.flecs}",
	"%{IncludeDir.fmt}",
}

global_libdirs =
{
	"Duin/vendor/rlimgui/bin/Debug",
    "Duin/vendor/flecs/lib",	
    "bin/" .. outputdir .. "/Duin",  -- Duin's output directory
}

global_defines =
{
	"DN_PLATFORM_WINDOWS",
    "DN_BUILD_STATIC",
}

global_links =
{
	"raylib.lib", 
	"rlImGui.lib",
	-- "winmm.lib",
    "flecs_static_debug.lib",
	"Duin.lib",
}

-- 



project "DuinEditor"
	location "DuinEditor"
	kind "ConsoleApp"
	language "C++"

    dependson { "Duin" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
	}

	includedirs(global_includedirs)
	includedirs
	{
	}

	libdirs(global_libdirs)
	libdirs
	{
	}

	defines(global_defines)
	defines
	{
	}
	
	links(global_links)
	links
	{
	}
	
	filter "system:windows"
		cppdialect "C++20"

	filter "configurations:Debug"
		defines "DN_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "DN_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "DN_DIST"
		optimize "On"

		


project "Sandbox"
	location "ExampleProjects/Sandbox"
	kind "ConsoleApp"
	language "C++"

    dependson { "Duin" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"ExampleProjects/%{prj.name}/src/**.h",
		"ExampleProjects/%{prj.name}/src/**.hpp",
		"ExampleProjects/%{prj.name}/src/**.cpp",
	}

	includedirs(global_includedirs)
	includedirs
	{
	}

	libdirs(global_libdirs)
	libdirs
	{
	}

	defines(global_defines)
	defines
	{
	}
	
	links(global_links)
	links
	{
	}
	
	filter "system:windows"
		cppdialect "C++20"
		-- staticruntime "On"

	filter "configurations:Debug"
		defines "DN_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "DN_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "DN_DIST"
		optimize "On"


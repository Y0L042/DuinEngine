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
	IncludeDir["raygui"] = "Duin/vendor/raygui/src"
	IncludeDir["imgui"] = "Duin/vendor/rlimgui/imgui-master"
	IncludeDir["flecs"] = "Duin/vendor/flecs/include"
	IncludeDir["fmt"] = "Duin/vendor/fmt/include"
	IncludeDir["patches"] = "Duin/vendor/patches"
	IncludeDir["rapidjson"] = "Duin/vendor/rapidjson/include"
	IncludeDir["imguifilex"] = "Duin/vendor/ImGuiFileDialog/"





project "Duin"
	location "Duin"
    kind "StaticLib"
	language "C++"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    pchheader "dnpch.h"
    pchsource "Duin/src/dnpch.cpp"

    -- Exclude precompiled headers for C files
    filter "files:**.c"
        flags { "NoPCH" }
        pchheader ""  -- Ensures .c files ignore PCH
    filter {} -- Clear the filter

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
	}

    externalincludedirs 
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
    }

	libdirs
	{
		"%{prj.name}/vendor/rlimgui/bin/Debug",
		"%{prj.name}/vendor/flecs/build_vs2022/build/Debug",	
	}

	links
	{
		"raylib.lib", 
		"rlImGui.lib",
        "flecs_static.lib",
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
        "flecs_STATIC",
	}
	
    -- Define DN_STATIC if building as a Static Library
    filter "kind:StaticLib"
        defines "DN_BUILD_STATIC"
        defines "flecs_STATIC"

    -- Define DN_BUILD_DLL if building as a Shared Library
    filter "kind:SharedLib"
        defines "DN_BUILD_DLL"

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/DuinEditor"),
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/DuinFPS"),
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
}

global_libdirs =
{
    "bin/" .. outputdir .. "/Duin",  -- Duin's output directory
	"Duin/vendor/rlimgui/bin/Debug",
    "Duin/vendor/flecs/build_vs2022/build/Debug",	
}

global_defines =
{
	"DN_PLATFORM_WINDOWS",
    "DN_BUILD_STATIC",
    "flecs_STATIC",
}

global_links =
{
	"raylib.lib", 
	"rlImGui.lib",
    "flecs_static.lib",
	"winmm.lib",
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
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
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
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
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


project "DuinFPS"
	location "ExampleProjects/DuinFPS"
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
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
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


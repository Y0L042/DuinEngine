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

    -- Include directories relative to the root folder (solution directory)
    IncludeDir = {}
    IncludeDir["spdlog"] = "Duin/vendor/spdlog/include"
    IncludeDir["raylib"] = "Duin/vendor/raylib5/include"
	IncludeDir["raylibsrc"] = "Duin/vendor/raylib5/src"
    IncludeDir["raylib_cpp"] = "Duin/vendor/raylib-cpp/include"
	IncludeDir["imgui"] = "Duin/vendor/imgui"
	IncludeDir["rlgui"] = "Duin/vendor/rlgui"
	IncludeDir["patches"] = "Duin/vendor/patches/include"
	IncludeDir["entt"] = "Duin/vendor/entt/single_include"

project "Duin"
	location "Duin"
	kind "SharedLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "dnpch.h"
	pchsource "Duin/src/dnpch.cpp"

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
	}

	includedirs
	{
        "%{prj.name}/src",
		"%{IncludeDir.patches}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.raylib}",
        "%{IncludeDir.raylibsrc}",
        "%{IncludeDir.raylib_cpp}",
        "%{IncludeDir.imgui}",
		"%{IncludeDir.rlgui}",
		"%{IncludeDir.entt}",
	}

	libdirs
	{
		"%{prj.name}/vendor/raylib5/lib",	
		"%{prj.name}/vendor/rlimgui/lib",
	}

	links
	{
		"raylib.lib", 
		"rlImGui.lib",
		"winmm.lib",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		-- systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS",
		"DN_BUILD_DLL",
		"IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
	}

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
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






project "Sandbox"
	location "ExampleProjects/Sandbox"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"ExampleProjects/%{prj.name}/src/**.h",
		"ExampleProjects/%{prj.name}/src/**.hpp",
		"ExampleProjects/%{prj.name}/src/**.cpp",
	}

	includedirs
	{
        "Duin/src",
		"%{IncludeDir.patches}",
        "%{IncludeDir.spdlog}",
        "%{IncludeDir.raylib}",
		"%{IncludeDir.raylibsrc}",
        "%{IncludeDir.raylib_cpp}",
        "%{IncludeDir.imgui}",
		"%{IncludeDir.rlgui}",
		"%{IncludeDir.entt}",
	}

	libdirs
	{
		"Duin/vendor/raylib5/lib",
		"Duin/vendor/rlimgui/lib",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "Off"
		-- systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS",
	}

	links
	{
		"raylib.lib", 
		"rlImGui.lib",
		"winmm.lib",
		"Duin",
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
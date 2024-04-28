workspace "Duin"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
		"%{prj.name}/vendor/spdlog/include",
		"%{prj.name}/vendor/sdl2/include",

	}

	libdirs
	{
		"%{prj.name}/vendor/sdl2/lib/x64",
	}

	links
	{
		"SDL2",
		"SDL2main", -- Add more SDL2 libraries as needed
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS",
		"DN_BUILD_DLL",
		"SDL_MAIN_HANDLED",
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
		"%{prj.name}/src",
		"Duin/vendor/spdlog/include",
		"Duin/vendor/sdl2/include",
		"Duin/src",
	}

	libdirs
	{
		"Duin/vendor/sdl2/lib/x64",
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS",
	}

	links
	{
		"Duin",
		"SDL2",
		"SDL2main", -- Add more SDL2 libraries as needed
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
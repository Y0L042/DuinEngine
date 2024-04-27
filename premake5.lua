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

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"%{prj.name}/vendor/spdlog/include"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS",
		"DN_BUILD_DLL"
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
		"ExampleProjects/%{prj.name}/src/**.cpp"
	}

	includedirs
	{
		"Duin/vendor/spdlog/include",
		"Duin/src"
	}

	filter "system:windows"
		cppdialect "C++20"
		staticruntime "On"
		systemversion "10.0"

	defines
	{
		"DN_PLATFORM_WINDOWS"
	}

	links
	{
		"Duin"
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
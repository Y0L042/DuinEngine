SolutionRoot = "../.."
ProjectRoot = "."

project "Sandbox"
    location ""
    kind "ConsoleApp"
    language "C++"

    externalanglebrackets "On"
    linkoptions { "-IGNORE:4006" }
    externalwarnings    "Off"

    dependson { "Duin" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
		"./src/**.h",
		"./src/**.hpp",
		"./src/**.c",
		"./src/**.cpp",
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
    }

    externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))
	externalincludedirs
	{
        SolutionRoot .. "/" .. IncludeDir["flecs_das"],
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))
    libdirs
    {
        SolutionRoot .. "/Duin/vendor/flecs-daslang/flecs_das/bin/Debug-windows-x86_64/flecs_das",
    }

    defines(global_defines)

    links(global_links)
    links
    {
        "flecs_das.lib",
    }

    filter { "files:**/external/**" }
        enablepch "Off"
        warnings "Off"
        pchheader ""
    filter {}

    filter { "files:**/vendor/**" }
        enablepch "Off"
        warnings "Off"
        pchheader ""
    filter {}

    filter "system:windows"
        buildoptions { "/openmp" }
        cppdialect "C++20"

    filter "action:vs*"
        buildoptions { 
            "/utf-8", 
            '/Zc:__cplusplus', 
            '/Zc:preprocessor' ,
        }  -- Changed: Added /utf-8 flag for Unicode support
        multiprocessorcompile "On"
    filter {}

    filter "configurations:Debug"
        defines "DN_DEBUG"
        symbols "On"

    filter "configurations:DebugCoverage"
        defines "DN_DEBUG"
        symbols "On"

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "DN_DIST"
        optimize "On"

    filter "configurations:Archive"
        defines "DN_ARCHIVE"
        symbols "On"

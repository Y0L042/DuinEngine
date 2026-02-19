SolutionRoot = ".."
ProjectRoot = "."

project "DuinTests"
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
        "./src/**.cpp",
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
    }

    LocalIncludeDir = {}
    externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))
	externalincludedirs
	{
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))
	libdirs
	{
	}

    defines(global_defines)

    links(global_links)
	links
	{
	}

    filter "system:windows"
        buildoptions { "/openmp" }
        cppdialect "C++20"

    filter "action:vs*"
        buildoptions { 
            "/utf-8", 
            '/Zc:__cplusplus', 
            '/Zc:preprocessor' ,
        }
        flags { "MultiProcessorCompile" }
    filter {}

    filter "configurations:Debug"
        defines "DN_DEBUG"
        symbols "On"
        -- Enable code coverage for Debug builds
        -- buildoptions { "/Z7" }  -- Full symbolic debug information
        -- linkoptions { "/PROFILE" }  -- Enable profiling/coverage
        -- flags { "NoIncrementalLink" }  -- Required for /PROFILE

    filter "configurations:DebugCoverage"
        defines "DN_DEBUG"
        symbols "On"
        -- Enable code coverage for Debug builds
        buildoptions { "/Z7" }  -- Full symbolic debug information
        linkoptions { "/PROFILE" }  -- Enable profiling/coverage
        flags { "NoIncrementalLink" }  -- Required for /PROFILE

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "DN_DIST"
        optimize "On"

    filter "configurations:Archive"
        defines "DN_ARCHIVE"
        symbols "On"

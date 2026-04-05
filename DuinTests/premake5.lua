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
        SolutionRoot .. "/" .. IncludeDir["daslang"],
        SolutionRoot .. "/Duin/vendor/daslang/src/builtin",
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))
	libdirs
	{
        SolutionRoot .. "/Duin/vendor/daslang/lib/Debug",
	}

    defines(global_defines)
    defines
    {
        "DN_ALLOW_ASSERT_FAILS",
        "DN_HEADLESS",
        "DN_TESTING",
        "DAS_SMART_PTR_DEBUG=1",
        "DAS_ENABLE_DYN_INCLUDES=1",
        "DAS_ENABLE_EXCEPTIONS=1",
    }

    links(global_links)
	links
	{
        "libDaScript.lib",
        "libUriParser.lib",
        "dbghelp.lib",
	}

    postbuildcommands
    {
        '{COPYDIR} "' .. path.getabsolute("scripts") .. '" "%{cfg.targetdir}/scripts"',
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

    -- Script test files include daScript headers that conflict with any PCH
    filter "files:**/Script/**"
        enablepch "Off"
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
        }
        multiprocessorcompile "On"
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
        incrementallink "Off"  -- Required for /PROFILE

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "DN_DIST"
        optimize "On"

    filter "configurations:Archive"
        defines "DN_ARCHIVE"
        symbols "On"

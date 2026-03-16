SolutionRoot = "../.."
ProjectRoot = "."
FPSSrc = "../DuinFPS/src"

project "DuinFPSTests"
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
        -- DuinFPS source files that contain tests (add more as needed):
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
        FPSSrc,  -- allows #include "EntityManager.h" etc.
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
    defines
    {
        "DN_ALLOW_ASSERT_FAILS",
        "DN_HEADLESS",
        "DN_TESTING",
    }

    links(global_links)
    links
    {
    }

    filter { "files:**/external/**" }
        flags { "NoPCH" }
        warnings "Off"
        pchheader ""
    filter {}

    filter { "files:**/vendor/**" }
        flags { "NoPCH" }
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
            '/Zc:preprocessor',
        }
        flags { "MultiProcessorCompile" }
    filter {}

    filter "configurations:Debug"
        defines "DN_DEBUG"
        symbols "On"

    filter "configurations:DebugCoverage"
        defines "DN_DEBUG"
        symbols "On"
        buildoptions { "/Z7" }
        linkoptions { "/PROFILE" }
        flags { "NoIncrementalLink" }

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"

    filter "configurations:Dist"
        defines "DN_DIST"
        optimize "On"

    filter "configurations:Archive"
        defines "DN_ARCHIVE"
        symbols "On"

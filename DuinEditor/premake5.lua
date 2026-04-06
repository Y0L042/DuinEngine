SolutionRoot = ".."
ProjectRoot = "."

project "DuinEditor"
    location ""
    kind "ConsoleApp"
    language "C++"

    externalanglebrackets "On"
    linkoptions { "-IGNORE:4006" }
    externalwarnings    "Off"

    dependson { "Duin" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    -- Shared app PCH
    pchheader "apppch.h"
    pchsource (SolutionRoot .. "/Duin/extern/apppch.cpp")
    forceincludes { "apppch.h" }

    -- Exclude .c files from PCH and force-include
    filter "files:**.c"
        enablepch "Off"
        pchheader ""
        forceincludes {}
    filter {}

    files
    {
        "./src/**.h",
        "./src/**.hpp",
        "./src/**.cpp",
        SolutionRoot .. "/Duin/extern/apppch.cpp",
        SolutionRoot .. "/Duin/extern/apppch.h",
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
        SolutionRoot .. "/Duin/extern",
    }

    LocalIncludeDir = {}
    LocalIncludeDir["nativefiledialog"] = "./vendor/nativefiledialog-extended/src/include"
    externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))
	externalincludedirs
	{
        "./vendor/nativefiledialog-extended/src/include",
        SolutionRoot .. "/" .. IncludeDir["flecs_das"],
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))
	libdirs
	{
        "./vendor/nativefiledialog-extended/build/src/Debug",
        SolutionRoot .. "/Duin/vendor/flecs-daslang/flecs_das/bin/Debug-windows-x86_64/flecs_das",
	}

    defines(global_defines)

    links(global_links)
	links
	{
        "flecs_das.lib",
        "nfd.lib",
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

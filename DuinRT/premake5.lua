SolutionRoot = ".."
ProjectRoot = "."

daslang_llvm_dll_src = path.getabsolute("Duin/vendor/daslang/lib/LLVM.dll")

project "DuinRT"
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

    postbuildcommands
    {
        '{COPYFILE} "' .. daslang_llvm_dll_src .. '" "%{cfg.targetdir}/../LLVM.dll"',
        '{COPYFILE} "' .. daslang_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn.dll"',
        '{COPYFILE} "' .. daslang_runtime_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn_runtime.dll"',
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

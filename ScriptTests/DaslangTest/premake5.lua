SolutionRoot = "../.."
ProjectRoot = "."

project "DaslangTest"
    location ""
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"

    externalanglebrackets "On"
    linkoptions { "-IGNORE:4006", "/STACK:8388608" }
    externalwarnings    "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    defines
    {
        "DAS_ENABLE_DYN_INCLUDES=1",
        "DAS_ENABLE_EXCEPTIONS=1",
        "DAS_SMART_PTR_DEBUG=1",
    }

    files
    {
        "./src/**.h",
        "./src/**.hpp",
        "./src/**.c",
        "./src/**.cpp",
    }

    includedirs
    {
        ProjectRoot .. "/src",
    }

    externalincludedirs
    {
        SolutionRoot .. "/" .. IncludeDir["daslang"],
    }

    libdirs
    {
        SolutionRoot .. "/Duin/vendor/daslang/lib/Debug",
    }

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

    filter "action:vs*"
        buildoptions {
            "/utf-8",
        }
        flags { "MultiProcessorCompile" }
    filter {}

    filter "configurations:Debug"
        defines "DN_DEBUG"
        symbols "On"
        staticruntime "On"
        runtime "Debug"

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"
        staticruntime "On"
        runtime "Release"

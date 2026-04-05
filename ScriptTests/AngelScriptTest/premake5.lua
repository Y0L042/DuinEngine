SolutionRoot = "../.."
ProjectRoot = "."

project "AngelScriptTest"
    location ""
    kind "ConsoleApp"
    language "C++"

    externalanglebrackets "On"
    linkoptions { "-IGNORE:4006" }
    externalwarnings    "Off"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files
    {
        "./src/**.h",
        "./src/**.hpp",
        "./src/**.c",
        "./src/**.cpp",
        -- AngelScript add-ons (compiled as part of this project)
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/add_on/scriptstdstring/scriptstdstring.cpp",
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/add_on/scriptstdstring/scriptstdstring_utils.cpp",
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/add_on/scriptarray/scriptarray.cpp",
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/add_on/scriptbuilder/scriptbuilder.cpp",
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/add_on/scriptmath/scriptmath.cpp",
    }

    includedirs
    {
        ProjectRoot .. "/src",
    }

    externalincludedirs
    {
        SolutionRoot .. "/" .. IncludeDir["angelscript"],
        SolutionRoot .. "/" .. IncludeDir["angelscript_addon"],
    }

    libdirs
    {
        SolutionRoot .. "/Duin/vendor/angelscript/sdk/angelscript/lib",
    }

    links
    {
        "angelscript64d.lib",
    }

    postbuildcommands
    {
        '{COPYDIR} "' .. path.getabsolute("scripts") .. '" "%{cfg.targetdir}/scripts"',
    }

    filter "system:windows"
        cppdialect "C++20"

    filter "action:vs*"
        buildoptions {
            "/utf-8",
            '/Zc:__cplusplus',
            '/Zc:preprocessor',
        }
        multiprocessorcompile "On"
    filter {}

    filter "configurations:Debug"
        defines "DN_DEBUG"
        symbols "On"
        runtime "Debug"

    filter "configurations:Release"
        defines "DN_RELEASE"
        optimize "On"
        runtime "Release"

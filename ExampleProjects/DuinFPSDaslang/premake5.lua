SolutionRoot = "../.."
ProjectRoot = "."

project "DuinFPSDaslang"
location ""
kind "ConsoleApp"
language "C++"

externalanglebrackets "On"
linkoptions { "-IGNORE:4006", "/STACK:8388608" }
externalwarnings "Off"

dependson { "Duin" }

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

-- Shared app PCH (daScript TUs excluded below)
pchheader "apppch.h"
pchsource(SolutionRoot .. "/Duin/extern/apppch.cpp")
forceincludes { "apppch.h" }

-- Exclude .c files from PCH and force-include
filter "files:**.c"
enablepch "Off"
pchheader ""
forceincludes {}
filter {}

-- daScript headers conflict with the shared PCH
filter "files:**/Script/**"
enablepch "Off"
pchheader ""
forceincludes {}
filter {}

defines(global_defines)
defines
{
    "DAS_ENABLE_EXCEPTIONS=1",
    "DAS_SMART_PTR_DEBUG=1",
}

files
{
    "./src/**.h",
    "./src/**.hpp",
    "./src/**.c",
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
    SolutionRoot .. "/" .. IncludeDir["daslang"],
    SolutionRoot .. "/" .. IncludeDir["flecs_das"],
}

libdirs(prependRoot(SolutionRoot, global_libdirs))

links(global_links)

postbuildcommands
{
    '{COPYDIR} "' .. path.getabsolute("scripts") .. '" "%{cfg.targetdir}/scripts"',
    '{COPYDIR} "' .. path.getabsolute("data") .. '" "%{cfg.targetdir}/data"',
}

filter "system:windows"
cppdialect "C++20"

filter "action:vs*"
buildoptions {
    "/utf-8",
    "/bigobj",
    '/Zc:__cplusplus',
    '/Zc:preprocessor',
}
multiprocessorcompile "On"
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

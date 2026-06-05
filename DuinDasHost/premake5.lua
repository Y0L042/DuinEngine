local Cfg = require "premakeCfg"
SolutionRoot = ".."
ProjectRoot = "."

project "DuinDasHost"
location ""
kind "ConsoleApp"
language "C++"

externalanglebrackets "On"
linkoptions { "-IGNORE:4006", "/STACK:8388608" }
externalwarnings "Off"

dependson { "Duin", "dasimgui", "flecs_das" }

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

pchheader "apppch.h"
pchsource(SolutionRoot .. "/Duin/extern/apppch.cpp")
forceincludes { "apppch.h" }

filter "files:**.c"
enablepch "Off"
pchheader ""
forceincludes {}
filter {}

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

-- libhv (dasHV's underlying C++ lib) — scoped to DuinDasHost only, for the WebSocket
-- daemon endpoint (M3). Only a Debug build is vendored, matching the project-wide
-- "Debug vendored binaries used in every config" convention (cf. daslang_dll_src).
local hvRoot = SolutionRoot .. "/Duin/vendor/daslang/modules/dasHV/hv/Debug"
externalincludedirs { hvRoot .. "/include" }

libdirs(prependRoot(SolutionRoot, global_libdirs))
libdirs { hvRoot .. "/lib" }

links(global_links)
links { "hv.lib" }

postbuildcommands
{
    '{COPYDIR} "' .. path.getabsolute("scripts") .. '" "%{cfg.targetdir}/scripts"',
    '{COPYFILE} "' .. daslang_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn.dll"',
    '{COPYFILE} "' .. daslang_runtime_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn_runtime.dll"',
    '{COPYFILE} "' .. path.getabsolute(hvRoot .. "/bin/hv.dll") .. '" "%{cfg.targetdir}/hv.dll"',
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
staticruntime(Cfg.premake_staticrt)
runtime "Debug"

filter "configurations:Release"
defines "DN_RELEASE"
optimize "On"
staticruntime(Cfg.premake_staticrt)
runtime "Release"

local Cfg = require "premakeCfg"
SolutionRoot = "../.."
ProjectRoot = "."

project "DaslangTest"
location ""
kind "ConsoleApp"
language "C++"

externalanglebrackets "On"
linkoptions { "-IGNORE:4006", "/STACK:8388608" }
externalwarnings "Off"

dependson { "Duin" }

targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

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
}

includedirs(prependRoot(SolutionRoot, global_includedirs))
includedirs
{
    ProjectRoot .. "/src",
}

externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))

libdirs(prependRoot(SolutionRoot, global_libdirs))

links(global_links)

postbuildcommands
{
    '{COPYDIR} "' .. path.getabsolute("scripts") .. '" "%{cfg.targetdir}/scripts"',
}

postbuildcommands
{
    '{COPYFILE} "' .. daslang_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn.dll"',
    '{COPYFILE} "' .. daslang_runtime_dll_src .. '" "%{cfg.targetdir}/libDaScriptDyn_runtime.dll"',
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

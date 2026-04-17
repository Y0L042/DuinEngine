local projectRoot = path.getabsolute(".")
package.path = package.path .. ";" .. projectRoot .. "/vendor/?.lua"
package.path = package.path .. ";" .. projectRoot .. "/../?.lua"

local Cfg = require "premakeCfg"

newoption {
    trigger     = "deps",
    description = "Process dependencies (update or rebuild based on dependency-specific flags)"
}

SolutionRoot = ".."
ProjectRoot = "."


project "Duin"
location ""
kind "StaticLib"
language "C++"

externalanglebrackets "On"
linkoptions { "-IGNORE:4006" }
externalwarnings "Off"


targetdir("bin/" .. outputdir .. "/%{prj.name}")
objdir("bin-int/" .. outputdir .. "/%{prj.name}")

pchheader "dnpch.h"
pchsource "./src/dnpch.cpp"

-- DNAssert.cpp must opt out of the PCH so that DOCTEST_CONFIG_IMPLEMENT
-- and DOCTEST_CONFIG_SUPER_FAST_ASSERTS are defined before doctest.h is
-- first included. MSVC ignores any #defines that precede #include "dnpch.h"
-- in a PCH-enabled translation unit.
filter "files:**/Core/Debug/DNAssert.cpp"
enablepch "Off"
pchheader ""
filter {}

-- Exclude precompiled headers for C files
filter "files:**.c"
enablepch "Off"
pchheader "" -- Ensures .c files ignore PCH
filter {}    -- Clear the filter

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

-- Script/ files include daScript headers that conflict with the PCH.
filter "files:**/Script/**"
enablepch "Off"
pchheader ""
filter {}


-- files(global_files)
files
{
    ProjectRoot .. "/src/**.h",
    ProjectRoot .. "/src/**.hpp",
    ProjectRoot .. "/src/**.c",
    ProjectRoot .. "/src/**.cpp",
    ProjectRoot .. "/src/**.inc",
}

removefiles
{
    ProjectRoot .. "/src/**/Physics/physx/**",
}

-- includedirs(global_includedirs)
includedirs
{
    ProjectRoot .. "/src",
}
externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))

libdirs(prependRoot(SolutionRoot, global_libdirs))

defines(global_defines)
defines
{
    "DN_HEADLESS",
    "DAS_ENABLE_EXCEPTIONS=1",
    "DAS_SMART_PTR_DEBUG=1",
    "DAS_ENABLE_DLL=1",
    "DAS_MOD_EXPORTS",
    "JPH_ENABLE_ASSERTS",
    "JPH_FLOATING_POINT_EXCEPTIONS_ENABLED "
}

-- global_links contains "Duin.lib" which is this project itself; static libs
-- must not link their own output. Consumers (executables) own the full link.
-- links(global_links)

filter "action:vs*"
buildoptions {
    "/utf-8",
    '/Zc:__cplusplus',
    '/Zc:preprocessor',
    '/bigobj'
} -- Changed: Added /utf-8 flag for Unicode support
multiprocessorcompile "On"
filter {}


filter "system:windows"
buildoptions { "/openmp" }
cppdialect "C++20"

filter "configurations:Debug"
defines { "DN_DEBUG", "_DEBUG" }
symbols "On"
staticruntime(Cfg.premake_staticrt)
runtime "Debug"
buildoptions { "/Gy" } -- Function-level linking: enables dead-stripping of unused functions
-- Enable code coverage for Debug builds
-- buildoptions { "/Z7" }  -- Full symbolic debug information
-- linkoptions { "/PROFILE" }  -- Enable profiling/coverage
-- flags { "NoIncrementalLink" }  -- Required for /PROFILE

filter "configurations:DebugCoverage"
defines { "DN_DEBUG", "_DEBUG" }
symbols "On"
staticruntime(Cfg.premake_staticrt)
runtime "Debug"
-- Enable code coverage for Debug builds
buildoptions { "/Z7" }     -- Full symbolic debug information
linkoptions { "/PROFILE" } -- Enable profiling/coverage
incrementallink "Off"      -- Required for /PROFILE

filter "configurations:Release"
defines { "DN_RELEASE", "NDEBUG" }
optimize "On"
staticruntime(Cfg.premake_staticrt)
runtime "Release"

filter "configurations:Dist"
defines "DN_DIST"
optimize "On"

-- include "vendor"

if _OPTIONS["deps"] then
    local vendorDeps = require "dependencies"
    local allDeps = vendorDeps.getDependencyNames()
    local flaggedDeps = {}

    for _, dep in ipairs(allDeps) do
        if _OPTIONS[dep] then
            table.insert(flaggedDeps, dep)
        end
    end

    local answer
    repeat
        if #flaggedDeps > 0 then
            io.write("This will fetch and rebuild the following dependencies:\n")
            for _, dep in ipairs(flaggedDeps) do
                io.write("  - " .. dep .. "\n")
            end
        else
            io.write("This will fetch and rebuild ALL dependencies:\n")
            for _, dep in ipairs(allDeps) do
                io.write("  - " .. dep .. "\n")
            end
            io.write("\nThis may take a long time.\n")
        end
        io.write("\nContinue with this operation (yes/n)? ")
        io.flush()
        answer = io.read()
    until answer == "yes" or answer == "n"

    if answer == "yes" then
        print("Operation continued.")
        vendorDeps.processDependencies()
    elseif answer == "n" then
        print("Operation aborted.")
    end
end

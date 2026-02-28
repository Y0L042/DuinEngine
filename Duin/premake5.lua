local projectRoot = path.getabsolute(".")
package.path = package.path .. ";" .. projectRoot .. "/vendor/?.lua"

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
    externalwarnings    "Off"


    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    pchheader "dnpch.h"
    pchsource "./src/dnpch.cpp"

    -- Exclude precompiled headers for C files
    filter "files:**.c"
        flags { "NoPCH" }
        pchheader ""  -- Ensures .c files ignore PCH
    filter {} -- Clear the filter

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


    -- files(global_files)
	files 
	{
		ProjectRoot .. "/src/**.h",
		ProjectRoot .. "/src/**.hpp",
		ProjectRoot .. "/src/**.c",
		ProjectRoot .. "/src/**.cpp",
	}

    -- includedirs(global_includedirs) 
    includedirs 
    { 
        ProjectRoot .. "/src",
    }
    -- externalincludedirs(global_externalincludedirs) 
    externalincludedirs 
    { 
        SolutionRoot .. "/%{IncludeDir.sdl}",
        SolutionRoot .. "/%{IncludeDir.bx}",
        SolutionRoot .. "/%{IncludeDir.bimg}",
        SolutionRoot .. "/%{IncludeDir.bgfx}",
        SolutionRoot .. "/%{IncludeDir.bgfx_examples}",
        SolutionRoot .. "/%{IncludeDir.bgfx_3p}",
        SolutionRoot .. "/%{IncludeDir.spdlog}",
		SolutionRoot .. "/%{IncludeDir.imgui}",
        SolutionRoot .. "/%{IncludeDir.imguizmo}",
		SolutionRoot .. "/%{IncludeDir.imguibackends}",
		SolutionRoot .. "/%{IncludeDir.imguifilex}",
		SolutionRoot .. "/%{IncludeDir.flecs}",
		SolutionRoot .. "/%{IncludeDir.fmt}",
		SolutionRoot .. "/%{IncludeDir.patches}",
        SolutionRoot .. "/%{IncludeDir.rapidjson}",
        SolutionRoot .. "/%{IncludeDir.toml11}",
		SolutionRoot .. "/%{IncludeDir.physx}",
        SolutionRoot .. "/%{IncludeDir.reflectcpp}",
    }
    -- libdirs(global_libdirs) 
    libdirs 
    { 
        ProjectRoot .. "/vendor/sdl/build/Debug",
        ProjectRoot .. "/vendor/bgfx/.build/win64_vs2022/bin",
		ProjectRoot .. "/vendor/flecs/build_vs2022/Debug",	
        ProjectRoot .. "/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
        ProjectRoot .. "/vendor/toml11/build/src/Debug",
        ProjectRoot .. "/vendor/reflectcpp/build/Debug",
    }
    -- defines(global_defines)
    defines 
    { 
        -- TODO
        "flecs_STATIC",
        "DN_BUILD_STATIC",
		"DN_PLATFORM_WINDOWS",
        "PX_PHYSX_STATIC_LIB",
        "BX_CONFIG_DEBUG=0",
		--"IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
    }
    -- links(global_links)
    links 
    { 
        "flecs_static.lib",
		"winmm.lib",
        "legacy_stdio_definitions.lib",
        "Setupapi.lib",
        "Version.lib",
        "Imm32.lib",
        "Cfgmgr32.lib",
        "SDL3-static.lib",
        "toml11.lib",
        "bxDebug.lib",
        "bimgDebug.lib",
        "bgfxDebug.lib",
        "PhysX_static_64.lib",
        "PhysXCooking_static_64.lib",
        "PhysXCommon_static_64.lib",
        "PhysXFoundation_static_64.lib",
        "PhysXPvdSDK_static_64.lib",
        "PhysXExtensions_static_64.lib",
        "PhysXCharacterKinematic_static_64.lib",
        "reflectcpp.lib",
    }

    filter "action:vs*"
        buildoptions { 
            "/utf-8", 
            '/Zc:__cplusplus', 
            '/Zc:preprocessor' ,
            '/bigobj'
        }  -- Changed: Added /utf-8 flag for Unicode support
        flags { "MultiProcessorCompile" }
    filter {}
      

    filter "system:windows"
        buildoptions { "/openmp" }
        cppdialect "C++20"

    filter "configurations:Debug"
        defines { "DN_DEBUG", "_DEBUG" }
        symbols "On"
        -- Enable code coverage for Debug builds
        -- buildoptions { "/Z7" }  -- Full symbolic debug information
        -- linkoptions { "/PROFILE" }  -- Enable profiling/coverage
        -- flags { "NoIncrementalLink" }  -- Required for /PROFILE

    filter "configurations:DebugCoverage"
        defines { "DN_DEBUG", "_DEBUG" }
        symbols "On"
        -- Enable code coverage for Debug builds
        buildoptions { "/Z7" }  -- Full symbolic debug information
        linkoptions { "/PROFILE" }  -- Enable profiling/coverage
        flags { "NoIncrementalLink" }  -- Required for /PROFILE

    filter "configurations:Release"
        defines { "DN_RELEASE", "NDEBUG" }
        optimize "On"

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
        answer=io.read()
    until answer=="yes" or answer=="n"

    if answer == "yes" then
        print("Operation continued.")
        vendorDeps.processDependencies()
    elseif answer == "n" then
        print("Operation aborted.")
    end
end

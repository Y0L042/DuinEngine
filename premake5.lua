archiveBaseDir = "build_archive"

function os.gettimestamp()
    return os.date('%Y%m%d_%H%M%S')
end



workspace "Duin"
	architecture "x64"
	startproject "Sandbox"	

	configurations
	{
		"Debug",
		"Release",
		"Dist",
        "Archive"
	}

	outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    staticruntime "On"

    -- Include directories relative to the root folder (solution directory)
    IncludeDir = {}
    IncludeDir["spdlog"] = "Duin/vendor/spdlog/include"
	IncludeDir["rlimgui"] = "Duin/vendor/rlimgui"
	IncludeDir["raylib"] = "Duin/vendor/rlimgui/raylib-master/src"
	IncludeDir["raygui"] = "Duin/vendor/raygui/src"
	IncludeDir["imgui"] = "Duin/vendor/rlimgui/imgui-docking"
	IncludeDir["imguibackends"] = "Duin/vendor/rlimgui/imgui-docking/backends"
    IncludeDir["glfw"] = "Duin/vendor/rlimgui/raylib-master/src/external/glfw/include"
	IncludeDir["flecs"] = "Duin/vendor/flecs/include"
	IncludeDir["fmt"] = "Duin/vendor/fmt/include"
	IncludeDir["patches"] = "Duin/vendor/patches"
	IncludeDir["rapidjson"] = "Duin/vendor/rapidjson/include"
	IncludeDir["imguifilex"] = "Duin/vendor/ImGuiFileDialog/"
    IncludeDir["nativefiledialog"] = "DuinEditor/vendor/nativefiledialog-extended/src/include"
	IncludeDir["physx"] = "Duin/vendor/PhysX/physx/include"





project "Duin"
	location "Duin"
    kind "StaticLib"
	language "C++"


	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")


    pchheader "dnpch.h"
    pchsource "Duin/src/dnpch.cpp"

    -- Exclude precompiled headers for C files
    filter "files:**.c"
        flags { "NoPCH" }
        pchheader ""  -- Ensures .c files ignore PCH
    filter {} -- Clear the filter


	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.c",
		"%{prj.name}/src/**.cpp",

        -- To enable multi-view with ImGui
        "%{IncludeDir.imguibackends}/imgui_impl_glfw.cpp",
        "%{IncludeDir.imguibackends}/imgui_impl_opengl3.cpp",
	}

    -- Exclude precompiled headers for imgui_impl_glfw.cpp
    filter "files:**/imgui_impl_glfw.cpp"
        flags { "NoPCH" }
        pchheader ""  -- Ensures this file ignores PCH

    -- Exclude precompiled headers for imgui_impl_opengl3.cpp
    filter "files:**/imgui_impl_opengl3.cpp"
        flags { "NoPCH" }
        pchheader ""  -- Ensures this file ignores PCH
    filter {} -- Clear the filter

	includedirs
	{
        "%{prj.name}/src",
	}

    externalincludedirs 
    {
        "%{IncludeDir.spdlog}",
		"%{IncludeDir.raylib}",
		"%{IncludeDir.raygui}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.imguibackends}",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.imguifilex}",
		"%{IncludeDir.rlimgui}",
		"%{IncludeDir.flecs}",
		"%{IncludeDir.fmt}",
		"%{IncludeDir.patches}",
        "%{IncludeDir.rapidjson}",
		"%{IncludeDir.physx}",
    }

	libdirs
	{
		"%{prj.name}/vendor/rlimgui/bin/Debug",
		"%{prj.name}/vendor/flecs/build_vs2022/build/Debug",	
        "%{prj.name}/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
	}

	links
	{
		"raylib.lib", 
		"rlImGui.lib",
        "flecs_static.lib",
		"winmm.lib",

        "PhysX_static_64.lib",
        "PhysXCooking_static_64.lib",
        "PhysXCommon_static_64.lib",
        "PhysXFoundation_static_64.lib",
        "PhysXPvdSDK_static_64.lib",
        "PhysXExtensions_static_64.lib",
        "PhysXCharacterKinematic_static_64.lib",

	}

	filter "system:windows"
		cppdialect "C++20"

	defines
	{
		"DN_PLATFORM_WINDOWS",
        "DN_BUILD_STATIC",
		"IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
		"SUPPORT_GIF_RECORDING",
        "flecs_STATIC",
        "PX_PHYSX_STATIC_LIB",
	}
	
    -- Define DN_STATIC if building as a Static Library
    filter "kind:StaticLib"
        defines "DN_BUILD_STATIC"
        defines "flecs_STATIC"

    -- Define DN_BUILD_DLL if building as a Shared Library
    filter "kind:SharedLib"
        defines "DN_BUILD_DLL"

   -- Enable multi-processor compilation
   filter "action:vs*"
      flags { "MultiProcessorCompile" }

	postbuildcommands
	{
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/DuinEditor"),
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox"),
		("{COPY} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/DuinFPS"),
	}
	
	filter "configurations:Debug"
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

        -- Construct the archive directory path
        archiveDir = archiveBaseDir .. "/%{cfg.buildcfg}/%{prj.name}/" .. os.gettimestamp()

        -- Post-build commands
        postbuildcommands
        {
            -- Create the archive directory
            "{MKDIR} " .. archiveDir,

            -- Copy build outputs to the archive directory
            "{COPY} %{cfg.buildtarget.relpath} " .. archiveDir,

            -- Retrieve the latest Git commit information
            "cmd /c Latest Commit: > " .. archiveDir .. "/latest_commit.txt",
            "git log -1 --pretty=format:\"%H%n%an%n%ad%n%s\" >> " .. archiveDir .. "/latest_commit.txt",

            -- Retrieve the last 50 Git commits
            "cmd /c Last 50 Commits: > " .. archiveDir .. "/last_50_commits.txt",
            "git log -n 50 --pretty=format:\"%H%n%an%n%ad%n%s\" >> " .. archiveDir .. "/last_50_commits.txt"
        }

-- MODIFY THESE WHEN ADDING LIBS AND NEW PROJECTS


global_includedirs = 
{
	"Duin/src",
}

global_externalincludedirs =
{
    "%{IncludeDir.spdlog}",
    "%{IncludeDir.raylib}",
    "%{IncludeDir.raygui}",
    "%{IncludeDir.imgui}",
    "%{IncludeDir.imguifilex}",
    "%{IncludeDir.rlimgui}",
    "%{IncludeDir.flecs}",
    "%{IncludeDir.fmt}",
    "%{IncludeDir.patches}",
    "%{IncludeDir.rapidjson}",
    "%{IncludeDir.physx}",
}

global_libdirs =
{
    "bin/" .. outputdir .. "/Duin",  -- Duin's output directory
	"Duin/vendor/rlimgui/bin/Debug",
    "Duin/vendor/flecs/build_vs2022/build/Debug",	
    "Duin/vendor/PhysX/physx/bin/win.x86_64.vc143.mt/debug",
}

global_defines =
{
	"DN_PLATFORM_WINDOWS",
    "DN_BUILD_STATIC",
    "flecs_STATIC",
    "IMGUI_IMPL_OPENGL_LOADER_GLAD", --necessary?
    "PX_PHYSX_STATIC_LIB",
}

global_links =
{
	"raylib.lib", 
	"rlImGui.lib",
    "flecs_static.lib",
	"winmm.lib",
	"Duin.lib",
    "PhysX_static_64.lib",
    "PhysXCooking_static_64.lib",
    "PhysXCommon_static_64.lib",
    "PhysXFoundation_static_64.lib",
    "PhysXPvdSDK_static_64.lib",
    "PhysXExtensions_static_64.lib",
    "PhysXCharacterKinematic_static_64.lib",
}

-- 



project "DuinEditor"
	location "DuinEditor"
	kind "ConsoleApp"
	language "C++"

    dependson { "Duin" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.hpp",
		"%{prj.name}/src/**.cpp",
	}

	includedirs(global_includedirs)
	includedirs
	{
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
	{
        "%{IncludeDir.nativefiledialog}",
	}

	libdirs(global_libdirs)
	libdirs
	{
        "%{prj.name}/vendor/nativefiledialog-extended/build/src/Release",
	}

	defines(global_defines)
	defines
	{
	}
	
	links(global_links)
	links
	{
        "nfd.lib",
	}
	
	filter "system:windows"
		cppdialect "C++20"

	filter "configurations:Debug"
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
		
   -- Enable multi-processor compilation
   filter "action:vs*"
      flags { "MultiProcessorCompile" }


project "Sandbox"
	location "ExampleProjects/Sandbox"
	kind "ConsoleApp"
	language "C++"

    dependson { "Duin" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"ExampleProjects/%{prj.name}/src/**.h",
		"ExampleProjects/%{prj.name}/src/**.hpp",
		"ExampleProjects/%{prj.name}/src/**.cpp",
	}

	includedirs(global_includedirs)
	includedirs
	{
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
	{
	}

	libdirs(global_libdirs)
	libdirs
	{
	}

	defines(global_defines)
	defines
	{
	}
	
	links(global_links)
	links
	{
	}
	
	filter "system:windows"
		cppdialect "C++20"
		-- staticruntime "On"

	filter "configurations:Debug"
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

   -- Enable multi-processor compilation
   filter "action:vs*"
      flags { "MultiProcessorCompile" }



project "DuinFPS"
	location "ExampleProjects/DuinFPS"
	kind "ConsoleApp"
	language "C++"

    dependson { "Duin" }

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"ExampleProjects/%{prj.name}/src/**.h",
		"ExampleProjects/%{prj.name}/src/**.hpp",
		"ExampleProjects/%{prj.name}/src/**.cpp",
	}

	includedirs(global_includedirs)
	includedirs
	{
        "%{prj.name}/src",
	}

	externalincludedirs(global_externalincludedirs)
	externalincludedirs
	{
	}

	libdirs(global_libdirs)
	libdirs
	{
	}

	defines(global_defines)
	defines
	{
	}
	
	links(global_links)
	links
	{
	}
	
	filter "system:windows"
		cppdialect "C++20"

	filter "configurations:Debug"
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

   -- Enable multi-processor compilation
   filter "action:vs*"
      flags { "MultiProcessorCompile" }



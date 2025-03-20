SolutionRoot = ".."
ProjectRoot = "."

project "DuinEditor"
    location ""
    kind "ConsoleApp"
    language "C++"

    dependson { "Duin" }

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files 
    {
        "./src/**.h",
        "./src/**.hpp",
        "./src/**.cpp",
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
    }

    LocalIncludeDir = {}
    LocalIncludeDir["nativefiledialog"] = "./vendor/nativefiledialog-extended/src/include"
    externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))
	externalincludedirs
	{
        "%{LocalIncludeDir.nativefiledialog}",
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))
	libdirs
	{
        "./vendor/nativefiledialog-extended/build/src/Release",
	}

    defines(global_defines)

    links(global_links)
	links
	{
        "nfd.lib",
	}

    filter "system:windows"
        buildoptions { "/openmp" }
        cppdialect "C++20"

    filter "action:vs*"
        buildoptions { "/utf-8" }  -- Changed: Added /utf-8 flag for Unicode support

    -- Enable multi-processor compilation
    filter "action:vs*"
        flags { "MultiProcessorCompile" }

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

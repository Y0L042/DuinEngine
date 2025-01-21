SolutionRoot = "../.."
ProjectRoot = "."

project "DuinFPS"
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
		"./src/**.c",
		"./src/**.cpp",
    }

    includedirs(prependRoot(SolutionRoot, global_includedirs))
    includedirs
    {
        ProjectRoot .. "/src",
    }

    externalincludedirs(prependRoot(SolutionRoot, global_externalincludedirs))
	externalincludedirs
	{
	}

    libdirs(prependRoot(SolutionRoot, global_libdirs))

    defines(global_defines)

    links(global_links)

    filter "system:windows"
        cppdialect "C++20"

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

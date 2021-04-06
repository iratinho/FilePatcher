-- Workspace definition
workspace "FilePatcher"
    configurations { "Debug", "Release" }
    platforms { "Win32", "Win64" }

    outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

    filter "configurations:Debug"
        defines { "DEBUG" }
        runtime "Debug"
        flags   { "symbols" }

    filter "configurations:Release"
        defines { "NODEBUG" }
        runtime "Release"
        symbols "On"

    filter "platforms:Win32"
        system "Windows"
        architecture "x86"
        defines { "WINDOWS_PLATFORM" }

    filter "platforms:Win32"
        system "Windows"
        architecture "x86_64"
        defines { "WINDOWS_PLATFORM" }

-- Engine Module
project "FilePatcher"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    location "Intermediate/ProjectFiles"

	targetdir ("%{wks.location}/Binaries/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/Intermediate/" .. outputdir .. "/%{prj.name}")

    includedirs { "" }

    files {
        "**.cpp",
        "**.h"
    }



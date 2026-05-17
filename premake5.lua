-- premake5.lua
workspace "sead"
    configurations { "Debug", "Release", "Dist" }
    platforms { "Win32", "Win64", "macOS", "LinuxARM64", "Linux64" }
	
	-- toolset "clang"
	staticruntime "on"

    startproject "Sandbox"
	
	buildoptions {
	-- suppressed errors
		"-Wno-invalid-offsetof",
		"-Wno-undefined-var-template",
		"-Wno-missing-braces",
	-- keep, but as warnings
		"-Wno-error=switch",
		"-Wno-error=unused-private-field",
		"-Wno-error=unused-const-variable",
		"-Wno-error=logical-op-parentheses",
		"-Wno-error=bitwise-op-parentheses",
	}

    filter { "platforms:Win32" }
        system "Windows"
        architecture "x86"
    
    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

	filter { "platforms:macOS" }
        system "macosx"
        architecture "ARM64"
		toolset "clang"
		
	filter { "platforms:LinuxARM64" }
        system "Linux"
        architecture "ARM64"
        toolset "clang"

	filter { "platforms:Linux64" }
        system "Linux"
        architecture "x86_64"
        toolset "clang"

include "sead"
include "Sandbox"

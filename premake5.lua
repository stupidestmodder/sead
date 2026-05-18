-- premake5.lua
workspace "sead"
	platforms { "x86", "x86_64", "ARM64" }
	configurations { "Debug", "Release", "Dist" }
	
	toolset "clang"
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
	
	filter "platforms:x86"
		architecture "x86"
		stl "gnu"

	filter "platforms:x86_64"
		architecture "x86_64"
		vectorextensions "AVX2"

	filter "platforms:ARM64"
		architecture "ARM64"

include "sead"
include "Sandbox"

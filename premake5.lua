-- premake5.lua
workspace "sead"
    configurations { "Debug", "Release", "Dist" }
    platforms { "Win32", "Win64" }

    startproject "Sandbox"

    filter { "platforms:Win32" }
        system "Windows"
        architecture "x86"
    
    filter { "platforms:Win64" }
        system "Windows"
        architecture "x86_64"

include "sead"
include "Sandbox"

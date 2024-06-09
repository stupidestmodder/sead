-- premake5.lua
workspace "sead"
    architecture "x86" -- TODO: x64
    configurations { "Debug", "Release", "Dist" }
    startproject "Sandbox"

include "Sandbox"
include "sead"

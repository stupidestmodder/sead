project "sead"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "off"
    systemversion "latest"

    exceptionhandling "Off"
    rtti "Off"

    targetdir ("bin/%{prj.name}-%{cfg.buildcfg}/out")
    objdir ("bin/%{prj.name}-%{cfg.buildcfg}/int")

    links {
        "glad"
    }

    includedirs {
        "include",

        "vendor/glad/include",
    }

    files {
        "src/**.cpp"
    }

    removefiles {
        "src/**Win.cpp"
    }

    flags {
        "MultiProcessorCompile",
        "ShadowedVariables",
        "FatalWarnings"
    }

    filter "system:windows"
        defines "SEAD_PLATFORM_WINDOWS"
        files {
            "src/**Win.cpp"
        }
        links "winmm.lib"

    filter "configurations:Debug"
        defines { "SEAD_DEBUG" }
        runtime "Debug"
        symbols "on"
        optimize "off"

    filter "configurations:Release"
        defines { "SEAD_DEBUG" }
        runtime "Release"
        symbols "on"
        optimize "speed"
        flags { "LinkTimeOptimization" }

    filter "configurations:Dist"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "off"
        optimize "speed"
        flags { "LinkTimeOptimization" }

group "Dependencies"
    include "vendor/glad"
group ""

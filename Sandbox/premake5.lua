project "Sandbox"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    exceptionhandling "off"
    rtti "off"

    targetdir "build/lib"
    objdir "build/bin"
    targetname "%{prj.name}-%{cfg.buildcfg}-%{cfg.architecture}"
    debugdir "../workdir"

    links {
        "sead",
        "glad",
        "SDL3"
    }

    includedirs {
        "include",

        "../sead/include",
        "../sead/vendor/glad/include",
        "../sead/vendor/SDL3/repo/include",
    }
    
    libdirs {
        "../sead/vendor/SDL3/lib",
    }

    files {
        "src/**.cpp",
    }

    flags {
        "MultiProcessorCompile",
        "ShadowedVariables",
        "FatalWarnings"
    }

    filter "system:windows"
        defines {
            "SEAD_PLATFORM_WINDOWS",
            "SEAD_USE_GL"
        }

    filter "system:macosx"
        systemversion "11.0"
        defines {
            "SEAD_PLATFORM_SDL",
            "SEAD_USE_GL"
        }
    
    filter "system:linux"
        defines {
            "SEAD_PLATFORM_SDL",
            "SEAD_USE_GL"
        }
    
    filter "configurations:Debug"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "Debug"
        symbols "on"
        optimize "debug"

    filter "configurations:Release"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "Release"
        symbols "on"
        optimize "speed"

    filter "configurations:Dist"
        defines { "NDEBUG" }
        runtime "Release"
        symbols "off"
        optimize "speed"
        flags { "LinkTimeOptimization" }

group "Dependencies"
    include "../sead"
group ""

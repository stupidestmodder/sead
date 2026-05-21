-- premake5.lua
project "sead"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    multiprocessorcompile "on"
    staticruntime "on"
    exceptionhandling "off"
    rtti "off"
    fatalwarnings { "all" }

    targetdir "bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/out"
    objdir "bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/int"

    includedirs {
        "include",
    }

    files {
        "src/**.cpp",
    }

    removefiles {
        "src/**sdl/**",
        "src/**win/**",
        "src/**posix/**",

        "src/**gl/**",
    }

    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
        systemversion "latest"

        files {
            "src/prim/win/seadMemUtilWin.cpp",
        }

    filter "system:linux"
        systemversion "latest"

        files {
            "src/prim/posix/seadMemUtilPosix.cpp",
        }

    filter "system:macosx"
        systemversion "11.0"

        files {
            "src/prim/posix/seadMemUtilPosix.cpp",
        }

    filter "platforms:SDL_*"
        defines {
            "SEAD_PLATFORM_SDL",
            "SEAD_USE_GL",
        }

        includedirs {
            "libs/glad/include",
            "libs/SDL3/repo/include",
        }

        files {
            "src/**sdl/**",

            "src/**gl/**",
        }

        links {
            -- "glad",
            "SDL3",
        }

    filter "platforms:Win_*"
        defines {
            "SEAD_PLATFORM_WINDOWS",
            "SEAD_USE_GL",
        }

        includedirs {
            "libs/glad/include",
        }

        files {
            "src/**win/**",

            "src/**gl/**",
        }

        links {
            "Winmm.lib",
            "Ws2_32.lib",
            "opengl32.lib",
            -- "glad",
        }

    filter "configurations:Debug"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "debug"
        optimize "debug"
        symbols "on"
        linktimeoptimization "off"

    filter "configurations:Develop"
        defines { "SEAD_TARGET_DEBUG" } -- TODO: Use SEAD_TARGET_DEVELOP
        runtime "release"
        optimize "speed"
        symbols "on"
        linktimeoptimization "off"

    filter "configurations:Release"
        defines { "SEAD_TARGET_RELEASE", "NDEBUG" }
        runtime "release"
        optimize "speed"
        symbols "off"
        linktimeoptimization "on"

include "libs/SDL3"

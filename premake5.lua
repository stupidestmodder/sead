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
        "src/**",
    }

    removefiles {
        "src/**posix/**",
        "src/**win/**",
        
        "src/**gl/**",

        "src/**sdl/**",
        "src/**glfw/**",
    }

    defines { "_CRT_SECURE_NO_WARNINGS" }

    filter "system:windows"
        systemversion "latest"

        -- TODO: Remove
        defines { "SEAD_PLATFORM_WINDOWS" }
        files {
            "src/**win/**",
        }
        links {
            "Winmm.lib",
            "Ws2_32.lib",
            "opengl32.lib",
            -- "glad",
        }

    filter "system:linux"
        systemversion "latest"
        
        -- TODO: Remove
        defines { "SEAD_PLATFORM_POSIX", "SEAD_PLATFORM_LINUX" }
        files {
            "src/**posix/**",
        }

    filter "system:macosx"
        systemversion "11.0"

        -- TODO: Remove
        defines { "SEAD_PLATFORM_POSIX", "SEAD_PLATFORM_MACOSX" }
        files {
            "src/**posix/**",
        }

    filter "platforms:GLFW_*"
        defines {
            "SEAD_PLATFORM_GLFW",
            "SEAD_USE_GL",
        }

        includedirs {
            "libs/glad/include",
            "libs/glfw/include",
        }

        files {
            "src/**glfw/**",

            "src/**gl/**",
        }

        removefiles {
            "src/framework/win/**",
        }

        links {
            -- "glad",
            "glfw",
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

project "glfw"
    kind "StaticLib"
    language "C"

    multiprocessorcompile "on"
    staticruntime "on"
    warnings "off"

    targetdir "bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/out"
    objdir "bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/int"
    
    files {
        "libs/glfw/include/GLFW/glfw3.h",
        "libs/glfw/include/GLFW/glfw3native.h",
        "libs/glfw/src/glfw_config.h",
        "libs/glfw/src/context.c",
        "libs/glfw/src/init.c",
        "libs/glfw/src/input.c",
        "libs/glfw/src/monitor.c",
        "libs/glfw/src/null_init.c",
        "libs/glfw/src/null_joystick.c",
        "libs/glfw/src/null_monitor.c",
        "libs/glfw/src/null_window.c",
        "libs/glfw/src/platform.c",
        "libs/glfw/src/vulkan.c",
        "libs/glfw/src/window.c"
    }
    
    filter "system:windows"
        files {
            "libs/glfw/src/win32_init.c",
            "libs/glfw/src/win32_joystick.c",
            "libs/glfw/src/win32_module.c",
            "libs/glfw/src/win32_monitor.c",
            "libs/glfw/src/win32_time.c",
            "libs/glfw/src/win32_thread.c",
            "libs/glfw/src/win32_window.c",
            "libs/glfw/src/wgl_context.c",
            "libs/glfw/src/egl_context.c",
            "libs/glfw/src/osmesa_context.c"
        }
    
        defines {
            "_GLFW_WIN32"
        }
    
    filter "system:linux"
        files {
            "libs/glfw/src/x11_init.c",
            "libs/glfw/src/x11_monitor.c",
            "libs/glfw/src/x11_window.c",
            "libs/glfw/src/xkb_unicode.c",
            "libs/glfw/src/posix_module.c",
            "libs/glfw/src/posix_time.c",
            "libs/glfw/src/posix_thread.c",
            "libs/glfw/src/posix_poll.c",
            "libs/glfw/src/glx_context.c",
            "libs/glfw/src/egl_context.c",
            "libs/glfw/src/osmesa_context.c",
            "libs/glfw/src/linux_joystick.c"
        }
    
        defines {
            "_GLFW_X11"
        }
    
    filter "system:macosx"
        files {
            "libs/glfw/src/cocoa_init.m",
            "libs/glfw/src/cocoa_monitor.m",
            "libs/glfw/src/cocoa_window.m",
            "libs/glfw/src/cocoa_joystick.m",
            "libs/glfw/src/cocoa_time.c",
            "libs/glfw/src/nsgl_context.m",
            "libs/glfw/src/posix_thread.c",
            "libs/glfw/src/posix_module.c",
            "libs/glfw/src/osmesa_context.c",
            "libs/glfw/src/egl_context.c"
        }
    
        defines {
            "_GLFW_COCOA"
        }

project "SDL3"
    kind "StaticLib"
    language "C"
    cdialect "C17"
    staticruntime "on"

    flags {
        "NoRuntimeChecks",
        "NoBufferSecurityCheck"
    }

    vectorextensions "AVX2"

    targetdir "build/lib"
    objdir "build/bin"
    targetname "%{prj.name}-%{cfg.buildcfg}-%{cfg.architecture}"

    includedirs {
        "repo/include",
        "repo/include/build_config",
        "repo/src"
    }

    defines {
        "SDL_STATIC",
        
        "SDL_AUDIO_DRIVER_DUMMY",
        "SDL_AUDIO_DRIVER_DISK",
        "SDL_CAMERA_DRIVER_DUMMY",
        "SDL_DIALOG_DUMMY",
    }

    files {
        "repo/src/atomic/*.c",
        "repo/src/audio/disk/*.c",
        "repo/src/audio/dummy/*.c",
        "repo/src/audio/*.c",
        "repo/src/camera/*.c",
        "repo/src/camera/dummy/*.c",
        "repo/src/cpuinfo/*.c",
        "repo/src/dialog/*.c",
        "repo/src/dialog/dummy/*.c",
        "repo/src/dynapi/*.c",
        "repo/src/events/*.c",
        "repo/src/filesystem/*.c",
        "repo/src/filesystem/dummy/*.c",
        "repo/src/gpu/*.c",
        "repo/src/haptic/*.c",
        "repo/src/haptic/dummy/*.c",
        -- TODO: hidapi ??
        "repo/src/io/*.c",
        "repo/src/joystick/*.c",
        "repo/src/joystick/dummy/*.c",
        "repo/src/libm/*.c",
        "repo/src/loadso/dummy/*.c",
        "repo/src/locale/*.c",
        "repo/src/locale/dummy/*.c",
        "repo/src/main/*.c",
        "repo/src/misc/*.c",
        "repo/src/misc/dummy/*.c",
        "repo/src/power/*.c",
        "repo/src/process/*.c",
        "repo/src/process/dummy/*.c",
        "repo/src/render/*.c",
        "repo/src/render/software/*.c",
        "repo/src/sensor/*.c",
        "repo/src/sensor/dummy/*.c",
        "repo/src/stdlib/*.c",
        "repo/src/storage/*.c",
        "repo/src/thread/*.c",
        "repo/src/time/*.c",
        "repo/src/timer/*.c",
        "repo/src/tray/*.c",
        "repo/src/tray/dummy/*.c",
        "repo/src/video/dummy/*.c",
        "repo/src/video/*.c",
        "repo/src/*.c",
        "repo/src/video/yuv2rgb/*.c",
    }
    
    filter "system:windows"
        systemversion "latest"
    
        files {
            "repo/src/audio/directsound/*.c",
            "repo/src/audio/winmm/*.c",
            "repo/src/audio/wasapi/*.c",
            "repo/src/core/windows/*.c",
            "repo/src/dialog/windows/*.c",
            "repo/src/filesystem/windows/*.c",
            "repo/src/haptic/windows/*.c",
            "repo/src/joystick/windows/*.c",
            "repo/src/hidapi/windows/*.c",
            "repo/src/loadso/windows/*.c",
            "repo/src/power/windows/*.c",
            "repo/src/render/direct3d11/*.c",
            "repo/src/render/direct3d/*.c",
            "repo/src/video/windows/*.c",
        }
        
        links {
            "setupapi",
            "winmm",
            "imm32",
            "version",
        }
        
        defines {
            "SDL_PLATFORM_WINDOWS"
        }
    
    filter "system:linux"
        systemversion "latest"
        
        files {
            "repo/src/audio/alsa/*.c",
            "repo/src/audio/disk/*.c",
            "repo/src/audio/jack/*.c",
            "repo/src/audio/pipewire/*.c",
            "repo/src/audio/pulseaudio/*.c",
            "repo/src/camera/pipewire/*.c",
            "repo/src/camera/v4l2/*.c",
            "repo/src/core/linux/*.c",
            "repo/src/dialog/unix/*.c",
            "repo/src/filesystem/unix/*.c",
            "repo/src/gpu/vulkan/*.c",
            "repo/src/haptic/linux/*.c",
            "repo/src/hidapi/linux/*.c",
            "repo/src/joystick/linux/*.c",
            "repo/src/loadso/dlopen/*.c",
            "repo/src/locale/unix/*.c",
            "repo/src/misc/unix/*.c",
            "repo/src/power/linux/*.c",
            "repo/src/process/posix/*.c",
            "repo/src/render/gpu/*.c",
            "repo/src/render/opengl/*.c",
            "repo/src/render/opengles2/*.c",
            "repo/src/render/vulkan/*.c",
            "repo/src/thread/pthread/*.c",
            "repo/src/time/unix/*.c",
            "repo/src/timer/unix/*.c",
            "repo/src/tray/unix/*.c",
            "repo/src/video/kmsdrm/*.c",
            "repo/src/video/offscreen/*.c",
            "repo/src/video/wayland/*.c",
            "repo/src/video/x11/*.c",
        }
        
        links {
            
        }
        
        defines {
            "SDL_PLATFORM_LINUX",
            
            "SDL_AUDIO_DRIVER_ALSA_DYNAMIC",
            "SDL_AUDIO_DRIVER_DISK",
            "SDL_AUDIO_DRIVER_JACK_DYNAMIC",
            "SDL_AUDIO_DRIVER_PIPEWIRE_DYNAMIC",
            "SDL_AUDIO_DRIVER_PULSEAUDIO_DYNAMIC",
            
            "SDL_CAMERA_DRIVER_PIPEWIRE",
            "SDL_CAMERA_DRIVER_V4L2",
            
            "SDL_VIDEO_DRIVER_KMSDRM_DYNAMIC",
            "SDL_VIDEO_DRIVER_OFFSCREEN",
            "SDL_VIDEO_DRIVER_X11_DYNAMIC",
            "SDL_VIDEO_DRIVER_WAYLAND_DYNAMIC",
            
            "HAVE_STDIO_H",
            "HAVE_STDLIB_H",
            "HAVE_STDDEF_H",
            "HAVE_STDARG_H",
            "HAVE_STRING_H",
            "HAVE_STRINGS_H",
            "HAVE_WCHAR_H",
            "HAVE_MATH_H",
            "HAVE_FLOAT_H",
            "HAVE_LIBC"
        }

    filter "configurations:Debug"
        defines {
            "_DEBUG"
        }
        
        runtime "debug"
        symbols "on"
        optimize "debug"
    
    filter "configurations:Release"
        defines {
            "NDEBUG"
        }
        
        runtime "release"
        optimize "speed"
        symbols "on"

    filter "configurations:Dist"
        defines {
            "NDEBUG"
        }
        
        runtime "release"
        symbols "off"
        optimize "speed"

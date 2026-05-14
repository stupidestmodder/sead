project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"
    systemversion "latest"

    targetdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/out")
    objdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/int")

    includedirs {
        "include"
    }

    files {
        "src/gl.c"
    }

    flags {
        "MultiProcessorCompile"
    }

    filter "system:windows"
        systemversion "latest"
        
        links {
        "opengl32.lib"
        }

    filter "system:macosx"
        systemversion "11.0"
        
        links {
        "OpenGL.framework"
        }

    filter "configurations:Debug"
        defines "DEBUG"
        runtime "Debug"
        symbols "on"
        optimize "off"

    filter "configurations:Release"
        defines "NDEBUG"
        runtime "Release"
        symbols "on"
        optimize "speed"
        flags { "LinkTimeOptimization" }

    filter "configurations:Dist"
        defines "NDEBUG"
        runtime "Release"
        symbols "off"
        optimize "speed"
        flags { "LinkTimeOptimization" }

project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir "build/lib"
    objdir "build/bin"
    targetname "%{prj.name}-%{cfg.buildcfg}-%{cfg.architecture}"

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
    
    filter "system:linux"
        systemversion "latest"

    filter "configurations:Debug"
        runtime "Debug"
        symbols "on"
        optimize "debug"

    filter "configurations:Release"
        runtime "Release"
        symbols "on"
        optimize "speed"

    filter "configurations:Dist"
        defines "NDEBUG"
        runtime "Release"
        symbols "off"
        optimize "speed"
        flags { "LinkTimeOptimization" }

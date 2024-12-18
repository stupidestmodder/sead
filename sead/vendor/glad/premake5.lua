project "glad"
    kind "StaticLib"
    language "C"
    staticruntime "off"
    systemversion "latest"

    targetdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/out")
    objdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/int")

    links {
        "opengl32.lib"
    }

    includedirs {
        "include"
    }

    files {
        "src/**.c"
    }

    flags {
        "MultiProcessorCompile"
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

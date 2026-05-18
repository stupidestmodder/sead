project "sead"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    exceptionhandling "off"
    rtti "off"

    targetdir "build/lib"
    objdir "build/bin"
    targetname "%{prj.name}-%{cfg.buildcfg}-%{cfg.architecture}"

    links {
        "glad"
    }

    includedirs {
        "include",

        "vendor/glad/include",
        "vendor/SDL3/repo/include",
    }
    
    links {
        "SDL3"
    }

    files {
        "src/**.cpp"
    }

    -- TODO: Just exclude backends in the main project and include them per-platform
    removefiles {
        "src/**Win.cpp",
       -- "src/**seadHostIO**.cpp",
        "src/**WinGL.cpp"
    }

    flags {
        "MultiProcessorCompile",
        "ShadowedVariables",
        "FatalCompileWarnings"
    }

	filter { "toolset:clang" }
		buildoptions {
		-- suppressed errors
			"-Wno-invalid-offsetof",
			"-Wno-undefined-var-template",
			"-Wno-missing-braces",
		-- keep, but as warnings
			"-Wno-error=switch",
			"-Wno-error=unused-private-field",
			"-Wno-error=unused-const-variable",
			"-Wno-error=logical-op-parentheses",
			"-Wno-error=bitwise-op-parentheses",
		}

    filter "system:windows"
        systemversion "latest"
    
        defines {
            "SEAD_PLATFORM_WINDOWS",
            "SEAD_USE_GL"
        }
        
        files {
            "src/**Win.cpp"
        }
        
        links {
            "Winmm.lib",
            "Ws2_32.lib"
        }

    filter "system:macosx"    
        systemversion "11.0"

        defines {
            "SEAD_PLATFORM_SDL",
            "SEAD_USE_GL"
        }

        files {
            "src/**SDL.cpp"
        }

        links {
            "OpenGL.framework"
        }
        
    filter "system:linux"
        systemversion "latest"
    
        defines {
            "SEAD_PLATFORM_SDL",
            "SEAD_USE_GL"
        }

        files {
            "src/**SDL.cpp"
        }

        links {
            "GL",
            "dl",
            "pthread"
        }

    filter "configurations:Debug"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "Debug"
        symbols "on"
        optimize "debug"
       -- sanitize "address"

    filter "configurations:Release"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "Release"
        symbols "on"
        optimize "speed"

    filter "configurations:Dist"
        defines { "SEAD_TARGET_DEBUG", "NDEBUG" }
        runtime "Release"
        symbols "off"
        optimize "speed"
        flags { "LinkTimeOptimization" }

group "Dependencies"
    include "vendor/glad"
    include "vendor/SDL3"
group ""

project "sead"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    systemversion "latest"

    exceptionhandling "Off"
    rtti "Off"

    targetdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/out")
    objdir ("bin/%{prj.name}-%{cfg.platform}-%{cfg.buildcfg}/int")

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
        "FatalCompileWarnings"
    }
	
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

    filter "configurations:Debug"
        defines { "SEAD_TARGET_DEBUG" }
        runtime "Debug"
        symbols "on"
        optimize "off"

    filter "configurations:Release"
        defines { "SEAD_TARGET_DEBUG" }
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

workspace "Jukcraft"
	architecture "x86_64"

	configurations {
		"Debug",
		"Beta",
		"Release"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}"

project "GLFW"
	location "vendor/glfw"
    kind "StaticLib"
    language "C"
    staticruntime "on"

    targetdir ("bin/" .. outputdir .. "/%{prj.name}")
    objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

    files {
        "vendor/glfw/include/GLFW/glfw3.h",
        "vendor/glfw/include/GLFW/glfw3native.h",
        "vendor/glfw/src/internal.h",
        "vendor/glfw/src/platform.h",
        "vendor/glfw/src/mappings.h",
        "vendor/glfw/src/context.c",
        "vendor/glfw/src/init.c",
        "vendor/glfw/src/input.c",
        "vendor/glfw/src/monitor.c",
        "vendor/glfw/src/platform.c",
        "vendor/glfw/src/vulkan.c",
        "vendor/glfw/src/window.c",
        "vendor/glfw/src/egl_context.c",
        "vendor/glfw/src/osmesa_context.c",
        "vendor/glfw/src/null_platform.h",
        "vendor/glfw/src/null_joystick.h",
        "vendor/glfw/src/null_init.c",

        "vendor/glfw/src/null_monitor.c",
        "vendor/glfw/src/null_window.c",
        "vendor/glfw/src/null_joystick.c",

    }
    filter "system:linux"
        pic "On"

        systemversion "latest"
        staticruntime "On"

        files {
            "vendor/glfw/src/x11_init.c",
            "vendor/glfw/src/x11_monitor.c",
            "vendor/glfw/src/x11_window.c",
            "vendor/glfw/src/xkb_unicode.c",
            "vendor/glfw/src/posix_time.c",
            "vendor/glfw/src/posix_thread.c",
            "vendor/glfw/src/glx_context.c",
            "vendor/glfw/src/egl_context.c",
            "vendor/glfw/src/osmesa_context.c",
            "vendor/glfw/src/linux_joystick.c"
        }

        defines {
            "_GLFW_X11",
			"GLFW_INCLUDE_NONE"
        }

    filter "system:windows"
        systemversion "latest"
    

        files {
            "vendor/glfw/src/win32_init.c",
            "vendor/glfw/src/win32_module.c",
            "vendor/glfw/src/win32_joystick.c",
            "vendor/glfw/src/win32_monitor.c",
            "vendor/glfw/src/win32_time.h",
            "vendor/glfw/src/win32_time.c",
            "vendor/glfw/src/win32_thread.h",
            "vendor/glfw/src/win32_thread.c",
            "vendor/glfw/src/win32_window.c",
            "vendor/glfw/src/wgl_context.c",
            "vendor/glfw/src/egl_context.c",
            "vendor/glfw/src/osmesa_context.c"
        }

        defines { 
            "_GLFW_WIN32",
            "_CRT_SECURE_NO_WARNINGS"
        }

    filter "configurations:Debug"
		runtime "debug"
		symbols "on"

	filter "configurations:Beta"
 		optimize "debug"
        inlining "auto"

	filter "configurations:Release"
		runtime "release"
		optimize "speed"
		inlining "auto"
		intrinsics "on"
		floatingpoint "fast"
		exceptionhandling "off"	
		omitframepointer "on"
		rtti "off"

		flags {
			"NoRuntimeChecks",
			"LinkTimeOptimization"
		}

project "Glad"
	kind "StaticLib"
	language "C"
	location "vendor/glad"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files {
		"vendor/glad/include/glad/glad.h",
		"vendor/glad/include/KHR/khrplatform.h",
		"vendor/glad/src/glad.c"
	}
	
	includedirs {
		"vendor/glad/include"
	}
	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "debug"
		symbols "on"

	filter "configurations:Beta"
		runtime "debug"
 		optimize "on"
		inlining "auto"
		intrinsics "on"
		floatingpoint "fast"

	filter "configurations:Release"
		runtime "release"
		optimize "speed"
		inlining "auto"
		intrinsics "on"
		floatingpoint "fast"
		exceptionhandling "off"	
		omitframepointer "on"
		rtti "off"

		flags {
			"NoRuntimeChecks",
			"LinkTimeOptimization"
		}

		
project "glm"
	location "vendor/glm"
	kind "SharedItems"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	files {
		"vendor/glm/**.hpp"
	}


project "Jukcraft"
	location "Jukcraft"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "-%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "-%{prj.name}")

	files {
		"Jukcraft/src/**.h",
		"Jukcraft/src/**.cpp",
	}

	includedirs {
		"Jukcraft/src/",
		"vendor/glad/include/",
		"vendor/glfw/include/",
		"vendor/glm/",
		"vendor/stb",
		"vendor/nlohmann",
		"vendor/spdlog/include"
	}

	links {
		"GLFW",
		"Glad",
		"opengl32",
		"gdi32"
	}

	defines {
		"GLFW_INCLUDE_NONE"
	}

	pchheader "pch.h"
	pchsource "Jukcraft/src/pch.cpp"

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		runtime "debug"
		symbols "on"

		defines "JUK_DEBUG"

	filter "configurations:Beta"
		runtime "debug"
 		optimize "on"
		inlining "auto"
		intrinsics "on"
		floatingpoint "fast"

		defines {
			"GLM_FORCE_INLINE",
			"JUK_BETA"
		}

	filter "configurations:Release"
		runtime "release"
		optimize "speed"
		inlining "auto"
		intrinsics "on"
		floatingpoint "fast"
		exceptionhandling "off"	
		omitframepointer "on"
		rtti "off"

		defines {
			"GLM_FORCE_INLINE",
			"SPDLOG_NO_EXCEPTIONS"
		}

		flags {
			"NoRuntimeChecks",
			"LinkTimeOptimization"
		}
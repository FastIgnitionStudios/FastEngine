project "FastEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "../Binaries/%{cfg.buildcfg}"
    objdir "../Intermediate/%{cfg.buildcfg}"
    
    pchheader "EnginePCH.h"
    pchsource "EnginePCH.cpp"
    
    files {
        "Source/**.cpp",
        "Source/**.h",
        "ThirdParty/SPDLog/**.h",
        "Engine.h",
        "EnginePCH.h",
        "EnginePCH.cpp",
        "ThirdParty/GLM/GLM/**.hpp"
    }
    
    includedirs {
        "Source",
        "Source/Core",
        "ThirdParty/SPDLog/include/",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.VMA}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLM}"
    }

    links {
        "GLFW",
        "%{Library.Vulkan}",
        "VulkanMemoryAllocator",
        "ImGUI"
    }
    
    buildoptions {"/utf-8"}

    flags
    {
        "MultiProcessorCompile"
    }
    
    filter "system:windows"
    defines {
        "PLATFORM_WINDOWS",
        "ENGINE_BUILD_DLL"
    }

    filter "configurations:Debug"
        defines { "ENGINE_DEBUG", "ENGINE_ENABLE_ASSERTS" }
        symbols "on"
        
        links
        {
            "%{Library.ShaderC_Debug}",
            "%{Library.SPIRV_Cross_Debug}",
            "%{Library.SPIRV_Cross_GLSL_Debug}"
        }
        
    filter "configurations:Release"
        defines { "ENGINE_RELEASE" }
        optimize "on"
        
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}"
        }
        
    filter "configurations:Dev"
        defines { "ENGINE_DEV", "ENGINE_ENABLE_ASSERTS" }
        optimize "on"
        
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}"
        }


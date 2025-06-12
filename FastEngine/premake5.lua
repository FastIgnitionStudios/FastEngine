project "FastEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "%{wks.location}/Binaries/%{cfg.buildcfg}"
    objdir "%{wks.location}/Intermediate/%{cfg.buildcfg}"
    
    pchheader "EnginePCH.h"
    pchsource "EnginePCH.cpp"
    
    files {
        "Source/**.cpp",
        "Source/**.h",
        "ThirdParty/SPDLog/**.h",
        "Engine.h",
        "EnginePCH.h",
        "EnginePCH.cpp",
        "ThirdParty/GLM/GLM/**.hpp",
        "ThirdParty/EnTT/include/entt.hpp",
        "ThirdParty/STBImage/include/stb_image.h",
        "%{wks.location}/Intermediate/Generated/**.h",
        "%{wks.location}/Intermediate/Generated/**.cpp",
    }
    
    includedirs {
        "Source",
        "Source/Core",
        "ThirdParty/SPDLog/include/",
        "%{IncludeDir.GLFW}",
        "%{IncludeDir.VulkanSDK}",
        "%{IncludeDir.VMA}",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLM}",
        "ThirdParty/GLM/GLM",
        "%{IncludeDir.FastGLTF}",
        "%{IncludeDir.EnTT}",
        "%{IncludeDir.YamlCPP}",
        "ThirdParty",
        "%{wks.location}/Intermediate/Generated/"
    }

    links {
        "GLFW",
        "%{Library.Vulkan}",
        "VulkanMemoryAllocator",
        "ImGUI",
        "FastGLTF",
        "yaml-cpp"
    }
    
    buildoptions {"/utf-8"}

    dependson {"FastReflection"}

    prebuildcommands{
        "\"../Binaries/Debug/net8.0/FastReflection.exe\" %{wks.location} \"../Intermediate/Generated\"",
        "\"../Setup/GenProject-Windows.bat\""
    }


    flags
    {
        "MultiProcessorCompile"
    }
    
    filter "system:windows"
    defines {
        "PLATFORM_WINDOWS",
        "ENGINE_BUILD_DLL",
        "GLM_FORCE_DEPTH_ZERO_TO_ONE"
    }

    filter "configurations:Debug"
        defines { "DEBUG_BUILD", "ENGINE_ENABLE_ASSERTS" }
        symbols "on"
        
        links
        {
            "%{Library.ShaderC_Debug}",
            "%{Library.SPIRV_Cross_Debug}",
            "%{Library.SPIRV_Cross_GLSL_Debug}"
        }
        
    filter "configurations:Release"
        defines { "RELEASE_BUILD" }
        optimize "on"
        
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}"
        }
        
    filter "configurations:Dev"
        defines { "DEV_BUILD", "ENGINE_ENABLE_ASSERTS" }
        optimize "on"
        
        links
        {
            "%{Library.ShaderC_Release}",
            "%{Library.SPIRV_Cross_Release}",
            "%{Library.SPIRV_Cross_GLSL_Release}"
        }


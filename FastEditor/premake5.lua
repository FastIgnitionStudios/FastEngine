project "FastEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"
    
    targetdir "../Binaries/%{cfg.buildcfg}"
    objdir "../Intermediate/%{cfg.buildcfg}"
    
    files {
        "Source/**.cpp",
        "Source/**.h",
        "../FastEngine/ThirdParty/EnTT/include/entt.hpp",
    }
    
    includedirs {
        "Source",
        "../FastEngine/Source",
        "../FastEngine/",
        "../FastEngine/Source/Core/",
        "../FastEngine/ThirdParty/SPDLog/Include/",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLM}",
        "../FastEngine/ThirdParty/GLM/GLM",
        "%{IncludeDir.EnTT}",
        "%{IncludeDir.FastGLTF}",
        "%{wks.location}/Intermediate/Generated/"
    }
    
    links {"FastEngine", "ImGUI", "FastGLTF"}
    
    buildoptions {"/utf-8"}

    flags
    {
        "MultiProcessorCompile"
    }
    
    defines {"EDITOR", "YAML_CPP_STATIC_DEFINE"}
    
    filter "system:windows"
        defines {"PLATFORM_WINDOWS"}
    
    filter "configurations:Debug"
        defines { "EDITOR_DEBUG", "EDITOR_ENABLE_ASSERTS" }
        symbols "on"
    
    
    filter "configurations:Release"
        defines { "EDITOR_RELEASE" }
        optimize "on"

    
    filter "configurations:Dev"
        defines { "EDITOR_DEV", "EDITOR_ENABLE_ASSERTS" }
        optimize "on"


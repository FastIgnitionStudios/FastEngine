workspace "FastEngine"
    architecture "x64"
    configurations {"Debug", "Dev", "Release"}




group "dependencies"
    include "FastEngine/ThirdParty/GLFW/"
    include "FastEngine/ThirdParty/VMA"
    include "FastEngine/ThirdParty/ImGUI"
group ""

include "Dependencies.lua"

include "FastEngine/"

project "FastEditor"
    location "FastEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"
    staticruntime "on"

    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Intermediate/%{cfg.buildcfg}"

    files {
        "%{prj.name}/Source/**.cpp",
        "%{prj.name}/Source/**.h"
    }

    includedirs {
        "%{prj.name}/Source",
        "FastEngine/",
        "FastEngine/Source",
        "FastEngine/ThirdParty/SPDLog/Include/",
        "%{IncludeDir.ImGui}",
        "%{IncludeDir.GLM}"
    }

    links {"FastEngine", "ImGUI"}

    buildoptions {"/utf-8"}

    defines {"EDITOR"}

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


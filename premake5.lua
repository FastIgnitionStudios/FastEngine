workspace "FastEngine"
    architecture "x64"
    configurations {"Debug", "Dev", "Release"}

    

project "FastEngine"
    location "FastEngine"
    kind "StaticLib"
    language "C++"
    cppdialect "C++20"

    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Intermediate/%{cfg.buildcfg}"
    
    pchheader "EnginePCH.h"
    pchsource "FastEngine/EnginePCH.cpp"

    files {
        "%{prj.name}/Source/**.cpp",
        "%{prj.name}/Source/**.h",
        "%{prj.name}/ThirdParty/SPDLog/**.h",
        "%{prj.name}/Engine.h",
        "%{prj.name}/EnginePCH.h",
        "%{prj.name}/EnginePCH.cpp",
    }

    includedirs {
        "%{prj.name}/Source",
        "%{prj.name}/Source/Core",
        "%{prj.name}/ThirdParty/SPDLog/include/"
    }

    buildoptions {"/utf-8"}

    filter "system:windows"
        defines { 
            "PLATFORM_WINDOWS",
            "ENGINE_BUILD_DLL"
        }

    filter "configurations:Debug"
        defines {"ENGINE_DEBUG"}


project "FastEditor"
    location "FastEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

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
        "FastEngine/ThirdParty/SPDLog/Include/"
    }

    links {"FastEngine"}

    buildoptions {"/utf-8"}

    defines {"EDITOR"}

    filter "system:windows"
        defines {"PLATFORM_WINDOWS"}

    filter "configurations:Debug"
        defines {"EDITOR_DEBUG"}
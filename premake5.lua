workspace "FastEngine"
    architecture "x64"
    configurations {"Debug", "Dev", "Release"}

project "FastEngine"
    location "FastEngine"
    kind "SharedLib"
    language "C++"
    cppdialect "C++20"

    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Intermediate/%{cfg.buildcfg}"
    
    files {
        "%{prj.name}/Source/**.cpp",
        "%{prj.name}/Source/**.h",
        "%{prj.name}/ThirdParty/SPDLog/**.h",
        "%{prj.name}/Engine.h"
    }

    includedirs {
        "%{prj.name}/Source",
        "%{prj.name}/ThirdParty/SPDLog/include/"
    }

    buildoptions {"/utf-8"}

    filter "system:windows"
        defines { 
            "PLATFORM_WINDOWS",
            "ENGINE_BUILD_DLL"
        }


project "FastEditor"
    location "FastEditor"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++20"

    targetdir "Binaries/%{cfg.buildcfg}"
    objdir "Intermediate/%{cfg.buildcfg}"

    files {"%{prj.name}/Source/**.cpp", "%{prj.name}Source/**.h"}

    includedirs {
        "%{prj.name}/Source",
        "FastEngine/",
        "FastEngine/Source"
    }

    links {"FastEngine"}

    defines {"EDITOR"}

    filter "system:windows"
        defines {"PLATFORM_WINDOWS"}
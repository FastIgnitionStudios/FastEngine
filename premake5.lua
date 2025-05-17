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
        "FastEngine/Source/**.cpp",
        "FastEngine/Source/**.h",
        "FastEngine/Engine.h"
    }

    includedirs {
        "FastEngine/Source",
    }

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

    files {"FastEditor/Source/**.cpp", "FastEditor/Source/**.h"}

    includedirs {
        "FastEditor/Source",
        "FastEngine/",
        "FastEngine/Source"
    }

    links {"FastEngine"}

    defines {"EDITOR"}

    filter "system:windows"
        defines {"PLATFORM_WINDOWS"}
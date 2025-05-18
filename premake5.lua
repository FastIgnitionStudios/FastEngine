workspace "FastEngine"
    architecture "x64"
    configurations {"Debug", "Dev", "Release"}


IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/FastEngine/ThirdParty/GLFW/include"

include "FastEngine/ThirdParty/GLFW/"
include "FastEngine/"

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
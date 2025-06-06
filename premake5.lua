workspace "FastEngine"
    architecture "x64"
    configurations {"Debug", "Dev", "Release"}




group "dependencies"
    include "FastEngine/ThirdParty/GLFW/"
    include "FastEngine/ThirdParty/VMA"
    include "FastEngine/ThirdParty/ImGUI"
    include "FastEngine/ThirdParty/FastGLTF"
    include "FastEngine/ThirdParty/YamlCPP"
group ""


include "Dependencies.lua"

include "FastEngine/"
include "FastEditor/"
include "GravitySim/"
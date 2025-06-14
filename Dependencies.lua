VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["GLFW"] = "%{wks.location}/FastEngine/ThirdParty/GLFW/include"
IncludeDir["ImGui"] = "%{wks.location}/FastEngine/ThirdParty/ImGUI"
IncludeDir["GLM"] = "%{wks.location}/FastEngine/ThirdParty/Glm/"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/include"
IncludeDir["VMA"] = "%{wks.location}/FastEngine/ThirdParty/VMA/include"
IncludeDir["FastGLTF"] = "%{wks.location}/FastEngine/ThirdParty/FastGLTF/include"
IncludeDir["EnTT"] = "%{wks.location}/FastEngine/ThirdParty/EnTT/include"
IncludeDir["YamlCPP"] = "%{wks.location}/FastEngine/ThirdParty/YamlCPP/include"


LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

#pragma once
#include <vulkan/vulkan.h>

#include "Rendering/Shader.h"

namespace Engine
{
    class ShaderVK : public Shader
    {
    public:

        ShaderVK(const std::string& vertexPath, const std::string& fragmentPath);
        ShaderVK(const std::string& computePath);

        VkShaderModule GetShaderModule(ShaderType shader) { return shaderModules[shader]; }
        void CreateShaderModule(VkDevice device);
    

    private:
        std::unordered_map<ShaderType, std::string> shaderSources;
        std::unordered_map<ShaderType, std::vector<uint32_t>> SPIRVCode;
        std::unordered_map<ShaderType, VkShaderModule> shaderModules;

        void Compile();
        
        bool isCompiled = false;
    };
}

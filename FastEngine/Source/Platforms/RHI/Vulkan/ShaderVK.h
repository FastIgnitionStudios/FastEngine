#pragma once
#include "Rendering/Shader.h"

namespace Engine
{
    class ShaderVK : public Shader
    {
    public:

        ShaderVK(const std::string& vertexPath, const std::string& fragmentPath);
        ShaderVK(const std::string& computePath);

        virtual ~ShaderVK() = default;

    private:
        std::unordered_map<ShaderType, std::string> shaderSources;
        std::unordered_map<ShaderType, std::vector<uint32_t>> SPIRVCode;

        void Compile();
        bool isCompiled = false;
    };
}

#pragma once
#include "Core.h"

namespace Engine
{

    enum class ShaderType
    {
        VERTEX,
        FRAGMENT,
        COMPUTE
    };
    
    class Shader : public Engine
    {
    public:
        virtual ~Shader() = default;

        static Ref<Shader> Create(const std::string& vertexSrc, const std::string& fragmentSrc);
        static Ref<Shader> Create(const std::string& computeSrc);
        
    };
}

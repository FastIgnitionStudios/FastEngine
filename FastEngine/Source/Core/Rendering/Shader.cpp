#include "EnginePCH.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "Platforms/RHI/Vulkan/ShaderVK.h"

namespace Engine
{
    Ref<Shader> Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
    {
        switch (RendererAPI::GetRendererAPI()) {
        case RHI::None:
            break;
        case RHI::Vulkan:
            return Ref<ShaderVK>::Create(vertexSrc, fragmentSrc);
        case RHI::OpenGL:
            break;
        case RHI::DirectX:
            break;
        }

        return nullptr;
    }

    Ref<Shader> Shader::Create(const std::string& computeSrc)
    {
        switch (RendererAPI::GetRendererAPI()) {
        case RHI::None:
            break;
        case RHI::Vulkan:
            return Ref<ShaderVK>::Create(computeSrc);
        case RHI::OpenGL:
            break;
        case RHI::DirectX:
            break;
        }

        return nullptr;
    }
}

#include "EnginePCH.h"
#include "Renderer.h"

#include "EngineApp.h"
#include "RendererAPI.h"
#include "Platforms/RHI/Vulkan/RendererVK.h"

namespace Engine
{
    
    Renderer::~Renderer()
    {
    }

    Ref<Renderer> Renderer::CreateRenderer()
    {
        switch (RendererAPI::GetRendererAPI())
        {
        case RHI::None:
            return nullptr;
            break;
        case RHI::Vulkan:
            {
                return Ref<RendererVK>::Create();
            }
        case RHI::OpenGL:
            return nullptr;
            break;
        case RHI::DirectX:
            return nullptr;
            break;
        }
        return nullptr;
    }

    Ref<Renderer> Renderer::GetRenderer()
    {
        return EngineApp::Get()->GetRenderer();
    }
}

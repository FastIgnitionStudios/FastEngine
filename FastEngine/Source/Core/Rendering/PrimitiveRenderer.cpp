#include "EnginePCH.h"
#include "PrimitiveRenderer.h"

#include "RendererAPI.h"
#include "Platforms/RHI/Vulkan/PrimitiveRendererVK.h"

namespace Engine
{
    PrimitiveRenderer* PrimitiveRenderer::Instance = nullptr;
    
    void PrimitiveRenderer::InitPrimitiveRenderer()
    {
        switch (RendererAPI::GetRendererAPI())
        {
        case RHI::None:
            break;
        case RHI::Vulkan:
            Instance = new PrimitiveRendererVK();
            break;
        case RHI::OpenGL:
            break;
        case RHI::DirectX:
            break;
        }
    }
}

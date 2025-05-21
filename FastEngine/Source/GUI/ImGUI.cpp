#include "EnginePCH.h"
#include "ImGUI.h"

#include "Platforms/RHI/Vulkan/ImGUIVK.h"
#include "Rendering/RendererAPI.h"

Engine::Ref<Engine::ImGUI> Engine::ImGUI::CreateImGUI()
{
    switch (RendererAPI::GetRendererAPI())
    {
    case RHI::None:
        break;
    case RHI::Vulkan:
        return Ref<ImGUIVK>::Create();
    case RHI::OpenGL:
        break;
    case RHI::DirectX:
        break;
    }
    return nullptr;
}

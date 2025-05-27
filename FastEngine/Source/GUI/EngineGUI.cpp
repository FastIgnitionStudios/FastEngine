#include "EnginePCH.h"
#include "EngineGUI.h"


#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Rendering/RendererAPI.h"



void Engine::EngineGUI::BeginFrame()
{
    switch (RendererAPI::GetRendererAPI()) {
    case RHI::None:
        break;
    case RHI::Vulkan:
        {
            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            break;
        }
    case RHI::OpenGL:
        break;
    case RHI::DirectX:
        break;
    }

}

void Engine::EngineGUI::EndFrame()
{
    switch (RendererAPI::GetRendererAPI())
    {
    case RHI::None:
        break;
    case RHI::Vulkan:
        {
            ImGui::Render();
            break;
        }
    case RHI::OpenGL:
        break;
    case RHI::DirectX:
        break;
    }
}

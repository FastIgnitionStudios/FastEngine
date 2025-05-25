#include "EnginePCH.h"
#include "ImGUILayer.h"

#include <imgui.h>

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"

namespace Engine
{
    void ImGUILayer::BeginGUIFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::ShowDemoWindow();
    }

    void ImGUILayer::EndGUIFrame()
    {
        ImGui::Render();
    }
}

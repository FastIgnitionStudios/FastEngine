#include "SceneHierachyPanel.h"
#include "imgui.h"

namespace Engine
{
    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& scene)
    {
        SetContext(scene);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& scene)
    {
        SceneContext = scene;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");

        auto view = SceneContext->Registry.view<NameComponent>();
        for (auto entity : view)
        {
            auto& tag = view.get<NameComponent>(entity);
            ImGui::Text(tag.Name.c_str());
        }
    
        ImGui::End();
    }
}

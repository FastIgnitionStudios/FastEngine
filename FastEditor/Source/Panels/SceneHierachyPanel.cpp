#include "SceneHierachyPanel.h"
#include "imgui.h"
#include "gtc/type_ptr.hpp"

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
        for (auto entityID : view)
        {
            Entity entity{entityID, SceneContext.Raw()};
            DrawEntityNode(entity);
        }

        ImGui::End();

        ImGui::Begin("Properties");

        if (SelectedEntity)
        {
            DrawComponents(SelectedEntity);
        }

        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        ImGuiTreeNodeFlags flags = ((SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) |
            ImGuiTreeNodeFlags_OpenOnArrow;
        auto isOpen = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)entity, flags,
                                        entity.GetComponent<NameComponent>().Name.c_str());
        if (ImGui::IsItemClicked())
        {
            SelectedEntity = entity;
        }
        if (isOpen)
            ImGui::TreePop();
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        DrawComponent<NameComponent>("Name##1", entity, [](NameComponent& comp)
        {
            auto& name = comp.Name;
            char buffer[64];
            memset(buffer, 0, sizeof(buffer));
            strcpy_s(buffer, sizeof(buffer), name.c_str());
            if (ImGui::InputText("Name##2", buffer, sizeof(buffer)))
                name = buffer;
        });

        DrawComponent<TransformComponent>("Transform", entity, [](TransformComponent& comp)
        {
            auto& translation = comp.Translation;
            auto& rotation = comp.Rotation;
            auto& scale = comp.Scale;

            if (ImGui::Button("X##1"))
                translation = glm::vec3(0.0f);
            ImGui::SameLine();
            ImGui::DragFloat3("Translation", glm::value_ptr(translation), 0.1f);

            if (ImGui::Button("X##2"))
                rotation = glm::vec3(0.0f);
            ImGui::SameLine();
            ImGui::DragFloat3("Rotation", value_ptr(rotation), 0.1f);

            if (ImGui::Button("X##3"))
                scale = glm::vec3(1.0f);
            ImGui::SameLine();
            ImGui::DragFloat3("Scale", value_ptr(scale), 0.1f);
        });
    }
}

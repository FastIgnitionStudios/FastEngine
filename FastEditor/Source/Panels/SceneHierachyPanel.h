#pragma once
#include "Engine.h"
#include "imgui.h"

namespace Engine
{
    class SceneHierarchyPanel : public Engine
    {
    public:
        SceneHierarchyPanel(const Ref<Scene>& scene);
        SceneHierarchyPanel() = default;

        void SetContext(const Ref<Scene>& scene);

        void OnImGuiRender();

        

        template<typename T>
        static void DrawComponent(std::string name, Entity entity, std::function<void(T&)> func)
        {
            if (entity.HasComponent<T>())
            {
                T& component = entity.GetComponent<T>();
                ImGuiTreeNodeFlags flags = 0;
                bool isOpen = ImGui::CollapsingHeader(name.c_str(), flags);
                if (isOpen)
                    func(component);
            }
        }

    private:

        void DrawComponents(Entity entity);
        void DrawEntityNode(Entity entity);
        
        Ref<Scene> SceneContext;
        Entity SelectedEntity {};
    };
}

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
        // TODO: Add some sort of reflection system to auto generate all classes/structs with the macro REGISTER_COMPONENT() and add them to this menu
        template<typename T, typename... ConstructArgs>
        static void AddEntityPopup(std::string className, Scene* scene, ConstructArgs&&... args)
        {
            if (ImGui::MenuItem(className.c_str()))
            {
                auto newEntity = scene->CreateEntity(className);
                newEntity.AddComponent<T>(std::forward<ConstructArgs>(args)...);
            }
        }

        template<typename T>
        void SetComponentPropertiesPanel(std::function<void()>&& func)
        {
            ComponentPanels.push_back(func);
        }

    private:

        void DrawComponents(Entity entity);
        void DrawEntityNode(Entity entity);
        
        Ref<Scene> SceneContext;
        Entity SelectedEntity {};

        std::vector<std::function<void()>> ComponentPanels;
    };
}

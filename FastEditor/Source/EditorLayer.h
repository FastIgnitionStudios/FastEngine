#pragma once
#include "Engine.h"

namespace Engine
{
    class EditorLayer : public Layer
    {
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        void OnAttach() override;
        void OnDetach() override;
        void OnUpdate(float deltaTime) override;
        void OnRender(float deltaTime) override;
        void OnEvent(Event& e) override;

        Ref<Scene> GetActiveScene() { return ActiveScene; }

    

    private:

        Ref<Scene> ActiveScene;
    
        Ref<SceneHierarchyPanel> SceneHierarchy;
    };
}

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
        void OnUpdate() override;
        void OnRender() override;
        void OnEvent(Event& e) override;

    private:

        Ref<Scene> ActiveScene;
    
        Ref<SceneHierarchyPanel> SceneHierarchy;
    };
}

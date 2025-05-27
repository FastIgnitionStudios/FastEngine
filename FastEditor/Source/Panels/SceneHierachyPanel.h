#pragma once
#include "Engine.h"

namespace Engine
{
    class SceneHierarchyPanel : public Engine
    {
    public:
        SceneHierarchyPanel(const Ref<Scene>& scene);
        SceneHierarchyPanel() = default;

        void SetContext(const Ref<Scene>& scene);

        void OnImGuiRender();

    private:
        Ref<Scene> SceneContext;
    };
}

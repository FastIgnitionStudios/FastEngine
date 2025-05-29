#pragma once
#include "Engine.h"

namespace Engine
{
    class EditorLayer;
}

namespace Engine
{
    class EditorApp : public EngineApp
    {
    public:
        EditorApp();
        ~EditorApp();

        virtual Ref<Scene> GetActiveScene() override;

    private:
        EditorLayer* editorLayer;
    };
}


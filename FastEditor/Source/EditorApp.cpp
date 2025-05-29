#include "EditorApp.h"
#include "Source/Core/EntryPoint.h"
#include "EditorLayer.h"
#include "imgui.h"

namespace Engine
{
    EditorApp::EditorApp()
    {
        editorLayer = new EditorLayer();
        PushLayer(editorLayer);
    }

    EditorApp::~EditorApp()
    {
    }

    Ref<Scene> EditorApp::GetActiveScene()
    {
        return editorLayer->GetActiveScene();
    }
}

Engine::EngineApp* Engine::CreateApplication()
{
    return new EditorApp();
}



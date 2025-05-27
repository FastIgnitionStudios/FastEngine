#include "EditorApp.h"
#include "Source/Core/EntryPoint.h"
#include "EditorLayer.h"
#include "imgui.h"

EditorApp::EditorApp()
{
    PushOverlay(new ::Engine::EditorLayer());
}

EditorApp::~EditorApp()
{
}

Engine::EngineApp* Engine::CreateApplication()
{
    return new EditorApp();
}



#include "RuntimeApp.h"
#include "Source/Core/EntryPoint.h"
#include "imgui.h"
#include "RuntimeLayer.h"

namespace Engine
{
    RuntimeApp::RuntimeApp()
    {
        runtimeLayer = new RuntimeLayer();
        PushLayer(runtimeLayer);
        
    }

    RuntimeApp::~RuntimeApp()
    {
    }

    Ref<Scene> RuntimeApp::GetActiveScene()
    {
        return runtimeLayer->GetActiveScene();
    }
}

Engine::EngineApp* Engine::CreateApplication()
{
    return new RuntimeApp();
}

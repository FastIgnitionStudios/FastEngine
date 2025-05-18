#include "EnginePCH.h"
#include "EngineApp.h"
#include "Utils/Log.h"

namespace Engine
{
    EngineApp::EngineApp()
    {
        window = std::unique_ptr<Window>(Window::Create());
        window->SetEventCallback(ENGINE_BIND_EVENT_FN(OnEvent));
    }

    EngineApp::~EngineApp()
    {
    }

    void EngineApp::Run()
    {
        while (isRunning)
        {
            window->OnUpdate();
        }
    }

    void EngineApp::OnEvent(Event& e)
    {
        for (auto it = LayerStack.end(); it != LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.isHandled)
                break;
        }

        ENGINE_CORE_INFO("Event: {0}", e.ToString());
    }

    void EngineApp::PushLayer(Layer* layer)
    {
        LayerStack.PushLayer(layer);
    }

    void EngineApp::PushOverlay(Layer* overlay)
    {
        LayerStack.PushOverlay(overlay);
    }
}

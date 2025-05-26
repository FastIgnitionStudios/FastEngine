#include "EnginePCH.h"
#include "EngineApp.h"

#include "Filesystem.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Utils/Log.h"
#include "Events/ApplicationEvent.h"
#include "GUI/ImGUILayer.h"
#include "Rendering/Renderer.h"

namespace Engine
{
    Ref<EngineApp> EngineApp::AppInstance;
    
    EngineApp::EngineApp()
    {
        AppInstance = this;
        window = Ref<Window>(Window::Create());
        window->SetEventCallback(ENGINE_BIND_EVENT_FN(OnEvent));
        Filesystem::InitFilesystem();
        PushOverlay(new ImGUILayer());
    }

    EngineApp::~EngineApp()
    {
    }

    void EngineApp::Run()
    {
        auto renderer = Renderer::CreateRenderer();
        while (isRunning)
        {
            renderer->PreFrame();
            window->OnUpdate();

            for (const auto& layer : LayerStack)
            {
                layer->OnUpdate();

                ImGUILayer* GUILayer = static_cast<ImGUILayer*>(layer);
                if (GUILayer)
                    GUILayer->BeginGUIFrame();

                layer->OnRender();


            }

            renderer->DrawViewport();

            for (const auto& layer : LayerStack)
            {
                ImGUILayer* GUILayer = static_cast<ImGUILayer*>(layer);
                if (GUILayer)
                    GUILayer->EndGUIFrame();
            }
            
            
            
            renderer->DrawFrame();
        }

        ENGINE_CORE_INFO("Engine shutting down");
    }

    void EngineApp::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<WindowCloseEvent>(ENGINE_BIND_EVENT_FN(OnWindowClose));
        for (auto it = LayerStack.end(); it != LayerStack.begin(); )
        {
            (*--it)->OnEvent(e);
            if (e.isHandled)
                break;
        }
        
    }

    void EngineApp::PushLayer(Layer* layer)
    {
        LayerStack.PushLayer(layer);
    }

    void EngineApp::PushOverlay(Layer* overlay)
    {
        LayerStack.PushOverlay(overlay);
    }

    bool EngineApp::OnWindowClose(WindowCloseEvent& event)
    {
        isRunning = false;
        return true;
    }
}

#include "EnginePCH.h"
#include "EngineApp.h"

#include "Filesystem.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "Utils/Log.h"
#include "Events/ApplicationEvent.h"
#include "GUI/EngineGUI.h"
#include "Rendering/PrimitiveRenderer.h"
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
    }

    EngineApp::~EngineApp()
    {
    }

    void EngineApp::Run()
    {
        renderer = Renderer::CreateRenderer();
        PrimitiveRenderer::InitPrimitiveRenderer();
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();
        float frameTime = 0.0f;
        std::vector<float> deltaTimes;
        while (isRunning)
        {
            start = std::chrono::high_resolution_clock::now();
            renderer->PreFrame();
            window->OnUpdate();
            EngineGUI::BeginFrame();
                            
            ImGui::Begin("DeltaTime");
            ImGui::Text("DeltaTime: %f", frameTime);
            ImGui::End();
            for (const auto& layer : LayerStack)
            {
                layer->OnUpdate(frameTime);



                layer->OnRender(frameTime);

                
            }

            EngineGUI::EndFrame();

            renderer->DrawViewport();
            
            renderer->DrawFrame();
            
            end = std::chrono::high_resolution_clock::now();
            frameTime = std::chrono::duration<float, std::milli>(end - start).count();
            deltaTimes.insert(deltaTimes.begin(), frameTime);
            if (deltaTimes.size() > 25)
            {
                deltaTimes.pop_back();
            }
            float sum = 0.0f;
            for (auto&dt : deltaTimes)
            {
                sum += dt;
            }
            frameTime = sum / deltaTimes.size();
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
        layer->OnAttach();
    }

    void EngineApp::PushOverlay(Layer* overlay)
    {
        LayerStack.PushOverlay(overlay);
        overlay->OnAttach();
    }

    bool EngineApp::OnWindowClose(WindowCloseEvent& event)
    {
        isRunning = false;
        return true;
    }
}

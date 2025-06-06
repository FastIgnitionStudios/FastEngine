#pragma once

#include "Core/Core.h"
#include "Events/Event.h"
#include "Layers/Layer.h"
#include "Layers/LayerStack.h"
#include "Window.h"


namespace Engine
{
    class WindowCloseEvent;
}

namespace Engine
{

    class Scene;
    class Renderer;
    

    class ENGINE_API EngineApp : public Engine
    {
    public:
        EngineApp();
        virtual ~EngineApp();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

        Ref<Window> GetWindow() { return window; }
        Ref<Renderer> GetRenderer() { return renderer; }

        virtual Ref<Scene> GetActiveScene() = 0;

        static Ref<EngineApp> Get() { return AppInstance; }
    protected:
        LayerStack LayerStack;

    private:

        bool OnWindowClose(WindowCloseEvent& event);

        bool isRunning = true;
        

        Ref<Window> window;

        Ref<Renderer> renderer;

        static Ref<EngineApp> AppInstance;
    
    };

    EngineApp* CreateApplication();
}

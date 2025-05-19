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

        static Ref<EngineApp> GetEngineApp() { return AppInstance; }

    private:

        bool OnWindowClose(WindowCloseEvent& event);

        bool isRunning = true;
        LayerStack LayerStack;

        Ref<Window> window;

        static Ref<EngineApp> AppInstance;
    
    };

    EngineApp* CreateApplication();
}

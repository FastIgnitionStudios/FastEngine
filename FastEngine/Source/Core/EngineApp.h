#pragma once

#include "Core/Core.h"
#include "Events/Event.h"
#include "Layers/Layer.h"
#include "Layers/LayerStack.h"
#include "Window.h"

namespace Engine
{
    

    class ENGINE_API EngineApp
    {
    public:
        EngineApp();
        virtual ~EngineApp();

        void Run();

        void OnEvent(Event& e);

        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);

    private:

        bool isRunning = true;
        LayerStack LayerStack;

        std::unique_ptr<Window> window;
    
    };

    EngineApp* CreateApplication();
}

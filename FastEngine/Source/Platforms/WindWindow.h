#pragma once

#include "Core/Window.h"
#include <GLFW/glfw3.h>


namespace Engine
{
    class WindWindow : public Window
    {
    public:
        WindWindow(const WindowParams& params);
        virtual ~WindWindow();

        void OnUpdate() override;

        inline unsigned int GetWidth() const { return Data.Width; }
        inline unsigned int GetHeight() const { return Data.Height; }

        inline void* GetNativeWindow() override { return window; }

        inline void SetEventCallback(const EventCallbackFn& callback) override { Data.EventCallback = callback; }
        void SetVSync(bool enabled) override;
        bool IsVSync() const override;
        
    private:

        virtual void Init(const WindowParams& params);
        virtual void Shutdown();
        
        struct WindowData
        {
            std::string Title;
            unsigned int Width, Height;
            bool VSync;
            EventCallbackFn EventCallback;
        };

        WindowData Data;
        
        GLFWwindow* window;
    };
}

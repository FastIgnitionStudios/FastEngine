#pragma once

#include "Events/Event.h"

namespace Engine
{
    struct WindowParams
    {
        std::string Title;
        unsigned int Width, Height;

        WindowParams(const std::string& title = "Fast Engine", unsigned int width = 1280, unsigned int height = 720)
            : Title(title), Width(width), Height(height) {}
    };


    /**
     * Abstract window class. \n
     * Use Window::Create() to create the window for the compiled platform
     */
    class Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;

        virtual ~Window() {}

        virtual void OnUpdate() = 0;


        
        virtual unsigned int GetWidth() const = 0;
        virtual unsigned int GetHeight() const = 0;

        virtual void SetEventCallback(const EventCallbackFn& callback) = 0;

        /// Likely obsolete
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;
        ///

        /// Create a platform-agnostic window
        /// @param params Window settings
        /// @return Pointer to created window
        static Window* Create(const WindowParams& params = WindowParams());
    };
}

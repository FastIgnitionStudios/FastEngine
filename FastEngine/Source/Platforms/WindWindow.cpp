#include "EnginePCH.h"
#include "WindWindow.h"

#include "Events/ApplicationEvent.h"
#include "Utils/Log.h"


namespace Engine
{
    static bool GLFWInitialized = false;

    Window* Window::Create(const WindowParams& params)
    {
        return new WindWindow(params);
    }
    
    WindWindow::WindWindow(const Engine::WindowParams& params)
    {
        Init(params);
    }

    WindWindow::~WindWindow()
    {
        Shutdown();
    }

    void WindWindow::OnUpdate()
    {
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    void WindWindow::SetVSync(bool enabled)
    {
    }

    bool WindWindow::IsVSync() const
    {
        return true;
    }

    void WindWindow::Init(const Engine::WindowParams& params)
    {
        Data = {};
        Data.Width = params.Width;
        Data.Height = params.Height;
        Data.Title = params.Title;
        
        ENGINE_CORE_TRACE("Initializing window: {0} {1}, {2}", params.Title, params.Width, params.Height);

        if (!GLFWInitialized)
        {
            int success = glfwInit();
            ENGINE_CORE_ASSERT(success, "Could not initialize GLFW!");
        }
        
        window = glfwCreateWindow(params.Width, params.Height, params.Title.c_str(), NULL, NULL);
        glfwMakeContextCurrent(window);
        glfwSetWindowUserPointer(window, &Data);
        SetVSync(true);

        // Set GLFW Callbacks

#pragma region GLFW Callbacks

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowResizeEvent event(width, height);
        });
        
#pragma endregion
    }

    void WindWindow::Shutdown()
    {
        glfwDestroyWindow(window);
    }
}

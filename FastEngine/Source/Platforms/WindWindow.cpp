#include "EnginePCH.h"
#include "WindWindow.h"

#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include "Utils/Log.h"


namespace Engine
{
    static bool GLFWInitialized = false;

    static void GLFWErrorCallback(int error, const char* description)
    {
        ENGINE_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window* Window::Create(const WindowParams& params)
    {
        return new WindWindow(params);
    }

    WindWindow::WindWindow(const WindowParams& params)
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
    }

    void WindWindow::SetVSync(bool enabled)
    {
    }

    bool WindWindow::IsVSync() const
    {
        return true;
    }

    void WindWindow::Init(const WindowParams& params)
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
            glfwSetErrorCallback(GLFWErrorCallback);
            GLFWInitialized = true;
        }
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        window = glfwCreateWindow(params.Width, params.Height, params.Title.c_str(), NULL, NULL);
        glfwSetWindowUserPointer(window, &Data);
        SetVSync(true);

        // Set GLFW Callbacks

#pragma region GLFW Callbacks

        glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
            case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            switch (action)
            {
            case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xoffset, (float)yoffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xpos, (float)ypos);
            data.EventCallback(event);
        });

#pragma endregion
    }

    void WindWindow::Shutdown()
    {
        glfwDestroyWindow(window);
    }
}

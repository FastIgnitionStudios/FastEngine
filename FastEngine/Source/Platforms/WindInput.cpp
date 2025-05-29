#include "EnginePCH.h"
#include "WindInput.h"

#include "EngineApp.h"
#include "GLFW/glfw3.h"

namespace Engine
{
    Input* Input::Instance = new WindInput();
    
    WindInput::WindInput()
    {
    }

    WindInput::~WindInput()
    {
    }

    bool WindInput::IsKeyPressed_Implementation(int keycode)
    {
        auto window = static_cast<GLFWwindow*>(EngineApp::Get()->GetWindow()->GetNativeWindow());
        auto state = glfwGetKey(window, keycode);
        return state == GLFW_PRESS || state == GLFW_REPEAT;
    }

    bool WindInput::IsMouseButtonPressed_Implementation(int button)
    {
        auto window = static_cast<GLFWwindow*>(EngineApp::Get()->GetWindow()->GetNativeWindow());
        auto state = glfwGetMouseButton(window, button);
        return state == GLFW_PRESS;
    }

    float WindInput::GetMouseX_Implementation()
    {
        auto window = static_cast<GLFWwindow*>(EngineApp::Get()->GetWindow()->GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return (float)x;
    }

    float WindInput::GetMouseY_Implementation()
    {
        auto window = static_cast<GLFWwindow*>(EngineApp::Get()->GetWindow()->GetNativeWindow());
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        return (float)y;
    }
}

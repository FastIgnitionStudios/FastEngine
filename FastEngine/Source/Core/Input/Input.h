#pragma once

#include "Core.h"
#include "Keycodes.h"

namespace Engine
{
    class Input : public Engine
    {
    public:
        inline static bool IsKeyPressed(int keycode) { return Instance->IsKeyPressed_Implementation(keycode); }
        inline static bool IsMouseButtonPressed(int button) { return Instance->IsMouseButtonPressed_Implementation(button); }
        inline static float GetMouseX() { return Instance->GetMouseX_Implementation(); }
        inline static float GetMouseY() { return Instance->GetMouseY_Implementation(); }


    protected:
        virtual bool IsKeyPressed_Implementation(int keycode) = 0;
        virtual bool IsMouseButtonPressed_Implementation(int button) = 0;
        virtual float GetMouseX_Implementation() = 0;
        virtual float GetMouseY_Implementation() = 0;

    private:
        static Input* Instance;
    };
}
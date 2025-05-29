#pragma once
#include "Input/Input.h"


namespace Engine
{
    class WindInput : public Input
    {
    public:
        WindInput();
        ~WindInput();

    protected:
        virtual bool IsKeyPressed_Implementation(int keycode) override;
        virtual bool IsMouseButtonPressed_Implementation(int button) override;
        virtual float GetMouseX_Implementation() override;
        virtual float GetMouseY_Implementation() override;
    };
}

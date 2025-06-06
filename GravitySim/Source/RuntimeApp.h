#pragma once

#include "Engine.h"


class RuntimeLayer;

namespace Engine
{
    class RuntimeApp : public EngineApp
    {
    public:
        RuntimeApp();
        ~RuntimeApp();

        virtual Ref<Scene> GetActiveScene() override;

    private:
        RuntimeLayer* runtimeLayer;
    };
}

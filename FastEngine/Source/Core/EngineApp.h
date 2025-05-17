#pragma once

#include "Core/Core.h"

namespace Engine
{
    class ENGINE_API EngineApp
    {
    public:
        EngineApp();
        virtual ~EngineApp();

        void Run();
    
    };

    EngineApp* CreateApplication();
}

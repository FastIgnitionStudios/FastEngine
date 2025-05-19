#pragma once

#include "Core.h"

namespace Engine
{
    class Device : public Engine
    {
    public:

        virtual ~Device();

        static Ref<Device> CreateDevice();
    
    };
}

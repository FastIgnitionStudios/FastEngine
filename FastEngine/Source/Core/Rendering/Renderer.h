#pragma once
#include "Core.h"

namespace Engine
{
    class Renderer : public Engine
    {
    public:
        virtual ~Renderer();


        static Ref<Renderer> CreateRenderer();

        virtual void DrawFrame() = 0;

        
        
    };
}

#pragma once
#include "Layers/Layer.h"

namespace Engine
{
    class ImGUILayer : public Layer
    {
    public:
        void BeginGUIFrame();
        void EndGUIFrame();
    };
}

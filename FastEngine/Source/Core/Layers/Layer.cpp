#include "EnginePCH.h"
#include "Layer.h"
#include "Events/Event.h"

namespace Engine
{
    Layer::Layer(const std::string& name) : DebugName(name)
    {
    }

    Layer::~Layer()
    {
    }
}

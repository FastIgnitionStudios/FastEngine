#include "EnginePCH.h"
#include "UUID.h"

#include <random>

namespace Engine
{
    static std::random_device RandomDevice;
    static std::mt19937_64 RandomEngine(RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_Distribution;
    
    
    UUID::UUID() : ID(s_Distribution(RandomEngine))
    {
        
    }

    UUID::UUID(uint64_t uuid)
    {
    }
}

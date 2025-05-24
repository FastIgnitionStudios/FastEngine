#pragma once

#include "Core.h"
#include <xhash>

namespace Engine
{
    class UUID
    {
    public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;

        operator uint64_t() const { return ID; }
    private:
        uint64_t ID;
    
    };
}

namespace std
{
    template<>
    struct hash<Engine::UUID>
    {
        std::size_t operator() (const Engine::UUID& uuid) const
        {
            return hash<uint64_t>()(uuid);
        }
    };
}

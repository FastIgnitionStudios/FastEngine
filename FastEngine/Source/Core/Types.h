#pragma once

// Fuck microsoft

#ifdef GetClassName
    #undef GetClassName
#endif

namespace Engine
{
    template <typename T>
   const char* GetClassName()
    {
        return nullptr;
    }
}
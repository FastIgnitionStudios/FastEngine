#include "EnginePCH.h"
#include "Core.h"

namespace Engine::Utils
{
    static std::unordered_set<void*> liveReferences;
    static std::mutex liveReferencesMutex;

    void AddLivingReference(void* instance)
    {
        std::lock_guard<std::mutex> lock(liveReferencesMutex);
        ENGINE_CORE_ASSERT(instance, "Instance is null")
        liveReferences.insert(instance);
    }

    void RemoveDeadReference(void* instance)
    {
        std::lock_guard<std::mutex> lock(liveReferencesMutex);
        ENGINE_CORE_ASSERT(instance, "Instance is null")
        ENGINE_CORE_ASSERT(liveReferences.find(instance) != liveReferences.end(), "Instance is not alive")
        liveReferences.erase(instance);
    }

    bool isAlive(void* instance)
    {
        ENGINE_CORE_ASSERT(instance, "Instance is null")
        return liveReferences.find(instance) != liveReferences.end();
    }
}
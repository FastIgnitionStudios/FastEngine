#pragma once
#include "Core.h"
#include "Entity.h"

namespace Engine
{
    class ScriptEntity : public Engine
    {
    public:

        virtual ~ScriptEntity() {}
        
        template<typename T>
        T& GetComponent()
        {
            return entity.GetComponent<T>();
        }

    protected:
        virtual void OnCreate() {}
        virtual void OnDestroy() {}
        virtual void OnUpdate(float DeltaTime) {}
    private:
        Entity entity;
        friend class Scene;
    };
}

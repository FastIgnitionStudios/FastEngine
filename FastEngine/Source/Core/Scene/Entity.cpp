#include "EnginePCH.h"
#include "Entity.h"

#include "Components.h"


namespace Engine
{

    Entity::Entity(entt::entity id, ::Engine::Scene* scene) : EntityID(id), Scene(scene)
    {
    }
}

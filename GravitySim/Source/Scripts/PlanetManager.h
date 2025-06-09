#pragma once
#include "Engine.h"

class PlanetManager : public Engine::ScriptEntity
{
public:
    explicit PlanetManager(const ::Engine::Entity& ent)
        : ScriptEntity(ent)
    {
    }

protected:
    void OnCreate() override;
    void OnUpdate(float DeltaTime) override;
};

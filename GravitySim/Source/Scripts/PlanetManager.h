#pragma once
#include "Engine.h"

class PlanetManager : public Engine::ScriptEntity
{
protected:
    void OnCreate() override;
    void OnUpdate(float DeltaTime) override;
};

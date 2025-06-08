#pragma once

#include "Engine.h"


class RuntimeLayer : public Engine::Layer
{
public:
    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate(float deltaTime) override;
    void OnRender(float deltaTime) override;
    void OnEvent(Engine::Event& e) override;

    Ref<Engine::Scene> GetActiveScene() { return ActiveScene; }

private:

    Ref<Engine::Scene> ActiveScene;
    
};

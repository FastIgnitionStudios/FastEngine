#include "RuntimeLayer.h"

void RuntimeLayer::OnAttach()
{
    ActiveScene = Ref<Engine::Scene>::Create();
    
    
}

void RuntimeLayer::OnDetach()
{
    Layer::OnDetach();
}

void RuntimeLayer::OnUpdate()
{
    Layer::OnUpdate();
}

void RuntimeLayer::OnRender()
{
    Layer::OnRender();
}

void RuntimeLayer::OnEvent(Engine::Event& e)
{
    Layer::OnEvent(e);
}

#include "PlanetScript.h"

#include "Rendering/PrimitiveRenderer.h"

void PlanetScript::OnCreate()
{
    ENGINE_INFO("Planet Script Created");
}

PlanetScript::PlanetScript(::Engine::Entity ent) : ScriptEntity(entity)
{
    
}

void PlanetScript::SetTransform(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
{
    auto& transform = GetComponent<::Engine::TransformComponent>();
    transform.Translation = position;
    transform.Rotation = rotation;
    transform.Scale = scale;
}

void PlanetScript::OnUpdate(float DeltaTime)
{
    velocity += acceleration * DeltaTime;
    auto& transform = GetComponent<::Engine::TransformComponent>();
    transform.Translation += velocity * DeltaTime;
    ::Engine::PrimitiveRenderer::DrawSphere(transform, 1.f);
}

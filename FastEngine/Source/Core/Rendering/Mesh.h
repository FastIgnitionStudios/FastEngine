#pragma once
#include "Buffer.h"
#include "Core.h"
#include "Renderer.h"
#include "Core/UUID.h"
namespace Engine
{
    struct MeshComponent
    {
        UUID id {UUID()};
        std::string filePath;

        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };

    class Mesh : public Engine, public IRenderable
    {
    public:
        Mesh() = default;

        virtual void Draw(const glm::mat4& worldMatrix, DrawContext& context) override = 0;

    protected:

    };
}

#pragma once
#include "Core.h"
#include "glm.hpp"

namespace Engine
{
    struct RenderObject
    {
        uint32_t indexCount;
        uint32_t firstIndex;
        void* indexBuffer;

        void* material;
        glm::mat4 transform;
        uint64_t vertexBufferAddress;
    };

    struct DrawContext
    {
        std::vector<RenderObject> OpaqueSurfaces;
    };

    class IRenderable
    {
        virtual void Draw(const glm::mat4& topMatrix, DrawContext& context) = 0;
    };

    
    
    class Renderer : public Engine
    {
    public:
        virtual ~Renderer();


        static Ref<Renderer> CreateRenderer();

        virtual void DrawFrame() = 0;
        virtual void PreFrame() = 0;
        virtual void DrawViewport() = 0;
        virtual RenderObject CreateRenderObject() = 0;

        
        
    };
}

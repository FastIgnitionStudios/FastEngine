#pragma once

namespace Engine
{
    enum class RHI
    {
        None = -1,
        Vulkan = 0,
        OpenGL = 1,
        DirectX = 2
    };
    
    class RendererAPI
    {
    public:
        static RHI GetRendererAPI() { return RenderAPI; }

    private:
        static RHI RenderAPI;
    
    };
}

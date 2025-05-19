#pragma once
#include <vulkan/vulkan.h>

#include "Core.h"

namespace Engine
{
    struct FrameData
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
    };

    constexpr unsigned int FRAME_OVERLAP = 2;

    // TODO: Complete the command queue and buffer submission to prepare for rendering commands
    
    class CommandQueueVK : public Engine
    {
    public:
        CommandQueueVK();
        ~CommandQueueVK();

    private:
        FrameData frames[FRAME_OVERLAP];

        FrameData& GetCurrentFrame() { return frames[frameNumber % FRAME_OVERLAP]; }

        unsigned int frameNumber = 0;

        VkQueue GraphicsQueue;
        uint32_t GraphicsQueueFamily;
        
    
    };
}

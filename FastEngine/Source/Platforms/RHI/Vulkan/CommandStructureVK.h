#pragma once
#include <queue>
#include <vulkan/vulkan.h>

#include "Core.h"
#include "vkbootstrap/VkBootstrap.h"

namespace Engine
{
    struct FrameData
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;
    };

    struct CommandQueueInfo
    {
        vkb::Device device;
    };

    constexpr unsigned int FRAME_OVERLAP = 2;

    // TODO: Complete the command queue and buffer submission to prepare for rendering commands
    
    class CommandStructureVK : public Engine
    {
    public:
        CommandStructureVK(const CommandQueueInfo& info);
        ~CommandStructureVK();

        VkQueue GetGraphicsQueue() { return GraphicsQueue; }
        
        FrameData& GetCurrentFrame() { return frames[frameNumber % FRAME_OVERLAP]; }
        unsigned int GetFrameNumber() { return frameNumber; }
        void NewFrame() { frameNumber++; }

        VkSemaphoreSubmitInfo CreateSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask, VkSemaphore semaphore);
        VkCommandBufferSubmitInfo CreateCommandBufferSubmitInfo(VkCommandBuffer commandBuffer);

    private:

        void CreateGraphicsQueues();
        void CreateCommandStructures();

        VkFenceCreateInfo CreateFenceInfo(VkFenceCreateFlags flags = 0);
        VkSemaphoreCreateInfo CreateSemaphoreInfo(VkSemaphoreCreateFlags flags = 0);

        
        FrameData frames[FRAME_OVERLAP];



        unsigned int frameNumber = 0;

        VkQueue GraphicsQueue;
        uint32_t GraphicsQueueFamily;

        CommandQueueInfo cmdQueueInfo;
        
    
    };


}

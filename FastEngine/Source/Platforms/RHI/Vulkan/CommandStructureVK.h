#pragma once


#include "Core.h"
#include "UtilsVK.h"
#include "vkbootstrap/VkBootstrap.h"

namespace Engine
{


    
    struct FrameData
    {
        VkCommandPool commandPool;
        VkCommandBuffer commandBuffer;
        VkSemaphore swapchainSemaphore, renderSemaphore;
        VkFence renderFence;

        DeletionQueue DeletionQueue;
    };

    struct CommandQueueInfo
    {
        vkb::Device device;
        DeletionQueue* MainDeletionQueue;
    };

    constexpr unsigned int FRAME_OVERLAP = 2;

    // TODO: Add queue of lambdas to execute rendering code from anywhere in the engine that can be added to the queue outside of the rendering loop
    
    class CommandStructureVK : public Engine
    {
    public:

        struct ImmediateCommandStructure
        {
            VkFence ImmFence;
            VkCommandBuffer ImmCommandBuffer;
            VkCommandPool ImmCommandPool;
        
        };
        
        CommandStructureVK(const CommandQueueInfo& info);
        ~CommandStructureVK();

        VkQueue GetGraphicsQueue() { return GraphicsQueue; }
        
        FrameData& GetCurrentFrame() { return frames[frameNumber % FRAME_OVERLAP]; }
        unsigned int GetFrameNumber() { return frameNumber; }
        void NewFrame() { frameNumber++; }
        ImmediateCommandStructure GetImmediateCommandStructure() { return immCommandStruct; }

        VkCommandBufferBeginInfo CreateCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);
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
        ImmediateCommandStructure immCommandStruct;
        
    
    };


}

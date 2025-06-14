#include "EnginePCH.h"
#include "CommandStructureVK.h"

namespace Engine
{

    CommandStructureVK::CommandStructureVK(const CommandQueueInfo& info) : cmdQueueInfo(info)
    {
        CreateGraphicsQueues();
        CreateCommandStructures();
        CreateDescriptors();
    }

    CommandStructureVK::~CommandStructureVK()
    {
        vkDeviceWaitIdle(cmdQueueInfo.device);

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            vkDestroyCommandPool(cmdQueueInfo.device, frames[i].commandPool, nullptr);

            vkDestroyFence(cmdQueueInfo.device, frames[i].renderFence, nullptr);
            vkDestroySemaphore(cmdQueueInfo.device, frames[i].renderSemaphore, nullptr);
            vkDestroySemaphore(cmdQueueInfo.device, frames[i].swapchainSemaphore, nullptr);

            frames[i].FrameDescriptors.DestroyPools(cmdQueueInfo.device);
            frames[i].DeletionQueue.Flush();
        }

        vkDestroyCommandPool(cmdQueueInfo.device, immCommandStruct.ImmCommandPool, nullptr);
        vkDestroyFence(cmdQueueInfo.device, immCommandStruct.ImmFence, nullptr);
    }

    void CommandStructureVK::CreateGraphicsQueues()
    {
        GraphicsQueue = cmdQueueInfo.device.get_queue(vkb::QueueType::graphics).value();
        GraphicsQueueFamily = cmdQueueInfo.device.get_queue_index(vkb::QueueType::graphics).value();
    }

    void CommandStructureVK::CreateCommandStructures()
    {
        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.pNext = VK_NULL_HANDLE;
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        cmdPoolInfo.queueFamilyIndex = GraphicsQueueFamily;

        VkFenceCreateInfo fenceCreateInfo = CreateFenceInfo(VK_FENCE_CREATE_SIGNALED_BIT);
        VkSemaphoreCreateInfo semaphoreCreateInfo = CreateSemaphoreInfo();

        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            VK_CHECK(vkCreateCommandPool(cmdQueueInfo.device, &cmdPoolInfo, nullptr, &frames[i].commandPool));

            VkCommandBufferAllocateInfo cmdAllocInfo = {};
            cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdAllocInfo.pNext = VK_NULL_HANDLE;
            cmdAllocInfo.commandPool = frames[i].commandPool;
            cmdAllocInfo.commandBufferCount = 1;
            cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

            VK_CHECK(vkAllocateCommandBuffers(cmdQueueInfo.device, &cmdAllocInfo, &frames[i].commandBuffer));

            VK_CHECK(vkCreateFence(cmdQueueInfo.device, &fenceCreateInfo, nullptr, &frames[i].renderFence));
            
            VK_CHECK(vkCreateSemaphore(cmdQueueInfo.device, &semaphoreCreateInfo, nullptr, &frames[i].swapchainSemaphore));
            VK_CHECK(vkCreateSemaphore(cmdQueueInfo.device, &semaphoreCreateInfo, nullptr, &frames[i].renderSemaphore));
            
            
        }

        VK_CHECK(vkCreateCommandPool(cmdQueueInfo.device, &cmdPoolInfo, nullptr, &immCommandStruct.ImmCommandPool));
        
        VkCommandBufferAllocateInfo cmdAllocInfo = {};
        cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        cmdAllocInfo.pNext = VK_NULL_HANDLE;
        cmdAllocInfo.commandPool = immCommandStruct.ImmCommandPool;
        cmdAllocInfo.commandBufferCount = 1;
        cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

        VK_CHECK(vkAllocateCommandBuffers(cmdQueueInfo.device, &cmdAllocInfo, &immCommandStruct.ImmCommandBuffer));

        VK_CHECK(vkCreateFence(cmdQueueInfo.device, &fenceCreateInfo, nullptr, &immCommandStruct.ImmFence));


    }

    void CommandStructureVK::CreateDescriptors()
    {
        for (int i = 0; i < FRAME_OVERLAP; i++)
        {
            std::vector<DescriptorAllocatorDynamic::PoolSizeRatio> frameSizes = {
                {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 3},
                {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 3},
                {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 4},
            };

            frames[i].FrameDescriptors = DescriptorAllocatorDynamic{};
            frames[i].FrameDescriptors.Init(cmdQueueInfo.device, 1000, frameSizes);
            
        }
    }

    VkFenceCreateInfo CommandStructureVK::CreateFenceInfo(VkFenceCreateFlags flags)
    {
        VkFenceCreateInfo fenceInfo = {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.pNext = VK_NULL_HANDLE;

        fenceInfo.flags = flags;
        return fenceInfo;
    }

    VkSemaphoreCreateInfo CommandStructureVK::CreateSemaphoreInfo(VkSemaphoreCreateFlags flags)
    {
        VkSemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        semaphoreInfo.pNext = VK_NULL_HANDLE;
        semaphoreInfo.flags = flags;
        return semaphoreInfo;
    }

    VkCommandBufferBeginInfo CommandStructureVK::CreateCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;

        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.flags = flags;
        return beginInfo;
    }

    VkSemaphoreSubmitInfo CommandStructureVK::CreateSemaphoreSubmitInfo(VkPipelineStageFlags2 stageMask,
                                                                        VkSemaphore semaphore)
    {
        VkSemaphoreSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
        submitInfo.pNext = nullptr;
        submitInfo.semaphore = semaphore;
        submitInfo.stageMask = stageMask;
        submitInfo.deviceIndex = 0;
        submitInfo.value = 1;

        return submitInfo;
    }

    VkCommandBufferSubmitInfo CommandStructureVK::CreateCommandBufferSubmitInfo(VkCommandBuffer commandBuffer)
    {
        VkCommandBufferSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
        submitInfo.pNext = VK_NULL_HANDLE;
        submitInfo.commandBuffer = commandBuffer;
        submitInfo.deviceMask = 0;
        return submitInfo;
    }

    void CommandStructureVK::FlushAllDeletors()
    {
        for (auto frame : frames)
        {
            frame.DeletionQueue.Flush();
        }
    }
}

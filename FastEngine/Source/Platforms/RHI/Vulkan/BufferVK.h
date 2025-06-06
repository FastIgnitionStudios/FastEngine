#pragma once
#include "Core/Rendering/Buffer.h"
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "glm.hpp"



namespace Engine
{
    
    
    struct AllocatedBuffer
    {
        
        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo allocationInfo;
        
    };

    struct GPUMeshBuffers
    {
        AllocatedBuffer indexBuffer;
        AllocatedBuffer vertexBuffer;
        VkDeviceAddress vertexBufferAddress;
        
    };

    struct GPUDrawPushConstants
    {
        glm::mat4 worldMatrix;
        VkDeviceAddress vertexBuffer;
    };

    AllocatedBuffer CreateBuffer(size_t allocSize, VmaAllocator allocator, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);
    void DestroyBuffer(const AllocatedBuffer& buffer, VmaAllocator allocator);
    
}

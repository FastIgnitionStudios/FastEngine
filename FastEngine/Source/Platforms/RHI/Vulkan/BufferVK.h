#pragma once
#include "vulkan/vulkan.h"
#include "vk_mem_alloc.h"
#include "glm.hpp"


namespace Engine
{
    
    struct Vertex
    {
        glm::vec3 position;
        float uv_x;
        glm::vec3 normal;
        float uv_y;
        glm::vec4 color;
    };
    
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

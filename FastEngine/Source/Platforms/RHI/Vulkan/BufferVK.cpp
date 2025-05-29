#include "EnginePCH.h"
#include "BufferVK.h"

#include "UtilsVK.h"
#include "Utils/Log.h"

namespace Engine
{

    AllocatedBuffer CreateBuffer(size_t allocSize, VmaAllocator VmaAllocator, VkBufferUsageFlags usage,
        VmaMemoryUsage memoryUsage)
    {
        VkBufferCreateInfo bufferInfo = {.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        bufferInfo.pNext = nullptr;
        bufferInfo.size = allocSize;

        bufferInfo.usage = usage;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memoryUsage;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
        AllocatedBuffer newBuffer;
        
        
        VK_CHECK(vmaCreateBuffer(VmaAllocator, &bufferInfo, &allocInfo, &newBuffer.buffer, &newBuffer.allocation, &newBuffer.allocationInfo));
        // ENGINE_CORE_ERROR("Creating buffer: {}, allocation: {}", (void*)newBuffer.buffer, (void*)newBuffer.allocation);

        return newBuffer;
        
    }

    void DestroyBuffer(const AllocatedBuffer& buffer, VmaAllocator allocator)
    {
        // ENGINE_CORE_ERROR("Destroying buffer: {}, allocation: {}", (void*)buffer.buffer, (void*)buffer.allocation);
        vmaDestroyBuffer(allocator, buffer.buffer, buffer.allocation);
    }
    
}

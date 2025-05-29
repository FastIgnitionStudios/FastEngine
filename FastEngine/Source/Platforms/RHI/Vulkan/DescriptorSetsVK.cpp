#include "EnginePCH.h"
#include "DescriptorSetsVK.h"

#include <set>

#include "UtilsVK.h"
#include "spdlog/fmt/bundled/ranges.h"

namespace Engine
{
    void DescriptorLayoutBuilder::AddBinding(uint32_t binding, VkDescriptorType type)
    {
        VkDescriptorSetLayoutBinding newBind{};
        newBind.binding = binding;
        newBind.descriptorCount = 1;
        newBind.descriptorType = type;

        bindings.push_back(newBind);
    }

    void DescriptorLayoutBuilder::Clear()
    {
        bindings.clear();
    }

    VkDescriptorSetLayout DescriptorLayoutBuilder::Build(VkDevice device, VkShaderStageFlags shaderStages, void* pNext,
        VkDescriptorSetLayoutCreateFlags flags)
    {
        for (auto& bind : bindings)
        {
            bind.stageFlags |= shaderStages;
        }

        VkDescriptorSetLayoutCreateInfo info {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO};
        info.pNext = pNext;

        info.pBindings = bindings.data();
        info.bindingCount = (uint32_t)bindings.size();
        info.flags = flags;

        VkDescriptorSetLayout Set;
        VK_CHECK(vkCreateDescriptorSetLayout(device, &info, nullptr, &Set));
        return Set;
    }

    void DescriptorAllocator::InitPool(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (PoolSizeRatio ratio : poolRatios)
        {
            poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * maxSets)
            });
        }

        VkDescriptorPoolCreateInfo poolInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        poolInfo.flags = 0;
        poolInfo.maxSets = maxSets;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();

        vkCreateDescriptorPool(device, &poolInfo, nullptr, &pool);
    }

    void DescriptorAllocator::ClearDescriptors(VkDevice device)
    {
        vkResetDescriptorPool(device, pool, 0);
    }

    void DescriptorAllocator::DestroyPool(VkDevice device)
    {
        vkDestroyDescriptorPool(device, pool, nullptr);
    }

    VkDescriptorSet DescriptorAllocator::Allocate(VkDevice device, VkDescriptorSetLayout layout)
    {
        VkDescriptorSetAllocateInfo allocInfo {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocInfo.pNext = nullptr;
        allocInfo.descriptorPool = pool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VK_CHECK(vkAllocateDescriptorSets(device, &allocInfo, &ds));
        return ds;
    }

    void DescriptorAllocatorDynamic::Init(VkDevice device, uint32_t maxSets, std::span<PoolSizeRatio> poolRatios)
    {
        ratios.clear();

        for (auto r : poolRatios)
        {
            ratios.push_back(r);
        }

        VkDescriptorPool newPool = CreatePool(device, maxSets, ratios);

        setsPerPool = maxSets * 1.5;

        readyPools.push_back(newPool);
    }

    void DescriptorAllocatorDynamic::ClearPools(VkDevice device)
    {
        for (auto pool : readyPools)
        {
            vkResetDescriptorPool(device, pool, 0);
        }
        for (auto pool : fullPools)
        {
            vkResetDescriptorPool(device, pool, 0);
            readyPools.push_back(pool);
        }

        fullPools.clear();
    }

    void DescriptorAllocatorDynamic::DestroyPools(VkDevice device)
    {
        for (auto& pool : readyPools)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
        readyPools.clear();
        for (auto& pool : fullPools)
        {
            vkDestroyDescriptorPool(device, pool, nullptr);
        }
        fullPools.clear();
    }

    VkDescriptorSet DescriptorAllocatorDynamic::Allocate(VkDevice device, VkDescriptorSetLayout layout, void* pNext)
    {
        VkDescriptorPool poolToUse = GetPool(device);

        VkDescriptorSetAllocateInfo allocInfo {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO};
        allocInfo.pNext = pNext;
        allocInfo.descriptorPool = poolToUse;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        VkDescriptorSet ds;
        VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &ds);

        if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
        {
            fullPools.push_back(poolToUse);

            poolToUse = GetPool(device);
            allocInfo.descriptorPool = poolToUse;

            VkResult retry = vkAllocateDescriptorSets(device, &allocInfo, &ds);
            ENGINE_CORE_ASSERT(retry == VK_SUCCESS, "Failed to allocate descriptor set");
            
        }

        readyPools.push_back(poolToUse);
        return ds;
    }

    VkDescriptorPool DescriptorAllocatorDynamic::GetPool(VkDevice device)
    {
        VkDescriptorPool newPool;
        if (readyPools.size() != 0)
        {
            newPool = readyPools.back();
            readyPools.pop_back();
        }
        else
        {
            newPool = CreatePool(device, setsPerPool, ratios);

            setsPerPool = setsPerPool * 1.5;
            if (setsPerPool > 4092)
            {
                setsPerPool = 4092;
            }
        }

        return newPool;
    }

    VkDescriptorPool DescriptorAllocatorDynamic::CreatePool(VkDevice device, uint32_t setCount,
        std::span<PoolSizeRatio> poolRatios)
    {
        std::vector<VkDescriptorPoolSize> poolSizes;
        for (PoolSizeRatio ratio : poolRatios)
        {
            poolSizes.push_back(VkDescriptorPoolSize{
            .type = ratio.type,
            .descriptorCount = uint32_t(ratio.ratio * setCount)
            });
        }

        VkDescriptorPoolCreateInfo poolInfo = {.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO};
        poolInfo.flags = 0;
        poolInfo.maxSets = setCount;
        poolInfo.poolSizeCount = (uint32_t)poolSizes.size();
        poolInfo.pPoolSizes = poolSizes.data();

        VkDescriptorPool newPool;
        vkCreateDescriptorPool(device, &poolInfo, nullptr, &newPool);
        return newPool;
    }

    void DescriptorWriter::WriteImage(int binding, VkImageView view, VkSampler sampler, VkImageLayout layout,
        VkDescriptorType type)
    {
        VkDescriptorImageInfo& info = imageInfos.emplace_back(VkDescriptorImageInfo{
            .sampler = sampler,
            .imageView = view,
            .imageLayout = layout
        });

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pImageInfo = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::WriteBuffer(int binding, VkBuffer buffer, size_t size, size_t offset, VkDescriptorType type)
    {
        VkDescriptorBufferInfo& info = bufferInfos.emplace_back(VkDescriptorBufferInfo{
            .buffer = buffer,
            .offset = offset,
            .range = size
        });

        VkWriteDescriptorSet write = {.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};

        write.dstBinding = binding;
        write.dstSet = VK_NULL_HANDLE;
        write.descriptorCount = 1;
        write.descriptorType = type;
        write.pBufferInfo = &info;

        writes.push_back(write);
    }

    void DescriptorWriter::Clear()
    {
        imageInfos.clear();
        writes.clear();
        bufferInfos.clear();
    }

    void DescriptorWriter::UpdateSet(VkDevice device, VkDescriptorSet set)
    {
        for (VkWriteDescriptorSet& write : writes)
        {
            write.dstSet = set;
        }

        vkUpdateDescriptorSets(device, (uint32_t)writes.size(), writes.data(), 0, nullptr);
    }
}

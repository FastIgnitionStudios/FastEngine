#include "EnginePCH.h"
#include "DescriptorSetsVK.h"

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
        ENGINE_CORE_ASSERT(vkCreateDescriptorSetLayout(device, &info, nullptr, &Set) == VK_SUCCESS, "Failed to create descriptor set layout!");
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
        ENGINE_CORE_ASSERT(vkAllocateDescriptorSets(device, &allocInfo, &ds) == VK_SUCCESS, "Failed to allocate descriptor sets!");
        return ds;
    }
}

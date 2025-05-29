#pragma once
#include "vulkan/vulkan.h"
#include "Rendering/Material.h"
#include "Core.h"
#include "DescriptorSetsVK.h"
#include "glm.hpp"
#include "ImageVK.h"

namespace Engine
{


    
    struct MaterialPipelineVK
    {
        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;
    };

    struct MaterialInstanceVK
    {
        MaterialPipelineVK* pipeline;
        VkDescriptorSet materialSet;
        MaterialPass passType;
    };


    struct PBRMaterialVK
    {
        MaterialPipelineVK opaquePipeline;
        MaterialPipelineVK transparentPipeline;

        VkDescriptorSetLayout materialLayout;

        struct MaterialConstants
        {
            glm::vec4 colorFactors;
            glm::vec4 metallicRoughnessFactors;
            // Padding for extra values
            glm::vec4 extra[14];
        };

        struct MaterialResources
        {
            ImageVK::AllocatedImage colorImage;
            VkSampler colorSampler;
            ImageVK::AllocatedImage metallicRoughnessImage;
            VkSampler metalRoughnessSampler;
            VkBuffer dataBuffer;
            uint32_t dataBufferOffset;
        };

        DescriptorWriter writer;

        void BuildPipelines(RendererVK* engine);
        void ClearResources(VkDevice device, VmaAllocator allocator, const MaterialResources& resources);

        MaterialInstanceVK WriteMaterial(VkDevice device, MaterialPass pass, const MaterialResources& resources, DescriptorAllocatorDynamic& descriptorAllocator);


    };
}

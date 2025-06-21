#pragma once
#include "Core.h"
#include "DescriptorSetsVK.h"
#include "UtilsVK.h"
#include "glm.hpp"

namespace Engine
{

    struct ComputePushConstants
    {
        glm::vec4 data1;
        glm::vec4 data2;
        glm::vec4 data3;
        glm::vec4 data4;
    };
    
    struct ComputePipelineVKInitInfo
    {
        VkDevice Device;
        VkImageView DrawImageView;
        DeletionQueue* MainDeletionQueue;
    };
    
    class ComputePipelineVK : public Engine
    {
    public:
        ComputePipelineVK(ComputePipelineVKInitInfo info);
        ~ComputePipelineVK();

        VkPipeline GetPipeline() { return GradientPipeline; }
        VkPipelineLayout GetPipelineLayout() { return GradientPipelineLayout; }
        VkDescriptorSet GetDescriptorSet() { return DrawImageDescriptors; }
        
    private:

        void InitDescriptors();

        void InitPipeline();

        DescriptorAllocator globalDescriptorAllocator;

        VkDescriptorSet DrawImageDescriptors;
        VkDescriptorSetLayout DrawImageDescriptorLayout;

        ComputePipelineVKInitInfo initInfo;

        VkPipeline GradientPipeline;
        VkPipelineLayout GradientPipelineLayout;
        
    };
    // TODO: Add Compute shader builder and create Compute API to easily run code on the GPU - EXTRA: Use async-compute to optimise graphics/compute rendering
    // class ComputePipelineBuilder
    // {
    // public:
    //
    //     ComputePipelineBuilder() { Clear(); }
    //     
    //     void Clear();
    //     void SetComputeShader(VkShaderModule shaderModule);
    //     
    // };
}

#pragma once
#include "Core.h"
#include "DescriptorSetsVK.h"
#include "UtilsVK.h"

namespace Engine
{
    struct PipelineVKInitInfo
    {
        VkDevice Device;
        VkImageView DrawImageView;
        DeletionQueue* MainDeletionQueue;
    };
    
    class PipelineVK : public Engine
    {
    public:
        PipelineVK(PipelineVKInitInfo info);

        VkPipeline GetPipeline() { return GradientPipeline; }
        VkPipelineLayout GetPipelineLayout() { return GradientPipelineLayout; }
        VkDescriptorSet GetDescriptorSet() { return DrawImageDescriptors; }
        
    private:

        void InitDescriptors();

        void InitPipeline();

        DescriptorAllocator globalDescriptorAllocator;

        VkDescriptorSet DrawImageDescriptors;
        VkDescriptorSetLayout DrawImageDescriptorLayout;

        PipelineVKInitInfo initInfo;

        VkPipeline GradientPipeline;
        VkPipelineLayout GradientPipelineLayout;

        
    
    };
}

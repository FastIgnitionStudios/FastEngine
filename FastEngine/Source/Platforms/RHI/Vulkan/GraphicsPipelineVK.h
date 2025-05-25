#pragma once
#include "Core.h"
#include "vulkan/vulkan.h"


namespace Engine
{
    static VkPipelineShaderStageCreateInfo CreateShaderStageInfo(VkShaderStageFlagBits stage, VkShaderModule module);
    
    class PipelineBuilder
    {
    public:
        std::vector<VkPipelineShaderStageCreateInfo> shaderStages;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly;
        VkPipelineRasterizationStateCreateInfo rasterizer;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineMultisampleStateCreateInfo multisampling;
        VkPipelineLayout pipelineLayout;
        VkPipelineDepthStencilStateCreateInfo depthStencil;
        VkPipelineRenderingCreateInfo renderInfo;
        VkFormat colorAttachmentFormat;

        PipelineBuilder() { Clear(); }

        void Clear();
        void SetShaders(VkShaderModule vertexShader, VkShaderModule fragmentShader);
        void SetInputTopology(VkPrimitiveTopology topology);
        void SetPolygonMode(VkPolygonMode mode);
        void SetCullMode(VkCullModeFlags mode, VkFrontFace frontFace);
        void SetMultisamplingNone();
        void DisableBlending();
        void SetColorAttachmentFormat(VkFormat format);
        void SetDepthFormat(VkFormat format);
        void DisableDepthTest();
        void EnableDepthTest(bool depthWriteEnable, VkCompareOp depthCompareOp);
        

        VkPipeline BuildPipeline(VkDevice device);
    };
    
}

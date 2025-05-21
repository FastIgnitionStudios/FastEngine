#include "EnginePCH.h"
#include "PipelineVK.h"

#include "Filesystem.h"
#include "ShaderVK.h"
#include "Rendering/Shader.h"

namespace Engine
{

    PipelineVK::PipelineVK(PipelineVKInitInfo info) : initInfo(info)
    {
        InitDescriptors();
        InitPipeline();
    }

    void PipelineVK::InitDescriptors()
    {
        std::vector<DescriptorAllocator::PoolSizeRatio> sizes = {
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}
        };

        globalDescriptorAllocator.InitPool(initInfo.Device, 10, sizes);

        {
            DescriptorLayoutBuilder layoutBuilder;
            layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
            DrawImageDescriptorLayout = layoutBuilder.Build(initInfo.Device, VK_SHADER_STAGE_COMPUTE_BIT);
        }

        DrawImageDescriptors = globalDescriptorAllocator.Allocate(initInfo.Device, DrawImageDescriptorLayout);

        VkDescriptorImageInfo imgInfo {};
        imgInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
        imgInfo.imageView = initInfo.DrawImageView;

        VkWriteDescriptorSet drawImageWrite {};
        drawImageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        drawImageWrite.pNext = nullptr;

        drawImageWrite.dstBinding = 0;
        drawImageWrite.dstSet = DrawImageDescriptors;
        drawImageWrite.descriptorCount = 1;
        drawImageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
        drawImageWrite.pImageInfo = &imgInfo;

        vkUpdateDescriptorSets(initInfo.Device, 1, &drawImageWrite, 0, nullptr);

        initInfo.MainDeletionQueue->PushFunction([&]()
        {
            globalDescriptorAllocator.DestroyPool(initInfo.Device);
            vkDestroyDescriptorSetLayout(initInfo.Device, DrawImageDescriptorLayout, nullptr);
        });
        
    }

    void PipelineVK::InitPipeline()
    {
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext = nullptr;
        computeLayout.pSetLayouts = &DrawImageDescriptorLayout;
        computeLayout.setLayoutCount = 1;

        ENGINE_CORE_ASSERT(vkCreatePipelineLayout(initInfo.Device, &computeLayout, nullptr, &GradientPipelineLayout) == VK_SUCCESS, "Failed to create pipeline layout!");
        
        Ref<ShaderVK> computeDrawShader = Shader::Create((std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/gradient.comp.glsl").generic_string());
        computeDrawShader->CreateShaderModule(initInfo.Device);

        VkPipelineShaderStageCreateInfo stageInfo {};
        stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        stageInfo.pNext = nullptr;
        stageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
        stageInfo.module = computeDrawShader->GetShaderModule(ShaderType::COMPUTE);
        stageInfo.pName = "main";

        VkComputePipelineCreateInfo computePipeline {};
        computePipeline.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        computePipeline.pNext = nullptr;
        computePipeline.layout = GradientPipelineLayout;
        computePipeline.stage = stageInfo;

        ENGINE_CORE_ASSERT(vkCreateComputePipelines(initInfo.Device, VK_NULL_HANDLE, 1, &computePipeline, nullptr, &GradientPipeline) == VK_SUCCESS, "Failed to create compute pipeline!");
        initInfo.MainDeletionQueue->PushFunction([&]()
        {
            vkDestroyPipelineLayout(initInfo.Device, GradientPipelineLayout, nullptr);
            vkDestroyPipeline(initInfo.Device, GradientPipeline, nullptr);
        });
    }
}

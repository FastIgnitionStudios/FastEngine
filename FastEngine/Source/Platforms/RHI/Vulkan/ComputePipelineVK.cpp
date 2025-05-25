#include "EnginePCH.h"
#include "ComputePipelineVK.h"

#include "Filesystem.h"
#include "ShaderVK.h"
#include "Rendering/Shader.h"


namespace Engine
{

    ComputePipelineVK::ComputePipelineVK(ComputePipelineVKInitInfo info) : initInfo(info)
    {
        InitDescriptors();
        InitPipeline();
    }

    ComputePipelineVK::~ComputePipelineVK()
    {
        
        globalDescriptorAllocator.DestroyPool(initInfo.Device);
        vkDestroyDescriptorSetLayout(initInfo.Device, DrawImageDescriptorLayout, nullptr);
        
        vkDestroyPipelineLayout(initInfo.Device, GradientPipelineLayout, nullptr);
        vkDestroyPipeline(initInfo.Device, GradientPipeline, nullptr);
    }

    void ComputePipelineVK::InitDescriptors()
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


        DescriptorWriter writer;
        writer.WriteImage(0, initInfo.DrawImageView, VK_NULL_HANDLE, VK_IMAGE_LAYOUT_GENERAL, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE);
        writer.UpdateSet(initInfo.Device, DrawImageDescriptors);

        
    }

    void ComputePipelineVK::InitPipeline()
    {
        VkPipelineLayoutCreateInfo computeLayout{};
        computeLayout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        computeLayout.pNext = nullptr;
        computeLayout.pSetLayouts = &DrawImageDescriptorLayout;
        computeLayout.setLayoutCount = 1;

        VkPushConstantRange pushConstant{};
        pushConstant.offset = 0;
        pushConstant.size = sizeof(ComputePushConstants);
        pushConstant.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

        computeLayout.pPushConstantRanges = &pushConstant;
        computeLayout.pushConstantRangeCount = 1;

        VK_CHECK(vkCreatePipelineLayout(initInfo.Device, &computeLayout, nullptr, &GradientPipelineLayout));
        
        Ref<ShaderVK> computeDrawShader = Shader::Create((std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/gradient_color.comp.glsl").generic_string());
        computeDrawShader->CreateShaderModule(initInfo.Device);

        Ref<ShaderVK> computeSkyShader = Shader::Create((std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/gradient_inv_color.comp.glsl").generic_string());
        computeSkyShader->CreateShaderModule(initInfo.Device);

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

        VK_CHECK(vkCreateComputePipelines(initInfo.Device, VK_NULL_HANDLE, 1, &computePipeline, nullptr, &GradientPipeline));

    }
}

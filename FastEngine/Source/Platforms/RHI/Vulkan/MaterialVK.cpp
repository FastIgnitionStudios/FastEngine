#include "EnginePCH.h"
#include "MaterialVK.h"

#include "BufferVK.h"
#include "DeviceVK.h"
#include "Filesystem.h"
#include "GraphicsPipelineVK.h"
#include "RendererVK.h"
#include "ShaderVK.h"
#include "Rendering/Shader.h"

namespace Engine
{
    void PBRMaterialVK::BuildPipelines(RendererVK* engine)
    {
        VkShaderModule meshFragShader;
        VkShaderModule meshVertShader;
        Ref<ShaderVK> meshShader = Shader::Create(Filesystem::GetEngineAssetPath("Shaders/mesh.vert").generic_string(), Filesystem::GetEngineAssetPath("Shaders/mesh.frag").generic_string());
        meshShader->CreateShaderModule(engine->GetDevice());
        meshFragShader = meshShader->GetShaderModule(ShaderType::FRAGMENT);
        meshVertShader = meshShader->GetShaderModule(ShaderType::VERTEX);

        VkPushConstantRange matrixRange{};
        matrixRange.offset = 0;
        matrixRange.size = sizeof(GPUDrawPushConstants);
        matrixRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        DescriptorLayoutBuilder layoutBuilder;
        layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        layoutBuilder.AddBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        layoutBuilder.AddBinding(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

        materialLayout = layoutBuilder.Build(engine->GetDevice(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        
        VkDescriptorSetLayout layouts[] =
        {
            engine->GetSceneDataLayout(), materialLayout
        };

        VkPipelineLayoutCreateInfo meshLayoutInfo {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        meshLayoutInfo.setLayoutCount = 2;
        meshLayoutInfo.pSetLayouts = layouts;
        meshLayoutInfo.pushConstantRangeCount = 1;
        meshLayoutInfo.pPushConstantRanges = &matrixRange;

        VkPipelineLayout newLayout;
        VK_CHECK(vkCreatePipelineLayout(engine->GetDevice(), &meshLayoutInfo, nullptr, &newLayout));

        opaquePipeline.pipelineLayout = newLayout;
        transparentPipeline.pipelineLayout = newLayout;

        PipelineBuilder pipelineBuilder;
        pipelineBuilder.SetShaders(meshVertShader, meshFragShader);
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

        pipelineBuilder.SetColorAttachmentFormat(engine->GetDrawImage().imageFormat);
        pipelineBuilder.SetDepthFormat(engine->GetDepthImage().imageFormat);

        pipelineBuilder.pipelineLayout = newLayout;

        opaquePipeline.pipeline = pipelineBuilder.BuildPipeline(engine->GetDevice());

        pipelineBuilder.EnableBlendingAdditive();
        pipelineBuilder.EnableDepthTest(false, VK_COMPARE_OP_GREATER_OR_EQUAL);

        transparentPipeline.pipeline = pipelineBuilder.BuildPipeline(engine->GetDevice());

        vkDestroyShaderModule(engine->GetDevice(), meshFragShader, nullptr);
        vkDestroyShaderModule(engine->GetDevice(), meshVertShader, nullptr);
        
    }

    void PBRMaterialVK::ClearResources(VkDevice device, VmaAllocator allocator, const MaterialResources& resources)
    {
        vkDestroyDescriptorSetLayout(device,materialLayout,nullptr);
        vkDestroyPipelineLayout(device,transparentPipeline.pipelineLayout,nullptr);

        vkDestroyPipeline(device, transparentPipeline.pipeline, nullptr);
        vkDestroyPipeline(device, opaquePipeline.pipeline, nullptr);
        
    }

    MaterialInstanceVK PBRMaterialVK::WriteMaterial(VkDevice device, MaterialPass pass,
        const MaterialResources& resources, DescriptorAllocatorDynamic& descriptorAllocator)
    {
        MaterialInstanceVK matData;
        matData.passType = pass;
        if (pass == MaterialPass::Transparent)
        {
            matData.pipeline = &transparentPipeline;
        }
        else
        {
            matData.pipeline = &opaquePipeline;
        }

        matData.materialSet = descriptorAllocator.Allocate(device, materialLayout);
        writer.Clear();
        writer.WriteBuffer(0, resources.dataBuffer, sizeof(MaterialConstants), resources.dataBufferOffset, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER );
        writer.WriteImage(1, resources.colorImage.imageView, resources.colorSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        writer.WriteImage(2, resources.metallicRoughnessImage.imageView, resources.metalRoughnessSampler, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        
        writer.UpdateSet(device, matData.materialSet);
        

        return matData;
    }
}

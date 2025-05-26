#pragma once
#include "CommandStructureVK.h"
#include "ComputePipelineVK.h"
#include "SwapchainVK.h"
#include "UtilsVK.h"
#include "BufferVK.h"
#include "DeviceVK.h"
#include "MaterialVK.h"
#include "Rendering/Renderer.h"


    


namespace Engine
{
    class Device;

    struct GPUMeshBuffers;
    struct Vertex;

    struct GPUSceneData
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewproj;
        glm::vec4 ambientColor;
        glm::vec4 sunlightDirection;
        glm::vec4 sunlightColor;
    };
    
    struct ComputeEffect
    {
        const char* name;

        VkPipeline pipeline;
        VkPipelineLayout pipelineLayout;

        ComputePushConstants data;
    };

    struct MeshAssetVK;
    
    class RendererVK : public Renderer
    {
    public:

        RendererVK();
        virtual ~RendererVK();

        void DrawFrame() override;
        void PreFrame() override;
        void DrawViewport() override;
        RenderObject CreateRenderObject() override;
        
    
        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
        void DrawImgui(VkCommandBuffer cmd, VkImageView targetView);
        void DrawGeometry(VkCommandBuffer cmd);
        GPUMeshBuffers UploadMeshes(std::span<uint32_t> indices, std::span<Vertex> vertices);

        VkDevice GetDevice() { return Ref<DeviceVK>(Device)->GetDevice(); }
        GPUSceneData GetSceneData() { return sceneData; }
        VkDescriptorSetLayout GetSceneDataLayout() { return SceneDataLayout; }
        ImageVK::AllocatedImage GetDrawImage() { return Swapchain->GetDrawImage(); }
        ImageVK::AllocatedImage GetDepthImage() { return Swapchain->GetDepthImage(); }
        

    private:

        void CreateInstance();

        // TODO: Replace with function to call queue of lambdas
        void DrawBackground(VkCommandBuffer cmd);

        

        VkSubmitInfo2 CreateSubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);
        VkRenderingInfo CreateRenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment, VkRenderingAttachmentInfo* depthAttachment);
        

            
        VkInstance Instance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        Ref<Device> Device;
        Ref<SwapchainVK> Swapchain;
        Ref<CommandStructureVK> CommandStructure;
        Ref<ComputePipelineVK> GradientPipeline;

        bool resizeRequested = false;

        VkPipeline meshPipeline;
        VkPipelineLayout meshPipelineLayout;
        

        DeletionQueue MainDeletionQueue;
        
        
        VmaAllocator Allocator;

        std::vector<ComputeEffect> backgroundEffects;
        int currentBackgroundEffect{0};

        std::vector<std::shared_ptr<MeshAssetVK>> testMeshes;

        GPUSceneData sceneData;

        MaterialInstanceVK defaultData;
        PBRMaterialVK metalRoughnessMaterial;

        VkDescriptorSetLayout SceneDataLayout;

        ImageVK::AllocatedImage whiteImage;
        ImageVK::AllocatedImage blackImage;
        ImageVK::AllocatedImage greyImage;
        ImageVK::AllocatedImage errorCheckerboardImage;
        

        VkSampler defaultSamplerLinear;
        VkSampler defaultSamplerNearest;

        VkDescriptorSetLayout SingleImageLayout;

        VkCommandBuffer currentCommandBuffer;
        uint32_t CurrentSwapchainImageIndex;

        bool isReady = false;

        
    };
    
}

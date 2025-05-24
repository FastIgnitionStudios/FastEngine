#pragma once
#include "CommandStructureVK.h"
#include "ComputePipelineVK.h"
#include "SwapchainVK.h"
#include "UtilsVK.h"
#include "BufferVK.h"
#include "Rendering/Renderer.h"


    


namespace Engine
{
    class Device;

    struct GPUMeshBuffers;
    struct Vertex;
    
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
        
    
        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
        void DrawImgui(VkCommandBuffer cmd, VkImageView targetView);
        void DrawGeometry(VkCommandBuffer cmd);
        GPUMeshBuffers UploadMeshes(std::span<uint32_t> indices, std::span<Vertex> vertices);
        

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

        VkPipeline graphicsPipeline;
        VkPipelineLayout graphicsPipelineLayout;

        VkPipeline meshPipeline;
        VkPipelineLayout meshPipelineLayout;

        GPUMeshBuffers Rectangle;

        DeletionQueue MainDeletionQueue;
        
        
        VmaAllocator Allocator;

        std::vector<ComputeEffect> backgroundEffects;
        int currentBackgroundEffect{0};

        std::vector<std::shared_ptr<MeshAssetVK>> testMeshes;
        
    };
    
}

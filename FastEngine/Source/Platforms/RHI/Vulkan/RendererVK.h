#pragma once
#include "CommandStructureVK.h"
#include "ImageVK.h"
#include "PipelineVK.h"
#include "SwapchainVK.h"
#include "UtilsVK.h"
#include "vk_mem_alloc.h"
#include "Rendering/Renderer.h"
#include "vulkan/vulkan.h"


    


namespace Engine
{
    class Device;
    
    
    class RendererVK : public Renderer
    {
    public:

        RendererVK();
        virtual ~RendererVK();

        virtual void DrawFrame() override;
        
    
        void ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function);
        void DrawImgui(VkCommandBuffer cmd, VkImageView targetView);
        

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
        Ref<PipelineVK> GradientPipeline;

        DeletionQueue MainDeletionQueue;
        
        
        VmaAllocator Allocator;
        
    };
    
}

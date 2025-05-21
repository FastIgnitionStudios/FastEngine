#pragma once
#include "CommandStructureVK.h"
#include "ImageVK.h"
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

        

    private:

        void CreateInstance();

        // TODO: Replace with function to call queue of lambdas
        void DrawBackground(VkCommandBuffer cmd);

        

        VkSubmitInfo2 CreateSubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);
        
        

            
        VkInstance Instance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        Ref<Device> Device;
        Ref<SwapchainVK> Swapchain;
        Ref<CommandStructureVK> CommandStructure;

        DeletionQueue MainDeletionQueue;
        
        
        VmaAllocator Allocator;
        
    };


}

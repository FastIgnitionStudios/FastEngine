#pragma once
#include "CommandStructureVK.h"
#include "SwapchainVK.h"
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

        
        // TODO: Move this function into CommandStructureVK.h
        VkCommandBufferBeginInfo CreateCommandBufferBeginInfo(VkCommandBufferUsageFlags flags = 0);

        VkSubmitInfo2 CreateSubmitInfo(VkCommandBufferSubmitInfo* cmd, VkSemaphoreSubmitInfo* signalSemaphoreInfo, VkSemaphoreSubmitInfo* waitSemaphoreInfo);
        
        

            
        VkInstance Instance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        Ref<Device> Device;
        Ref<SwapchainVK> Swapchain;
        Ref<CommandStructureVK> CommandStructure;

        
        
    };


}

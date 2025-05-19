#pragma once
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

    

    private:

        void CreateInstance();

    
        VkInstance Instance;
        VkDebugUtilsMessengerEXT DebugMessenger;
        Ref<Device> Device;

        
        
    };
}

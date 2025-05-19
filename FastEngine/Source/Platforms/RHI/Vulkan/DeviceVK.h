#pragma once
#include <vulkan/vulkan.h>

#include "Platforms/WindWindow.h"
#include "Rendering/Device.h"
#include "vkbootstrap/VkBootstrap.h"


namespace Engine
{

    struct DeviceInitInfo
    {
        vkb::Instance instance;
    };
    
    class DeviceVK : public Device
    {
    public:
        DeviceVK();
        virtual ~DeviceVK();

        static Ref<Device> InitVkDevice(const DeviceInitInfo& initInfo);

    private:

        void CreateDevice();

        VkSurfaceKHR surface;
        vkb::Instance instance;
        GLFWwindow* window;
        
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
        
    };
}

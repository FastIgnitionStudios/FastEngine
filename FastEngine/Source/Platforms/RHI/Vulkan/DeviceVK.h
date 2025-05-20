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

        VkDevice GetDevice() { return logicalDevice; }
        VkPhysicalDevice GetPhysicalDevice() { return physicalDevice; }
        VkSurfaceKHR GetSurface() { return surface; }
        vkb::Device GetVKBDevice() { return vkbDevice; }

    private:

        void CreateDevice();

        VkSurfaceKHR surface;
        vkb::Instance instance;
        GLFWwindow* window;
        
        VkPhysicalDevice physicalDevice;
        VkDevice logicalDevice;
        vkb::Device vkbDevice;
        
    };
}

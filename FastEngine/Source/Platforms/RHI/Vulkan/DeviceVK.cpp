#include "EnginePCH.h"
#include "DeviceVK.h"

#include "EngineApp.h"


namespace Engine
{
    DeviceVK::DeviceVK()
    {
    }

    DeviceVK::~DeviceVK()
    {
    }

    Ref<Device> DeviceVK::InitVkDevice(const DeviceInitInfo& initInfo)
    {
        auto device = Ref<DeviceVK>::Create();
        device->instance = initInfo.instance;
        device->window = (GLFWwindow*)EngineApp::GetEngineApp()->GetWindow()->GetNativeWindow();
        device->CreateDevice();
        return device;
    }

    void DeviceVK::CreateDevice()
    {
        glfwCreateWindowSurface(instance, window, NULL, &surface);

        //vulkan 1.3 features
        VkPhysicalDeviceVulkan13Features features{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES };
        features.dynamicRendering = true;
        features.synchronization2 = true;

        //vulkan 1.2 features
        VkPhysicalDeviceVulkan12Features features12{ .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
        features12.bufferDeviceAddress = true;
        features12.descriptorIndexing = true;

        vkb::PhysicalDeviceSelector selector {instance};
        vkb::PhysicalDevice physDevice = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features)
            .set_required_features_12(features12)
            .set_surface(surface)
            .select()
            .value();

        vkb::DeviceBuilder deviceBuilder {physDevice};

        vkbDevice = deviceBuilder.build().value();

        logicalDevice = vkbDevice.device;
        physicalDevice = physDevice.physical_device;

        

        
        
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        ENGINE_CORE_INFO("Vulkan device created: {0}", deviceProperties.deviceName);
    }
}

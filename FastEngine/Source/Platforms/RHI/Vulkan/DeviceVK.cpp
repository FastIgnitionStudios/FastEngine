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

        vkb::PhysicalDeviceSelector selector {instance};
        vkb::PhysicalDevice physDevice = selector
            .set_minimum_version(1, 3)
            .set_surface(surface)
            .select()
            .value();

        vkb::DeviceBuilder deviceBuilder {physDevice};

        vkb::Device vkbDevice = deviceBuilder.build().value();

        logicalDevice = vkbDevice.device;
        physicalDevice = physDevice.physical_device;

        
        
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
        ENGINE_CORE_INFO("Vulkan device created: {0}", deviceProperties.deviceName);
    }
}

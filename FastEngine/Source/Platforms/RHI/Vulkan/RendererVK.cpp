#include "EnginePCH.h"
#include "RendererVK.h"
#include "Utils/Log.h"
#include "DeviceVK.h"
#include "EngineApp.h"
#include "vkbootstrap/VkBootstrap.h"

#ifndef ENGINE_RELEASE
constexpr bool bUseValidationLayers = true;
    #else
constexpr bool bUseValidationLayers = false;
#endif

static VkBool32 EventCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            {
                ENGINE_CORE_WARN("Vulkan Warning: {0}", pCallbackData->pMessage);
                break;
            }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            {
                ENGINE_CORE_WARN("Vulkan Error: {0}", pCallbackData->pMessage);
                break;
            }
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            {
                ENGINE_CORE_INFO("Vulkan Info: {0}", pCallbackData->pMessage);
                break;
            }
        
    }
    return VK_FALSE;
}

namespace Engine
{


    RendererVK::RendererVK()
    {
        CreateInstance();
    }

    RendererVK::~RendererVK()
    {
    }

    void RendererVK::CreateInstance()
    {
        /*    Create Vulkan Instance  */
        
        vkb::InstanceBuilder builder;
        auto instance = builder.set_app_name("Fast Engine")
            .request_validation_layers(bUseValidationLayers)
            .set_debug_callback(EventCallback)
            .require_api_version(1, 3, 0)
            .build();

        ENGINE_CORE_INFO("Vulkan Instance created");
        vkb::Instance vkbInstance = instance.value();
        Instance = vkbInstance.instance;
        DebugMessenger = vkbInstance.debug_messenger;

        /*    Create Vulkan Device    */

        DeviceInitInfo vkDeviceInitInfo;
        vkDeviceInitInfo.instance = instance.value();
        Device = DeviceVK::InitVkDevice(vkDeviceInitInfo);

        /*    Create Vulkan Swapchain    */

        SwapchainInitInfo vkSwapchainInitInfo{};
        vkSwapchainInitInfo.device = Ref<DeviceVK>(Device)->GetDevice();
        vkSwapchainInitInfo.physicalDevice = Ref<DeviceVK>(Device)->GetPhysicalDevice();
        vkSwapchainInitInfo.surface = Ref<DeviceVK>(Device)->GetSurface();
        uint32_t windowWidth, windowHeight;
        windowWidth = EngineApp::GetEngineApp()->GetWindow()->GetWidth();
        windowHeight = EngineApp::GetEngineApp()->GetWindow()->GetHeight();
        vkSwapchainInitInfo.width = windowWidth;
        vkSwapchainInitInfo.height = windowHeight;

        Swapchain = Ref<SwapchainVK>::Create(vkSwapchainInitInfo);
        
    }
}

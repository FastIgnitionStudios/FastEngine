#include "EnginePCH.h"
#include "SwapchainVK.h"

#include "EngineApp.h"
#include "vkbootstrap/VkBootstrap.h"

namespace Engine
{
    SwapchainVK::SwapchainVK(const SwapchainInitInfo& initInfo)
    {
        SwapchainInfo = initInfo;
        CreateSwapchain(initInfo.width, initInfo.height);
    }

    SwapchainVK::~SwapchainVK()
    {
        DestroySwapchain();
    }

    void SwapchainVK::CreateSwapchain(uint32_t width, uint32_t height)
    {
        vkb::SwapchainBuilder swapchainBuilder{SwapchainInfo.physicalDevice, SwapchainInfo.device, SwapchainInfo.surface};

        SwapchainImageFormat = VK_FORMAT_B8G8R8A8_UNORM;

        vkb::Swapchain vkbSwapchain = swapchainBuilder
            .set_desired_format(VkSurfaceFormatKHR{ .format = SwapchainImageFormat, .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(width, height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build()
            .value();

        SwapchainExtent = vkbSwapchain.extent;
        Swapchain = vkbSwapchain.swapchain;
        SwapchainImages = vkbSwapchain.get_images().value();
        SwapchainImageViews = vkbSwapchain.get_image_views().value();

        ENGINE_CORE_INFO("Vulkan Swapchain created");
    }

    void SwapchainVK::DestroySwapchain()
    {
        vkDestroySwapchainKHR(SwapchainInfo.device, Swapchain, nullptr);

        for (int i = 0; i < SwapchainImageViews.size(); ++i)
        {
            vkDestroyImageView(SwapchainInfo.device, SwapchainImageViews[i], nullptr);
        }
    }
}

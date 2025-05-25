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

        VkExtent3D drawImageExtent = {
        width, height, 1};

        DrawImage.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT;
        DepthImage.imageFormat = VK_FORMAT_D32_SFLOAT;
        DrawImage.imageExtent = drawImageExtent;
        DepthImage.imageExtent = drawImageExtent;

        VkImageUsageFlags depthImageUsages{};
        depthImageUsages |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

        VkImageUsageFlags drawImageUsages{};
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_STORAGE_BIT;
        drawImageUsages |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        VkImageCreateInfo drawImageInfo = ImageVK::CreateImageInfo(DrawImage.imageFormat, drawImageUsages, drawImageExtent);
        VkImageCreateInfo depthImageInfo = ImageVK::CreateImageInfo(DepthImage.imageFormat, depthImageUsages, drawImageExtent);

        VmaAllocationCreateInfo drawImageAllocInfo {};
        drawImageAllocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        drawImageAllocInfo.requiredFlags = VkMemoryPropertyFlags(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        vmaCreateImage(SwapchainInfo.allocator, &drawImageInfo, &drawImageAllocInfo, &DrawImage.image, &DrawImage.allocation, nullptr);

        vmaCreateImage(SwapchainInfo.allocator, &depthImageInfo, &drawImageAllocInfo, &DepthImage.image, &DepthImage.allocation, nullptr);
        

        VkImageViewCreateInfo drawImageViewInfo = ImageVK::CreateImageViewInfo(DrawImage.imageFormat, DrawImage.image, VK_IMAGE_ASPECT_COLOR_BIT);
        VkImageViewCreateInfo depthImageViewInfo = ImageVK::CreateImageViewInfo(DepthImage.imageFormat, DepthImage.image, VK_IMAGE_ASPECT_DEPTH_BIT);

        VK_CHECK(vkCreateImageView(SwapchainInfo.device, &drawImageViewInfo, nullptr, &DrawImage.imageView));
        VK_CHECK(vkCreateImageView(SwapchainInfo.device, &depthImageViewInfo, nullptr, &DepthImage.imageView));

        SwapchainInfo.MainDeletionQueue->PushFunction([&]()
        {
            vkDestroyImageView(SwapchainInfo.device, DrawImage.imageView, nullptr);
            vmaDestroyImage(SwapchainInfo.allocator, DrawImage.image, DrawImage.allocation);

            vkDestroyImageView(SwapchainInfo.device, DepthImage.imageView, nullptr);
            vmaDestroyImage(SwapchainInfo.allocator, DepthImage.image, DepthImage.allocation);
        });
        
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

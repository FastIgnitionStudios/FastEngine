#pragma once
#include <vulkan/vulkan.h>

#include "Core.h"
#include "ImageVK.h"
#include "UtilsVK.h"

namespace Engine
{
    struct SwapchainInitInfo
    {
        uint32_t width;
        uint32_t height;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
        VmaAllocator allocator;
        DeletionQueue* MainDeletionQueue;
        std::function<void()> OnSwapchainResized;
    };
    
    class SwapchainVK : public Engine
    {
    public:
        SwapchainVK(const SwapchainInitInfo& initInfo);
        ~SwapchainVK();

        VkSwapchainKHR GetSwapchain() { return Swapchain; }
        VkImage GetSwapchainImage(int imageIndex) { return SwapchainImages[imageIndex]; }
        VkImageView GetSwapchainImageView(int imageIndex) { return SwapchainImageViews[imageIndex]; }
        VkExtent2D GetSwapchainExtent() { return SwapchainExtent; }
        VkFormat GetSwapchainImageFormat() { return SwapchainImageFormat; }

        ImageVK::AllocatedImage GetDrawImage() { return DrawImage; }
        ImageVK::AllocatedImage GetDepthImage() { return DepthImage; }

        void SetDrawExtent(uint32_t width, uint32_t height) { DrawExtent = { width, height }; }
        VkExtent2D GetDrawExtent() { return DrawExtent; }

        void ResizeSwapchain(const SwapchainInitInfo& initInfo);

    private:

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();

        SwapchainInitInfo SwapchainInfo;
        
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;

        ImageVK::AllocatedImage DrawImage;
        ImageVK::AllocatedImage DepthImage;
        VkExtent2D DrawExtent;

        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VkExtent2D SwapchainExtent;
    
    };
}

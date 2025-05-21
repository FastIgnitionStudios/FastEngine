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
    };
    
    class SwapchainVK : public Engine
    {
    public:
        SwapchainVK(const SwapchainInitInfo& initInfo);
        ~SwapchainVK();

        VkSwapchainKHR GetSwapchain() { return Swapchain; }
        VkImage GetSwapchainImage(int imageIndex) { return SwapchainImages[imageIndex]; }
        VkExtent2D GetSwapchainExtent() { return SwapchainExtent; }

        ImageVK::AllocatedImage GetDrawImage() { return DrawImage; }

        void SetDrawExtent(uint32_t width, uint32_t height) { DrawExtent = { width, height }; }
        VkExtent2D GetDrawExtent() { return DrawExtent; }

    private:

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();

        SwapchainInitInfo SwapchainInfo;
        
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;

        ImageVK::AllocatedImage DrawImage;
        VkExtent2D DrawExtent;

        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VkExtent2D SwapchainExtent;
    
    };
}

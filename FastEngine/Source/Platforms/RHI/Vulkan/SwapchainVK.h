#pragma once
#include <vulkan/vulkan.h>

#include "Core.h"

namespace Engine
{
    struct SwapchainInitInfo
    {
        uint32_t width;
        uint32_t height;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
    };
    
    class SwapchainVK : public Engine
    {
    public:
        SwapchainVK(const SwapchainInitInfo& initInfo);
        ~SwapchainVK();

    private:

        void CreateSwapchain(uint32_t width, uint32_t height);
        void DestroySwapchain();

        SwapchainInitInfo SwapchainInfo;
        
        VkSwapchainKHR Swapchain;
        VkFormat SwapchainImageFormat;

        std::vector<VkImage> SwapchainImages;
        std::vector<VkImageView> SwapchainImageViews;
        VkExtent2D SwapchainExtent;
    
    };
}

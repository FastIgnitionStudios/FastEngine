#pragma once

#include "vulkan/vulkan.h"

namespace Engine
{
    namespace ImageVK
    {
        void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
        VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspectMask);
    }
}

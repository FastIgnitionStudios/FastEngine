#pragma once

#include <vk_mem_alloc.h>

namespace Engine
{
    class RendererVK;
    
    namespace ImageVK
    {

        
        struct AllocatedImage
        {
            VkImage image;
            VkImageView imageView;
            VmaAllocation allocation;
            VkExtent3D imageExtent;
            VkFormat imageFormat;
        };
        
        void TransitionImage(VkCommandBuffer cmd, VkImage image, VkImageLayout currentLayout, VkImageLayout newLayout);
        VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspectMask);
        VkImageCreateInfo CreateImageInfo(VkFormat format, VkImageUsageFlags usageFlags, VkExtent3D extent);
        VkImageViewCreateInfo CreateImageViewInfo(VkFormat format, VkImage image, VkImageAspectFlags aspectFlags);
        VkRenderingAttachmentInfo CreateAttachmentInfo(VkImageView view, VkClearValue* clear, VkImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingAttachmentInfo CreateDepthAttachmentInfo(VkImageView view, VkImageLayout layout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        void CopyImageToImage(VkCommandBuffer cmd, VkImage source, VkImage destination, VkExtent2D srcSize, VkExtent2D dstSize);

        AllocatedImage CreateImage(VkDevice device, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false, VmaAllocator allocator = nullptr);
        AllocatedImage CreateImage(VkDevice device, RendererVK* renderer, void* data, VkExtent3D size, VkFormat format, VkImageUsageFlags usage, bool mipmapped = false, VmaAllocator allocator = nullptr);
        void DestroyImage(VkDevice device, VmaAllocator allocator, const AllocatedImage& image);
    }
}

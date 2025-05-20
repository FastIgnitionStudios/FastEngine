#include "EnginePCH.h"
#include "RendererVK.h"
#include "Utils/Log.h"
#include "DeviceVK.h"
#include "EngineApp.h"
#include "ImageVK.h"
#include "vkbootstrap/VkBootstrap.h"

#ifndef ENGINE_RELEASE
constexpr bool bUseValidationLayers = true;
#else
constexpr bool bUseValidationLayers = false;
#endif

static VkBool32 EventCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                              VkDebugUtilsMessageTypeFlagsEXT messageTypes,
                              const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
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
        MainDeletionQueue.Flush();
    }

    void RendererVK::DrawFrame()
    {
        Ref<DeviceVK> deviceRef = Ref<DeviceVK>(Device);

        ENGINE_CORE_ASSERT(
            vkWaitForFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence, true,
                1000000000) == VK_SUCCESS, "vkWaitForFences failed");

        CommandStructure->GetCurrentFrame().DeletionQueue.Flush();

        ENGINE_CORE_ASSERT(
            vkResetFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence) == VK_SUCCESS,
            "vkWaitForFences failed");

        uint32_t swapchainImageIndex;
        ENGINE_CORE_ASSERT(
            vkAcquireNextImageKHR(deviceRef->GetDevice(), Swapchain->GetSwapchain(), 1000000000, CommandStructure->
                GetCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex) == VK_SUCCESS,
            "vkAcquireNextImageKHR failed");

        VkCommandBuffer cmd = CommandStructure->GetCurrentFrame().commandBuffer;

        ENGINE_CORE_ASSERT(vkResetCommandBuffer(cmd, 0) == VK_SUCCESS, "vkResetCommandBuffer failed");

        VkCommandBufferBeginInfo beginInfo = CreateCommandBufferBeginInfo(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        ENGINE_CORE_ASSERT(vkBeginCommandBuffer(cmd, &beginInfo) == VK_SUCCESS, "vkBeginCommandBuffer failed");

        ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex), VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_GENERAL);

        VkClearColorValue clearValue;
        float flash = std::abs(std::sin(CommandStructure->GetFrameNumber() / 120.f));
        clearValue = {{0.0f, 0.0f, flash, 1.0f}};

        VkImageSubresourceRange clearRange = ImageVK::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdClearColorImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex), VK_IMAGE_LAYOUT_GENERAL,
                             &clearValue, 1, &clearRange);

        ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex), VK_IMAGE_LAYOUT_GENERAL,
                                 VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        ENGINE_CORE_ASSERT(vkEndCommandBuffer(cmd) == VK_SUCCESS, "vkEndCommandBuffer failed");

        VkCommandBufferSubmitInfo cmdinfo = CommandStructure->CreateCommandBufferSubmitInfo(cmd);

        VkSemaphoreSubmitInfo waitInfo = CommandStructure->CreateSemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, CommandStructure->GetCurrentFrame().swapchainSemaphore);
        VkSemaphoreSubmitInfo signalInfo = CommandStructure->CreateSemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, CommandStructure->GetCurrentFrame().renderSemaphore);

        VkSubmitInfo2 submit = CreateSubmitInfo(&cmdinfo, &signalInfo, &waitInfo);


        ENGINE_CORE_ASSERT(
            vkQueueSubmit2(CommandStructure->GetGraphicsQueue(), 1, &submit, CommandStructure->GetCurrentFrame().
                renderFence) == VK_SUCCESS, "vkQueuePresentKHR failed");

        VkSwapchainKHR swapchain = Swapchain->GetSwapchain();

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &CommandStructure->GetCurrentFrame().renderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &swapchainImageIndex;

        ENGINE_CORE_ASSERT(vkQueuePresentKHR(CommandStructure->GetGraphicsQueue(), &presentInfo) == VK_SUCCESS,
                           "vkQueuePresentKHR failed");

        CommandStructure->NewFrame();
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

        /*    Initialize Vulkan Memory Allocator      */

        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = Ref<DeviceVK>(Device)->GetPhysicalDevice();
        allocatorInfo.device = Ref<DeviceVK>(Device)->GetVKBDevice();
        allocatorInfo.instance = Instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
        vmaCreateAllocator(&allocatorInfo, &Allocator);

        MainDeletionQueue.PushFunction([&]()
        {
            vmaDestroyAllocator(Allocator);
        });

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
        vkSwapchainInitInfo.allocator = Allocator;
        vkSwapchainInitInfo.MainDeletionQueue = &MainDeletionQueue;

        Swapchain = Ref<SwapchainVK>::Create(vkSwapchainInitInfo);

        /*    Create Vulkan Command structure     */

        CommandQueueInfo vkCommandQueueInfo{};
        vkCommandQueueInfo.device = Ref<DeviceVK>(Device)->GetVKBDevice();

        CommandStructure = Ref<CommandStructureVK>::Create(vkCommandQueueInfo);

        ENGINE_CORE_INFO("Vulkan Command Buffer Created");


    }

    VkCommandBufferBeginInfo RendererVK::CreateCommandBufferBeginInfo(VkCommandBufferUsageFlags flags)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pNext = nullptr;

        beginInfo.pInheritanceInfo = nullptr;
        beginInfo.flags = flags;
        return beginInfo;
    }

    VkSubmitInfo2 RendererVK::CreateSubmitInfo(VkCommandBufferSubmitInfo* cmd,
                                               VkSemaphoreSubmitInfo* signalSemaphoreInfo,
                                               VkSemaphoreSubmitInfo* waitSemaphoreInfo)
    {
        VkSubmitInfo2 info{};
        info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
        info.pNext = nullptr;

        info.waitSemaphoreInfoCount = waitSemaphoreInfo == nullptr ? 0 : 1;
        info.pWaitSemaphoreInfos = waitSemaphoreInfo;

        info.signalSemaphoreInfoCount = signalSemaphoreInfo == nullptr ? 0 : 1;
        info.pSignalSemaphoreInfos = signalSemaphoreInfo;

        info.commandBufferInfoCount = 1;
        info.pCommandBufferInfos = cmd;

        return info;
    }
}

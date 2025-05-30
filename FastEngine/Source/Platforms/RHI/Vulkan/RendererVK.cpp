#include "EnginePCH.h"
#include "RendererVK.h"

#include <filesystem>

#define GLM_ENABLE_EXPERIMENTAL


#include "vk_mem_alloc.h"

#include "MeshVK.h"
#include "Utils/Log.h"
#include "DeviceVK.h"
#include "EngineApp.h"
#include "GraphicsPipelineVK.h"
#include "ImGUIVK.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_vulkan.h"
#include "ShaderVK.h"
#include "vkbootstrap/VkBootstrap.h"

#include <gtx/transform.hpp>
#include <glm/glm.hpp>

#include "fastgltf/types.hpp"
#include "Rendering/Camera.h"
#include "Scene/Components.h"
#include "Scene/Scene.h"

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
        vkDeviceWaitIdle(Ref<DeviceVK>(Device)->GetDevice());
        loadedMeshes.clear();
        testMeshes.clear();
        Swapchain = nullptr;
        CommandStructure = nullptr;
        MainDeletionQueue.Flush();
        globalDescriptorAllocator.DestroyPools(Device.As<DeviceVK>()->GetDevice());
        GradientPipeline = nullptr;
        Device = nullptr;
    }

    void RendererVK::DrawFrame()
    {
        Ref<DeviceVK> deviceRef = Ref<DeviceVK>(Device);

        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(currentCommandBuffer);

        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_GENERAL,
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetDepthImage().image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        DrawGeometry(currentCommandBuffer);

        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetSwapchainImage(CurrentSwapchainImageIndex), VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        ImageVK::CopyImageToImage(currentCommandBuffer, Swapchain->GetDrawImage().image,
                                  Swapchain->GetSwapchainImage(CurrentSwapchainImageIndex), Swapchain->GetDrawExtent(),
                                  Swapchain->GetSwapchainExtent());

        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetSwapchainImage(CurrentSwapchainImageIndex),
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImgui(currentCommandBuffer, Swapchain->GetSwapchainImageView(CurrentSwapchainImageIndex));

        ImageVK::TransitionImage(currentCommandBuffer, Swapchain->GetSwapchainImage(CurrentSwapchainImageIndex),
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(currentCommandBuffer));

        // Work on this frame is complete, prepare to submit frame to present queue

        VkCommandBufferSubmitInfo cmdinfo = CommandStructure->CreateCommandBufferSubmitInfo(currentCommandBuffer);

        VkSemaphoreSubmitInfo waitInfo = CommandStructure->CreateSemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, CommandStructure->GetCurrentFrame().swapchainSemaphore);
        VkSemaphoreSubmitInfo signalInfo = CommandStructure->CreateSemaphoreSubmitInfo(
            VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT, CommandStructure->GetCurrentFrame().renderSemaphore);

        VkSubmitInfo2 submit = CreateSubmitInfo(&cmdinfo, &signalInfo, &waitInfo);


        VK_CHECK(
            vkQueueSubmit2(CommandStructure->GetGraphicsQueue(), 1, &submit, CommandStructure->GetCurrentFrame().
                renderFence));

        // Prepare to present queue to screen

        VkSwapchainKHR swapchain = Swapchain->GetSwapchain();

        VkPresentInfoKHR presentInfo = {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pNext = nullptr;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.swapchainCount = 1;

        presentInfo.pWaitSemaphores = &CommandStructure->GetCurrentFrame().renderSemaphore;
        presentInfo.waitSemaphoreCount = 1;

        presentInfo.pImageIndices = &CurrentSwapchainImageIndex;

        // Present queue

        VkResult presentResult = vkQueuePresentKHR(CommandStructure->GetGraphicsQueue(), &presentInfo);
        if (presentResult == VK_ERROR_OUT_OF_DATE_KHR)
            resizeRequested = true;

        CommandStructure->NewFrame();
    }

    void RendererVK::PreFrame()
    {
        if (resizeRequested)
        {
            SwapchainInitInfo vkSwapchainInitInfo{};
            vkSwapchainInitInfo.device = Ref<DeviceVK>(Device)->GetDevice();
            vkSwapchainInitInfo.physicalDevice = Ref<DeviceVK>(Device)->GetPhysicalDevice();
            vkSwapchainInitInfo.surface = Ref<DeviceVK>(Device)->GetSurface();
            uint32_t windowWidth, windowHeight;
            windowWidth = EngineApp::Get()->GetWindow()->GetWidth();
            windowHeight = EngineApp::Get()->GetWindow()->GetHeight();
            vkSwapchainInitInfo.width = windowWidth;
            vkSwapchainInitInfo.height = windowHeight;
            vkSwapchainInitInfo.allocator = Allocator;
            vkSwapchainInitInfo.MainDeletionQueue = &MainDeletionQueue;
            vkSwapchainInitInfo.OnSwapchainResized = [&]()
            {
                ComputePipelineVKInitInfo vkPipelineInitInfo{};
                vkPipelineInitInfo.Device = Ref<DeviceVK>(Device)->GetVKBDevice();
                vkPipelineInitInfo.DrawImageView = Swapchain->GetDrawImage().imageView;
                vkPipelineInitInfo.MainDeletionQueue = &MainDeletionQueue;
                GradientPipeline = Ref<ComputePipelineVK>::Create(vkPipelineInitInfo);

                resizeRequested = false;
            
                ENGINE_CORE_INFO("Window Resize Complete");
            };

            Swapchain->ResizeSwapchain(vkSwapchainInitInfo);
        

        }

        // Get DeviceVK

        Ref<DeviceVK> deviceRef = Ref<DeviceVK>(Device);

        UpdateScene();
        
        // Wait for previous frame to complete

        VK_CHECK(
            vkWaitForFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence, true,
                1000000000));

        CommandStructure->GetCurrentFrame().DeletionQueue.Flush();
        CommandStructure->GetCurrentFrame().FrameDescriptors.ClearPools(deviceRef->GetDevice());

        VK_CHECK(
            vkResetFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence));

        // Get the next image index from the swapchain

        uint32_t swapchainImageIndex;
        VkResult e = vkAcquireNextImageKHR(deviceRef->GetDevice(), Swapchain->GetSwapchain(), 1000000000, CommandStructure->
                GetCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex);
        if (e == VK_ERROR_OUT_OF_DATE_KHR)
        {
            resizeRequested = true;
            return;
        }

        // Get the current frames command buffer

        VkCommandBuffer cmd = CommandStructure->GetCurrentFrame().commandBuffer;

        // Reset command buffer ready for this frame

        VK_CHECK(vkResetCommandBuffer(cmd, 0));

        // Setup command buffer and swapchain for this frame

        VkCommandBufferBeginInfo beginInfo = CommandStructure->CreateCommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        Swapchain->SetDrawExtent(Swapchain->GetDrawImage().imageExtent.width,
                                 Swapchain->GetDrawImage().imageExtent.height);

        // Begin work on this frame
        // All rendering code should be called between vkBeginCommandBuffer and vkEndCommandBuffer

        VK_CHECK(vkBeginCommandBuffer(cmd, &beginInfo));

        currentCommandBuffer = cmd;
        CurrentSwapchainImageIndex = swapchainImageIndex;
    }

    void RendererVK::DrawViewport()
    {
        // if (!isReady) return;
        // ImmediateSubmit([&](VkCommandBuffer cmd)
        // {
        //     ImageVK::TransitionImage(cmd, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        //     
        //     CommandStructure->GetCurrentFrame().FrameDescriptors.Allocate(Ref<DeviceVK>(Device)->GetDevice(), SingleImageLayout);
        //     
        //     DescriptorLayoutBuilder layoutBuilder;
        //     layoutBuilder.Clear();
        //     layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        //     SingleImageLayout = layoutBuilder.Build(Ref<DeviceVK>(Device)->GetDevice(), VK_SHADER_STAGE_FRAGMENT_BIT);
        //     
        //     VkDescriptorSet imageSet = CommandStructure->GetCurrentFrame().FrameDescriptors.Allocate(Ref<DeviceVK>(Device)->GetDevice(), SingleImageLayout);
        //     
        //     DescriptorWriter writer;
        //     writer.WriteImage(0, Swapchain->GetDrawImage().imageView, defaultSamplerNearest, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        //     writer.UpdateSet(Ref<DeviceVK>(Device)->GetDevice(), imageSet);
        //     
        //     ImGui::Image((ImTextureID)imageSet, {(float)Swapchain->GetSwapchainExtent().width, (float)Swapchain->GetSwapchainExtent().height});
        //     ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(CurrentSwapchainImageIndex), VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
        // });
    }
    


    void RendererVK::ImmediateSubmit(std::function<void(VkCommandBuffer cmd)>&& function)
    {
        Ref<DeviceVK> deviceRef = Ref<DeviceVK>(Device);
        CommandStructureVK::ImmediateCommandStructure cmdStruct = CommandStructure->GetImmediateCommandStructure();
        VK_CHECK(vkResetFences(deviceRef->GetDevice(), 1, &cmdStruct.ImmFence));
        VK_CHECK(vkResetCommandBuffer(cmdStruct.ImmCommandBuffer, 0));

        VkCommandBuffer cmd = cmdStruct.ImmCommandBuffer;

        VkCommandBufferBeginInfo cmdBeginInfo = CommandStructure->CreateCommandBufferBeginInfo(
            VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

        VK_CHECK(vkBeginCommandBuffer(cmd, &cmdBeginInfo));

        function(cmd);

        VK_CHECK(vkEndCommandBuffer(cmd));

        VkCommandBufferSubmitInfo cmdInfo = CommandStructure->CreateCommandBufferSubmitInfo(cmd);
        VkSubmitInfo2 submit = CreateSubmitInfo(&cmdInfo, nullptr, nullptr);

        VK_CHECK(vkQueueSubmit2(CommandStructure->GetGraphicsQueue(), 1, &submit, cmdStruct.ImmFence));
        VK_CHECK(vkWaitForFences(deviceRef->GetDevice(), 1, &cmdStruct.ImmFence, true, 9999999999));
    }

    void RendererVK::DrawImgui(VkCommandBuffer cmd, VkImageView targetView)
    {
        VkRenderingAttachmentInfo colorAttachment = ImageVK::CreateAttachmentInfo(
            targetView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingInfo renderInfo = CreateRenderingInfo(Swapchain->GetSwapchainExtent(), &colorAttachment, nullptr);

        vkCmdBeginRendering(cmd, &renderInfo);

        ImGui::EndFrame();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

        vkCmdEndRendering(cmd);
    }

    void RendererVK::DrawGeometry(VkCommandBuffer cmd)
    {

        /*    Create Scene Descriptor Layout      */



        AllocatedBuffer SceneDataBuffer = CreateBuffer(sizeof(GPUSceneData), Allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
        vmaSetAllocationName(Allocator, SceneDataBuffer.allocation, "SceneDataBuffer");
        
        CommandStructure->GetCurrentFrame().DeletionQueue.PushFunction([=, this]
        {
            DestroyBuffer(SceneDataBuffer, Allocator);
        });

        GPUSceneData* sceneUniformData = (GPUSceneData*)SceneDataBuffer.allocationInfo.pMappedData;
        *sceneUniformData = sceneData;

        VkDescriptorSet globalDescriptor = CommandStructure->GetCurrentFrame().FrameDescriptors.Allocate(Ref<DeviceVK>(Device)->GetDevice(), SceneDataLayout);
        
        DescriptorWriter writer;
        writer.WriteBuffer(0, SceneDataBuffer.buffer, sizeof(GPUSceneData), 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
        writer.UpdateSet(Ref<DeviceVK>(Device)->GetDevice(), globalDescriptor);
        
        VkRenderingAttachmentInfo colorAttachment = ImageVK::CreateAttachmentInfo(
            Swapchain->GetDrawImage().imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
        VkRenderingAttachmentInfo depthAttachment = ImageVK::CreateDepthAttachmentInfo(Swapchain->GetDepthImage().imageView, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);

        VkRenderingInfo renderInfo = CreateRenderingInfo(Swapchain->GetSwapchainExtent(), &colorAttachment, &depthAttachment);
        vkCmdBeginRendering(cmd, &renderInfo);


        
        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

        VkDescriptorSet imageSet = CommandStructure->GetCurrentFrame().FrameDescriptors.Allocate(Ref<DeviceVK>(Device)->GetDevice(), SingleImageLayout);
        DescriptorWriter imageWriter;
        imageWriter.WriteImage(0, errorCheckerboardImage.imageView, defaultSamplerNearest, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
        imageWriter.UpdateSet(Ref<DeviceVK>(Device)->GetDevice(), imageSet);

        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipelineLayout, 0, 1, &imageSet, 0, nullptr);

        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = Swapchain->GetDrawImage().imageExtent.width;
        viewport.height = Swapchain->GetDrawImage().imageExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        vkCmdSetViewport(cmd, 0, 1, &viewport);

        VkRect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent.width = Swapchain->GetDrawImage().imageExtent.width;
        scissor.extent.height = Swapchain->GetDrawImage().imageExtent.height;

        vkCmdSetScissor(cmd, 0, 1, &scissor);



#if 0
        
        GPUDrawPushConstants pushConstants;
        pushConstants.worldMatrix = projection * view;
        pushConstants.vertexBuffer = testMeshes[2]->buffers.vertexBufferAddress;

        vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);
        vkCmdBindIndexBuffer(cmd, testMeshes[2]->buffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, testMeshes[2]->geometries[0].indexCount, 1, testMeshes[2]->geometries[0].startIndex, 0, 0);
#endif

        for (const RenderObject& draw : mainDrawContext.OpaqueSurfaces)
        {
            auto material = static_cast<MaterialInstanceVK*>(draw.material);
            vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipeline);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipelineLayout, 0, 1, &globalDescriptor, 0, nullptr);
            vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, material->pipeline->pipelineLayout, 1, 1, &material->materialSet, 0, nullptr);
            vkCmdBindIndexBuffer(cmd, *(VkBuffer*)draw.indexBuffer, 0, VK_INDEX_TYPE_UINT32);

            GPUDrawPushConstants pushConstants;
            pushConstants.vertexBuffer = draw.vertexBufferAddress;
            pushConstants.worldMatrix = draw.transform;
            vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);

            vkCmdDrawIndexed(cmd, draw.indexCount, 1, draw.firstIndex, 0, 0);
        }

        vkCmdEndRendering(cmd);
    }

    void RendererVK::UpdateScene()
    {
        
        auto view = EngineApp::Get()->GetActiveScene()->GetRegistry().view<TransformComponent, CameraComponent>();
        for (auto entity : view)
        {
            auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);
            camera.camera->Update(.01);
            glm::mat4 viewMatrix = camera.camera->GetViewMatrix();

            glm::mat4 projection = glm::perspective(glm::radians(70.f), 16/9.f, 10000.f, 0.1f);

            projection[1][1] *= -1;

            sceneData.view = viewMatrix;
            sceneData.projection = projection;
            sceneData.viewproj = projection * viewMatrix;
            
            
            
        }
        mainDrawContext.OpaqueSurfaces.clear();
        loadedMeshes["Suzanne"]->Draw(glm::mat4(1.f), mainDrawContext);

        // sceneData.view =  glm::translate(glm::vec3(0.f, 0.f, -5.f));
        // sceneData.projection = glm::perspective(glm::radians(70.f), 16/9.f,0.1f, 10000.f);
        // sceneData.projection[1][1] *= -1;
        // sceneData.viewproj = sceneData.projection * sceneData.view;

        sceneData.ambientColor = glm::vec4(0.1f, 0.1f, 0.1f, 1.f);
        sceneData.sunlightColor = glm::vec4(1.f, 1.f, 1.f, 1.f);
        sceneData.sunlightDirection = glm::vec4(0.f, -1.f, 0.f, 1.f);

        for (int x = -3; x < 3; x++) {

            glm::mat4 scale = glm::scale(glm::vec3{0.2});
            glm::mat4 translation =  glm::translate(glm::vec3{x, 1, 0});

            loadedMeshes["Cube"]->Draw(translation * scale, mainDrawContext);
        }
        
    }

    GPUMeshBuffers RendererVK::UploadMeshes(std::span<uint32_t> indices, std::span<Vertex> vertices)
    {
        const size_t vertexBufferSize = vertices.size() * sizeof(Vertex);
        const size_t indexBufferSize = indices.size() * sizeof(uint32_t);

        GPUMeshBuffers newSurface;

        newSurface.vertexBuffer = CreateBuffer(vertexBufferSize, Allocator, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        VkBufferDeviceAddressInfo bufferDeviceAddressInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO, .buffer = newSurface.vertexBuffer.buffer};
        newSurface.vertexBufferAddress = vkGetBufferDeviceAddress(Ref<DeviceVK>(Device)->GetDevice(), &bufferDeviceAddressInfo);

        newSurface.indexBuffer = CreateBuffer(indexBufferSize, Allocator, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

        AllocatedBuffer staging = CreateBuffer(vertexBufferSize + indexBufferSize, Allocator, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        void* data = staging.allocationInfo.pMappedData;

        memcpy(data, vertices.data(), vertexBufferSize);
        memcpy((char*)data + vertexBufferSize, indices.data(), indexBufferSize);

        ImmediateSubmit([&](VkCommandBuffer cmd)
        {
            VkBufferCopy vertexCopy{0};
            vertexCopy.dstOffset = 0;
            vertexCopy.srcOffset = 0;
            vertexCopy.size = vertexBufferSize;

            vkCmdCopyBuffer(cmd, staging.buffer, newSurface.vertexBuffer.buffer, 1, &vertexCopy);

            VkBufferCopy indexCopy{0};
            indexCopy.dstOffset = 0;
            indexCopy.srcOffset = vertexBufferSize;
            indexCopy.size = indexBufferSize;

            vkCmdCopyBuffer(cmd, staging.buffer, newSurface.indexBuffer.buffer, 1, &indexCopy);
        });
        

 

        DestroyBuffer(staging, Allocator);

        return newSurface;
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
        windowWidth = EngineApp::Get()->GetWindow()->GetWidth();
        windowHeight = EngineApp::Get()->GetWindow()->GetHeight();
        vkSwapchainInitInfo.width = windowWidth;
        vkSwapchainInitInfo.height = windowHeight;
        vkSwapchainInitInfo.allocator = Allocator;
        vkSwapchainInitInfo.MainDeletionQueue = &MainDeletionQueue;
        vkSwapchainInitInfo.OnSwapchainResized = [&]()
        {
            resizeRequested = false;
        };

        Swapchain = Ref<SwapchainVK>::Create(vkSwapchainInitInfo);

        /*    Create Vulkan Command structure     */

        CommandQueueInfo vkCommandQueueInfo{};
        vkCommandQueueInfo.device = Ref<DeviceVK>(Device)->GetVKBDevice();
        vkCommandQueueInfo.MainDeletionQueue = &MainDeletionQueue;

        CommandStructure = Ref<CommandStructureVK>::Create(vkCommandQueueInfo);

        ENGINE_CORE_INFO("Vulkan Command Buffer Created");

        {
            DescriptorLayoutBuilder layoutBuilder;
            layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);
            SingleImageLayout = layoutBuilder.Build(Ref<DeviceVK>(Device)->GetDevice(), VK_SHADER_STAGE_FRAGMENT_BIT);
        }
        std::vector<DescriptorAllocatorDynamic::PoolSizeRatio> sizes = {{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1}};
        globalDescriptorAllocator.Init(Device.As<DeviceVK>()->GetDevice(), 100, sizes);


        /*    Create Vulkan Compute Pipeline */

        ComputePipelineVKInitInfo vkPipelineInitInfo{};
        vkPipelineInitInfo.Device = Ref<DeviceVK>(Device)->GetVKBDevice();
        vkPipelineInitInfo.DrawImageView = Swapchain->GetDrawImage().imageView;
        vkPipelineInitInfo.MainDeletionQueue = &MainDeletionQueue;
        GradientPipeline = Ref<ComputePipelineVK>::Create(vkPipelineInitInfo);

        /*    Initialize EngineGUI    */

        Ref<ImGUIVK> ImGui = Ref<ImGUIVK>::Create();
        ImGUIVKInfo guiInfo{};
        guiInfo.Device = Ref<DeviceVK>(Device)->GetVKBDevice();
        guiInfo.Instance = Instance;
        guiInfo.Queue = CommandStructure->GetGraphicsQueue();
        guiInfo.DeletionQueue = &MainDeletionQueue;
        guiInfo.ImageFormat = Swapchain->GetSwapchainImageFormat();
        guiInfo.PhysicalDevice = Ref<DeviceVK>(Device)->GetPhysicalDevice();
        ImGui->InitImGUI(guiInfo);
        

        /*    Initiate Mesh Pipeline      */

        Ref<ShaderVK> meshShader = Shader::Create(
    (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/colored_triangle_mesh.vert").
    generic_string(),
    (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/tex_image.frag").
    generic_string());
        meshShader->CreateShaderModule(Ref<DeviceVK>(Device)->GetDevice());

        VkPushConstantRange bufferRange {};
        bufferRange.offset = 0;
        bufferRange.size = sizeof(GPUDrawPushConstants);
        bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo meshPipelineLayoutInfo {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        meshPipelineLayoutInfo.pushConstantRangeCount = 1;
        meshPipelineLayoutInfo.pPushConstantRanges = &bufferRange;
        meshPipelineLayoutInfo.pSetLayouts = &SingleImageLayout;
        meshPipelineLayoutInfo.setLayoutCount = 1;

        VK_CHECK(vkCreatePipelineLayout(Ref<DeviceVK>(Device)->GetDevice(), &meshPipelineLayoutInfo, nullptr, &meshPipelineLayout));

        PipelineBuilder meshPipelineBuilder;

        meshPipelineBuilder.pipelineLayout = meshPipelineLayout;

        meshPipelineBuilder.SetShaders(meshShader->GetShaderModule(ShaderType::VERTEX),
                                       meshShader->GetShaderModule(ShaderType::FRAGMENT));
        meshPipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        meshPipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        meshPipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        meshPipelineBuilder.SetMultisamplingNone();
        meshPipelineBuilder.DisableBlending();
        meshPipelineBuilder.EnableDepthTest(true, VK_COMPARE_OP_GREATER_OR_EQUAL);

        meshPipelineBuilder.SetColorAttachmentFormat(Swapchain->GetDrawImage().imageFormat);
        meshPipelineBuilder.SetDepthFormat(Swapchain->GetDepthImage().imageFormat);

        meshPipeline = meshPipelineBuilder.BuildPipeline(Ref<DeviceVK>(Device)->GetDevice());

        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), meshShader->GetShaderModule(ShaderType::FRAGMENT),
                              nullptr);
        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), meshShader->GetShaderModule(ShaderType::VERTEX), nullptr);

        MainDeletionQueue.PushFunction([&]()
        {
            vkDestroyPipelineLayout(Ref<DeviceVK>(Device)->GetDevice(), meshPipelineLayout, nullptr);
            vkDestroyPipeline(Ref<DeviceVK>(Device)->GetDevice(), meshPipeline, nullptr);
        });

        {
            DescriptorLayoutBuilder layoutBuilder;
            layoutBuilder.AddBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER);
            SceneDataLayout = layoutBuilder.Build(Ref<DeviceVK>(Device)->GetDevice(), VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
        }

        metalRoughnessMaterial.BuildPipelines(this);


        /*    Init default mesh data      */
        
        MeshComponent meshComp;
        meshComp.id = ::UUID();
        meshComp.filePath = "..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb";
        testMesh = Ref<MeshVK>::Create(meshComp, this);
        testMeshes = testMesh->meshes;

        uint32_t white = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
        whiteImage = ImageVK::CreateImage(Ref<DeviceVK>(Device)->GetDevice(), this, (void*)&white, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, Allocator);

        uint32_t grey = glm::packUnorm4x8(glm::vec4(0.5, 0.5, 0.5, 1));
        greyImage = ImageVK::CreateImage(Ref<DeviceVK>(Device)->GetDevice(), this, (void*)&grey, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, Allocator);

        uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
        blackImage = ImageVK::CreateImage(Ref<DeviceVK>(Device)->GetDevice(), this, (void*)&black, VkExtent3D{1, 1, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, Allocator);

        uint32_t checkerboard = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
        std::array<uint32_t, 16 *16> pixels;
        for (int x = 0; x < 16; x++)
        {
            for (int y = 0; y < 16; y++)
            {
                pixels[y*16 + x] = ((x % 2) ^ (y % 2)) ? checkerboard : black;
            }
        }

        errorCheckerboardImage = ImageVK::CreateImage(Ref<DeviceVK>(Device)->GetDevice(), this, (void*)pixels.data(), VkExtent3D{16, 16, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, Allocator);
        
        VkSamplerCreateInfo sampler1 {.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO};

        sampler1.magFilter = VK_FILTER_NEAREST;
        sampler1.minFilter = VK_FILTER_NEAREST;

        vkCreateSampler(Ref<DeviceVK>(Device)->GetDevice(), &sampler1, nullptr, &defaultSamplerNearest);

        sampler1.magFilter = VK_FILTER_LINEAR;
        sampler1.minFilter = VK_FILTER_LINEAR;

        vkCreateSampler(Ref<DeviceVK>(Device)->GetDevice(), &sampler1, nullptr, &defaultSamplerLinear);
        

        MainDeletionQueue.PushFunction([&]()
        {
            vkDestroySampler(Ref<DeviceVK>(Device)->GetDevice(), defaultSamplerNearest, nullptr);
            vkDestroySampler(Ref<DeviceVK>(Device)->GetDevice(), defaultSamplerLinear, nullptr);

            ImageVK::DestroyImage(Ref<DeviceVK>(Device)->GetDevice(), Allocator, whiteImage);
            ImageVK::DestroyImage(Ref<DeviceVK>(Device)->GetDevice(), Allocator, greyImage);
            ImageVK::DestroyImage(Ref<DeviceVK>(Device)->GetDevice(), Allocator, blackImage);
            ImageVK::DestroyImage(Ref<DeviceVK>(Device)->GetDevice(), Allocator, errorCheckerboardImage);
        });

        isReady = true;

        PBRMaterialVK::MaterialResources materialResources;
        materialResources.colorImage = whiteImage;
        materialResources.colorSampler = defaultSamplerLinear;
        materialResources.metallicRoughnessImage = whiteImage;
        materialResources.metalRoughnessSampler = defaultSamplerLinear;

        AllocatedBuffer materialConstants = CreateBuffer(sizeof(PBRMaterialVK::MaterialConstants), Allocator, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);

        PBRMaterialVK::MaterialConstants* sceneUniformData = (PBRMaterialVK::MaterialConstants*)materialConstants.allocationInfo.pMappedData;
        sceneUniformData->colorFactors = glm::vec4(1, 1, 1, 1);
        sceneUniformData->metallicRoughnessFactors = glm::vec4(1, 0.5, 0, 0);

        MainDeletionQueue.PushFunction([=, this]()
        {
            DestroyBuffer(materialConstants, Allocator);
        });

        materialResources.dataBuffer = materialConstants.buffer;
        materialResources.dataBufferOffset = 0;

        defaultData = metalRoughnessMaterial.WriteMaterial(GetDevice(), MaterialPass::MainColor, materialResources, globalDescriptorAllocator);

        for (auto& m : testMeshes)
        {
            std::shared_ptr<MeshVK> newMesh = std::make_shared<MeshVK>(meshComp, this);
            newMesh->meshes = {m};

            for (auto& s : newMesh->meshes[0]->geometries) // Temporary
            {
                GLTFMaterial mat = {.data = defaultData};
                s.material = mat;
            }

            loadedMeshes[m->name] = std::move(newMesh);
        }
    }

    void RendererVK::DrawBackground(VkCommandBuffer cmd)
    {
        VkClearColorValue clearValue;
        float flash = std::abs(std::sin(CommandStructure->GetFrameNumber() / 120.f));
        clearValue = {{0.0f, 0.0f, flash, 1.0f}};

        VkImageSubresourceRange clearRange = ImageVK::GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, GradientPipeline->GetPipeline());

        auto descriptors = GradientPipeline->GetDescriptorSet();
        vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_COMPUTE, GradientPipeline->GetPipelineLayout(), 0, 1,
                                &descriptors, 0, nullptr);

        ComputePushConstants pc;
        pc.data1 = glm::vec4{1, 0, 0, 1};
        pc.data2 = glm::vec4{0, 0, 1, 1};

        vkCmdPushConstants(cmd, GradientPipeline->GetPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(pc), &pc);

        vkCmdDispatch(cmd, std::ceil(Swapchain->GetDrawExtent().width / 16.0),
                      std::ceil(Swapchain->GetDrawExtent().height / 16.0), 1);
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

    VkRenderingInfo RendererVK::CreateRenderingInfo(VkExtent2D renderExtent, VkRenderingAttachmentInfo* colorAttachment,
                                                    VkRenderingAttachmentInfo* depthAttachment)
    {
        VkRenderingInfo renderInfo{};
        renderInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderInfo.pNext = nullptr;

        renderInfo.renderArea = VkRect2D{VkOffset2D{0, 0}, renderExtent};
        renderInfo.layerCount = 1;
        renderInfo.colorAttachmentCount = 1;
        renderInfo.pColorAttachments = colorAttachment;
        renderInfo.pDepthAttachment = depthAttachment;
        renderInfo.pStencilAttachment = nullptr;

        return renderInfo;
    }
}

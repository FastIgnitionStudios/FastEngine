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
#include "ShaderVK.h"
#include "backends/imgui_impl_vulkan.h"
#include "gtx/transform.hpp"
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
        vkDeviceWaitIdle(Ref<DeviceVK>(Device)->GetDevice());
        MainDeletionQueue.Flush();
        CommandStructure = nullptr;
        GradientPipeline = nullptr;
        Device = nullptr;
    }

    void RendererVK::DrawFrame()
    {
        // Get DeviceVK

        Ref<DeviceVK> deviceRef = Ref<DeviceVK>(Device);

        // Wait for previous frame to complete

        VK_CHECK(
            vkWaitForFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence, true,
                1000000000));

        CommandStructure->GetCurrentFrame().DeletionQueue.Flush();

        VK_CHECK(
            vkResetFences(deviceRef->GetDevice(), 1, &CommandStructure->GetCurrentFrame().renderFence));

        // Get the next image index from the swapchain

        uint32_t swapchainImageIndex;
        VK_CHECK(
            vkAcquireNextImageKHR(deviceRef->GetDevice(), Swapchain->GetSwapchain(), 1000000000, CommandStructure->
                GetCurrentFrame().swapchainSemaphore, nullptr, &swapchainImageIndex));

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

        ImageVK::TransitionImage(cmd, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_GENERAL);

        DrawBackground(cmd);

        ImageVK::TransitionImage(cmd, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_GENERAL,
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawGeometry(cmd);

        ImageVK::TransitionImage(cmd, Swapchain->GetDrawImage().image, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                 VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
        ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex), VK_IMAGE_LAYOUT_UNDEFINED,
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

        ImageVK::CopyImageToImage(cmd, Swapchain->GetDrawImage().image,
                                  Swapchain->GetSwapchainImage(swapchainImageIndex), Swapchain->GetDrawExtent(),
                                  Swapchain->GetSwapchainExtent());

        ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex),
                                 VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        DrawImgui(cmd, Swapchain->GetSwapchainImageView(swapchainImageIndex));

        ImageVK::TransitionImage(cmd, Swapchain->GetSwapchainImage(swapchainImageIndex),
                                 VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);

        VK_CHECK(vkEndCommandBuffer(cmd));

        // Work on this frame is complete, prepare to submit frame to present queue

        VkCommandBufferSubmitInfo cmdinfo = CommandStructure->CreateCommandBufferSubmitInfo(cmd);

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

        presentInfo.pImageIndices = &swapchainImageIndex;

        // Present queue

        VK_CHECK(vkQueuePresentKHR(CommandStructure->GetGraphicsQueue(), &presentInfo));

        CommandStructure->NewFrame();
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

        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

        vkCmdEndRendering(cmd);
    }

    void RendererVK::DrawGeometry(VkCommandBuffer cmd)
    {
        VkRenderingAttachmentInfo colorAttachment = ImageVK::CreateAttachmentInfo(
            Swapchain->GetDrawImage().imageView, nullptr, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        VkRenderingInfo renderInfo = CreateRenderingInfo(Swapchain->GetSwapchainExtent(), &colorAttachment, nullptr);
        vkCmdBeginRendering(cmd, &renderInfo);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

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

        vkCmdDraw(cmd, 3, 1, 0, 0);

        vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, meshPipeline);

        glm::mat4 view = glm::translate(glm::vec3{0, 0, -5});

        glm::mat4 projection = glm::perspective(glm::radians(70.f), (float)Swapchain->GetDrawImage().imageExtent.width / (float)Swapchain->GetDrawImage().imageExtent.height, 0.1f, 10000.f);

        projection[1][1] *= -1;

        GPUDrawPushConstants pushConstants;
        pushConstants.worldMatrix = projection * view;
        pushConstants.vertexBuffer = Rectangle.vertexBufferAddress;

        vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);
        vkCmdBindIndexBuffer(cmd, Rectangle.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, 6, 1, 0, 0, 0);

        pushConstants.vertexBuffer = testMeshes[2]->buffers.vertexBufferAddress;

        vkCmdPushConstants(cmd, meshPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(GPUDrawPushConstants), &pushConstants);
        vkCmdBindIndexBuffer(cmd, testMeshes[2]->buffers.indexBuffer.buffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdDrawIndexed(cmd, testMeshes[2]->geometries[0].indexCount, 1, testMeshes[2]->geometries[0].startIndex, 0, 0);

        vkCmdEndRendering(cmd);
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
        vkCommandQueueInfo.MainDeletionQueue = &MainDeletionQueue;

        CommandStructure = Ref<CommandStructureVK>::Create(vkCommandQueueInfo);

        ENGINE_CORE_INFO("Vulkan Command Buffer Created");

        /*    Create Vulkan Compute Pipeline */

        ComputePipelineVKInitInfo vkPipelineInitInfo{};
        vkPipelineInitInfo.Device = Ref<DeviceVK>(Device)->GetVKBDevice();
        vkPipelineInitInfo.DrawImageView = Swapchain->GetDrawImage().imageView;
        vkPipelineInitInfo.MainDeletionQueue = &MainDeletionQueue;
        GradientPipeline = Ref<ComputePipelineVK>::Create(vkPipelineInitInfo);

        /*    Initialize ImGUI    */

        Ref<ImGUIVK> ImGui = Ref<ImGUIVK>::Create();
        ImGUIVKInfo guiInfo{};
        guiInfo.Device = Ref<DeviceVK>(Device)->GetVKBDevice();
        guiInfo.Instance = Instance;
        guiInfo.Queue = CommandStructure->GetGraphicsQueue();
        guiInfo.DeletionQueue = &MainDeletionQueue;
        guiInfo.ImageFormat = Swapchain->GetSwapchainImageFormat();
        guiInfo.PhysicalDevice = Ref<DeviceVK>(Device)->GetPhysicalDevice();
        ImGui->InitImGUI(guiInfo);

        /*    Initialize Graphics Pipeline    */

        Ref<ShaderVK> shader = Shader::Create(
            (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/colored_triangle.vert").
            generic_string(),
            (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/colored_triangle.frag").
            generic_string());
        shader->CreateShaderModule(Ref<DeviceVK>(Device)->GetDevice());

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        VK_CHECK(
            vkCreatePipelineLayout(Ref<DeviceVK>(Device)->GetDevice(), &pipelineLayoutInfo, nullptr, &
                graphicsPipelineLayout));

        PipelineBuilder pipelineBuilder;

        pipelineBuilder.pipelineLayout = graphicsPipelineLayout;

        pipelineBuilder.SetShaders(shader->GetShaderModule(ShaderType::VERTEX),
                                   shader->GetShaderModule(ShaderType::FRAGMENT));
        pipelineBuilder.SetInputTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
        pipelineBuilder.SetPolygonMode(VK_POLYGON_MODE_FILL);
        pipelineBuilder.SetCullMode(VK_CULL_MODE_NONE, VK_FRONT_FACE_CLOCKWISE);
        pipelineBuilder.SetMultisamplingNone();
        pipelineBuilder.DisableBlending();
        pipelineBuilder.DisableDepthTest();

        pipelineBuilder.SetColorAttachmentFormat(Swapchain->GetDrawImage().imageFormat);
        pipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

        graphicsPipeline = pipelineBuilder.BuildPipeline(Ref<DeviceVK>(Device)->GetDevice());

        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), shader->GetShaderModule(ShaderType::FRAGMENT),
                              nullptr);
        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), shader->GetShaderModule(ShaderType::VERTEX), nullptr);

        MainDeletionQueue.PushFunction([&]()
        {
            vkDestroyPipelineLayout(Ref<DeviceVK>(Device)->GetDevice(), graphicsPipelineLayout, nullptr);
            vkDestroyPipeline(Ref<DeviceVK>(Device)->GetDevice(), graphicsPipeline, nullptr);
        });

        /*    Initiate Mesh Pipeline      */

        Ref<ShaderVK> meshShader = Shader::Create(
    (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/colored_triangle_mesh.vert").
    generic_string(),
    (std::filesystem::current_path() / "../FastEngine/Source/Assets/Shaders/colored_triangle.frag").
    generic_string());
        meshShader->CreateShaderModule(Ref<DeviceVK>(Device)->GetDevice());

        VkPushConstantRange bufferRange {};
        bufferRange.offset = 0;
        bufferRange.size = sizeof(GPUDrawPushConstants);
        bufferRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

        VkPipelineLayoutCreateInfo meshPipelineLayoutInfo {.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO};
        meshPipelineLayoutInfo.pushConstantRangeCount = 1;
        meshPipelineLayoutInfo.pPushConstantRanges = &bufferRange;

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
        meshPipelineBuilder.DisableDepthTest();

        meshPipelineBuilder.SetColorAttachmentFormat(Swapchain->GetDrawImage().imageFormat);
        meshPipelineBuilder.SetDepthFormat(VK_FORMAT_UNDEFINED);

        meshPipeline = meshPipelineBuilder.BuildPipeline(Ref<DeviceVK>(Device)->GetDevice());

        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), meshShader->GetShaderModule(ShaderType::FRAGMENT),
                              nullptr);
        vkDestroyShaderModule(Ref<DeviceVK>(Device)->GetDevice(), meshShader->GetShaderModule(ShaderType::VERTEX), nullptr);

        MainDeletionQueue.PushFunction([&]()
        {
            vkDestroyPipelineLayout(Ref<DeviceVK>(Device)->GetDevice(), meshPipelineLayout, nullptr);
            vkDestroyPipeline(Ref<DeviceVK>(Device)->GetDevice(), meshPipeline, nullptr);
        });


        /*    Init default mesh data      */

        std::array<Vertex, 4> rectVertices;

        
        rectVertices[0].position = {0.5,-0.5, 0};
        rectVertices[1].position = {0.5,0.5, 0};
        rectVertices[2].position = {-0.5,-0.5, 0};
        rectVertices[3].position = {-0.5,0.5, 0};
            
        rectVertices[0].color = {0,0, 0,1};
        rectVertices[1].color = { 0.5,0.5,0.5 ,1};
        rectVertices[2].color = { 1,0, 0,1 };
        rectVertices[3].color = { 0,1, 0,1 };

        std::array<uint32_t,6> rectIndices;

        rectIndices[0] = 0;
        rectIndices[1] = 1;
        rectIndices[2] = 2;

        rectIndices[3] = 2;
        rectIndices[4] = 1;
        rectIndices[5] = 3;

        Rectangle = UploadMeshes(rectIndices, rectVertices);

        //delete the rectangle data on engine shutdown
        MainDeletionQueue.PushFunction([&](){
            DestroyBuffer(Rectangle.vertexBuffer, Allocator);
            DestroyBuffer(Rectangle.indexBuffer, Allocator);
        });

        MeshComponent meshComp;
        meshComp.id = UUID();
        meshComp.filePath = "..\\FastEngine\\Source\\Assets\\Meshes\\basicmesh.glb";
        testMeshes = MeshVK::CreateMeshAsset(meshComp, this);
        
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

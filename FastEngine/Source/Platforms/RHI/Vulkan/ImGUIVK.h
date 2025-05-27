#pragma once
#include <vulkan/vulkan.h>

#include "UtilsVK.h"
#include "Core.h"


namespace Engine
{
    struct ImGUIVKInfo
    {
        VkInstance Instance;
        VkPhysicalDevice PhysicalDevice;
        VkDevice Device;
        VkQueue Queue;
        VkFormat ImageFormat;
        DeletionQueue* DeletionQueue;
    };
    
    class ImGUIVK : public Engine
    {
    public:
        ImGUIVK() = default;

        void InitImGUI(ImGUIVKInfo info);
        
    };
}

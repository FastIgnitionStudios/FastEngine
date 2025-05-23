#pragma once
#include <deque>
namespace Engine
{
#define VK_CHECK(x) {VkResult err = x; if (err) {ENGINE_CORE_CRITICAL("Vulkan Check Failed: {0}", (int)err); }}

    
    struct DeletionQueue
    {
        std::deque<std::function<void()>> deletors;
        
        
        void PushFunction(std::function<void()>&& func)
        {
            deletors.push_back(func);
        }

        void Flush()
        {
            for (auto it = deletors.rbegin(); it != deletors.rend(); it++)
            {
                (*it)();
            }
        }
    };
}

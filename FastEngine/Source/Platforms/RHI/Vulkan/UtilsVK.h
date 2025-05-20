#pragma once
#include <deque>

namespace Engine
{
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

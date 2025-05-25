#include "EnginePCH.h"
#define VMA_IMPLEMENTATION
#ifdef ENGINE_DEBUG
#define VMA_LEAK_LOG_FORMAT(...) printf(__VA_ARGS__)
#endif

#include "vk_mem_alloc.h"

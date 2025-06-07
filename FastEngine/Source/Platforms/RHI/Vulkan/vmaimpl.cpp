#include "EnginePCH.h"
#define VMA_IMPLEMENTATION
#ifdef DEBUG_BUILD
#define VMA_LEAK_LOG_FORMAT(...) printf(__VA_ARGS__)
#endif

#include "vk_mem_alloc.h"

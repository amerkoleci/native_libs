#if defined(_WIN32)
#   define VMA_CALL_PRE __declspec(dllexport)
#else
#   define VMA_CALL_PRE __attribute__((visibility("default")))
#endif

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include "vk_mem_alloc.h"

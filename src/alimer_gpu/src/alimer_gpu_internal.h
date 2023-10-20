// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef ALIMER_GPU_INTERNAL_H
#define ALIMER_GPU_INTERNAL_H

#include "alimer_gpu.h"
#include <stdbool.h>
#include <string.h> // memset
#include <assert.h>

 /* Compiler defines */
#if defined(__clang__)
#define ALIMER_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALIMER_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ALIMER_UNREACHABLE() __builtin_unreachable()
#define ALIMER_DEBUG_BREAK() __builtin_trap()
// CLANG ENABLE/DISABLE WARNING DEFINITION
#define ALIMER_DISABLE_WARNINGS() \
    _Pragma("clang diagnostic push")\
	_Pragma("clang diagnostic ignored \"-Wall\"") \
	_Pragma("clang diagnostic ignored \"-Wextra\"") \
	_Pragma("clang diagnostic ignored \"-Wtautological-compare\"") \
    _Pragma("clang diagnostic ignored \"-Wnullability-completeness\"") \
    _Pragma("clang diagnostic ignored \"-Wnullability-extension\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-function\"") \
    _Pragma("clang diagnostic ignored \"-Wunused-variable\"") \
    _Pragma("clang diagnostic ignored \"-Wmissing-field-initializers\"")

#define ALIMER_ENABLE_WARNINGS() _Pragma("clang diagnostic pop")
#elif defined(__GNUC__) || defined(__GNUG__)
#define ALIMER_LIKELY(x) __builtin_expect(!!(x), 1)
#define ALIMER_UNLIKELY(x) __builtin_expect(!!(x), 0)
#define ALIMER_UNREACHABLE() __builtin_unreachable()
#define ALIMER_DEBUG_BREAK() __builtin_trap()
// GCC ENABLE/DISABLE WARNING DEFINITION
#	define ALIMER_DISABLE_WARNINGS() \
	_Pragma("GCC diagnostic push") \
	_Pragma("GCC diagnostic ignored \"-Wall\"") \
	_Pragma("GCC diagnostic ignored \"-Wextra\"") \
	_Pragma("GCC diagnostic ignored \"-Wtautological-compare\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-parameter\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-function\"") \
    _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
    _Pragma("GCC diagnostic ignored \"-Wmissing-field-initializers\"")

#define ALIMER_ENABLE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define ALIMER_LIKELY(x) (x)
#define ALIMER_UNLIKELY(x) (x)
#define ALIMER_UNREACHABLE() __assume(false)
#define ALIMER_DEBUG_BREAK() __debugbreak()
#define ALIMER_DISABLE_WARNINGS() __pragma(warning(push, 0))
#define ALIMER_ENABLE_WARNINGS() __pragma(warning(pop))
#endif

#define ALIMER_STRINGIZE_HELPER(X) #X
#define ALIMER_STRINGIZE(X) ALIMER_STRINGIZE_HELPER(X)
#define ALIMER_UNUSED(x) (void)(x)

// Macro for determining size of arrays.
#if defined(_MSC_VER)
#   include <stdlib.h> 
#   define ALIMER_ARRAYSIZE(arr) _countof(arr)
#else
#   define ALIMER_ARRAYSIZE(arr) (sizeof(arr) / sizeof(arr[0]))
#endif

#ifndef ALIMER_ASSERT
#   include <assert.h>
#   define ALIMER_ASSERT(c) assert(c)
#endif

#define ALIMER_MIN(a, b) (a < b ? a : b)
#define ALIMER_MAX(a, b) (a > b ? a : b)
#define ALIMER_CLAMP(val, min, max) ALIMER_MAX(min, ALIMER_MIN(val, max))
#define _check_flags(flags, flag) (((flags) & (flag)) != 0)

_ALIMER_GPU_EXTERN void AlimerLogInfo(const char* fmt, ...);
_ALIMER_GPU_EXTERN void AlimerLogWarn(const char* fmt, ...);
_ALIMER_GPU_EXTERN void AlimerLogError(const char* fmt, ...);


#if defined(__LP64__) || defined(_WIN64) || (defined(__x86_64__) && !defined(__ILP32__) ) || defined(_M_X64) || defined(__ia64) || defined (_M_IA64) || defined(__aarch64__) || defined(__powerpc64__)
typedef struct VkSurfaceKHR_T* VkSurfaceKHR;
#else
typedef uint64_t VkSurfaceKHR;
#endif

typedef struct GPUSurfaceImpl
{
    void* windowHandle;
    struct {
        VkSurfaceKHR handle;
    } vk;
} GPUSurfaceImpl;

typedef struct GPUDriver
{
    GPUBackendType backend;
    bool(*IsSupported)(void);
    bool(*Init)(const GPUConfig* config);
    void(*Shutdown)(void);
    bool(*InitSurface)(GPUSurface surface);
    GPUDevice (*CreateDevice)(/*const VGPUDeviceDescriptor* descriptor*/);
} GPUDriver;

_ALIMER_GPU_EXTERN GPUDriver D3D12_Driver;
_ALIMER_GPU_EXTERN GPUDriver Vulkan_Driver;

#endif /* ALIMER_INTERNAL_H */

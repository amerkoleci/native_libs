// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef ALIMER_GPU_H
#define ALIMER_GPU_H

#if defined(ALIMER_GPU_SHARED_LIBRARY)
#   if defined(_WIN32) || defined(__CYGWIN__)
#       if defined(ALIMER_GPU_IMPLEMENTATION)
#           define _ALIMER_GPU_EXPORT __declspec(dllexport)
#       else
#           define _ALIMER_GPU_EXPORT __declspec(dllimport)
#       endif
#       define ALIMER_GPU_CALL __cdecl
#   else
#       if defined(ALIMER_AUDIO_IMPLEMENTATION)
#           define _ALIMER_GPU_EXPORT __attribute__((visibility("default")))
#       else
#           define _ALIMER_GPU_EXPORT
#       endif
#       define ALIMER_GPU_CALL
#   endif
#else
#   define _ALIMER_GPU_EXPORT
#   define ALIMER_GPU_CALL
#endif

#ifdef __cplusplus
#    define _ALIMER_GPU_EXTERN extern "C"
#else
#    define _ALIMER_GPU_EXTERN extern
#endif

#define ALIMER_GPU_API _ALIMER_GPU_EXTERN _ALIMER_GPU_EXPORT 

#include <stddef.h>
#include <stdint.h>

typedef uint32_t GPUFlags;
typedef uint32_t GPUBool32;

/* Defines */
#define ALIMER_GPU_VERSION_MAJOR	1
#define ALIMER_GPU_VERSION_MINOR	0
#define ALIMER_GPU_VERSION_PATCH	0

/* Types */
typedef struct GPUSurfaceImpl* GPUSurface;
typedef struct GPUDeviceImpl* GPUDevice;
typedef struct GPUBufferImpl* GPUBuffer;
typedef struct GPUTextureImpl* GPUTexture;
typedef struct GPUTextureViewImpl* GPUTextureView;

/* Enums */
typedef enum GPUBackendType {
    GPUBackendType_Undefined = 0,
    GPUBackendType_Vulkan = 1,
    GPUBackendType_D3D12 = 2,
    GPUBackendType_Metal = 3,

    _GPUBackendType_Force32 = 0x7FFFFFFF
} GPUBackendType;

typedef enum GPUValidationMode {
    GPUValidationMode_Disabled = 0,
    GPUValidationMode_Enabled,
    GPUValidationMode_Verbose,
    GPUValidationMode_GPU,

    _GPUValidationMode_Count,
    _GPUValidationMode_Force32 = 0x7FFFFFFF
} GPUValidationMode;

typedef enum GPUPowerPreference {
    GPUPowerPreference_Undefined = 0,
    GPUPowerPreference_LowPower = 1,
    VGPUPowerPreference_HighPerformance = 2,

    _GPUPowerPreference_Force32 = 0x7FFFFFFF
} GPUPowerPreference;

typedef enum GPUCommandQueue {
    GPUCommandQueue_Graphics,
    GPUCommandQueue_Compute,
    GPUCommandQueue_Copy,

    _GPUCommandQueue_Count,
    _GPUCommandQueue_Force32 = 0x7FFFFFFF
} GPUCommandQueue;

/* Structs */
typedef struct GPUViewport {
    float x;
    float y;
    float width;
    float height;
    float minDepth;
    float maxDepth;
} GPUViewport;

typedef struct GPUDispatchIndirectCommand
{
    uint32_t x;
    uint32_t y;
    uint32_t z;
} GPUDispatchIndirectCommand;

typedef struct GPUDrawIndirectCommand
{
    uint32_t vertexCount;
    uint32_t instanceCount;
    uint32_t firstVertex;
    uint32_t firstInstance;
} GPUDrawIndirectCommand;

typedef struct GPUDrawIndexedIndirectCommand
{
    uint32_t indexCount;
    uint32_t instanceCount;
    uint32_t firstIndex;
    int32_t  baseVertex;
    uint32_t firstInstance;
} GPUDrawIndexedIndirectCommand;

typedef struct GPUConfig {
    GPUValidationMode validationMode;
    void* userdata;
    void (*callback)(void* userdata, const char* message, GPUBool32 error);
    const char* applicationName;
    const char* engineName;
} GPUConfig;

ALIMER_GPU_API void GPU_GetVersion(int* major, int* minor, int* patch);

ALIMER_GPU_API GPUBool32 GPU_Init(const GPUConfig* config);
ALIMER_GPU_API void GPU_Shutdown(void);

ALIMER_GPU_API GPUSurface GPU_CreateSurface(uintptr_t windowHandle);

#endif /* ALIMER_GPU_H */

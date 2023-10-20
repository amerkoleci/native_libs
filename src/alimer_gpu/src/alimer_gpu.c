// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_gpu_internal.h"
#include <stdlib.h> 
#include <stdio.h>
#include <stdarg.h>

static struct {
    GPUConfig config;
} state;

#define GPU_MAX_MESSAGE_SIZE 1024

void AlimerLogInfo(const char* fmt, ...)
{
    if (!state.config.callback)
        return;

    char msg[GPU_MAX_MESSAGE_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    state.config.callback(state.config.userdata, msg, false);
}

void AlimerLogWarn(const char* fmt, ...)
{
    if (!state.config.callback)
        return;

    char msg[GPU_MAX_MESSAGE_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    state.config.callback(state.config.userdata, msg, false);
}

void AlimerLogError(const char* fmt, ...)
{
    if (!state.config.callback)
        return;

    char msg[GPU_MAX_MESSAGE_SIZE];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    state.config.callback(state.config.userdata, msg, true);
}

static const GPUDriver* drivers[] = {
#if defined(ALIMER_D3D12)
    &D3D12_Driver,
#endif
#if defined(ALIMER_VULKAN)
    &Vulkan_Driver,
#endif
    NULL
};

void GPU_GetVersion(int* major, int* minor, int* patch)
{
    if (major) *major = ALIMER_GPU_VERSION_MAJOR;
    if (minor) *minor = ALIMER_GPU_VERSION_MINOR;
    if (patch) *patch = ALIMER_GPU_VERSION_PATCH;
}

GPUBool32 GPU_Init(const GPUConfig* config)
{
    state.config = *config;

    for (uint32_t i = 0; i < ALIMER_ARRAYSIZE(drivers); ++i)
    {
        if (!drivers[i])
            break;

        if (drivers[i]->IsSupported())
        {
            drivers[i]->Init(config);
            //device = drivers[i]->createDevice(desc);
        }
    }

    return true;
}

void GPU_Shutdown(void)
{
    for (uint32_t i = 0; i < ALIMER_ARRAYSIZE(drivers); ++i)
    {
        if (!drivers[i])
            return;

        if (drivers[i]->IsSupported())
        {
            drivers[i]->Shutdown();
        }
    }
}

GPUSurface GPU_CreateSurface(uintptr_t windowHandle)
{
    GPUSurfaceImpl* surface = (GPUSurfaceImpl*)malloc(sizeof(GPUSurfaceImpl));
    ALIMER_ASSERT(surface);
    memset(surface, 0, sizeof(GPUSurfaceImpl));
    surface->windowHandle = windowHandle;

    for (uint32_t i = 0; i < ALIMER_ARRAYSIZE(drivers); ++i)
    {
        if (!drivers[i])
            break;;

        if (drivers[i]->IsSupported())
        {
            drivers[i]->InitSurface(surface);
        }
    }

    return surface;
}

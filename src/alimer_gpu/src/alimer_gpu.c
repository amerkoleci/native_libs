// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_gpu_internal.h"

static const GPUDriver* drivers[] = {
#if defined(ALIMER_VULKAN)
    &Vulkan_Driver,
#endif
    NULL
};

static struct {
    GPUConfig config;
} state;

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

// Copyright (c) Amer Koleci and Contributors.
// Distributed under the MIT license. See the LICENSE file in the project root for more information.

#include "alimer_gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    GPUConfig config;
    memset(&config, 0, sizeof(config));

#if defined(_DEBUG)
    config.validationMode = GPUValidationMode_Enabled;
#endif

    if (!GPU_Init(&config))
    {
        return EXIT_FAILURE;
    }

    GPU_Shutdown();
    return EXIT_SUCCESS;
}

// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_internal.h"
#include <stdlib.h> // malloc, free

static void* _default_alloc(size_t size, void* user_data)
{
    ALIMER_UNUSED(user_data);
    void* ptr = malloc(size);
    ALIMER_ASSERT(ptr);
    return ptr;
}

static void _default_free(void* ptr, void* user_data)
{
    ALIMER_UNUSED(user_data);
    free(ptr);
}

static void* s_memoryUserData = NULL;
const MemoryAllocationCallbacks DEFAULT_MEMORY_ALLOC_CB = { _default_alloc, _default_free };
const MemoryAllocationCallbacks* MEMORY_ALLOC_CB = &DEFAULT_MEMORY_ALLOC_CB;

void* Alimer_alloc(size_t size)
{
    return MEMORY_ALLOC_CB->Allocate(size, s_memoryUserData);
}

void* Alimer_alloc_clear(size_t size)
{
    void* ptr = Alimer_alloc(size);
    Alimer_clear(ptr, size);
    return ptr;
}

void Alimer_free(void* ptr)
{
    MEMORY_ALLOC_CB->Free(ptr, s_memoryUserData);
}

void Alimer_clear(void* ptr, size_t size)
{
    ALIMER_ASSERT(ptr && (size > 0));
    memset(ptr, 0, size);
}

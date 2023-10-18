// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef ALIMER_INTERNAL_H
#define ALIMER_INTERNAL_H

#include "alimer.h"
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
    _Pragma("clang diagnostic ignored \"-Wunused-function\"")

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
    _Pragma("GCC diagnostic ignored \"-Wunused-function\"")

#define ALIMER_ENABLE_WARNINGS() _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define ALIMER_LIKELY(x) (x)
#define ALIMER_UNLIKELY(x) (x)
#define ALIMER_UNREACHABLE() __assume(false)
#define ALIMER_DEBUG_BREAK() __debugbreak()
#define ALIMER_DISABLE_WARNINGS() __pragma(warning(push, 0))
#define ALIMER_ENABLE_WARNINGS() __pragma(warning(pop))
#endif

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

// Custom allocation callbacks.
_ALIMER_EXTERN const MemoryAllocationCallbacks* MEMORY_ALLOC_CB;

_ALIMER_EXTERN void* Alimer_alloc(size_t size);
_ALIMER_EXTERN void* Alimer_alloc_clear(size_t size);
_ALIMER_EXTERN void Alimer_free(void* ptr);
_ALIMER_EXTERN void Alimer_clear(void* ptr, size_t size);

// Convenience macros for invoking custom memory allocation callbacks.
#define ALIMER_ALLOC(type)     ((type*)Alimer_alloc_clear(sizeof(type)))
#define ALIMER_ALLOCN(type, n) ((type*)Alimer_alloc_clear(sizeof(type) * n))
#define ALIMER_FREE(ptr)       (Alimer_free((void*)(ptr)))

#endif /* ALIMER_INTERNAL_H */

// Copyright © Amer Koleci.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_IMAGE_H
#define _ALIMER_IMAGE_H

#if defined(ALIMER_IMAGE_SHARED_LIBRARY)
#    if defined(_WIN32)
#        if defined(ALIMER_IMAGE_IMPLEMENTATION)
#            define _ALIMER_IMAGE_EXPORT __declspec(dllexport)
#        else
#            define _ALIMER_IMAGE_EXPORT __declspec(dllimport)
#        endif
#    else
#        if defined(ALIMER_IMAGE_IMPLEMENTATION)
#            define _ALIMER_IMAGE_EXPORT __attribute__((visibility("default")))
#        else
#            define _ALIMER_IMAGE_EXPORT
#        endif
#    endif
#else
#    define _ALIMER_IMAGE_EXPORT
#endif

#ifdef __cplusplus
#    define _ALIMER_IMAGE_EXTERN extern "C"
#else
#    define _ALIMER_IMAGE_EXTERN extern
#endif

#define ALIMER_IMAGE_API _ALIMER_IMAGE_EXTERN _ALIMER_IMAGE_EXPORT 

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Similar to WebGPU Format (WGPUTextureFormat) */
typedef enum ImageFormat {
    ImageFormat_Undefined = 0x00000000,
    ImageFormat_R8Unorm = 0x00000001,
    ImageFormat_R8Snorm = 0x00000002,
    ImageFormat_R8Uint = 0x00000003,
    ImageFormat_R8Sint = 0x00000004,
    ImageFormat_R16Uint = 0x00000005,
    ImageFormat_R16Sint = 0x00000006,
    ImageFormat_R16Float = 0x00000007,
    ImageFormat_RG8Unorm = 0x00000008,
    ImageFormat_RG8Snorm = 0x00000009,
    ImageFormat_RG8Uint = 0x0000000A,
    ImageFormat_RG8Sint = 0x0000000B,
    ImageFormat_R32Float = 0x0000000C,
    ImageFormat_R32Uint = 0x0000000D,
    ImageFormat_R32Sint = 0x0000000E,
    ImageFormat_RG16Uint = 0x0000000F,
    ImageFormat_RG16Sint = 0x00000010,
    ImageFormat_RG16Float = 0x00000011,
    ImageFormat_RGBA8Unorm = 0x00000012,
    ImageFormat_RGBA8UnormSrgb = 0x00000013,
    ImageFormat_RGBA8Snorm = 0x00000014,
    ImageFormat_RGBA8Uint = 0x00000015,
    ImageFormat_RGBA8Sint = 0x00000016,
    ImageFormat_BGRA8Unorm = 0x00000017,
    ImageFormat_BGRA8UnormSrgb = 0x00000018,
    ImageFormat_RGB10A2Unorm = 0x00000019,
    ImageFormat_RG11B10Ufloat = 0x0000001A,
    ImageFormat_RGB9E5Ufloat = 0x0000001B,
    ImageFormat_RG32Float = 0x0000001C,
    ImageFormat_RG32Uint = 0x0000001D,
    ImageFormat_RG32Sint = 0x0000001E,
    ImageFormat_RGBA16Uint = 0x0000001F,
    ImageFormat_RGBA16Sint = 0x00000020,
    ImageFormat_RGBA16Float = 0x00000021,
    ImageFormat_RGBA32Float = 0x00000022,
    ImageFormat_RGBA32Uint = 0x00000023,
    ImageFormat_RGBA32Sint = 0x00000024,
    ImageFormat_Stencil8 = 0x00000025,
    ImageFormat_Depth16Unorm = 0x00000026,
    ImageFormat_Depth24Plus = 0x00000027,
    ImageFormat_Depth24PlusStencil8 = 0x00000028,
    ImageFormat_Depth32Float = 0x00000029,
    ImageFormat_Depth32FloatStencil8 = 0x0000002A,
    ImageFormat_BC1RGBAUnorm = 0x0000002B,
    ImageFormat_BC1RGBAUnormSrgb = 0x0000002C,
    ImageFormat_BC2RGBAUnorm = 0x0000002D,
    ImageFormat_BC2RGBAUnormSrgb = 0x0000002E,
    ImageFormat_BC3RGBAUnorm = 0x0000002F,
    ImageFormat_BC3RGBAUnormSrgb = 0x00000030,
    ImageFormat_BC4RUnorm = 0x00000031,
    ImageFormat_BC4RSnorm = 0x00000032,
    ImageFormat_BC5RGUnorm = 0x00000033,
    ImageFormat_BC5RGSnorm = 0x00000034,
    ImageFormat_BC6HRGBUfloat = 0x00000035,
    ImageFormat_BC6HRGBFloat = 0x00000036,
    ImageFormat_BC7RGBAUnorm = 0x00000037,
    ImageFormat_BC7RGBAUnormSrgb = 0x00000038,
    ImageFormat_ETC2RGB8Unorm = 0x00000039,
    ImageFormat_ETC2RGB8UnormSrgb = 0x0000003A,
    ImageFormat_ETC2RGB8A1Unorm = 0x0000003B,
    ImageFormat_ETC2RGB8A1UnormSrgb = 0x0000003C,
    ImageFormat_ETC2RGBA8Unorm = 0x0000003D,
    ImageFormat_ETC2RGBA8UnormSrgb = 0x0000003E,
    ImageFormat_EACR11Unorm = 0x0000003F,
    ImageFormat_EACR11Snorm = 0x00000040,
    ImageFormat_EACRG11Unorm = 0x00000041,
    ImageFormat_EACRG11Snorm = 0x00000042,
    ImageFormat_ASTC4x4Unorm = 0x00000043,
    ImageFormat_ASTC4x4UnormSrgb = 0x00000044,
    ImageFormat_ASTC5x4Unorm = 0x00000045,
    ImageFormat_ASTC5x4UnormSrgb = 0x00000046,
    ImageFormat_ASTC5x5Unorm = 0x00000047,
    ImageFormat_ASTC5x5UnormSrgb = 0x00000048,
    ImageFormat_ASTC6x5Unorm = 0x00000049,
    ImageFormat_ASTC6x5UnormSrgb = 0x0000004A,
    ImageFormat_ASTC6x6Unorm = 0x0000004B,
    ImageFormat_ASTC6x6UnormSrgb = 0x0000004C,
    ImageFormat_ASTC8x5Unorm = 0x0000004D,
    ImageFormat_ASTC8x5UnormSrgb = 0x0000004E,
    ImageFormat_ASTC8x6Unorm = 0x0000004F,
    ImageFormat_ASTC8x6UnormSrgb = 0x00000050,
    ImageFormat_ASTC8x8Unorm = 0x00000051,
    ImageFormat_ASTC8x8UnormSrgb = 0x00000052,
    ImageFormat_ASTC10x5Unorm = 0x00000053,
    ImageFormat_ASTC10x5UnormSrgb = 0x00000054,
    ImageFormat_ASTC10x6Unorm = 0x00000055,
    ImageFormat_ASTC10x6UnormSrgb = 0x00000056,
    ImageFormat_ASTC10x8Unorm = 0x00000057,
    ImageFormat_ASTC10x8UnormSrgb = 0x00000058,
    ImageFormat_ASTC10x10Unorm = 0x00000059,
    ImageFormat_ASTC10x10UnormSrgb = 0x0000005A,
    ImageFormat_ASTC12x10Unorm = 0x0000005B,
    ImageFormat_ASTC12x10UnormSrgb = 0x0000005C,
    ImageFormat_ASTC12x12Unorm = 0x0000005D,
    ImageFormat_ASTC12x12UnormSrgb = 0x0000005E,

    ImageFormat_Force32 = 0x7FFFFFFF
} ImageFormat;

typedef enum ImageDimension {
    IMAGE_DIMENSION_1D = 0,
    IMAGE_DIMENSION_2D = 1,
    IMAGE_DIMENSION_3D = 2,
} ImageDimension;

typedef struct image_t* image;

ALIMER_IMAGE_API image image_create_new(uint32_t width, uint32_t height, ImageFormat format);
ALIMER_IMAGE_API image image_from_memory(const void* data, size_t size);
ALIMER_IMAGE_API void image_destroy(image image);

ALIMER_IMAGE_API ImageDimension image_get_dimension(image image);
ALIMER_IMAGE_API ImageFormat image_get_format(image image);
ALIMER_IMAGE_API uint32_t image_get_width(image image, uint32_t level);
ALIMER_IMAGE_API uint32_t image_get_height(image image, uint32_t level);
ALIMER_IMAGE_API uint32_t image_get_depth(image image, uint32_t level);
ALIMER_IMAGE_API uint32_t image_get_layer_count(image image);
ALIMER_IMAGE_API uint32_t image_get_level_count(image image);
ALIMER_IMAGE_API bool image_is_array(image image);
ALIMER_IMAGE_API bool image_is_cubemap(image image);

ALIMER_IMAGE_API uint32_t image_get_data_size(image image);
ALIMER_IMAGE_API void* image_get_data(image image);

typedef void (*image_save_callback)(uint8_t* pData, uint32_t dataSize);

ALIMER_IMAGE_API bool image_save_png_memory(image image, image_save_callback callback);

#endif /* _ALIMER_IMAGE_H */

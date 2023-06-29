// Copyright © Amer Koleci.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_image.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset
#include <assert.h>

#define _min(a,b) (((a)<(b))?(a):(b))
#define _max(a,b) (((a)>(b))?(a):(b))

#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include "stb_image.h"
#include "stb_image_write.h"
//#include <ktx.h>

typedef struct image_t {
    ImageDimension  dimension;
    uint32_t        width;
    uint32_t        height;
    uint32_t        depthOrArrayLayers;
    ImageFormat     format;
    uint32_t        numLevels;
    uint32_t        numFaces;
    bool            isArray;
    bool            isCubemap;

    uint32_t        dataSize;
    void*           pData;
} image_t;

static image dds_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static image astc_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static image ktx_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;

#if TODO_KTX
    ktxTexture* ktx_texture = 0;
    KTX_error_code ktx_result = ktxTexture_CreateFromMemory(
        data,
        size,
        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
        &ktx_texture
    );

    // Not ktx texture.
    if (ktx_result != KTX_SUCCESS)
    {
        return NULL;
    }

    ImageFormat format = IMAGE_FORMAT_RGBA8;
    if (ktx_texture->classId == ktxTexture2_c)
    {
        ktxTexture2* ktx_texture2 = (ktxTexture2*)ktx_texture;

        if (ktxTexture2_NeedsTranscoding(ktx_texture2))
        {
            // Once transcoded, the ktxTexture object contains the texture data in a native GPU format (e.g. BC7)
            ktx_result = ktxTexture2_TranscodeBasis(ktx_texture2, KTX_TTF_BC7_RGBA, 0);
            //result->format = TEXTURE_FORMAT_BC7;
        }
        else
        {
        }
    }

    Image* result = image_create_new(ktx_texture->baseWidth, ktx_texture->baseHeight, format);
    if (ktx_texture->baseDepth > 1)
    {
        result->dimension = IMAGE_DIMENSION_3D;
        result->depthOrArrayLayers = ktx_texture->baseDepth;
    }
    else
    {
        result->dimension = IMAGE_DIMENSION_2D;
        result->depthOrArrayLayers = ktx_texture->numLayers;
    }

    result->numLevels = ktx_texture->numLevels;
    result->numFaces = ktx_texture->numFaces;
    result->isArray = ktx_texture->isArray;
    result->isCubemap = ktx_texture->isCubemap;

    result->dataSize = (uint32_t)ktx_texture->dataSize;
    result->pData = malloc(ktx_texture->dataSize);
    memcpy(result->pData, ktx_texture->pData, ktx_texture->dataSize);
    ktxTexture_Destroy(ktx_texture);
    return result;
#endif // TODO_KTX

}

static image stb_load_from_memory(const uint8_t* data, size_t size)
{
    int width, height, channels;
    ImageFormat format = ImageFormat_RGBA8Unorm;
    void* image_data;
    uint32_t memorySize = 0;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        image_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels)
        {
            case 1:
                format = ImageFormat_R16Uint;
                memorySize = width * height * sizeof(uint16_t);
                break;
            case 2:
                format = ImageFormat_RG16Uint;
                memorySize = width * height * 2 * sizeof(uint16_t);
                break;
            case 4:
                format = ImageFormat_RGBA16Uint;
                memorySize = width * height * 4 * sizeof(uint16_t);
                break;
            default:
                assert(0);
        }
    }
    else if (stbi_is_hdr_from_memory(data, (int)size))
    {
        image_data = stbi_loadf_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = ImageFormat_RGBA32Float;
        memorySize = width * height * 4 * sizeof(float);
    }
    else
    {
        image_data = stbi_load_from_memory(data, (int)size, &width, &height, NULL, 4);
        format = ImageFormat_RGBA8Unorm;
        memorySize = width * height * 4 * sizeof(uint8_t);
    }

    if (!image_data) {
        return NULL;
    }

    image result = image_create_new(width, height, format);
    result->dataSize = memorySize;
    result->pData = malloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

image image_create_new(uint32_t width, uint32_t height, ImageFormat format) {
    image result = (image_t*)malloc(sizeof(image_t));
    assert(result);
    memset(result, 0, sizeof(image_t));

    result->dimension = IMAGE_DIMENSION_2D;
    result->width = width;
    result->height = height;
    result->depthOrArrayLayers = 1;
    result->numLevels = 1;
    result->numFaces = 1;
    result->format = format;
    result->isArray = false;
    result->isCubemap = false;

    return result;
}

image image_from_memory(const void* data, size_t size) {
    image image = NULL;

    if ((image = dds_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = astc_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = ktx_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = stb_load_from_memory(data, size)) != NULL) {
        return image;
    }

    return NULL;
}

void image_destroy(image image) {
    if (!image)
        return;

    if (image->pData) {
        free(image->pData);
    }

    free(image);
}

ImageDimension image_get_dimension(image image) {
    return image->dimension;
}

ImageFormat image_get_format(image image) {
    return image->format;
}

uint32_t image_get_width(image image, uint32_t level) {
    return _max(image->width >> level, 1);
}

uint32_t image_get_height(image image, uint32_t level) {
    return _max(image->height >> level, 1);
}

uint32_t image_get_depth(image image, uint32_t level) {
    if (image->dimension != IMAGE_DIMENSION_3D) {
        return 1u;
    }

    return _max(image->depthOrArrayLayers >> level, 1);
}

uint32_t image_get_layer_count(image image) {
    if (image->dimension == IMAGE_DIMENSION_3D) {
        return 1u;
    }

    return image->depthOrArrayLayers;
}

uint32_t image_get_level_count(image image) {
    return image->numLevels;
}

bool image_is_array(image image) {
    return image->isArray;
}

bool image_is_cubemap(image image) {
    return image->isCubemap;
}

uint32_t image_get_data_size(image image) {
    return image->dataSize;
}

void* image_get_data(image image) {
    return image->pData;
}

bool image_save_png_memory(image image, image_save_callback callback) {
    int len;
    unsigned char* data = stbi_write_png_to_mem(image->pData, image->width * 4, image->width, image->height, 4, &len);
    if (data == NULL)
        return false;

    callback(data, len);
    STBIW_FREE(data);
    return true;
}

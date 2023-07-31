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

typedef struct ImageImpl {
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
} ImageImpl;

static Image dds_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static Image astc_load_from_memory(const uint8_t* data, size_t size)
{
    return NULL;
}

static Image ktx_load_from_memory(const uint8_t* data, size_t size)
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

static Image stb_load_from_memory(const uint8_t* data, size_t size)
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

    Image result = alimerImageCreateNew(width, height, format);
    result->dataSize = memorySize;
    result->pData = malloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

Image alimerImageCreateNew(uint32_t width, uint32_t height, ImageFormat format) {
    Image result = (ImageImpl*)malloc(sizeof(ImageImpl));
    assert(result);
    memset(result, 0, sizeof(ImageImpl));

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

Image alimerImageCreateFromMemory(const void* data, size_t size) {
    Image image = NULL;

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

void alimerImageDestroy(Image image) {
    if (!image)
        return;

    if (image->pData) {
        free(image->pData);
    }

    free(image);
}

ImageDimension alimerImageGetDimension(Image image) {
    return image->dimension;
}

ImageFormat alimerImageGetFormat(Image image) {
    return image->format;
}

uint32_t alimerImageGetWidth(Image image, uint32_t level) {
    return _max(image->width >> level, 1);
}

uint32_t alimerImageGetHeight(Image image, uint32_t level) {
    return _max(image->height >> level, 1);
}

uint32_t alimerImageGetDepth(Image image, uint32_t level) {
    if (image->dimension != IMAGE_DIMENSION_3D) {
        return 1u;
    }

    return _max(image->depthOrArrayLayers >> level, 1);
}

uint32_t alimerImageGetLayerCount(Image image) {
    if (image->dimension == IMAGE_DIMENSION_3D) {
        return 1u;
    }

    return image->depthOrArrayLayers;
}

uint32_t alimerImageGetLevelCount(Image image) {
    return image->numLevels;
}

bool alimerImageIsArray(Image image) {
    return image->isArray;
}

bool alimerImageIsCubemap(Image image) {
    return image->isCubemap;
}

uint32_t alimerImageGetDataSize(Image image) {
    return image->dataSize;
}

void* alimerImageGetData(Image image) {
    return image->pData;
}

bool alimerImageSavePngMemory(Image image, image_save_callback callback) {
    int len;
    unsigned char* data = stbi_write_png_to_mem(image->pData, image->width * 4, image->width, image->height, 4, &len);
    if (data == NULL)
        return false;

    callback(data, len);
    STBIW_FREE(data);
    return true;
}

// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_internal.h"
#include <stdio.h>

ALIMER_DISABLE_WARNINGS()
#define STBI_NO_STDIO
#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include "third_party/stb_image.h"

#define STBI_WRITE_NO_STDIO
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party/stb_image_write.h"

//#define TINYEXR_IMPLEMENTATION
//#include "third_party/tinyexr.h"

#define QOI_NO_STDIO
#define QOI_IMPLEMENTATION
#define QOI_MALLOC(sz) STBI_MALLOC(sz)
#define QOI_FREE(p) STBI_FREE(p) 
#include "third_party/qoi.h"
//#include <ktx.h>

ALIMER_ENABLE_WARNINGS()

typedef struct AlimerImageImpl {
    ImageDimension  dimension;
    PixelFormat     format;
    uint32_t        width;
    uint32_t        height;
    uint32_t        depthOrArraySize;
    uint32_t        mipLevels;
    Bool32          isCubemap;

    size_t          dataSize;
    void*           pData;
} AlimerImageImpl;

static AlimerImage dds_load_from_memory(const uint8_t* data, size_t size)
{
    ALIMER_UNUSED(data);
    ALIMER_UNUSED(size);

    return NULL;
}

static AlimerImage astc_load_from_memory(const uint8_t* data, size_t size)
{
    ALIMER_UNUSED(data);
    ALIMER_UNUSED(size);

    return NULL;
}

static AlimerImage ktx_load_from_memory(const uint8_t* data, size_t size)
{
    ALIMER_UNUSED(data);
    ALIMER_UNUSED(size);

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
    result->pData = Alimer_alloc(ktx_texture->dataSize);
    memcpy(result->pData, ktx_texture->pData, ktx_texture->dataSize);
    ktxTexture_Destroy(ktx_texture);
    return result;
#endif // TODO_KTX

}

bool AlimerImage_TestQOI(const uint8_t* data, size_t size)
{
    if (size < 4)
        return false;

    for (size_t i = 0; i < ALIMER_MAX(4, size); i++)
    {
        if (data[i] != "qoif"[i])
            return false;
    }

    return true;
}

static AlimerImage qoi_load_from_memory(const uint8_t* data, size_t size)
{
    if (!AlimerImage_TestQOI(data, size))
        return NULL;

    int channels = 4;
    qoi_desc qoi_desc;
    void* result = qoi_decode(data, (int)size, &qoi_desc, channels);

    if (result != NULL)
    {
        AlimerImage image = Alimer_ImageCreate2D(PixelFormat_RGBA8Unorm, qoi_desc.width, qoi_desc.height, 1u, 1u);
        image->dataSize = qoi_desc.width * qoi_desc.height * channels * sizeof(uint8_t);
        image->pData = Alimer_alloc(image->dataSize);
        memcpy(image->pData, result, image->dataSize);

        QOI_FREE(result);
        return image;
    }

    return NULL;
}

static AlimerImage stb_load_from_memory(const uint8_t* data, size_t size)
{
    int width, height, channels;
    PixelFormat format = PixelFormat_RGBA8Unorm;
    void* image_data;
    uint32_t memorySize = 0;
    if (stbi_is_16_bit_from_memory(data, (int)size))
    {
        image_data = stbi_load_16_from_memory(data, (int)size, &width, &height, &channels, 0);
        switch (channels)
        {
            case 1:
                format = PixelFormat_R16Uint;
                memorySize = width * height * sizeof(uint16_t);
                break;
            case 2:
                format = PixelFormat_RG16Uint;
                memorySize = width * height * 2 * sizeof(uint16_t);
                break;
            case 4:
                format = PixelFormat_RGBA16Uint;
                memorySize = width * height * 4 * sizeof(uint16_t);
                break;
            default:
                assert(0);
        }
    }
    else if (stbi_is_hdr_from_memory(data, (int)size))
    {
        image_data = stbi_loadf_from_memory(data, (int)size, &width, &height, &channels, 4);
        format = PixelFormat_RGBA32Float;
        memorySize = width * height * 4 * sizeof(float);
    }
    else
    {
        image_data = stbi_load_from_memory(data, (int)size, &width, &height, &channels, 4);
        format = PixelFormat_RGBA8Unorm;
        memorySize = width * height * 4 * sizeof(uint8_t);
    }

    if (!image_data) {
        return NULL;
    }

    AlimerImage result = Alimer_ImageCreate2D(format, width, height, 1, 1);
    result->dataSize = memorySize;
    result->pData = Alimer_alloc(memorySize);
    memcpy(result->pData, image_data, memorySize);
    stbi_image_free(image_data);
    return result;
}

bool IsPow2(size_t x)
{
    return ((x != 0) && !(x & (x - 1)));
}


uint32_t CountMips(uint32_t width, uint32_t height)
{
    uint32_t mipLevels = 1;

    while (height > 1 || width > 1)
    {
        if (height > 1)
            height >>= 1;

        if (width > 1)
            width >>= 1;

        ++mipLevels;
    }

    return mipLevels;
}


bool CalculateMipLevels(uint32_t width, uint32_t height, uint32_t* mipLevels)
{
    if (*mipLevels > 1)
    {
        const uint32_t maxMips = CountMips(width, height);
        if (*mipLevels > maxMips)
            return false;
    }
    else if (*mipLevels == 0)
    {
        *mipLevels = CountMips(width, height);
    }
    else
    {
        *mipLevels = 1;
    }
    return true;
}

uint32_t BitsPerPixel(PixelFormat format)
{
    switch (format)
    {
        case PixelFormat_RGBA32Uint:
        case PixelFormat_RGBA32Sint:
        case PixelFormat_RGBA32Float:
            return 128;

        case PixelFormat_RG32Uint:
        case PixelFormat_RG32Sint:
        case PixelFormat_RG32Float:
        case PixelFormat_RGBA16Unorm:
        case PixelFormat_RGBA16Snorm:
        case PixelFormat_RGBA16Uint:
        case PixelFormat_RGBA16Sint:
        case PixelFormat_RGBA16Float:
            return 64;

        case PixelFormat_R32Uint:
        case PixelFormat_R32Sint:
        case PixelFormat_R32Float:
        case PixelFormat_RG16Unorm:
        case PixelFormat_RG16Snorm:
        case PixelFormat_RG16Uint:
        case PixelFormat_RG16Sint:
        case PixelFormat_RG16Float:
        case PixelFormat_RGBA8Unorm:
        case PixelFormat_RGBA8UnormSrgb:
        case PixelFormat_RGBA8Snorm:
        case PixelFormat_RGBA8Uint:
        case PixelFormat_RGBA8Sint:
        case PixelFormat_BGRA8Unorm:
        case PixelFormat_BGRA8UnormSrgb:
        case PixelFormat_RGB10A2Unorm:
        case PixelFormat_RGB10A2Uint:
        case PixelFormat_RG11B10UFloat:
        case PixelFormat_RGB9E5UFloat:
            return 32;

        case PixelFormat_R16Unorm:
        case PixelFormat_R16Snorm:
        case PixelFormat_R16Uint:
        case PixelFormat_R16Sint:
        case PixelFormat_R16Float:
        case PixelFormat_RG8Unorm:
        case PixelFormat_RG8Snorm:
        case PixelFormat_RG8Uint:
        case PixelFormat_RG8Sint:
            // Packed 16-Bit formats
        case PixelFormat_BGRA4Unorm:
        case PixelFormat_B5G6R5Unorm:
        case PixelFormat_BGR5A1Unorm:
            return 16;

        case PixelFormat_R8Unorm:
        case PixelFormat_R8Snorm:
        case PixelFormat_R8Uint:
        case PixelFormat_R8Sint:
        case PixelFormat_BC2RGBAUnorm:
        case PixelFormat_BC2RGBAUnormSrgb:
        case PixelFormat_BC3RGBAUnorm:
        case PixelFormat_BC3RGBAUnormSrgb:
        case PixelFormat_BC5RGUnorm:
        case PixelFormat_BC5RGSnorm:
        case PixelFormat_BC6HRGBUfloat:
        case PixelFormat_BC6HRGBFloat:
        case PixelFormat_BC7RGBAUnorm:
        case PixelFormat_BC7RGBAUnormSrgb:
            return 8;

        case PixelFormat_BC1RGBAUnorm:
        case PixelFormat_BC1RGBAUnormSrgb:
        case PixelFormat_BC4RUnorm:
        case PixelFormat_BC4RSnorm:
            return 4;

        default:
            return 0;
    }
}

bool GetSurfaceInfo(PixelFormat format, uint32_t width, uint32_t height, uint32_t* pRowPitch, uint32_t* pSlicePitch, uint32_t* pRowCount)
{
    uint32_t rowPitch = 0;
    uint32_t slicePitch = 0;
    uint32_t rowCount = 0;

    bool bc = false;
    bool packed = false;
    bool planar = false;
    uint32_t bpe = 0;

    switch (format)
    {
        case PixelFormat_BC1RGBAUnorm:
        case PixelFormat_BC1RGBAUnormSrgb:
        case PixelFormat_BC4RUnorm:
        case PixelFormat_BC4RSnorm:
            bc = true;
            bpe = 8;
            break;

        case PixelFormat_BC2RGBAUnorm:
        case PixelFormat_BC2RGBAUnormSrgb:
        case PixelFormat_BC3RGBAUnorm:
        case PixelFormat_BC3RGBAUnormSrgb:
        case PixelFormat_BC5RGUnorm:
        case PixelFormat_BC5RGSnorm:
        case PixelFormat_BC6HRGBUfloat:
        case PixelFormat_BC6HRGBFloat:
        case PixelFormat_BC7RGBAUnorm:
        case PixelFormat_BC7RGBAUnormSrgb:
            bc = true;
            bpe = 16;
            break;

        default:
            break;
    }

    if (bc)
    {
        uint32_t numBlocksWide = 0;
        if (width > 0)
        {
            numBlocksWide = ALIMER_MAX(1u, (width + 3u) / 4u);
        }
        uint32_t numBlocksHigh = 0;
        if (height > 0)
        {
            numBlocksHigh = ALIMER_MAX(1u, (height + 3u) / 4u);
        }
        rowPitch = numBlocksWide * bpe;
        slicePitch = rowPitch * numBlocksHigh;
        rowCount = numBlocksHigh;
    }
    else if (packed)
    {
        rowPitch = ((width + 1) >> 1) * bpe;
        rowCount = height;
        slicePitch = rowPitch * height;
    }
    else if (planar)
    {
        rowPitch = ((width + 1) >> 1) * bpe;
        slicePitch = (rowPitch * height) + ((rowPitch * height + 1) >> 1);
        rowCount = height + ((height + 1u) >> 1);
    }
    else
    {
        uint32_t bpp = BitsPerPixel(format);
        rowPitch = (width * bpp + 7) / 8; // round up to nearest byte
        rowCount = height;
        slicePitch = rowPitch * height;
    }

    if (pRowPitch)
    {
        *pRowPitch = rowPitch;
    }

    if (pSlicePitch)
    {
        *pSlicePitch = slicePitch;
    }

    if (pRowCount)
    {
        *pRowCount = rowCount;
    }

    return true;
}

bool DetermineImageArray(AlimerImage image)
{
    assert(image->width > 0 && image->height > 0 && image->depthOrArraySize > 0);
    assert(image->mipLevels > 0);

    size_t totalPixelSize = 0;
    size_t nimages = 0;

    switch (image->dimension)
    {
        case ImageDimension_1D:
        case ImageDimension_2D:
            for (uint32_t arrayIndex = 0; arrayIndex < image->depthOrArraySize; ++arrayIndex)
            {
                uint32_t w = image->width;
                uint32_t h = image->height;

                for (uint32_t mipLevel = 0; mipLevel < image->mipLevels; ++mipLevel)
                {
                    uint32_t rowPitch, slicePitch;
                    if (!GetSurfaceInfo(image->format, w, h, &rowPitch, &slicePitch, NULL))
                    {
                        return false;
                    }

                    totalPixelSize += slicePitch;
                    ++nimages;

                    if (h > 1)
                        h >>= 1;

                    if (w > 1)
                        w >>= 1;
                }
            }
            break;

        case ImageDimension_3D:
        {
            uint32_t w = image->width;
            uint32_t h = image->height;
            uint32_t d = image->depthOrArraySize;

            for (uint32_t mipLevel = 0; mipLevel < image->mipLevels; ++mipLevel)
            {
                uint32_t rowPitch, slicePitch;
                if (!GetSurfaceInfo(image->format, w, h, &rowPitch, &slicePitch, NULL))
                {
                    return false;
                }

                for (size_t slice = 0; slice < d; ++slice)
                {
                    totalPixelSize += slicePitch;
                    ++nimages;
                }

                if (h > 1)
                    h >>= 1;

                if (w > 1)
                    w >>= 1;

                if (d > 1)
                    d >>= 1;
            }
        }
        break;

        default:
            return false;
    }

    //nImages = nimages;
    image->dataSize = totalPixelSize;

    return true;
}

AlimerImage Alimer_ImageCreate2D(PixelFormat format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevels)
{
    if (format == PixelFormat_Undefined || !width || !height || !arraySize)
        return NULL;

    if (!CalculateMipLevels(width, height, &mipLevels))
        return NULL;

    AlimerImageImpl* image = ALIMER_ALLOC(AlimerImageImpl);
    assert(image);

    image->dimension = ImageDimension_2D;
    image->format = format;
    image->width = width;
    image->height = height;
    image->depthOrArraySize = arraySize;
    image->mipLevels = mipLevels;
    image->isCubemap = false;
    if (!DetermineImageArray(image))
    {
        ALIMER_FREE(image);
        return NULL;
    }

    return image;
}

AlimerImage Alimer_ImageCreateFromMemory(const void* data, size_t size)
{
    AlimerImage image = NULL;

    if ((image = dds_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = astc_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = ktx_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = qoi_load_from_memory(data, size)) != NULL) {
        return image;
    }

    if ((image = stb_load_from_memory(data, size)) != NULL) {
        return image;
    }

    return NULL;
}

void Alimer_ImageDestroy(AlimerImage image)
{
    if (!image)
        return;

    if (image->pData)
    {
        ALIMER_FREE(image->pData);
    }

    ALIMER_FREE(image);
}

ImageDimension Alimer_ImageGetDimension(AlimerImage image)
{
    return image->dimension;
}

PixelFormat Alimer_ImageGetFormat(AlimerImage image)
{
    return image->format;
}

uint32_t Alimer_ImageGetWidth(AlimerImage image, uint32_t level)
{
    return ALIMER_MAX(image->width >> level, 1);
}

uint32_t Alimer_ImageGetHeight(AlimerImage image, uint32_t level)
{
    return ALIMER_MAX(image->height >> level, 1);
}

uint32_t Alimer_ImageGetDepth(AlimerImage image, uint32_t level)
{
    if (image->dimension != ImageDimension_3D) {
        return 1u;
    }

    return ALIMER_MAX(image->depthOrArraySize >> level, 1);
}

uint32_t Alimer_ImageGetArraySize(AlimerImage image)
{
    if (image->dimension == ImageDimension_3D) {
        return 1u;
    }

    return ALIMER_MAX(image->depthOrArraySize, 1u);
}

uint32_t Alimer_ImageGetMipLevels(AlimerImage image)
{
    return image->mipLevels;
}

Bool32 Alimer_ImageIsCubemap(AlimerImage image)
{
    return image->isCubemap;
}

void* Alimer_ImageGetData(AlimerImage image, size_t* size)
{
    if(size)
        *size = image->dataSize;

    return image->pData;
}

Bool32 Alimer_ImageSaveBmp(AlimerImage image, AlimerImageSaveCallback callback)
{
    int res = stbi_write_bmp_to_func((stbi_write_func*)callback, image, image->width, image->height, 4, image->pData);
    if (res != 0)
        return true;

    return false;
}

Bool32 Alimer_ImageSavePng(AlimerImage image, AlimerImageSaveCallback callback)
{
    int res = stbi_write_png_to_func((stbi_write_func*)callback, image, image->width, image->height, 4, image->pData, image->width * 4);
    if (res != 0)
        return true;

    return false;
}

Bool32 Alimer_ImageSaveJpg(AlimerImage image, int quality, AlimerImageSaveCallback callback)
{
    int res = stbi_write_jpg_to_func((stbi_write_func*)callback, image, image->width, image->height, 4, image->pData, quality);
    if (res != 0)
        return true;

    return false;
}

Bool32 Alimer_ImageSaveTga(AlimerImage image, AlimerImageSaveCallback callback)
{
    int res = stbi_write_tga_to_func((stbi_write_func*)callback, image, image->width, image->height, 4, image->pData);
    if (res != 0)
        return true;

    return false;
}

Bool32 Alimer_ImageSaveHdr(AlimerImage image, AlimerImageSaveCallback callback)
{
    int res = stbi_write_hdr_to_func((stbi_write_func*)callback, image, image->width, image->height, 4, (const float*)image->pData);
    if (res != 0)
        return true;

    return false;
}
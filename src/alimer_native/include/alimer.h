// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef ALIMER_H
#define ALIMER_H

#if defined(ALIMER_SHARED_LIBRARY)
#   if defined(_WIN32) || defined(__CYGWIN__)
#       if defined(ALIMER_IMPLEMENTATION)
#           define _ALIMER_API_EXPORT __declspec(dllexport)
#       else
#           define _ALIMER_API_EXPORT __declspec(dllimport)
#       endif
#       define ALIMER_CALL __cdecl
#   else
#       if defined(ALIMER_AUDIO_IMPLEMENTATION)
#           define _ALIMER_API_EXPORT __attribute__((visibility("default")))
#       else
#           define _ALIMER_API_EXPORT
#       endif
#       define ALIMER_CALL
#   endif
#else
#   define _ALIMER_API_EXPORT
#   define ALIMER_CALL
#endif

#ifdef __cplusplus
#    define _ALIMER_EXTERN extern "C"
#else
#    define _ALIMER_EXTERN extern
#endif

#define ALIMER_API _ALIMER_EXTERN _ALIMER_API_EXPORT 

#include <stddef.h>
#include <stdint.h>

typedef uint32_t Flags;
typedef uint32_t Bool32;

/* Defines */
#define ALIMER_VERSION_MAJOR	1
#define ALIMER_VERSION_MINOR	0
#define ALIMER_VERSION_PATCH	0

/* Types */
typedef struct AlimerImageImpl* AlimerImage;
typedef struct AlimerSoundImpl* AlimerSound;
typedef struct AlimerFontImpl* AlimerFont;

ALIMER_API void Alimer_GetVersion(int* major, int* minor, int* patch);

/* Global types */
typedef struct Alimer_Vector3
{
    float x;
    float y;
    float z;
} Alimer_Vector3;

/* Memory */
typedef struct MemoryAllocationCallbacks {
    void* (*Allocate)(size_t size, void* userData);
    void (*Free)(void* ptr, void* userData);
} MemoryAllocationCallbacks;

ALIMER_API void Alimer_SetAllocationCallbacks(const MemoryAllocationCallbacks* callback, void* userData);

/* Image */
/* Similar to WebGPU Format (WGPUTextureFormat) with some additions */
typedef enum PixelFormat {
    PixelFormat_Undefined = 0,

    // 8-bit formats
    PixelFormat_R8Unorm,
    PixelFormat_R8Snorm,
    PixelFormat_R8Uint,
    PixelFormat_R8Sint,
    // 16-bit formats
    PixelFormat_R16Unorm,
    PixelFormat_R16Snorm,
    PixelFormat_R16Uint,
    PixelFormat_R16Sint,
    PixelFormat_R16Float,
    PixelFormat_RG8Unorm,
    PixelFormat_RG8Snorm,
    PixelFormat_RG8Uint,
    PixelFormat_RG8Sint,
    // Packed 16-Bit formats
    PixelFormat_BGRA4Unorm,
    PixelFormat_B5G6R5Unorm,
    PixelFormat_BGR5A1Unorm,
    // 32-bit formats
    PixelFormat_R32Uint,
    PixelFormat_R32Sint,
    PixelFormat_R32Float,
    PixelFormat_RG16Unorm,
    PixelFormat_RG16Snorm,
    PixelFormat_RG16Uint,
    PixelFormat_RG16Sint,
    PixelFormat_RG16Float,
    PixelFormat_RGBA8Unorm,
    PixelFormat_RGBA8UnormSrgb,
    PixelFormat_RGBA8Snorm,
    PixelFormat_RGBA8Uint,
    PixelFormat_RGBA8Sint,
    PixelFormat_BGRA8Unorm,
    PixelFormat_BGRA8UnormSrgb,
    // Packed 32-Bit Pixel Formats
    PixelFormat_RGB10A2Unorm,
    PixelFormat_RGB10A2Uint,
    PixelFormat_RG11B10UFloat,
    PixelFormat_RGB9E5UFloat,
    // 64-bit formats
    PixelFormat_RG32Uint,
    PixelFormat_RG32Sint,
    PixelFormat_RG32Float,
    PixelFormat_RGBA16Unorm,
    PixelFormat_RGBA16Snorm,
    PixelFormat_RGBA16Uint,
    PixelFormat_RGBA16Sint,
    PixelFormat_RGBA16Float,
    // 128-bit formats
    PixelFormat_RGBA32Uint,
    PixelFormat_RGBA32Sint,
    PixelFormat_RGBA32Float,
    // Depth-stencil formats
    PixelFormat_Depth16Unorm,
    PixelFormat_Depth24UnormStencil8,
    PixelFormat_Depth32Float,
    PixelFormat_Depth32FloatStencil8,
    // BC compressed formats
    PixelFormat_BC1RGBAUnorm,
    PixelFormat_BC1RGBAUnormSrgb,
    PixelFormat_BC2RGBAUnorm,
    PixelFormat_BC2RGBAUnormSrgb,
    PixelFormat_BC3RGBAUnorm,
    PixelFormat_BC3RGBAUnormSrgb,
    PixelFormat_BC4RUnorm,
    PixelFormat_BC4RSnorm,
    PixelFormat_BC5RGUnorm,
    PixelFormat_BC5RGSnorm,
    PixelFormat_BC6HRGBUfloat,
    PixelFormat_BC6HRGBFloat,
    PixelFormat_BC7RGBAUnorm,
    PixelFormat_BC7RGBAUnormSrgb,
    // ETC2/EAC compressed formats
    PixelFormat_ETC2RGB8Unorm,
    PixelFormat_ETC2RGB8UnormSrgb,
    PixelFormat_ETC2RGB8A1Unorm,
    PixelFormat_ETC2RGB8A1UnormSrgb,
    PixelFormat_ETC2RGBA8Unorm,
    PixelFormat_ETC2RGBA8UnormSrgb,
    PixelFormat_EACR11Unorm,
    PixelFormat_EACR11Snorm,
    PixelFormat_EACRG11Unorm,
    PixelFormat_EACRG11Snorm,
    // ASTC compressed formats
    PixelFormat_ASTC4x4Unorm,
    PixelFormat_ASTC4x4UnormSrgb,
    PixelFormat_ASTC5x4Unorm,
    PixelFormat_ASTC5x4UnormSrgb,
    PixelFormat_ASTC5x5Unorm,
    PixelFormat_ASTC5x5UnormSrgb,
    PixelFormat_ASTC6x5Unorm,
    PixelFormat_ASTC6x5UnormSrgb,
    PixelFormat_ASTC6x6Unorm,
    PixelFormat_ASTC6x6UnormSrgb,
    PixelFormat_ASTC8x5Unorm,
    PixelFormat_ASTC8x5UnormSrgb,
    PixelFormat_ASTC8x6Unorm,
    PixelFormat_ASTC8x6UnormSrgb,
    PixelFormat_ASTC8x8Unorm,
    PixelFormat_ASTC8x8UnormSrgb,
    PixelFormat_ASTC10x5Unorm,
    PixelFormat_ASTC10x5UnormSrgb,
    PixelFormat_ASTC10x6Unorm,
    PixelFormat_ASTC10x6UnormSrgb,
    PixelFormat_ASTC10x8Unorm,
    PixelFormat_ASTC10x8UnormSrgb,
    PixelFormat_ASTC10x10Unorm,
    PixelFormat_ASTC10x10UnormSrgb,
    PixelFormat_ASTC12x10Unorm,
    PixelFormat_ASTC12x10UnormSrgb,
    PixelFormat_ASTC12x12Unorm,
    PixelFormat_ASTC12x12UnormSrgb,

    _PixelFormat_Count,
    _PixelFormat_Force32 = 0x7FFFFFFF
} PixelFormat;

typedef enum ImageDimension {
    ImageDimension_1D = 0,
    ImageDimension_2D = 1,
    ImageDimension_3D = 2,

    _ImageDimension_Force32 = 0x7FFFFFFF
} ImageDimension;

ALIMER_API AlimerImage Alimer_ImageCreate2D(PixelFormat format, uint32_t width, uint32_t height, uint32_t arraySize, uint32_t mipLevels);
ALIMER_API AlimerImage Alimer_ImageCreateFromMemory(const void* data, size_t size);
ALIMER_API void Alimer_ImageDestroy(AlimerImage image);

ALIMER_API ImageDimension Alimer_ImageGetDimension(AlimerImage image);
ALIMER_API PixelFormat Alimer_ImageGetFormat(AlimerImage image);
ALIMER_API uint32_t Alimer_ImageGetWidth(AlimerImage image, uint32_t level);
ALIMER_API uint32_t Alimer_ImageGetHeight(AlimerImage image, uint32_t level);
ALIMER_API uint32_t Alimer_ImageGetDepth(AlimerImage image, uint32_t level);
ALIMER_API uint32_t Alimer_ImageGetArraySize(AlimerImage image);
ALIMER_API uint32_t Alimer_ImageGetMipLevels(AlimerImage image);
ALIMER_API Bool32 Alimer_ImageIsCubemap(AlimerImage image);

ALIMER_API void* Alimer_ImageGetData(AlimerImage image, size_t* size);

typedef void (ALIMER_CALL* AlimerImageSaveCallback)(AlimerImage image, void* pData, uint32_t dataSize);

ALIMER_API Bool32 Alimer_ImageSaveBmp(AlimerImage image, AlimerImageSaveCallback callback);
ALIMER_API Bool32 Alimer_ImageSavePng(AlimerImage image, AlimerImageSaveCallback callback);
ALIMER_API Bool32 Alimer_ImageSaveJpg(AlimerImage image, int quality, AlimerImageSaveCallback callback);
ALIMER_API Bool32 Alimer_ImageSaveTga(AlimerImage image, AlimerImageSaveCallback callback);
ALIMER_API Bool32 Alimer_ImageSaveHdr(AlimerImage image, AlimerImageSaveCallback callback);

/* Audio */
typedef enum SoundFlags
{
    SoundFlags_None = 0,
    SoundFlags_Stream = 0x00000001,
    SoundFlags_Decode = 0x00000002,
    SoundFlags_Async = 0x00000004,
    SoundFlags_WaitInit = 0x00000008,

    SoundFlags_NoDefaultAttachment = 0x00001000,
    SoundFlags_NoPitch = 0x00002000,
    SoundFlags_NoSpatialization = 0x00004000,

    _SoundFlags_Force32 = 0x7FFFFFFF
} SoundFlags;

typedef struct AudioConfig {
    uint32_t listener_count;
    uint32_t channels;
    uint32_t sample_rate;
} AudioConfig;

ALIMER_API void Alimer_AudioGetVersion(int* major, int* minor, int* patch);
ALIMER_API Bool32 Alimer_AudioInit(void/*const AudioConfig* config*/);
ALIMER_API void Alimer_AudioShutdown(void);
ALIMER_API Bool32 Alimer_AudioStart(void);
ALIMER_API Bool32 Alimer_AudioStop(void);

ALIMER_API uint32_t Alimer_AudioGetOutputChannels(void);
ALIMER_API uint32_t Alimer_AudioGetOutputSampleRate(void);

ALIMER_API float Alimer_AudioGetMasterVolume(void);
ALIMER_API Bool32 Alimer_AudioSetMasterVolume(float volume);

ALIMER_API AlimerSound Alimer_SoundCreate(const char* path, uint32_t flags);
ALIMER_API void Alimer_SoundDestroy(AlimerSound sound);
ALIMER_API void Alimer_SoundPlay(AlimerSound sound);
ALIMER_API void Alimer_SoundStop(AlimerSound sound);
ALIMER_API float Alimer_SoundGetVolume(AlimerSound sound);
ALIMER_API void Alimer_SoundSetVolume(AlimerSound sound, float value);
ALIMER_API float Alimer_SoundGetPitch(AlimerSound sound);
ALIMER_API void Alimer_SoundSetPitch(AlimerSound sound, float value);
ALIMER_API float Alimer_SoundGetPan(AlimerSound sound);
ALIMER_API void Alimer_SoundSetPan(AlimerSound sound, float value);

ALIMER_API Bool32 Alimer_SoundIsPlaying(AlimerSound sound);
ALIMER_API Bool32 Alimer_SoundGetFinished(AlimerSound sound);

ALIMER_API Bool32 Alimer_SoundGetLengthPcmFrames(AlimerSound sound, uint64_t* result);
ALIMER_API Bool32 Alimer_SoundGetCursorPcmFrames(AlimerSound sound, uint64_t* result);
ALIMER_API Bool32 Alimer_SoundSetCursorPcmFrames(AlimerSound sound, uint64_t value);

ALIMER_API Bool32 Alimer_SoundIsLooping(AlimerSound sound);
ALIMER_API void Alimer_SoundSetLooping(AlimerSound sound, Bool32 value);
ALIMER_API void Alimer_SoundGetLoopPcmFrames(AlimerSound sound, uint64_t* pLoopBegInFrames, uint64_t* pLoopEndInFrames);
ALIMER_API Bool32 Alimer_SoundSetLoopPcmFrames(AlimerSound sound, uint64_t loopBegInFrames, uint64_t loopEndInFrames);

ALIMER_API Bool32 Alimer_SoundIsSpatialized(AlimerSound sound);
ALIMER_API void Alimer_SoundSetSpatialized(AlimerSound sound, Bool32 value);

ALIMER_API void Alimer_SoundGetPosition(AlimerSound sound, Alimer_Vector3* result);
ALIMER_API void Alimer_SoundSetPosition(AlimerSound sound, Alimer_Vector3* value);
ALIMER_API void Alimer_SoundGetVelocity(AlimerSound sound, Alimer_Vector3* result);
ALIMER_API void Alimer_SoundSetVelocity(AlimerSound sound, Alimer_Vector3* value);
ALIMER_API void Alimer_SoundGetDirection(AlimerSound sound, Alimer_Vector3* result);
ALIMER_API void Alimer_SoundSetDirection(AlimerSound sound, Alimer_Vector3* value);

/* Font */
ALIMER_API AlimerFont Alimer_FontCreateFromMemory(const uint8_t* data, size_t size);
ALIMER_API void Alimer_FontDestroy(AlimerFont font);
ALIMER_API void Alimer_FontGetMetrics(AlimerFont font, int* ascent, int* descent, int* linegap);
ALIMER_API int Alimer_FontGetGlyphIndex(AlimerFont font, int codepoint);
ALIMER_API float Alimer_FontGetScale(AlimerFont font, float size);
ALIMER_API float Alimer_FontGetKerning(AlimerFont font, int glyph1, int glyph2, float scale);
ALIMER_API void Alimer_FontGetCharacter(AlimerFont font, int glyph, float scale, int* width, int* height, float* advance, float* offsetX, float* offsetY, int* visible);
ALIMER_API void Alimer_FontGetPixels(AlimerFont font, uint8_t* dest, int glyph, int width, int height, float scale);

#endif /* ALIMER_H */

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

/* Types */
typedef struct AlimerSoundImpl* AlimerSound;
typedef struct AlimerFontImpl* AlimerFont;

/* Memory */
typedef struct MemoryAllocationCallbacks {
    void* (*Allocate)(size_t size, void* userData);
    void (*Free)(void* ptr, void* userData);
} MemoryAllocationCallbacks;

ALIMER_API void Alimer_SetAllocationCallbacks(const MemoryAllocationCallbacks* callback, void* userData);

/* Audio */

typedef struct AudioConfig {
    uint32_t listener_count;
    uint32_t channels;
    uint32_t sample_rate;
} AudioConfig;

ALIMER_API void Alimer_GetVersion(uint32_t* major, uint32_t* minor, uint32_t* patch);

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
ALIMER_API Bool32 Alimer_SoundIsPlaying(AlimerSound sound);
ALIMER_API Bool32 Alimer_SoundIsLooping(AlimerSound sound);
ALIMER_API void Alimer_SoundSetLooping(AlimerSound sound, Bool32 value);
ALIMER_API Bool32 Alimer_SoundGetFinished(AlimerSound sound);

/* Font */
ALIMER_API AlimerFont Alimer_FontCreateFromMemory(const uint8_t* data, size_t size);
ALIMER_API void Alimer_FontDestroy(AlimerFont font);
ALIMER_API void Alimer_FontGetMetrics(AlimerFont font, int* ascent, int* descent, int* linegap);
ALIMER_API int Alimer_FontGetGlyphIndex(AlimerFont font, int codepoint);
ALIMER_API float Alimer_FontGetScale(AlimerFont font, float size);
ALIMER_API float Alimer_FontGetKerning(AlimerFont font, int glyph1, int glyph2, float scale);
ALIMER_API void Alimer_FontGetCharacter(AlimerFont font, int glyph, float scale, int* width, int* height, float* advance, float* offsetX, float* offsetY, int* visible);
ALIMER_API void Alimer_FontGetPixels(AlimerFont font, uint8_t* dest, int glyph, int width, int height, float scale);


#endif /* _ALIMER_AUDIO_H_ */

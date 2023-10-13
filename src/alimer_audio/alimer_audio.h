// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#ifndef _ALIMER_AUDIO_H_
#define _ALIMER_AUDIO_H_

#if defined(ALIMER_AUDIO_SHARED_LIBRARY)
#   if defined(_WIN32) || defined(__CYGWIN__)
#       if defined(ALIMER_AUDIO_IMPLEMENTATION)
#           define _ALIMER_AUDIO_EXPORT __declspec(dllexport)
#       else
#           define _ALIMER_AUDIO_EXPORT __declspec(dllimport)
#       endif
#       define _ALIMER_AUDIO_CALL __cdecl
#   else
#       if defined(ALIMER_AUDIO_IMPLEMENTATION)
#           define _ALIMER_AUDIO_EXPORT __attribute__((visibility("default")))
#       else
#           define _ALIMER_AUDIO_EXPORT
#       endif
#       define _ALIMER_AUDIO_CALL
#   endif
#else
#   define _ALIMER_AUDIO_EXPORT
#   define _ALIMER_AUDIO_CALL
#endif

#ifdef __cplusplus
#    define _ALIMER_AUDIO_EXTERN extern "C"
#else
#    define _ALIMER_AUDIO_EXTERN extern
#endif

#define ALIMER_AUDIO_API _ALIMER_AUDIO_EXTERN _ALIMER_AUDIO_EXPORT 

#include <stddef.h>
#include <stdint.h>

typedef uint32_t Flags;
typedef uint32_t Bool32;

typedef struct AlimerSound AlimerSound;

typedef struct AudioConfig {
    uint32_t listener_count;
    uint32_t channels;
    uint32_t sample_rate;
} AudioConfig;

ALIMER_AUDIO_API Bool32 Alimer_AudioInit(const AudioConfig* config);
ALIMER_AUDIO_API void Alimer_AudioShutdown(void);
ALIMER_AUDIO_API Bool32 Alimer_AudioStart(void);
ALIMER_AUDIO_API Bool32 Alimer_AudioStop(void);

ALIMER_AUDIO_API uint32_t Alimer_AudioGetOutputChannels(void);
ALIMER_AUDIO_API uint32_t Alimer_AudioGetOutputSampleRate(void);

ALIMER_AUDIO_API float Alimer_AudioGetMasterVolume(void);
ALIMER_AUDIO_API Bool32 Alimer_AudioSetMasterVolume(float volume);

ALIMER_AUDIO_API AlimerSound* Alimer_SoundCreate(const char* path, uint32_t flags);
ALIMER_AUDIO_API void Alimer_SoundDestroy(AlimerSound* sound);
ALIMER_AUDIO_API void Alimer_SoundPlay(AlimerSound* sound);
ALIMER_AUDIO_API void Alimer_SoundStop(AlimerSound* sound);
ALIMER_AUDIO_API float Alimer_SoundGetVolume(AlimerSound* sound);
ALIMER_AUDIO_API void Alimer_SoundSetVolume(AlimerSound* sound, float value);
ALIMER_AUDIO_API float Alimer_SoundGetPitch(AlimerSound* sound);
ALIMER_AUDIO_API void Alimer_SoundSetPitch(AlimerSound* sound, float value);
ALIMER_AUDIO_API Bool32 Alimer_SoundIsPlaying(AlimerSound* sound);
ALIMER_AUDIO_API Bool32 Alimer_SoundIsLooping(AlimerSound* sound);
ALIMER_AUDIO_API void Alimer_SoundSetLooping(AlimerSound* sound, Bool32 value);
ALIMER_AUDIO_API Bool32 Alimer_SoundGetFinished(AlimerSound* sound);

#endif /* _ALIMER_AUDIO_H_ */

// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_audio.h"
#include <stdbool.h>
#include <stdlib.h> // malloc, free
#include <string.h> // memset

#ifndef _AUDIO_ASSERT
#   include <assert.h>
#   define _AUDIO_ASSERT(c) assert(c)
#endif

#define _UNUSED(x) (void)(x)

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"    /* Enables Vorbis decoding. */

//#define MA_API _ALIMER_AUDIO_EXPORT
#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_ALSA
#define MA_ENABLE_COREAUDIO
#define MA_ENABLE_OPENSL
#define MA_ENABLE_WEBAUDIO
#include "miniaudio.h"

static struct {
    bool initialized;
    ma_engine engine;
} state;

Bool32 Alimer_AudioInit(const AudioConfig* config) {
    if (state.initialized)
        return false;

    ma_engine_config ma_config = ma_engine_config_init();
    ma_config.listenerCount = config->listener_count;
    ma_config.channels = config->channels;
    ma_config.sampleRate = config->sample_rate;

    ma_result result = ma_engine_init(&ma_config, &state.engine);
    if (result != MA_SUCCESS) {
        return false;
    }

    state.initialized = true;
    return true;
}

void Alimer_AudioShutdown(void) {
    if (!state.initialized)
        return;

    ma_engine_uninit(&state.engine);
}

Bool32 Alimer_AudioStart(void) {
    return ma_engine_start(&state.engine) == MA_SUCCESS;
}

Bool32 Alimer_AudioStop(void) {
    return ma_engine_stop(&state.engine) == MA_SUCCESS;
}

uint32_t Alimer_AudioGetOutputChannels(void) {
    return ma_engine_get_channels(&state.engine);
}

uint32_t Alimer_AudioGetOutputSampleRate(void) {
    return ma_engine_get_sample_rate(&state.engine);
}

float Alimer_AudioGetMasterVolume(void) {
    if (!state.initialized) {
        return 0.0f;
    }

    return ma_engine_get_volume(&state.engine);
}

Bool32 Alimer_AudioSetMasterVolume(float volume) {
    return ma_engine_set_volume(&state.engine, volume) == MA_SUCCESS;
}

AlimerSound* Alimer_SoundCreate(const char* path, uint32_t flags)
{
    ma_sound* sound = malloc(sizeof(ma_sound));

    if (MA_SUCCESS != ma_sound_init_from_file(&state.engine, path, flags, NULL, NULL, sound))
    {
        //Alimer_LogError("Unable to create Sound from file");
        free(sound);
        return NULL;
    }

    return (AlimerSound*)sound;
}

void Alimer_SoundDestroy(AlimerSound* sound)
{
    ma_sound_uninit((ma_sound*)sound);
    free((ma_sound*)sound);
}

void Alimer_SoundPlay(AlimerSound* sound)
{
    ma_sound_start((ma_sound*)sound);
}

void Alimer_SoundStop(AlimerSound* sound)
{
    ma_sound_stop((ma_sound*)sound);
}

float Alimer_SoundGetVolume(AlimerSound* sound)
{
    return ma_sound_get_volume((ma_sound*)sound);
}

void Alimer_SoundSetVolume(AlimerSound* sound, float value)
{
    ma_sound_set_volume((ma_sound*)sound, value);
}

float Alimer_SoundGetPitch(AlimerSound* sound)
{
    return ma_sound_get_pitch((ma_sound*)sound);
}

void Alimer_SoundSetPitch(AlimerSound* sound, float value)
{
    ma_sound_set_pitch((ma_sound*)sound, value);
}

Bool32 Alimer_SoundIsPlaying(AlimerSound* sound)
{
    return ma_sound_is_playing((ma_sound*)sound);
}

Bool32 Alimer_SoundIsLooping(AlimerSound* sound)
{
    return ma_sound_is_looping((ma_sound*)sound);
}

void Alimer_SoundSetLooping(AlimerSound* sound, Bool32 value)
{
    ma_sound_set_looping((ma_sound*)sound, value);
}

Bool32 Alimer_SoundGetFinished(AlimerSound* sound)
{
    return ma_sound_at_end((ma_sound*)sound);
}

/* stb_vorbis implementation must come after the implementation of miniaudio. */
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

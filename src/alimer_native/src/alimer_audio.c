// Copyright (c) Amer Koleci and Contributors.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_internal.h"

ALIMER_DISABLE_WARNINGS()
#define STB_VORBIS_HEADER_ONLY
#include "third_party/stb_vorbis.c"

#define MINIAUDIO_IMPLEMENTATION
#define MA_ENABLE_ONLY_SPECIFIC_BACKENDS
#define MA_ENABLE_WASAPI
#define MA_ENABLE_ALSA
#define MA_ENABLE_COREAUDIO
#define MA_ENABLE_OPENSL
#define MA_ENABLE_WEBAUDIO
#include "third_party/miniaudio.h"
ALIMER_ENABLE_WARNINGS()

static struct {
    bool initialized;
    ma_engine engine;
} audio_state;

Bool32 Alimer_AudioInit(void/*const AudioConfig* config*/)
{
    ma_engine_config ma_config = ma_engine_config_init();

    ma_result result = ma_engine_init(&ma_config, &audio_state.engine);
    if (result != MA_SUCCESS)
    {
        //Alimer_LogError("Alimer: Failed to initialize audio engine!");
        return false;
    }

    audio_state.initialized = true;
    //Alimer_LogInfo("Audio Initialized");
    return true;
}

void Alimer_AudioShutdown()
{
    if (!audio_state.initialized)
        return;

    ma_engine_uninit(&audio_state.engine);
    memset(&audio_state, 0, sizeof(audio_state));
}

Bool32 Alimer_AudioStart(void)
{
    return ma_engine_start(&audio_state.engine) == MA_SUCCESS;
}

Bool32 Alimer_AudioStop(void)
{
    return ma_engine_stop(&audio_state.engine) == MA_SUCCESS;
}

float Alimer_AudioGetMasterVolume(void)
{
    return ma_engine_get_volume(&audio_state.engine);
}

Bool32 Alimer_AudioSetMasterVolume(float value)
{
    return ma_engine_set_volume(&audio_state.engine, value) == MA_SUCCESS;
}

uint32_t Alimer_AudioGetOutputChannels(void)
{
    return ma_engine_get_channels(&audio_state.engine);
}

uint32_t Alimer_AudioGetOutputSampleRate(void)
{
    return ma_engine_get_sample_rate(&audio_state.engine);
}

AlimerSound Alimer_SoundCreate(const char* path, uint32_t flags)
{
    ma_sound* sound = ALIMER_ALLOC(ma_sound);

    if (MA_SUCCESS != ma_sound_init_from_file(&audio_state.engine, path, flags, NULL, NULL, sound))
    {
        //Alimer_LogError("Unable to create Sound from file");
        ALIMER_FREE(sound);
        return NULL;
    }

    return (AlimerSound)sound;
}

void Alimer_SoundDestroy(AlimerSound sound)
{
    ma_sound_uninit((ma_sound*)sound);
    ALIMER_FREE((ma_sound*)sound);
}

void Alimer_SoundPlay(AlimerSound sound)
{
    ma_sound_start((ma_sound*)sound);
}

void Alimer_SoundStop(AlimerSound sound)
{
    ma_sound_stop((ma_sound*)sound);
}

float Alimer_SoundGetVolume(AlimerSound sound)
{
    return ma_sound_get_volume((ma_sound*)sound);
}

void Alimer_SoundSetVolume(AlimerSound sound, float value)
{
    ma_sound_set_volume((ma_sound*)sound, value);
}

float Alimer_SoundGetPitch(AlimerSound sound)
{
    return ma_sound_get_pitch((ma_sound*)sound);
}

void Alimer_SoundSetPitch(AlimerSound sound, float value)
{
    ma_sound_set_pitch((ma_sound*)sound, value);
}

Bool32 Alimer_SoundIsPlaying(AlimerSound sound)
{
    return ma_sound_is_playing((ma_sound*)sound);
}

Bool32 Alimer_SoundIsLooping(AlimerSound sound)
{
    return ma_sound_is_looping((ma_sound*)sound);
}

void Alimer_SoundSetLooping(AlimerSound sound, Bool32 value)
{
    ma_sound_set_looping((ma_sound*)sound, value);
}

Bool32 Alimer_SoundGetFinished(AlimerSound sound)
{
    return ma_sound_at_end((ma_sound*)sound);
}

#undef STB_VORBIS_HEADER_ONLY
#include "third_party/stb_vorbis.c"

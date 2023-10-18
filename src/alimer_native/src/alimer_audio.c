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

static_assert(sizeof(Alimer_Vector3) == sizeof(ma_vec3f), "Vector3 mismatch");
//static_assert(alignof(Alimer_Vector3) == alignof(ma_vec3f));
static_assert(offsetof(Alimer_Vector3, x) == offsetof(ma_vec3f, x), "Vector3 Layout mismatch");
static_assert(offsetof(Alimer_Vector3, y) == offsetof(ma_vec3f, y), "Vector3 Layout mismatch");
static_assert(offsetof(Alimer_Vector3, z) == offsetof(ma_vec3f, z), "Vector3 Layout mismatch");

// EPhysicsUpdateError
static_assert(sizeof(SoundFlags) == sizeof(ma_sound_flags), "SoundFlags mismatch");
static_assert((int)SoundFlags_Stream == (int)MA_SOUND_FLAG_STREAM, "SoundFlags Layout mismatch");
static_assert((int)SoundFlags_Decode == (int)MA_SOUND_FLAG_DECODE, "SoundFlags Layout mismatch");
static_assert((int)SoundFlags_Async == (int)MA_SOUND_FLAG_ASYNC, "SoundFlags Layout mismatch");
static_assert((int)SoundFlags_WaitInit == (int)MA_SOUND_FLAG_WAIT_INIT, "SoundFlags Layout mismatch");

static_assert((int)SoundFlags_NoDefaultAttachment == MA_SOUND_FLAG_NO_DEFAULT_ATTACHMENT, "SoundFlags Layout mismatch");
static_assert((int)SoundFlags_NoPitch == MA_SOUND_FLAG_NO_PITCH, "SoundFlags Layout mismatch");
static_assert((int)SoundFlags_NoSpatialization == MA_SOUND_FLAG_NO_SPATIALIZATION, "SoundFlags Layout mismatch");

static void FromVec3(const ma_vec3f vec, Alimer_Vector3* result)
{
    result->x = vec.x;
    result->y = vec.y;
    result->z = vec.z;
}

static struct {
    bool initialized;
    ma_engine engine;
} audio_state;

void Alimer_AudioGetVersion(int* major, int* minor, int* patch)
{
    if (major) *major = MA_VERSION_MAJOR;
    if (minor) *minor = MA_VERSION_MINOR;
    if (patch) *patch = MA_VERSION_REVISION;
}

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

float Alimer_SoundGetPan(AlimerSound sound)
{
    return ma_sound_get_pan((ma_sound*)sound);
}

void Alimer_SoundSetPan(AlimerSound sound, float value)
{
    ma_sound_set_pan((ma_sound*)sound, value);
}

Bool32 Alimer_SoundIsPlaying(AlimerSound sound)
{
    return ma_sound_is_playing((ma_sound*)sound);
}

Bool32 Alimer_SoundGetFinished(AlimerSound sound)
{
    return ma_sound_at_end((ma_sound*)sound);
}

Bool32 Alimer_SoundGetLengthPcmFrames(AlimerSound sound, uint64_t* result)
{
    return ma_sound_get_length_in_pcm_frames((ma_sound*)sound, result) == MA_SUCCESS;
}

Bool32 Alimer_SoundGetCursorPcmFrames(AlimerSound sound, uint64_t* result)
{
    return ma_sound_get_cursor_in_pcm_frames((ma_sound*)sound, result) == MA_SUCCESS;
}

Bool32 Alimer_SoundSetCursorPcmFrames(AlimerSound sound, uint64_t value)
{
    return ma_sound_seek_to_pcm_frame((ma_sound*)sound, value) == MA_SUCCESS;
}

Bool32 Alimer_SoundIsLooping(AlimerSound sound)
{
    return ma_sound_is_looping((ma_sound*)sound);
}

void Alimer_SoundSetLooping(AlimerSound sound, Bool32 value)
{
    ma_sound_set_looping((ma_sound*)sound, value);
}

void Alimer_SoundGetLoopPcmFrames(AlimerSound sound, uint64_t* pLoopBegInFrames, uint64_t* pLoopEndInFrames)
{
    ma_data_source* source = ma_sound_get_data_source((ma_sound*)sound);
    ma_data_source_get_loop_point_in_pcm_frames(source, pLoopBegInFrames, pLoopEndInFrames);
}

Bool32 Alimer_SoundSetLoopPcmFrames(AlimerSound sound, uint64_t loopBegInFrames, uint64_t loopEndInFrames)
{
    ma_data_source* source = ma_sound_get_data_source((ma_sound*)sound);
    return ma_data_source_set_loop_point_in_pcm_frames(source, loopBegInFrames, loopEndInFrames) == MA_SUCCESS;
}

Bool32 Alimer_SoundIsSpatialized(AlimerSound sound)
{
    return ma_sound_group_is_spatialization_enabled((ma_sound*)sound);
}

void Alimer_SoundSetSpatialized(AlimerSound sound, Bool32 value)
{
    ma_sound_group_set_spatialization_enabled((ma_sound*)sound, value);
}

void Alimer_SoundGetPosition(AlimerSound sound, Alimer_Vector3* result)
{
    FromVec3(ma_sound_get_position((ma_sound*)sound), result);
}

void Alimer_SoundSetPosition(AlimerSound sound, Alimer_Vector3* value)
{
    ma_sound_set_position((ma_sound*)sound, value->x, value->y, value->z);
}

void Alimer_SoundGetVelocity(AlimerSound sound, Alimer_Vector3* result)
{
    FromVec3(ma_sound_get_velocity((ma_sound*)sound), result);
}

void Alimer_SoundSetVelocity(AlimerSound sound, Alimer_Vector3* value)
{
    ma_sound_set_velocity((ma_sound*)sound, value->x, value->y, value->z);
}

void Alimer_SoundGetDirection(AlimerSound sound, Alimer_Vector3* result)
{
    FromVec3(ma_sound_get_direction((ma_sound*)sound), result);
}

void Alimer_SoundSetDirection(AlimerSound sound, Alimer_Vector3* value)
{
    ma_sound_set_direction((ma_sound*)sound, value->x, value->y, value->z);
}

#undef STB_VORBIS_HEADER_ONLY
#include "third_party/stb_vorbis.c"

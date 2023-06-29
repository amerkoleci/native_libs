// Copyright © Amer Koleci.
// Licensed under the MIT License (MIT). See LICENSE in the repository root for more information.

#include "alimer_audio.h"
#include <stdlib.h> // malloc, free
#include <string.h> // memset

#ifndef _AUDIO_ASSERT
#   include <assert.h>
#   define _AUDIO_ASSERT(c) assert(c)
#endif

#define _UNUSED(x) (void)(x)

#define STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"    /* Enables Vorbis decoding. */

#define MA_API _ALIMER_AUDIO_EXPORT
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

/* stb_vorbis implementation must come after the implementation of miniaudio. */
#undef STB_VORBIS_HEADER_ONLY
#include "extras/stb_vorbis.c"

static struct {
    bool initialized;
    ma_engine engine;
} state;

bool audio_init(const audio_config* config) {
    if (state.initialized)
        return false;

    ma_engine_config ma_config = ma_engine_config_init();
    ma_config.listenerCount = config->listener_count;
    ma_config.channels = config->channels;
    ma_config.sampleRate = config->sample_rate;

    ma_result result = ma_engine_init(&ma_config, &state.engine);
    if (result != MA_SUCCESS) {
        return NULL;
    }

    state.initialized = true;
    return true;
}

void audio_shutdown(void) {
    if (!state.initialized)
        return;

    ma_engine_uninit(&state.engine);
}

uint32_t audio_get_channels(void) {
    return ma_engine_get_channels(&state.engine);
}

uint32_t audio_get_sample_rate(void) {
    return ma_engine_get_sample_rate(&state.engine);
}

bool audio_start(void) {
    return ma_engine_start(&state.engine) == MA_SUCCESS;
}

bool audio_stop(void) {
    return ma_engine_stop(&state.engine) == MA_SUCCESS;
}

float audio_get_volume(void) {
    if (!state.initialized) {
        return 0.0f;
    }

    return ma_node_get_output_bus_volume(ma_node_graph_get_endpoint(&state.engine.nodeGraph), 0);
}

bool audio_set_volume(float volume) {
    return ma_engine_set_volume(&state.engine, volume) == MA_SUCCESS;
}


/* WIP */
bool audio_play_sound(const char* file_name) {
    return ma_engine_play_sound(&state.engine, file_name, NULL) == MA_SUCCESS;
}

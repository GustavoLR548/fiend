/*
 * audio.c - miniaudio wrapper implementation for Fiend
 * 
 * This implements the audio API using miniaudio for cross-platform support.
 */

#define MINIAUDIO_IMPLEMENTATION
#include "../vendor/miniaudio/miniaudio.h"
#include "audio.h"
#include <stdio.h>
#include <stdlib.h>

/* Global audio engine */
static ma_engine g_audio_engine;
static int g_audio_initialized = 0;

/* Sound structure wrapping ma_sound */
typedef struct {
    ma_sound sound;
    int is_valid;
} audio_sound_t;

/* Initialize audio system */
int audio_init(void) {
    ma_result result;
    ma_engine_config engineConfig;
    
    if (g_audio_initialized) {
        return 0; /* Already initialized */
    }
    
    /* Configure engine for better debugging */
    engineConfig = ma_engine_config_init();
    engineConfig.noAutoStart = MA_FALSE;  /* Auto-start playback */
    
    result = ma_engine_init(&engineConfig, &g_audio_engine);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize miniaudio engine: %d\n", result);
        return -1;
    }
    
    g_audio_initialized = 1;
    printf("miniaudio engine initialized successfully\n");
    
    /* Print device info for debugging */
    ma_device* pDevice = ma_engine_get_device(&g_audio_engine);
    if (pDevice) {
        printf("Audio device: %s\n", pDevice->playback.name);
        printf("Sample rate: %d Hz\n", pDevice->sampleRate);
        printf("Channels: %d\n", pDevice->playback.channels);
    }
    
    return 0;
}

/* Shutdown audio system */
void audio_shutdown(void) {
    if (!g_audio_initialized) {
        return;
    }
    
    ma_engine_uninit(&g_audio_engine);
    g_audio_initialized = 0;
}

/* Load a sound from file */
int audio_load_sound(const char* filename, void** sound_ptr) {
    audio_sound_t* snd;
    ma_result result;
    
    if (!g_audio_initialized) {
        fprintf(stderr, "Audio system not initialized\n");
        return -1;
    }
    
    snd = (audio_sound_t*)malloc(sizeof(audio_sound_t));
    if (!snd) {
        fprintf(stderr, "Failed to allocate memory for sound\n");
        return -1;
    }
    
    /* Load sound with flags for seeking (needed for looping) */
    result = ma_sound_init_from_file(&g_audio_engine, filename, 
                                      MA_SOUND_FLAG_DECODE | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                      NULL, NULL, &snd->sound);
    
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Failed to load sound '%s': %d\n", filename, result);
        free(snd);
        return -1;
    }
    
    snd->is_valid = 1;
    *sound_ptr = snd;
    printf("[AUDIO] Loaded sound: %s\n", filename);
    return 0;
}

/* Free a loaded sound */
void audio_free_sound(void* sound_ptr) {
    audio_sound_t* snd = (audio_sound_t*)sound_ptr;
    
    if (!snd || !snd->is_valid) {
        return;
    }
    
    ma_sound_uninit(&snd->sound);
    snd->is_valid = 0;
    free(snd);
}

/* Play a sound */
void* audio_play_sound(void* sound_ptr, int loop, int volume, int pan) {
    audio_sound_t* snd = (audio_sound_t*)sound_ptr;
    ma_result result;
    
    if (!snd || !snd->is_valid) {
        fprintf(stderr, "Invalid sound pointer\n");
        return NULL;
    }
    
    /* Stop sound if already playing, then restart */
    ma_sound_stop(&snd->sound);
    ma_sound_seek_to_pcm_frame(&snd->sound, 0);
    
    /* Set looping */
    ma_sound_set_looping(&snd->sound, loop ? MA_TRUE : MA_FALSE);
    
    /* Set volume (convert from 0-255 to 0.0-1.0) */
    float vol_normalized = (float)volume / 255.0f;
    ma_sound_set_volume(&snd->sound, vol_normalized);
    
    /* Set pan (convert from 0-255 to -1.0 to +1.0, where 128 = center = 0.0) */
    float pan_normalized = ((float)pan - 128.0f) / 128.0f;
    ma_sound_set_pan(&snd->sound, pan_normalized);
    
    /* Start playback */
    result = ma_sound_start(&snd->sound);
    if (result != MA_SUCCESS) {
        fprintf(stderr, "[AUDIO ERROR] Failed to start sound playback: %d\n", result);
        return NULL;
    }
    
    printf("[AUDIO] Playing sound: vol=%.2f, pan=%.2f, loop=%d\n", vol_normalized, pan_normalized, loop);
    
    /* Return the sound pointer itself as the "channel" */
    return sound_ptr;
}

/* Stop a playing channel */
void audio_stop_channel(void* channel_ptr) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        return;
    }
    
    ma_sound_stop(&snd->sound);
}

/* Check if channel is playing */
int audio_is_playing(void* channel_ptr) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        return 0;
    }
    
    return ma_sound_is_playing(&snd->sound) ? 1 : 0;
}

/* Set channel volume */
void audio_set_volume(void* channel_ptr, int volume) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        printf("[AUDIO] audio_set_volume: Invalid channel pointer\n");
        return;
    }
    
    printf("[AUDIO] audio_set_volume: channel=%p, volume=%d (%.2f)\n", channel_ptr, volume, (float)volume / 255.0f);
    ma_sound_set_volume(&snd->sound, (float)volume / 255.0f);
}

/* Set channel pan */
void audio_set_pan(void* channel_ptr, int pan) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        return;
    }
    
    float pan_normalized = ((float)pan - 128.0f) / 128.0f;
    ma_sound_set_pan(&snd->sound, pan_normalized);
}

/* Pause channel */
void audio_pause_channel(void* channel_ptr) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        return;
    }
    
    ma_sound_stop(&snd->sound);
}

/* Resume channel */
void audio_resume_channel(void* channel_ptr) {
    audio_sound_t* snd = (audio_sound_t*)channel_ptr;
    
    if (!snd || !snd->is_valid) {
        return;
    }
    
    ma_sound_start(&snd->sound);
}

/* Music functions - use same implementation as sounds, but with streaming flag */
int audio_load_music(const char* filename, void** music_ptr) {
    audio_sound_t* snd;
    ma_result result;
    
    if (!g_audio_initialized) {
        fprintf(stderr, "Audio system not initialized\n");
        return -1;
    }
    
    snd = (audio_sound_t*)malloc(sizeof(audio_sound_t));
    if (!snd) {
        fprintf(stderr, "Failed to allocate memory for music\n");
        return -1;
    }
    
    /* Load music with streaming flag for memory efficiency */
    result = ma_sound_init_from_file(&g_audio_engine, filename,
                                      MA_SOUND_FLAG_STREAM | MA_SOUND_FLAG_NO_SPATIALIZATION,
                                      NULL, NULL, &snd->sound);
    
    if (result != MA_SUCCESS) {
        fprintf(stderr, "Failed to load music '%s': %d\n", filename, result);
        free(snd);
        return -1;
    }
    
    snd->is_valid = 1;
    *music_ptr = snd;
    return 0;
}

void audio_free_music(void* music_ptr) {
    audio_free_sound(music_ptr);
}

void* audio_play_music(void* music_ptr, int loop, int volume) {
    return audio_play_sound(music_ptr, loop, volume, 128); /* Center pan */
}

void audio_stop_music(void* channel_ptr) {
    audio_stop_channel(channel_ptr);
}

void audio_pause_music(void* channel_ptr) {
    audio_pause_channel(channel_ptr);
}

void audio_resume_music(void* channel_ptr) {
    audio_resume_channel(channel_ptr);
}

int audio_is_music_paused(void* channel_ptr) {
    return !audio_is_playing(channel_ptr);
}

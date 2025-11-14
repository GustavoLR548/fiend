/*
 * audio.h - miniaudio wrapper for Fiend
 * 
 * This provides a simple audio API that replaces FMOD functionality.
 * Uses miniaudio library for cross-platform audio playback.
 */

#ifndef AUDIO_H
#define AUDIO_H

/* Initialize audio system (replaces FMOD_System_Create) */
int audio_init(void);

/* Shutdown audio system (replaces FMOD_System_Close) */
void audio_shutdown(void);

/* Load a sound from file (replaces FMOD_System_CreateSound)
 * Returns 0 on success, -1 on failure
 * sound_ptr: pointer to store the opaque sound handle
 */
int audio_load_sound(const char* filename, void** sound_ptr);

/* Free a loaded sound (replaces FMOD_Sound_Release) */
void audio_free_sound(void* sound_ptr);

/* Play a sound (replaces FMOD_System_PlaySound)
 * Returns channel pointer on success, NULL on failure
 * sound_ptr: sound to play
 * loop: 0 = no loop, 1 = loop
 * volume: 0-255 (matches FMOD volume scale)
 * pan: 0-255, 128 = center (matches FMOD pan scale)
 */
void* audio_play_sound(void* sound_ptr, int loop, int volume, int pan);

/* Stop a playing channel (replaces FMOD_Channel_Stop) */
void audio_stop_channel(void* channel_ptr);

/* Check if channel is playing (replaces FMOD_Channel_IsPlaying) */
int audio_is_playing(void* channel_ptr);

/* Set channel volume (replaces FMOD_Channel_SetVolume)
 * volume: 0-255 (matches FMOD scale, converted internally)
 */
void audio_set_volume(void* channel_ptr, int volume);

/* Set channel pan (replaces FMOD_Channel_SetPan)
 * pan: 0-255, 128 = center (matches FMOD scale)
 */
void audio_set_pan(void* channel_ptr, int pan);

/* Pause/resume channel (replaces FMOD_Channel_SetPaused) */
void audio_pause_channel(void* channel_ptr);
void audio_resume_channel(void* channel_ptr);

/* Music streaming functions */
int audio_load_music(const char* filename, void** music_ptr);
void audio_free_music(void* music_ptr);
void* audio_play_music(void* music_ptr, int loop, int volume);
void audio_stop_music(void* channel_ptr);
void audio_pause_music(void* channel_ptr);
void audio_resume_music(void* channel_ptr);
int audio_is_music_paused(void* channel_ptr);

#endif /* AUDIO_H */

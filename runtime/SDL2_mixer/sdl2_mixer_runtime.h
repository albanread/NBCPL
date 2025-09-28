#ifndef SDL2_MIXER_RUNTIME_H
#define SDL2_MIXER_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Forward declarations for BCPL runtime integration
typedef struct BCPLString BCPLString;
typedef uint32_t* bcpl_string_t;

// SDL_mixer Runtime Module - C-linkage interface for BCPL
// These functions provide a simplified SDL_mixer interface that can be called from BCPL code

// =============================================================================
// AUDIO SYSTEM INITIALIZATION AND CLEANUP
// =============================================================================

/**
 * Initialize SDL_mixer audio system
 * BCPL Usage: result = SDL2_MIXER_OPEN_AUDIO(frequency, format, channels, chunksize)
 * @param frequency: Audio frequency (e.g., 44100)
 * @param format: Audio format (use MIX_DEFAULT_FORMAT = -32784)
 * @param channels: Number of audio channels (1=mono, 2=stereo)
 * @param chunksize: Audio buffer size (e.g., 2048)
 * Returns: 0 on success, negative on failure
 */
int64_t SDL2_MIXER_OPEN_AUDIO(int64_t frequency, int64_t format, int64_t channels, int64_t chunksize);

/**
 * Close SDL_mixer audio system
 * BCPL Usage: SDL2_MIXER_CLOSE_AUDIO()
 */
void SDL2_MIXER_CLOSE_AUDIO(void);

/**
 * Quit SDL_mixer and free all resources
 * BCPL Usage: SDL2_MIXER_QUIT()
 */
void SDL2_MIXER_QUIT(void);

// =============================================================================
// VOLUME CONTROL
// =============================================================================

/**
 * Set volume for a specific channel
 * BCPL Usage: previous_volume = SDL2_MIXER_VOLUME(channel, volume)
 * @param channel: Channel number (-1 for all channels)
 * @param volume: Volume level (0-128, -1 to query current volume)
 * Returns: Previous volume level
 */
int64_t SDL2_MIXER_VOLUME(int64_t channel, int64_t volume);

/**
 * Set music volume
 * BCPL Usage: previous_volume = SDL2_MIXER_VOLUME_MUSIC(volume)
 * @param volume: Volume level (0-128, -1 to query current volume)
 * Returns: Previous volume level
 */
int64_t SDL2_MIXER_VOLUME_MUSIC(int64_t volume);

// =============================================================================
// SOUND EFFECTS (CHUNKS)
// =============================================================================

/**
 * Load a WAV file as a sound chunk
 * BCPL Usage: chunk_id = SDL2_MIXER_LOAD_WAV(filename)
 * @param filename: BCPL string with path to WAV file
 * Returns: Chunk ID (0 on failure)
 */
int64_t SDL2_MIXER_LOAD_WAV(bcpl_string_t filename);

/**
 * Play a sound chunk on a channel
 * BCPL Usage: channel = SDL2_MIXER_PLAY_CHANNEL(channel, chunk_id, loops)
 * @param channel: Channel to play on (-1 for first available)
 * @param chunk_id: ID of loaded chunk
 * @param loops: Number of loops (0=play once, -1=infinite)
 * Returns: Channel number used, or -1 on error
 */
int64_t SDL2_MIXER_PLAY_CHANNEL(int64_t channel, int64_t chunk_id, int64_t loops);

/**
 * Play a sound chunk with time limit
 * BCPL Usage: channel = SDL2_MIXER_PLAY_CHANNEL_TIMED(channel, chunk_id, loops, ticks)
 * @param channel: Channel to play on (-1 for first available)
 * @param chunk_id: ID of loaded chunk
 * @param loops: Number of loops (0=play once, -1=infinite)
 * @param ticks: Maximum time to play in milliseconds (-1 for no limit)
 * Returns: Channel number used, or -1 on error
 */
int64_t SDL2_MIXER_PLAY_CHANNEL_TIMED(int64_t channel, int64_t chunk_id, int64_t loops, int64_t ticks);

/**
 * Pause a channel
 * BCPL Usage: SDL2_MIXER_PAUSE(channel)
 * @param channel: Channel to pause (-1 for all channels)
 */
void SDL2_MIXER_PAUSE(int64_t channel);

/**
 * Resume a paused channel
 * BCPL Usage: SDL2_MIXER_RESUME(channel)
 * @param channel: Channel to resume (-1 for all channels)
 */
void SDL2_MIXER_RESUME(int64_t channel);

/**
 * Halt (stop) a channel
 * BCPL Usage: SDL2_MIXER_HALT_CHANNEL(channel)
 * @param channel: Channel to halt (-1 for all channels)
 */
void SDL2_MIXER_HALT_CHANNEL(int64_t channel);

/**
 * Free a loaded sound chunk
 * BCPL Usage: SDL2_MIXER_FREE_CHUNK(chunk_id)
 * @param chunk_id: ID of chunk to free
 */
void SDL2_MIXER_FREE_CHUNK(int64_t chunk_id);

// =============================================================================
// MUSIC (LONGER AUDIO FILES)
// =============================================================================

/**
 * Load a music file (supports various formats: MP3, OGG, WAV, etc.)
 * BCPL Usage: music_id = SDL2_MIXER_LOAD_MUS(filename)
 * @param filename: BCPL string with path to music file
 * Returns: Music ID (0 on failure)
 */
int64_t SDL2_MIXER_LOAD_MUS(bcpl_string_t filename);

/**
 * Play background music
 * BCPL Usage: result = SDL2_MIXER_PLAY_MUSIC(music_id, loops)
 * @param music_id: ID of loaded music
 * @param loops: Number of loops (0=play once, -1=infinite)
 * Returns: 0 on success, -1 on error
 */
int64_t SDL2_MIXER_PLAY_MUSIC(int64_t music_id, int64_t loops);

/**
 * Fade in music over time
 * BCPL Usage: result = SDL2_MIXER_FADE_IN_MUSIC(music_id, loops, ms)
 * @param music_id: ID of loaded music
 * @param loops: Number of loops (0=play once, -1=infinite)
 * @param ms: Fade in time in milliseconds
 * Returns: 0 on success, -1 on error
 */
int64_t SDL2_MIXER_FADE_IN_MUSIC(int64_t music_id, int64_t loops, int64_t ms);

/**
 * Pause music playback
 * BCPL Usage: SDL2_MIXER_PAUSE_MUSIC()
 */
void SDL2_MIXER_PAUSE_MUSIC(void);

/**
 * Resume paused music
 * BCPL Usage: SDL2_MIXER_RESUME_MUSIC()
 */
void SDL2_MIXER_RESUME_MUSIC(void);

/**
 * Halt (stop) music playback
 * BCPL Usage: SDL2_MIXER_HALT_MUSIC()
 */
void SDL2_MIXER_HALT_MUSIC(void);

/**
 * Fade out music over time
 * BCPL Usage: result = SDL2_MIXER_FADE_OUT_MUSIC(ms)
 * @param ms: Fade out time in milliseconds
 * Returns: 0 on success, -1 on error
 */
int64_t SDL2_MIXER_FADE_OUT_MUSIC(int64_t ms);

/**
 * Free a loaded music file
 * BCPL Usage: SDL2_MIXER_FREE_MUSIC(music_id)
 * @param music_id: ID of music to free
 */
void SDL2_MIXER_FREE_MUSIC(int64_t music_id);

// =============================================================================
// AUDIO STATUS AND QUERY FUNCTIONS
// =============================================================================

/**
 * Check if a channel is playing
 * BCPL Usage: is_playing = SDL2_MIXER_PLAYING(channel)
 * @param channel: Channel to check (-1 to check all channels)
 * Returns: 1 if playing, 0 if not
 */
int64_t SDL2_MIXER_PLAYING(int64_t channel);

/**
 * Check if a channel is paused
 * BCPL Usage: is_paused = SDL2_MIXER_PAUSED(channel)
 * @param channel: Channel to check (-1 to check all channels)
 * Returns: 1 if paused, 0 if not
 */
int64_t SDL2_MIXER_PAUSED(int64_t channel);

/**
 * Check if music is playing
 * BCPL Usage: is_playing = SDL2_MIXER_PLAYING_MUSIC()
 * Returns: 1 if playing, 0 if not
 */
int64_t SDL2_MIXER_PLAYING_MUSIC(void);

/**
 * Check if music is paused
 * BCPL Usage: is_paused = SDL2_MIXER_PAUSED_MUSIC()
 * Returns: 1 if paused, 0 if not
 */
int64_t SDL2_MIXER_PAUSED_MUSIC(void);

/**
 * Get the number of available mixing channels
 * BCPL Usage: num_channels = SDL2_MIXER_ALLOCATE_CHANNELS(numchans)
 * @param numchans: Number of channels to allocate (-1 to query current)
 * Returns: Number of channels allocated
 */
int64_t SDL2_MIXER_ALLOCATE_CHANNELS(int64_t numchans);

// =============================================================================
// ERROR HANDLING
// =============================================================================

/**
 * Get the last SDL_mixer error message
 * BCPL Usage: error_msg = SDL2_MIXER_GET_ERROR()
 * Returns: BCPL string with error message (empty if no error)
 */
bcpl_string_t SDL2_MIXER_GET_ERROR(void);

// =============================================================================
// SDL_MIXER CONSTANTS (for BCPL usage)
// =============================================================================

// Audio formats
#define MIX_DEFAULT_FREQUENCY   44100
#define MIX_DEFAULT_FORMAT      -32784  // AUDIO_S16LSB
#define MIX_DEFAULT_CHANNELS    2
#define MIX_MAX_VOLUME          128

// Special values
#define MIX_CHANNEL_ANY         -1
#define MIX_LOOPS_INFINITE      -1
#define MIX_NO_TIME_LIMIT       -1

// Chunk size recommendations
#define MIX_SMALL_CHUNK_SIZE    1024   // Low latency
#define MIX_MEDIUM_CHUNK_SIZE   2048   // Balanced
#define MIX_LARGE_CHUNK_SIZE    4096   // High performance

#ifdef __cplusplus
}
#endif

#endif // SDL2_MIXER_RUNTIME_H
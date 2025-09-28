# NewBCPL SDL2_mixer Runtime Module

This document provides an overview and reference for the SDL2_mixer runtime module in NewBCPL. It covers the purpose, available commands, usage patterns, integration details, and best practices for using audio features from BCPL code.

---

## Overview

The SDL2_mixer runtime module enables BCPL programs to play sound effects and music using the SDL2_mixer C library. All audio functionality is accessed through the runtime, ensuring portability and a consistent API.

- **Access**: All SDL2_mixer features are available via routines prefixed with `SDL2_MIXER_`.
- **Integration**: The module is conditionally included at build time (`--with-sdl2-mixer` or as part of static SDL2 builds).
- **Design**: The interface is designed for simplicity, using integer-based handles for audio resources and channels.

---

## Building and Enabling SDL2_mixer Support

To use SDL2_mixer features, build the runtime with SDL2_mixer support:

```sh
# Dynamic linking
./buildruntime --with-sdl2 --with-sdl2-mixer

# Static linking (recommended for deployment)
./buildruntime --with-sdl2-static --with-sdl2-mixer
```

---

## Audio System Initialization and Cleanup

### `SDL2_MIXER_OPEN_AUDIO(frequency, format, channels, chunksize)`
- Initializes the audio system.
- `frequency`: Audio frequency (e.g., 44100)
- `format`: Audio format (`MIX_DEFAULT_FORMAT = -32784`)
- `channels`: Number of channels (1=mono, 2=stereo)
- `chunksize`: Buffer size (e.g., 2048)
- Returns `0` on success, negative on failure.

### `SDL2_MIXER_CLOSE_AUDIO()`
- Closes the audio system.

### `SDL2_MIXER_QUIT()`
- Shuts down SDL2_mixer and frees all resources.

---

## Volume Control

### `SDL2_MIXER_VOLUME(channel, volume)`
- Sets volume for a specific channel (`-1` for all).
- `volume`: 0-128, `-1` to query current volume.
- Returns previous volume.

### `SDL2_MIXER_VOLUME_MUSIC(volume)`
- Sets music volume.
- `volume`: 0-128, `-1` to query current volume.
- Returns previous volume.

---

## Sound Effects (Chunks)

### `SDL2_MIXER_LOAD_WAV(filename)`
- Loads a WAV file as a sound chunk.
- Returns chunk ID (`0` on failure).

### `SDL2_MIXER_PLAY_CHANNEL(channel, chunk_id, loops)`
- Plays a sound chunk on a channel.
- `channel`: Channel number (`-1` for first available).
- `loops`: Number of loops (`0`=once, `-1`=infinite).
- Returns channel used, or `-1` on error.

### `SDL2_MIXER_PLAY_CHANNEL_TIMED(channel, chunk_id, loops, ticks)`
- Plays a chunk for a limited time.
- `ticks`: Max time in ms (`-1` for no limit).

### `SDL2_MIXER_PAUSE(channel)`
- Pauses a channel (`-1` for all).

### `SDL2_MIXER_RESUME(channel)`
- Resumes a paused channel (`-1` for all).

### `SDL2_MIXER_HALT_CHANNEL(channel)`
- Stops a channel (`-1` for all).

### `SDL2_MIXER_FREE_CHUNK(chunk_id)`
- Frees a loaded sound chunk.

---

## Music (Longer Audio Files)

### `SDL2_MIXER_LOAD_MUS(filename)`
- Loads a music file (MP3, OGG, WAV, etc.).
- Returns music ID (`0` on failure).

### `SDL2_MIXER_PLAY_MUSIC(music_id, loops)`
- Plays background music.
- `loops`: Number of loops (`0`=once, `-1`=infinite).

### `SDL2_MIXER_FADE_IN_MUSIC(music_id, loops, ms)`
- Fades in music over `ms` milliseconds.

### `SDL2_MIXER_PAUSE_MUSIC()`
- Pauses music playback.

### `SDL2_MIXER_RESUME_MUSIC()`
- Resumes paused music.

### `SDL2_MIXER_HALT_MUSIC()`
- Stops music playback.

### `SDL2_MIXER_FADE_OUT_MUSIC(ms)`
- Fades out music over `ms` milliseconds.

### `SDL2_MIXER_FREE_MUSIC(music_id)`
- Frees a loaded music file.

---

## Audio Status and Query Functions

### `SDL2_MIXER_PLAYING(channel)`
- Returns `1` if the channel is playing, `0` if not.

### `SDL2_MIXER_PAUSED(channel)`
- Returns `1` if the channel is paused, `0` if not.

### `SDL2_MIXER_PLAYING_MUSIC()`
- Returns `1` if music is playing, `0` if not.

### `SDL2_MIXER_PAUSED_MUSIC()`
- Returns `1` if music is paused, `0` if not.

### `SDL2_MIXER_ALLOCATE_CHANNELS(numchans)`
- Allocates mixing channels (`-1` to query current).
- Returns number of channels allocated.

---

## Error Handling

### `SDL2_MIXER_GET_ERROR()`
- Returns the last SDL2_mixer error message as a BCPL string.

---

## Constants Reference

- `MIX_DEFAULT_FREQUENCY = 44100`
- `MIX_DEFAULT_FORMAT = -32784` (AUDIO_S16LSB)
- `MIX_DEFAULT_CHANNELS = 2`
- `MIX_MAX_VOLUME = 128`
- `MIX_CHANNEL_ANY = -1`
- `MIX_LOOPS_INFINITE = -1`
- `MIX_NO_TIME_LIMIT = -1`
- `MIX_SMALL_CHUNK_SIZE = 1024`
- `MIX_MEDIUM_CHUNK_SIZE = 2048`
- `MIX_LARGE_CHUNK_SIZE = 4096`

---

## Example Usage

```bcpl
LET result = SDL2_MIXER_OPEN_AUDIO(44100, -32784, 2, 2048)
IF result < 0 THEN
$(
  LET err = SDL2_MIXER_GET_ERROR()
  WRITEF("Audio init failed: %s*N", err)
  FINISH()
$)

LET chunk_id = SDL2_MIXER_LOAD_WAV("beep.wav")
IF chunk_id = 0 THEN
$(
  WRITES("Failed to load sound*N")
$)

LET channel = SDL2_MIXER_PLAY_CHANNEL(-1, chunk_id, 0)
SDL2_MIXER_VOLUME(channel, 64)  // Set volume to 50%

// ... wait or poll ...

SDL2_MIXER_HALT_CHANNEL(channel)
SDL2_MIXER_FREE_CHUNK(chunk_id)
SDL2_MIXER_CLOSE_AUDIO()
SDL2_MIXER_QUIT()
```

---

## Integration Details

- **Runtime Registration**: All SDL2_mixer functions are registered with the runtime at initialization. BCPL code can call them as ordinary routines.
- **Handles**: Sound chunks and music are referenced by integer IDs, not pointers.
- **Strings**: All string parameters and results use BCPL string conventions.
- **Portability**: The runtime abstracts away platform-specific SDL2_mixer details.

---

## Best Practices

- Always check return values for initialization and loading functions.
- Use `SDL2_MIXER_GET_ERROR()` to retrieve error messages.
- Clean up resources in reverse order of creation.
- Call `SDL2_MIXER_QUIT()` before program termination to ensure all resources are freed.
- Use static linking for deployment to avoid runtime library issues.

---

## Advanced Notes

- The SDL2_mixer module is designed to be extensible. Additional features (e.g., effects, channel grouping) can be added as new routines.
- The runtime manages all SDL2_mixer resources and ensures safe cleanup.
- For best performance, use appropriate chunk sizes and minimize the number of simultaneous channels.

---

## Further Reading

- See the runtime source code in `runtime/SDL2_mixer/` for implementation details.
- For information on extending the SDL2_mixer module or integrating with other runtime features, see the main runtime documentation.

---
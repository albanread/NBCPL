#include "sdl2_mixer_runtime.h"
#include <SDL_mixer.h>
#include <iostream>
#include <unordered_map>
#include <memory>
#include <cstring>

// SDL_mixer Runtime Implementation
// This file provides the C-linkage implementations for SDL_mixer functions
// that can be called from BCPL code.

namespace {
    // Global storage for SDL_mixer resources
    // We use a simple ID-based system to manage chunks and music
    std::unordered_map<int64_t, Mix_Chunk*> loaded_chunks;
    std::unordered_map<int64_t, Mix_Music*> loaded_music;
    int64_t next_chunk_id = 1;
    int64_t next_music_id = 1;
    
    // Helper function to convert BCPL string to C string
    std::string bcpl_to_cstring(bcpl_string_t bcpl_str) {
        if (!bcpl_str) return "";
        
        // BCPL strings have length in first word, followed by packed characters
        uint32_t length = bcpl_str[0];
        if (length == 0) return "";
        
        std::string result;
        result.reserve(length);
        
        // Extract characters from packed format
        for (uint32_t i = 0; i < length; ++i) {
            uint32_t word_index = (i / 4) + 1;
            uint32_t byte_index = i % 4;
            uint8_t ch = (bcpl_str[word_index] >> (24 - byte_index * 8)) & 0xFF;
            result.push_back(static_cast<char>(ch));
        }
        
        return result;
    }
    
    // Helper function to create BCPL string from C string
    bcpl_string_t cstring_to_bcpl(const char* c_str) {
        if (!c_str) return nullptr;
        
        size_t length = strlen(c_str);
        if (length == 0) {
            // Return empty BCPL string
            static uint32_t empty_string[1] = {0};
            return empty_string;
        }
        
        // Allocate memory for BCPL string (length word + packed characters)
        size_t words_needed = 1 + (length + 3) / 4;
        uint32_t* bcpl_str = new uint32_t[words_needed];
        
        // Set length
        bcpl_str[0] = static_cast<uint32_t>(length);
        
        // Pack characters
        for (size_t i = 0; i < length; ++i) {
            uint32_t word_index = (i / 4) + 1;
            uint32_t byte_index = i % 4;
            if (byte_index == 0) bcpl_str[word_index] = 0; // Clear word
            bcpl_str[word_index] |= (static_cast<uint32_t>(c_str[i]) << (24 - byte_index * 8));
        }
        
        return bcpl_str;
    }
}

// =============================================================================
// AUDIO SYSTEM INITIALIZATION AND CLEANUP
// =============================================================================

extern "C" int64_t SDL2_MIXER_OPEN_AUDIO(int64_t frequency, int64_t format, int64_t channels, int64_t chunksize) {
    int result = Mix_OpenAudio(
        static_cast<int>(frequency),
        static_cast<Uint16>(format),
        static_cast<int>(channels),
        static_cast<int>(chunksize)
    );
    return static_cast<int64_t>(result);
}

extern "C" void SDL2_MIXER_CLOSE_AUDIO(void) {
    Mix_CloseAudio();
}

extern "C" void SDL2_MIXER_QUIT(void) {
    // Free all loaded resources
    for (auto& pair : loaded_chunks) {
        if (pair.second) {
            Mix_FreeChunk(pair.second);
        }
    }
    loaded_chunks.clear();
    
    for (auto& pair : loaded_music) {
        if (pair.second) {
            Mix_FreeMusic(pair.second);
        }
    }
    loaded_music.clear();
    
    Mix_Quit();
}

// =============================================================================
// VOLUME CONTROL
// =============================================================================

extern "C" int64_t SDL2_MIXER_VOLUME(int64_t channel, int64_t volume) {
    int previous = Mix_Volume(static_cast<int>(channel), static_cast<int>(volume));
    return static_cast<int64_t>(previous);
}

extern "C" int64_t SDL2_MIXER_VOLUME_MUSIC(int64_t volume) {
    int previous = Mix_VolumeMusic(static_cast<int>(volume));
    return static_cast<int64_t>(previous);
}

// =============================================================================
// SOUND EFFECTS (CHUNKS)
// =============================================================================

extern "C" int64_t SDL2_MIXER_LOAD_WAV(bcpl_string_t filename) {
    std::string c_filename = bcpl_to_cstring(filename);
    if (c_filename.empty()) {
        return 0; // Invalid filename
    }
    
    Mix_Chunk* chunk = Mix_LoadWAV(c_filename.c_str());
    if (!chunk) {
        std::cerr << "SDL_mixer error loading WAV: " << Mix_GetError() << std::endl;
        return 0;
    }
    
    int64_t chunk_id = next_chunk_id++;
    loaded_chunks[chunk_id] = chunk;
    return chunk_id;
}

extern "C" int64_t SDL2_MIXER_PLAY_CHANNEL(int64_t channel, int64_t chunk_id, int64_t loops) {
    auto it = loaded_chunks.find(chunk_id);
    if (it == loaded_chunks.end() || !it->second) {
        return -1; // Invalid chunk ID
    }
    
    int result = Mix_PlayChannel(
        static_cast<int>(channel),
        it->second,
        static_cast<int>(loops)
    );
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_PLAY_CHANNEL_TIMED(int64_t channel, int64_t chunk_id, int64_t loops, int64_t ticks) {
    auto it = loaded_chunks.find(chunk_id);
    if (it == loaded_chunks.end() || !it->second) {
        return -1; // Invalid chunk ID
    }
    
    int result = Mix_PlayChannelTimed(
        static_cast<int>(channel),
        it->second,
        static_cast<int>(loops),
        static_cast<int>(ticks)
    );
    return static_cast<int64_t>(result);
}

extern "C" void SDL2_MIXER_PAUSE(int64_t channel) {
    Mix_Pause(static_cast<int>(channel));
}

extern "C" void SDL2_MIXER_RESUME(int64_t channel) {
    Mix_Resume(static_cast<int>(channel));
}

extern "C" void SDL2_MIXER_HALT_CHANNEL(int64_t channel) {
    Mix_HaltChannel(static_cast<int>(channel));
}

extern "C" void SDL2_MIXER_FREE_CHUNK(int64_t chunk_id) {
    auto it = loaded_chunks.find(chunk_id);
    if (it != loaded_chunks.end()) {
        if (it->second) {
            Mix_FreeChunk(it->second);
        }
        loaded_chunks.erase(it);
    }
}

// =============================================================================
// MUSIC (LONGER AUDIO FILES)
// =============================================================================

extern "C" int64_t SDL2_MIXER_LOAD_MUS(bcpl_string_t filename) {
    std::string c_filename = bcpl_to_cstring(filename);
    if (c_filename.empty()) {
        return 0; // Invalid filename
    }
    
    Mix_Music* music = Mix_LoadMUS(c_filename.c_str());
    if (!music) {
        std::cerr << "SDL_mixer error loading music: " << Mix_GetError() << std::endl;
        return 0;
    }
    
    int64_t music_id = next_music_id++;
    loaded_music[music_id] = music;
    return music_id;
}

extern "C" int64_t SDL2_MIXER_PLAY_MUSIC(int64_t music_id, int64_t loops) {
    auto it = loaded_music.find(music_id);
    if (it == loaded_music.end() || !it->second) {
        return -1; // Invalid music ID
    }
    
    int result = Mix_PlayMusic(it->second, static_cast<int>(loops));
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_FADE_IN_MUSIC(int64_t music_id, int64_t loops, int64_t ms) {
    auto it = loaded_music.find(music_id);
    if (it == loaded_music.end() || !it->second) {
        return -1; // Invalid music ID
    }
    
    int result = Mix_FadeInMusic(it->second, static_cast<int>(loops), static_cast<int>(ms));
    return static_cast<int64_t>(result);
}

extern "C" void SDL2_MIXER_PAUSE_MUSIC(void) {
    Mix_PauseMusic();
}

extern "C" void SDL2_MIXER_RESUME_MUSIC(void) {
    Mix_ResumeMusic();
}

extern "C" void SDL2_MIXER_HALT_MUSIC(void) {
    Mix_HaltMusic();
}

extern "C" int64_t SDL2_MIXER_FADE_OUT_MUSIC(int64_t ms) {
    int result = Mix_FadeOutMusic(static_cast<int>(ms));
    return static_cast<int64_t>(result);
}

extern "C" void SDL2_MIXER_FREE_MUSIC(int64_t music_id) {
    auto it = loaded_music.find(music_id);
    if (it != loaded_music.end()) {
        if (it->second) {
            Mix_FreeMusic(it->second);
        }
        loaded_music.erase(it);
    }
}

// =============================================================================
// AUDIO STATUS AND QUERY FUNCTIONS
// =============================================================================

extern "C" int64_t SDL2_MIXER_PLAYING(int64_t channel) {
    int result = Mix_Playing(static_cast<int>(channel));
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_PAUSED(int64_t channel) {
    int result = Mix_Paused(static_cast<int>(channel));
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_PLAYING_MUSIC(void) {
    int result = Mix_PlayingMusic();
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_PAUSED_MUSIC(void) {
    int result = Mix_PausedMusic();
    return static_cast<int64_t>(result);
}

extern "C" int64_t SDL2_MIXER_ALLOCATE_CHANNELS(int64_t numchans) {
    int result = Mix_AllocateChannels(static_cast<int>(numchans));
    return static_cast<int64_t>(result);
}

// =============================================================================
// ERROR HANDLING
// =============================================================================

extern "C" bcpl_string_t SDL2_MIXER_GET_ERROR(void) {
    const char* error = Mix_GetError();
    return cstring_to_bcpl(error);
}
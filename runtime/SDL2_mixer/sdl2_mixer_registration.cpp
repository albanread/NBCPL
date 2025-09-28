#include "sdl2_mixer_runtime.h"
#include "../../RuntimeManager.h"
#include <iostream>

// SDL_mixer Runtime Registration Module
// This file registers all SDL_mixer functions with the RuntimeManager
// so they can be called from BCPL code.

namespace runtime {

// Flag to track if SDL_mixer functions have been registered
static bool sdl_mixer_functions_registered = false;

/**
 * Lazy registration of SDL_mixer runtime functions
 * This function is called only when an SDL_mixer function is first requested
 * to avoid static initializer issues with the SDL_mixer library.
 */
void lazy_register_sdl_mixer_runtime_functions() {
    if (sdl_mixer_functions_registered) {
        return; // Already registered
    }
    
    RuntimeManager& runtime = RuntimeManager::instance();
    
    try {
        // =============================================================================
        // AUDIO SYSTEM INITIALIZATION AND CLEANUP
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_OPEN_AUDIO", 4, 
                                 reinterpret_cast<void*>(SDL2_MIXER_OPEN_AUDIO), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_CLOSE_AUDIO", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_CLOSE_AUDIO), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_QUIT", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_QUIT), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // VOLUME CONTROL
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_VOLUME", 2, 
                                 reinterpret_cast<void*>(SDL2_MIXER_VOLUME), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_VOLUME_MUSIC", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_VOLUME_MUSIC), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // SOUND EFFECTS (CHUNKS)
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_LOAD_WAV", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_LOAD_WAV), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PLAY_CHANNEL", 3, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PLAY_CHANNEL), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PLAY_CHANNEL_TIMED", 4, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PLAY_CHANNEL_TIMED), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PAUSE", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PAUSE), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_RESUME", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_RESUME), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_HALT_CHANNEL", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_HALT_CHANNEL), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_FREE_CHUNK", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_FREE_CHUNK), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // MUSIC (LONGER AUDIO FILES)
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_LOAD_MUS", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_LOAD_MUS), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PLAY_MUSIC", 2, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PLAY_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_FADE_IN_MUSIC", 3, 
                                 reinterpret_cast<void*>(SDL2_MIXER_FADE_IN_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PAUSE_MUSIC", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PAUSE_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_RESUME_MUSIC", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_RESUME_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_HALT_MUSIC", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_HALT_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_FADE_OUT_MUSIC", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_FADE_OUT_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_FREE_MUSIC", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_FREE_MUSIC), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // AUDIO STATUS AND QUERY FUNCTIONS
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_PLAYING", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PLAYING), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PAUSED", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PAUSED), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PLAYING_MUSIC", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PLAYING_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_PAUSED_MUSIC", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_PAUSED_MUSIC), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_MIXER_ALLOCATE_CHANNELS", 1, 
                                 reinterpret_cast<void*>(SDL2_MIXER_ALLOCATE_CHANNELS), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // ERROR HANDLING
        // =============================================================================
        
        runtime.register_function("SDL2_MIXER_GET_ERROR", 0, 
                                 reinterpret_cast<void*>(SDL2_MIXER_GET_ERROR), 
                                 FunctionType::STANDARD);
        
        // Register a special function to report SDL_mixer linking type
        #ifdef SDL2_MIXER_RUNTIME_ENABLED
            // This function allows the runtime to report whether SDL_mixer is statically linked
            runtime.register_function("__SDL2_MIXER_STATIC_BUILD", 0, 
                                     reinterpret_cast<void*>(SDL2_MIXER_ALLOCATE_CHANNELS), 
                                     FunctionType::STANDARD);
        #endif
        
        // Mark SDL_mixer functions as registered
        sdl_mixer_functions_registered = true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error registering SDL_mixer runtime functions: " << e.what() << std::endl;
        throw;
    }
}

/**
 * Main registration function - registers SDL_mixer functions immediately for JIT compatibility
 */
void register_sdl_mixer_runtime_functions() {
    // Actually register SDL_mixer functions immediately for JIT compatibility
    lazy_register_sdl_mixer_runtime_functions();
    std::cout << "SDL_mixer runtime functions registered immediately" << std::endl;
}

} // namespace runtime

// Export the lazy registration function as a C function to avoid namespace issues
extern "C" void lazy_register_sdl_mixer_runtime_functions() {
    runtime::lazy_register_sdl_mixer_runtime_functions();
}

namespace runtime {

/**
 * Check if SDL_mixer runtime is available
 * This can be used to conditionally enable SDL_mixer features
 */
bool is_sdl_mixer_runtime_available() {
    RuntimeManager& runtime = RuntimeManager::instance();
    return runtime.is_function_registered("SDL2_MIXER_OPEN_AUDIO");
}

/**
 * Check if SDL_mixer is statically linked
 * This checks for the presence of a marker function that indicates static linking
 */
bool is_sdl_mixer_static_build() {
    RuntimeManager& runtime = RuntimeManager::instance();
    return runtime.is_function_registered("__SDL2_MIXER_STATIC_BUILD");
}

/**
 * Get the number of SDL_mixer functions registered
 */
size_t get_sdl_mixer_function_count() {
    // Count the number of SDL_mixer functions we registered
    return 23; // Update this if you add more functions
}

} // namespace runtime
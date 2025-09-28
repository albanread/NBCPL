#ifndef SDL2_MIXER_REGISTRATION_H
#define SDL2_MIXER_REGISTRATION_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdbool.h>

// SDL_mixer Runtime Registration Header
// This file provides the interface for registering SDL_mixer functions
// with the BCPL runtime system.

/**
 * Register all SDL_mixer runtime functions with the RuntimeManager
 * This function should be called during runtime initialization
 * to make SDL_mixer functions available to BCPL programs.
 */
void register_sdl_mixer_runtime_functions(void);

/**
 * Lazy registration of SDL_mixer runtime functions
 * This function is called only when an SDL_mixer function is first requested
 * to avoid static initializer issues with the SDL_mixer library.
 */
void lazy_register_sdl_mixer_runtime_functions(void);

#ifdef __cplusplus
}

// C++ namespace functions
namespace runtime {

/**
 * Check if SDL_mixer runtime is available
 * 
 * This function can be used to conditionally enable SDL_mixer features
 * by checking if the SDL_mixer functions have been registered.
 * 
 * @return true if SDL2_MIXER_OPEN_AUDIO function is registered (indicating SDL_mixer is available)
 * @return false if SDL_mixer functions are not available
 */
bool is_sdl_mixer_runtime_available();

/**
 * Check if SDL_mixer is statically linked
 * 
 * This checks for the presence of a marker function that indicates static linking
 * 
 * @return true if SDL_mixer appears to be statically linked
 * @return false if SDL_mixer is dynamically linked or not available
 */
bool is_sdl_mixer_static_build();

/**
 * Get the number of SDL_mixer functions registered
 * 
 * @return Number of SDL_mixer functions that have been registered
 */
size_t get_sdl_mixer_function_count();

} // namespace runtime

#endif // __cplusplus

#endif // SDL2_MIXER_REGISTRATION_H
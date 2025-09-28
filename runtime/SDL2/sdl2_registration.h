#ifndef SDL2_REGISTRATION_H
#define SDL2_REGISTRATION_H

#ifdef __cplusplus
namespace runtime {
#endif

/**
 * SDL2 Runtime Registration Header
 * 
 * This header provides the interface for registering SDL2 runtime functions
 * with the BCPL RuntimeManager. This allows SDL2 functionality to be 
 * conditionally included in the runtime when --with-sdl2 is specified.
 */

/**
 * Register all SDL2 runtime functions with the RuntimeManager
 * 
 * This function registers all SDL2 functions so they can be called from BCPL code.
 * It should be called during runtime initialization when SDL2 support is enabled.
 * 
 * Registers the following function categories:
 * - Initialization and cleanup (SDL2_INIT, SDL2_QUIT, etc.)
 * - Window management (SDL2_CREATE_WINDOW, SDL2_DESTROY_WINDOW, etc.)
 * - Rendering (SDL2_CREATE_RENDERER, SDL2_DRAW_*, SDL2_FILL_*, etc.)
 * - Event handling (SDL2_POLL_EVENT, SDL2_GET_EVENT_*, etc.)
 * - Timing functions (SDL2_DELAY, SDL2_GET_TICKS)
 * - Utility functions (SDL2_GET_ERROR, SDL2_CLEAR_ERROR)
 * 
 * @throws std::exception if any function registration fails
 */
void register_sdl2_runtime_functions(void);

/**
 * Lazy registration of SDL2 runtime functions
 * This function is called only when an SDL2 function is first requested
 * to avoid static initializer issues with the SDL2 library.
 */
void lazy_register_sdl2_runtime_functions(void);

/**
 * Check if SDL2 runtime is available
 * 
 * This function can be used to conditionally enable SDL2 features
 * by checking if the SDL2 functions have been registered.
 * 
 * @return true if SDL2_INIT function is registered (indicating SDL2 is available)
 * @return false if SDL2 functions are not available
 */
bool is_sdl2_runtime_available(void);

/**
 * Get the number of SDL2 functions registered
 * 
 * @return Number of SDL2 functions that have been registered
 */
int get_sdl2_function_count(void);

/**
 * Unregister SDL2 functions (for cleanup)
 * 
 * This function can be called to remove SDL2 functions from the runtime.
 * Useful for cleanup or when SDL2 support needs to be disabled at runtime.
 * 
 * Note: This is primarily for completeness - in practice, the runtime
 * typically doesn't need to unregister functions during normal operation.
 */
void unregister_sdl2_runtime_functions(void);

/**
 * Check if SDL2 is statically linked
 * This checks for the presence of a marker function that indicates static linking
 * @return true if SDL2 is statically linked, false otherwise
 */
bool is_sdl2_static_build(void);

#ifdef __cplusplus
} // namespace runtime
#endif

#endif // SDL2_REGISTRATION_H
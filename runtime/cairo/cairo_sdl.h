#ifndef CAIRO_SDL_H
#define CAIRO_SDL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Forward declarations
typedef uint64_t CairoSurfaceHandle;
typedef uint32_t* bcpl_string_t;

// =============================================================================
// CAIRO-SDL2 BRIDGE FUNCTIONS
// =============================================================================
// These functions provide integration between Cairo graphics and SDL2 display

/**
 * Convert Cairo surface to SDL2 texture for efficient blitting
 * BCPL Usage: texture = CAIRO_TO_SDL_TEXTURE(surface, sdl_renderer_id)
 * @param surface_handle Handle to Cairo surface
 * @param sdl_renderer_id Handle to SDL2 renderer
 * @return SDL2 texture handle (0 on failure)
 */
int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id);

/**
 * Display Cairo surface in existing SDL2 window
 * BCPL Usage: CAIRO_DISPLAY_SDL(surface, sdl_window_id)
 * @param surface_handle Handle to Cairo surface
 * @param sdl_window_id Handle to SDL2 window
 */
void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);

/**
 * Create SDL2 window optimized for Cairo display
 * BCPL Usage: window = CAIRO_CREATE_SDL_WINDOW(title, width, height)
 * @param title BCPL string for window title
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @return SDL2 window handle (0 on failure)
 */
int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height);

/**
 * Update SDL2 window with Cairo surface (for animation)
 * BCPL Usage: CAIRO_UPDATE_SDL_WINDOW(surface, sdl_window_id)
 * @param surface_handle Handle to Cairo surface
 * @param sdl_window_id Handle to SDL2 window
 */
void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);

// =============================================================================
// ERROR HANDLING AND UTILITIES
// =============================================================================

/**
 * Get last error from the Cairo-SDL2 bridge
 * @return Error message string (empty if no error)
 */
const char* CairoSDL_get_last_error(void);

/**
 * Clear any pending Cairo-SDL2 bridge error
 */
void CairoSDL_clear_error(void);

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * Initialize the Cairo-SDL2 bridge
 * Should be called during runtime startup
 */
void CairoSDL_initialize(void);

/**
 * Shutdown the Cairo-SDL2 bridge  
 * Should be called during runtime shutdown
 */
void CairoSDL_shutdown(void);

#ifdef __cplusplus
}
#endif

#endif // CAIRO_SDL_H
#ifndef SDL2_SAMM_BACKEND_H
#define SDL2_SAMM_BACKEND_H

#include <stdint.h>
#include <SDL.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// SDL2 BACKEND REGISTRATION
// =============================================================================

/**
 * Register SDL2 backend with SAMM graphics system
 * @return 1 on success, 0 on failure
 */
int SDL2SAMM_register_backend(void);

/**
 * Unregister SDL2 backend from SAMM graphics system
 */
void SDL2SAMM_unregister_backend(void);

/**
 * Check if SDL2 backend is registered
 * @return 1 if registered, 0 if not
 */
int SDL2SAMM_is_backend_registered(void);

/**
 * Enable/disable SDL2 backend tracing
 * @param enabled 1 to enable, 0 to disable
 */
void SDL2SAMM_set_trace_enabled(int enabled);

// =============================================================================
// SDL2 WINDOW TRACKING
// =============================================================================

/**
 * Track an existing SDL2 window in SAMM
 * @param window SDL2 window to track
 * @param width Window width (for optimization)
 * @param height Window height (for optimization)
 * @return Handle ID for the window, 0 on failure
 */
uint64_t SDL2SAMM_track_window(SDL_Window* window, int width, int height);

/**
 * Get SDL2 window by handle ID
 * @param handle_id Handle ID from tracking functions
 * @return SDL2 window pointer or NULL if not found
 */
SDL_Window* SDL2SAMM_get_window(uint64_t handle_id);

/**
 * Manually untrack an SDL2 window
 * @param handle_id Handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int SDL2SAMM_untrack_window(uint64_t handle_id);

// =============================================================================
// SDL2 RENDERER TRACKING
// =============================================================================

/**
 * Track an existing SDL2 renderer in SAMM
 * @param renderer SDL2 renderer to track
 * @param window_width Associated window width (for estimation)
 * @param window_height Associated window height (for estimation)
 * @return Handle ID for the renderer, 0 on failure
 */
uint64_t SDL2SAMM_track_renderer(SDL_Renderer* renderer, int window_width, int window_height);

/**
 * Get SDL2 renderer by handle ID
 * @param handle_id Handle ID from tracking functions
 * @return SDL2 renderer pointer or NULL if not found
 */
SDL_Renderer* SDL2SAMM_get_renderer(uint64_t handle_id);

/**
 * Manually untrack an SDL2 renderer
 * @param handle_id Handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int SDL2SAMM_untrack_renderer(uint64_t handle_id);

// =============================================================================
// SDL2 TEXTURE TRACKING
// =============================================================================

/**
 * Track an existing SDL2 texture in SAMM
 * @param texture SDL2 texture to track
 * @param width Texture width (for memory estimation)
 * @param height Texture height (for memory estimation)
 * @param format SDL texture format (for memory estimation)
 * @return Handle ID for the texture, 0 on failure
 */
uint64_t SDL2SAMM_track_texture(SDL_Texture* texture, int width, int height, uint32_t format);

/**
 * Get SDL2 texture by handle ID
 * @param handle_id Handle ID from tracking functions
 * @return SDL2 texture pointer or NULL if not found
 */
SDL_Texture* SDL2SAMM_get_texture(uint64_t handle_id);

/**
 * Manually untrack an SDL2 texture
 * @param handle_id Handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int SDL2SAMM_untrack_texture(uint64_t handle_id);

// =============================================================================
// SDL2 SURFACE CREATION WITH AUTOMATIC TRACKING
// =============================================================================

/**
 * Create a new SDL2 window with automatic SAMM tracking
 * @param title Window title
 * @param x Window X position
 * @param y Window Y position
 * @param width Window width in pixels
 * @param height Window height in pixels
 * @param flags SDL window flags
 * @return Handle ID for the window, 0 on failure
 */
uint64_t SDL2SAMM_create_window(const char* title, int x, int y, int width, int height, uint32_t flags);

/**
 * Create a new SDL2 renderer with automatic SAMM tracking
 * @param window_handle_id Handle ID of the associated window
 * @param index Rendering driver index (-1 for first supporting flags)
 * @param flags SDL renderer flags
 * @return Handle ID for the renderer, 0 on failure
 */
uint64_t SDL2SAMM_create_renderer(uint64_t window_handle_id, int index, uint32_t flags);

/**
 * Create a new SDL2 texture with automatic SAMM tracking
 * @param renderer_handle_id Handle ID of the associated renderer
 * @param format SDL pixel format
 * @param access SDL texture access pattern
 * @param width Texture width in pixels
 * @param height Texture height in pixels
 * @return Handle ID for the texture, 0 on failure
 */
uint64_t SDL2SAMM_create_texture(uint64_t renderer_handle_id, uint32_t format, int access, int width, int height);

/**
 * Load BMP as SDL2 surface with automatic SAMM tracking
 * @param filename Path to BMP file
 * @return Handle ID for the loaded surface, 0 on failure
 */
uint64_t SDL2SAMM_load_bmp(const char* filename);

// =============================================================================
// INTEGRATION AND INITIALIZATION
// =============================================================================

/**
 * Initialize SDL2 SAMM backend
 * Called automatically during runtime startup
 */
void SDL2SAMM_initialize(void);

/**
 * Shutdown SDL2 SAMM backend
 * Called automatically during runtime shutdown
 */
void SDL2SAMM_shutdown(void);

/**
 * Check if SDL2 SAMM backend is enabled
 * @return 1 if enabled, 0 if not
 */
int SDL2SAMM_is_enabled(void);

// =============================================================================
// MEMORY ESTIMATION UTILITIES
// =============================================================================

/**
 * Estimate memory usage for SDL2 window
 * @param width Window width
 * @param height Window height
 * @return Estimated memory usage in bytes
 */
size_t SDL2SAMM_estimate_window_memory(int width, int height);

/**
 * Estimate memory usage for SDL2 renderer
 * @param window_width Associated window width
 * @param window_height Associated window height
 * @param flags Renderer flags (affects memory usage)
 * @return Estimated memory usage in bytes
 */
size_t SDL2SAMM_estimate_renderer_memory(int window_width, int window_height, uint32_t flags);

/**
 * Estimate memory usage for SDL2 texture
 * @param width Texture width
 * @param height Texture height
 * @param format SDL pixel format
 * @return Estimated memory usage in bytes
 */
size_t SDL2SAMM_estimate_texture_memory(int width, int height, uint32_t format);

/**
 * Get bytes per pixel for SDL2 pixel format
 * @param format SDL pixel format constant
 * @return Number of bytes per pixel, or 4 if unknown
 */
int SDL2SAMM_get_bytes_per_pixel(uint32_t format);

// =============================================================================
// SCOPE MANAGEMENT INTEGRATION
// =============================================================================

/**
 * Move SDL2 window to a different scope level
 * @param handle_id Window handle ID
 * @param target_scope_offset 1 = parent scope, 2 = grandparent, etc.
 * @return 1 on success, 0 on failure
 */
int SDL2SAMM_move_window_to_scope(uint64_t handle_id, int target_scope_offset);

/**
 * Move SDL2 renderer to a different scope level
 * @param handle_id Renderer handle ID
 * @param target_scope_offset 1 = parent scope, 2 = grandparent, etc.
 * @return 1 on success, 0 on failure
 */
int SDL2SAMM_move_renderer_to_scope(uint64_t handle_id, int target_scope_offset);

/**
 * Move SDL2 texture to a different scope level
 * @param handle_id Texture handle ID
 * @param target_scope_offset 1 = parent scope, 2 = grandparent, etc.
 * @return 1 on success, 0 on failure
 */
int SDL2SAMM_move_texture_to_scope(uint64_t handle_id, int target_scope_offset);

// =============================================================================
// LEGACY COMPATIBILITY FUNCTIONS
// =============================================================================

/**
 * Legacy scope entry function (no-op with new system)
 * Maintained for compatibility with existing code
 */
void SDL2SAMM_enterScope(void);

/**
 * Legacy scope exit function (no-op with new system)
 * Maintained for compatibility with existing code
 */
void SDL2SAMM_exitScope(void);

/**
 * Legacy window tracking function (no-op with new system)
 * Maintained for compatibility with existing code
 * @param handle_id Window handle ID
 */
void SDL2SAMM_trackWindow(uint64_t handle_id);

/**
 * Legacy renderer tracking function (no-op with new system)
 * Maintained for compatibility with existing code
 * @param handle_id Renderer handle ID
 */
void SDL2SAMM_trackRenderer(uint64_t handle_id);

#ifdef __cplusplus
}
#endif

#endif // SDL2_SAMM_BACKEND_H
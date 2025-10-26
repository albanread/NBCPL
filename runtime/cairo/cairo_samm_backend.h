#ifndef CAIRO_SAMM_BACKEND_H
#define CAIRO_SAMM_BACKEND_H

#include <stdint.h>
#include <cairo.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// CAIRO BACKEND REGISTRATION
// =============================================================================

/**
 * Register Cairo backend with SAMM graphics system
 * @return 1 on success, 0 on failure
 */
int CairoSAMM_register_backend(void);

/**
 * Unregister Cairo backend from SAMM graphics system
 */
void CairoSAMM_unregister_backend(void);

/**
 * Check if Cairo backend is registered
 * @return 1 if registered, 0 if not
 */
int CairoSAMM_is_backend_registered(void);

/**
 * Enable/disable Cairo backend tracing
 * @param enabled 1 to enable, 0 to disable
 */
void CairoSAMM_set_trace_enabled(int enabled);

// =============================================================================
// CAIRO SURFACE TRACKING
// =============================================================================

/**
 * Track an existing Cairo surface in SAMM
 * @param surface Cairo surface to track
 * @param width Surface width (for optimization)
 * @param height Surface height (for optimization)
 * @return Handle ID for the surface, 0 on failure
 */
uint64_t CairoSAMM_track_surface(cairo_surface_t* surface, int width, int height);

/**
 * Track an existing Cairo image in SAMM
 * @param image Cairo surface used as image to track
 * @param width Image width (for optimization)
 * @param height Image height (for optimization)
 * @return Handle ID for the image, 0 on failure
 */
uint64_t CairoSAMM_track_image(cairo_surface_t* image, int width, int height);

/**
 * Get Cairo surface by handle ID
 * @param handle_id Handle ID from tracking functions
 * @return Cairo surface pointer or NULL if not found
 */
cairo_surface_t* CairoSAMM_get_surface(uint64_t handle_id);

/**
 * Get Cairo image by handle ID
 * @param handle_id Handle ID from tracking functions
 * @return Cairo surface pointer or NULL if not found
 */
cairo_surface_t* CairoSAMM_get_image(uint64_t handle_id);

/**
 * Manually untrack a Cairo surface
 * @param handle_id Handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int CairoSAMM_untrack_surface(uint64_t handle_id);

/**
 * Manually untrack a Cairo image
 * @param handle_id Handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int CairoSAMM_untrack_image(uint64_t handle_id);

// =============================================================================
// CAIRO SURFACE CREATION WITH AUTOMATIC TRACKING
// =============================================================================

/**
 * Create a new Cairo surface with automatic SAMM tracking
 * @param width Surface width in pixels
 * @param height Surface height in pixels
 * @return Handle ID for the surface, 0 on failure
 */
uint64_t CairoSAMM_create_surface(int width, int height);

/**
 * Load PNG as Cairo surface with automatic SAMM tracking
 * @param filename Path to PNG file
 * @return Handle ID for the loaded image, 0 on failure
 */
uint64_t CairoSAMM_load_png(const char* filename);

// =============================================================================
// INTEGRATION AND INITIALIZATION
// =============================================================================

/**
 * Initialize Cairo SAMM backend
 * Called automatically during runtime startup
 */
void CairoSAMM_initialize(void);

/**
 * Shutdown Cairo SAMM backend
 * Called automatically during runtime shutdown
 */
void CairoSAMM_shutdown(void);

/**
 * Check if Cairo SAMM backend is enabled
 * @return 1 if enabled, 0 if not
 */
int CairoSAMM_is_enabled(void);

// =============================================================================
// LEGACY COMPATIBILITY FUNCTIONS
// =============================================================================

/**
 * Legacy scope entry function (no-op with new system)
 * Maintained for compatibility with existing code
 */
void CairoSAMM_enterScope(void);

/**
 * Legacy scope exit function (no-op with new system)
 * Maintained for compatibility with existing code
 */
void CairoSAMM_exitScope(void);

/**
 * Legacy surface tracking function (no-op with new system)
 * Maintained for compatibility with existing code
 * @param handle_id Surface handle ID
 */
void CairoSAMM_trackSurface(uint64_t handle_id);

#ifdef __cplusplus
}
#endif

#endif // CAIRO_SAMM_BACKEND_H
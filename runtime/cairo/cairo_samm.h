#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Forward declarations
typedef uint64_t CairoSurfaceHandle;
typedef uint64_t CairoImageHandle;

/**
 * SAMM Integration for Cairo Graphics Resources
 * 
 * This module provides integration between Cairo graphics resources and the
 * SAMM (Scope Aware Memory Management) system. All Cairo surfaces and images
 * are automatically tracked and cleaned up when BCPL scopes exit.
 */

// =============================================================================
// SAMM SCOPE MANAGEMENT
// =============================================================================

/**
 * Enter new SAMM scope for Cairo resource tracking
 * Called automatically by BCPL runtime on function entry
 * BCPL Usage: Called implicitly - no direct BCPL function
 */
void CairoSAMM_enterScope(void);

/**
 * Exit SAMM scope and cleanup Cairo resources
 * Called automatically by BCPL runtime on function exit
 * BCPL Usage: Called implicitly - no direct BCPL function
 */
void CairoSAMM_exitScope(void);

/**
 * Get current SAMM scope depth for Cairo resources
 * BCPL Usage: depth = CAIRO_SAMM_SCOPE_DEPTH()
 * @return Current scope nesting level
 */
int64_t CAIRO_SAMM_SCOPE_DEPTH(void);

// =============================================================================
// RESOURCE TRACKING
// =============================================================================

/**
 * Track surface in current SAMM scope
 * Called automatically when surfaces are created
 * @param handle Surface handle to track
 */
void CairoSAMM_trackSurface(CairoSurfaceHandle handle);

/**
 * Track image in current SAMM scope
 * Called automatically when images are loaded
 * @param handle Image handle to track
 */
void CairoSAMM_trackImage(CairoImageHandle handle);

/**
 * Untrack surface from current scope (for retained resources)
 * @param handle Surface handle to untrack
 */
void CairoSAMM_untrackSurface(CairoSurfaceHandle handle);

/**
 * Untrack image from current scope (for retained resources)
 * @param handle Image handle to untrack
 */
void CairoSAMM_untrackImage(CairoImageHandle handle);

// =============================================================================
// RETAINED RESOURCE ALLOCATION
// =============================================================================

/**
 * Create surface that survives current scope
 * BCPL Usage: surface = CAIRO_CREATE_SURFACE_RETAINED(width, height, parent_offset)
 * @param width Surface width in pixels
 * @param height Surface height in pixels
 * @param parent_scope_offset How many scopes up to attach (1 = parent scope)
 * @return Surface handle (0 on failure)
 */
CairoSurfaceHandle CAIRO_CREATE_SURFACE_RETAINED(int64_t width, int64_t height, int64_t parent_scope_offset);

/**
 * Load PNG as surface that survives current scope
 * BCPL Usage: surface = CAIRO_LOAD_PNG_RETAINED(filename, parent_offset)
 * @param filename BCPL string path to PNG file
 * @param parent_scope_offset How many scopes up to attach (1 = parent scope)
 * @return Surface handle (0 on failure)
 */
CairoSurfaceHandle CAIRO_LOAD_PNG_RETAINED(const uint32_t* filename, int64_t parent_scope_offset);

/**
 * Load image that survives current scope
 * BCPL Usage: image = CAIRO_LOAD_IMAGE_RETAINED(filename, parent_offset)
 * @param filename BCPL string path to PNG file
 * @param parent_scope_offset How many scopes up to attach (1 = parent scope)
 * @return Image handle (0 on failure)
 */
CairoImageHandle CAIRO_LOAD_IMAGE_RETAINED(const uint32_t* filename, int64_t parent_scope_offset);

// =============================================================================
// MEMORY PRESSURE HANDLING
// =============================================================================

/**
 * Handle memory pressure by cleaning up unused Cairo resources
 * Called by SAMM when memory usage is high
 */
void CairoSAMM_handleMemoryPressure(void);

/**
 * Force immediate cleanup of all Cairo resources in completed scopes
 * BCPL Usage: CAIRO_SAMM_FORCE_CLEANUP()
 */
void CAIRO_SAMM_FORCE_CLEANUP(void);

/**
 * Set memory pressure threshold for Cairo resources
 * BCPL Usage: CAIRO_SAMM_SET_MEMORY_THRESHOLD(bytes)
 * @param threshold_bytes Trigger cleanup when usage exceeds this
 */
void CAIRO_SAMM_SET_MEMORY_THRESHOLD(int64_t threshold_bytes);

// =============================================================================
// STATISTICS AND DEBUGGING
// =============================================================================

/**
 * Get SAMM statistics for Cairo resources
 * BCPL Usage: CAIRO_SAMM_GET_STATS(surfaces_ptr, images_ptr, memory_ptr)
 * @param active_surfaces Pointer to receive active surface count
 * @param active_images Pointer to receive active image count  
 * @param memory_usage Pointer to receive estimated memory usage in bytes
 */
void CAIRO_SAMM_GET_STATS(int64_t* active_surfaces, int64_t* active_images, int64_t* memory_usage);

/**
 * Get detailed scope statistics
 * BCPL Usage: CAIRO_SAMM_GET_SCOPE_STATS(scope_depth, surfaces_in_scope, images_in_scope)
 * @param scope_level Which scope level to query (0 = current)
 * @param surfaces_ptr Pointer to receive surface count in that scope
 * @param images_ptr Pointer to receive image count in that scope
 */
void CAIRO_SAMM_GET_SCOPE_STATS(int64_t scope_level, int64_t* surfaces_ptr, int64_t* images_ptr);

/**
 * Enable/disable SAMM debug tracing for Cairo
 * BCPL Usage: CAIRO_SAMM_SET_TRACE(enabled)
 * @param enabled 1 to enable tracing, 0 to disable
 */
void CAIRO_SAMM_SET_TRACE(int64_t enabled);

/**
 * Dump current SAMM state for Cairo resources to stdout
 * BCPL Usage: CAIRO_SAMM_DUMP_STATE()
 */
void CAIRO_SAMM_DUMP_STATE(void);

// =============================================================================
// INTEGRATION WITH CORE SAMM SYSTEM
// =============================================================================

/**
 * Initialize Cairo SAMM integration
 * Called during runtime startup
 */
void CairoSAMM_initialize(void);

/**
 * Shutdown Cairo SAMM integration
 * Called during runtime shutdown
 */
void CairoSAMM_shutdown(void);

/**
 * Check if Cairo SAMM is enabled
 * @return true if SAMM integration is active
 */
bool CairoSAMM_isEnabled(void);

/**
 * Set Cairo SAMM enabled state
 * @param enabled Whether to enable SAMM integration
 */
void CairoSAMM_setEnabled(bool enabled);

// =============================================================================
// BACKGROUND CLEANUP INTEGRATION
// =============================================================================

/**
 * Queue Cairo resources for background cleanup
 * Called by SAMM background worker thread
 */
void CairoSAMM_queueBackgroundCleanup(void);

/**
 * Process queued Cairo cleanup tasks
 * Called by SAMM background worker thread
 */
void CairoSAMM_processBackgroundCleanup(void);

/**
 * Wait for all Cairo background cleanup to complete
 * Called during shutdown or before critical operations
 */
void CairoSAMM_waitForBackgroundCleanup(void);

/**
 * Get number of Cairo resources queued for background cleanup
 * BCPL Usage: count = CAIRO_SAMM_BACKGROUND_QUEUE_SIZE()
 * @return Number of resources waiting for cleanup
 */
int64_t CAIRO_SAMM_BACKGROUND_QUEUE_SIZE(void);

// =============================================================================
// ERROR HANDLING AND RECOVERY
// =============================================================================

/**
 * Recover from Cairo resource leaks by forcing cleanup
 * BCPL Usage: recovered = CAIRO_SAMM_RECOVER_LEAKS()
 * @return Number of leaked resources recovered
 */
int64_t CAIRO_SAMM_RECOVER_LEAKS(void);

/**
 * Validate Cairo SAMM internal state consistency
 * BCPL Usage: result = CAIRO_SAMM_VALIDATE_STATE()
 * @return 1 if state is consistent, 0 if corruption detected
 */
int64_t CAIRO_SAMM_VALIDATE_STATE(void);

/**
 * Reset Cairo SAMM to clean state (emergency recovery)
 * BCPL Usage: CAIRO_SAMM_EMERGENCY_RESET()
 * Warning: This will leak any active resources
 */
void CAIRO_SAMM_EMERGENCY_RESET(void);

#ifdef __cplusplus
}
#endif
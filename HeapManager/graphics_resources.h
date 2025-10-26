#ifndef GRAPHICS_RESOURCES_H
#define GRAPHICS_RESOURCES_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// =============================================================================
// GENERIC GRAPHICS RESOURCE TYPES
// =============================================================================

/**
 * Generic graphics surface - wraps native handles from any graphics library
 * (Cairo, SDL2, OpenGL, Vulkan, etc.)
 */
typedef struct {
    void* native_handle;                    // Points to backend-specific surface
    void (*cleanup_func)(void*);            // Backend-specific cleanup function
    size_t (*get_size_func)(void*);         // Get memory usage (optional, can be NULL)
    size_t estimated_memory;                // Cached memory usage estimate
    const char* backend_name;               // "cairo", "sdl2", "opengl", etc.
    uint64_t handle_id;                     // Unique handle for BCPL interface
    int width;                              // Surface dimensions
    int height;
    uint32_t format;                        // Backend-specific format info
    void* user_data;                        // Optional user data pointer
} GraphicsSurface;

/**
 * Generic graphics image - wraps native image handles
 */
typedef struct {
    void* native_handle;                    // Points to backend-specific image
    void (*cleanup_func)(void*);            // Backend-specific cleanup function
    size_t (*get_size_func)(void*);         // Get memory usage (optional, can be NULL)
    size_t estimated_memory;                // Cached memory usage estimate
    const char* backend_name;               // "cairo", "sdl2", "opengl", etc.
    uint64_t handle_id;                     // Unique handle for BCPL interface
    int width;                              // Image dimensions
    int height;
    uint32_t format;                        // Backend-specific format info
    void* user_data;                        // Optional user data pointer
} GraphicsImage;

/**
 * Backend registration structure
 * Graphics libraries register their cleanup functions at runtime
 */
typedef struct {
    const char* backend_name;
    void (*surface_cleanup)(void* native_handle);
    void (*image_cleanup)(void* native_handle);
    size_t (*surface_get_size)(void* native_handle);
    size_t (*image_get_size)(void* native_handle);
    int (*surface_get_dimensions)(void* native_handle, int* width, int* height);
    int (*image_get_dimensions)(void* native_handle, int* width, int* height);
} GraphicsBackend;

// =============================================================================
// BACKEND REGISTRATION API
// =============================================================================

/**
 * Register a graphics backend with SAMM
 * Called by graphics libraries during initialization
 * @param backend Backend function pointers and metadata
 * @return 1 on success, 0 on failure
 */
int SAMM_register_graphics_backend(const GraphicsBackend* backend);

/**
 * Unregister a graphics backend
 * @param backend_name Name of backend to unregister
 * @return 1 on success, 0 if not found
 */
int SAMM_unregister_graphics_backend(const char* backend_name);

/**
 * Check if a graphics backend is registered
 * @param backend_name Name to check
 * @return 1 if registered, 0 if not
 */
int SAMM_is_graphics_backend_available(const char* backend_name);

/**
 * List all registered graphics backends
 * @param backends Array to fill with backend names (can be NULL to get count)
 * @param max_backends Size of backends array
 * @return Number of registered backends
 */
int SAMM_list_graphics_backends(const char** backends, int max_backends);

// =============================================================================
// GRAPHICS RESOURCE TRACKING API
// =============================================================================

/**
 * Track a graphics surface in current SAMM scope
 * @param surface Surface to track (takes ownership)
 * @return 1 on success, 0 on failure
 */
int SAMM_track_graphics_surface(GraphicsSurface* surface);

/**
 * Track a graphics image in current SAMM scope
 * @param image Image to track (takes ownership)
 * @return 1 on success, 0 on failure
 */
int SAMM_track_graphics_image(GraphicsImage* image);

/**
 * Untrack a surface from current scope (for retained resources)
 * @param handle_id Surface handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int SAMM_untrack_graphics_surface(uint64_t handle_id);

/**
 * Untrack an image from current scope (for retained resources)
 * @param handle_id Image handle ID to untrack
 * @return 1 on success, 0 if not found
 */
int SAMM_untrack_graphics_image(uint64_t handle_id);

/**
 * Move a surface to a different scope level
 * @param handle_id Surface handle ID
 * @param target_scope_offset 1 = parent scope, 2 = grandparent, etc.
 * @return 1 on success, 0 on failure
 */
int SAMM_move_graphics_surface_to_scope(uint64_t handle_id, int target_scope_offset);

/**
 * Move an image to a different scope level
 * @param handle_id Image handle ID
 * @param target_scope_offset 1 = parent scope, 2 = grandparent, etc.
 * @return 1 on success, 0 on failure
 */
int SAMM_move_graphics_image_to_scope(uint64_t handle_id, int target_scope_offset);

// =============================================================================
// RESOURCE LOOKUP AND ACCESS
// =============================================================================

/**
 * Get a tracked graphics surface by handle ID
 * @param handle_id Surface handle ID
 * @return Pointer to GraphicsSurface or NULL if not found
 */
GraphicsSurface* SAMM_get_graphics_surface(uint64_t handle_id);

/**
 * Get a tracked graphics image by handle ID
 * @param handle_id Image handle ID
 * @return Pointer to GraphicsImage or NULL if not found
 */
GraphicsImage* SAMM_get_graphics_image(uint64_t handle_id);

/**
 * Check if a surface handle is valid and tracked
 * @param handle_id Surface handle ID to check
 * @return 1 if valid and tracked, 0 if not
 */
int SAMM_is_graphics_surface_valid(uint64_t handle_id);

/**
 * Check if an image handle is valid and tracked
 * @param handle_id Image handle ID to check
 * @return 1 if valid and tracked, 0 if not
 */
int SAMM_is_graphics_image_valid(uint64_t handle_id);

// =============================================================================
// SCOPE MANAGEMENT INTEGRATION
// =============================================================================

/**
 * Cleanup graphics resources when exiting a SAMM scope
 * Called automatically by SAMM scope exit
 * @param scope_depth The scope being exited
 */
void SAMM_cleanup_graphics_resources_for_scope(int scope_depth);

/**
 * Force immediate cleanup of all graphics resources in completed scopes
 */
void SAMM_force_graphics_cleanup(void);

/**
 * Handle memory pressure by cleaning up unused graphics resources
 */
void SAMM_graphics_handle_memory_pressure(void);

// =============================================================================
// STATISTICS AND DEBUGGING
// =============================================================================

/**
 * Get graphics resource statistics
 * @param active_surfaces Pointer to receive active surface count (can be NULL)
 * @param active_images Pointer to receive active image count (can be NULL)
 * @param total_memory Pointer to receive estimated total memory usage (can be NULL)
 */
void SAMM_get_graphics_stats(int64_t* active_surfaces, int64_t* active_images, int64_t* total_memory);

/**
 * Get statistics for a specific graphics backend
 * @param backend_name Backend to query
 * @param surfaces Pointer to receive surface count for this backend
 * @param images Pointer to receive image count for this backend
 * @param memory Pointer to receive memory usage for this backend
 * @return 1 on success, 0 if backend not found
 */
int SAMM_get_graphics_backend_stats(const char* backend_name, 
                                    int64_t* surfaces, int64_t* images, int64_t* total_memory);

/**
 * Dump current graphics resource state to stdout (debug)
 */
void SAMM_dump_graphics_state(void);

/**
 * Validate graphics resource tracking consistency
 * @return 1 if consistent, 0 if corruption detected
 */
int SAMM_validate_graphics_state(void);

/**
 * Enable/disable graphics resource tracing
 * @param enabled 1 to enable, 0 to disable
 */
void SAMM_set_graphics_trace_enabled(int enabled);

// =============================================================================
// INITIALIZATION AND SHUTDOWN
// =============================================================================

/**
 * Initialize graphics resource tracking system
 * Called during SAMM initialization
 * @return 1 on success, 0 on failure
 */
int SAMM_initialize_graphics_resources(void);

/**
 * Shutdown graphics resource tracking system
 * Cleans up all tracked resources and frees internal structures
 */
void SAMM_shutdown_graphics_resources(void);

/**
 * Check if graphics resource tracking is initialized
 * @return 1 if initialized, 0 if not
 */
int SAMM_is_graphics_initialized(void);

// =============================================================================
// HANDLE GENERATION
// =============================================================================

/**
 * Generate a unique handle ID for graphics resources
 * Thread-safe handle generation
 * @return Unique 64-bit handle ID
 */
uint64_t SAMM_generate_graphics_handle(void);

#ifdef __cplusplus
}
#endif

#endif // GRAPHICS_RESOURCES_H
// NewBCPL/HeapManager/heap_c_wrappers.cpp
// ============================================================================
// MAINTAINER NOTE:
// If you add new HeapManager methods that need to be called from C or assembly
// (the runtime), you MUST add a corresponding extern "C" wrapper function here.
// Otherwise, you will get linker errors!
//
// See also: runtime/README.md for more information on runtime ABI requirements.
// ============================================================================

// C linkage wrappers for Heap_allocVec, Heap_allocObject, Heap_free, and BCPL_SET_ERROR

#include "HeapManager.h"
#include "../include/compiler_interface.h"

extern "C" void* Heap_allocVec(size_t numElements) {
    return HeapManager::getInstance().allocVec(numElements);
}

extern "C" void* Heap_allocObject(size_t size) {
    return HeapManager::getInstance().allocObject(size);
}

extern "C" void Heap_free(void* payload) {
    HeapManager::getInstance().free(payload);
}

// Forward declaration of the proper error tracking function
extern "C" void _BCPL_SET_ERROR(int code, const char* func, const char* msg);

// SAMM: Scope Aware Memory Management C wrapper functions
extern "C" void HeapManager_setSAMMEnabled(int enabled) {
    HeapManager::getInstance().setSAMMEnabled(enabled != 0);
}

extern "C" int HeapManager_isSAMMEnabled(void) {
    return HeapManager::getInstance().isSAMMEnabled() ? 1 : 0;
}

extern "C" void HeapManager_enterScope(void) {
    HeapManager::getInstance().enterScope();
}

// Alias for snake_case expected by BCPL/assembly
extern "C" void HeapManager_enter_scope(void) {
    HeapManager_enterScope();
}

extern "C" void HeapManager_exitScope(void) {
    HeapManager::getInstance().exitScope();
}

// Alias for snake_case expected by BCPL/assembly
extern "C" void HeapManager_exit_scope(void) {
    HeapManager_exitScope();
}

extern "C" void HeapManager_retainPointer(void* ptr, int parent_scope_offset) {
    HeapManager::getInstance().retainPointer(ptr, parent_scope_offset);
}

extern "C" void HeapManager_trackFreelistAllocation(void* ptr) {
    HeapManager::getInstance().trackFreelistAllocation(ptr);
}

extern "C" void HeapManager_handleMemoryPressure(void) {
    HeapManager::getInstance().handleMemoryPressure();
}

extern "C" void HeapManager_waitForSAMM(void) {
    HeapManager::getInstance().waitForSAMM();
}

extern "C" void HeapManager_shutdown(void) {
    HeapManager::getInstance().shutdown();
}

// SAMM: RETAIN allocation variants
extern "C" void* HeapManager_allocObjectRetained(size_t size, int parent_scope_offset) {
    return HeapManager::getInstance().allocObjectRetained(size, parent_scope_offset);
}

extern "C" void* HeapManager_allocVecRetained(size_t numElements, int parent_scope_offset) {
    return HeapManager::getInstance().allocVecRetained(numElements, parent_scope_offset);
}

extern "C" void* HeapManager_allocStringRetained(size_t numChars, int parent_scope_offset) {
    return HeapManager::getInstance().allocStringRetained(numChars, parent_scope_offset);
}

extern "C" void* HeapManager_allocListRetained(int parent_scope_offset) {
    return HeapManager::getInstance().allocListRetained(parent_scope_offset);
}

// Graphics Resources C API wrapper implementations
extern "C" int HeapManager_registerGraphicsBackend(const char* backend_name,
                                        void (*surface_cleanup)(void*),
                                        void (*image_cleanup)(void*)) {
    if (!backend_name || !surface_cleanup || !image_cleanup) {
        return 0;
    }
    
    GraphicsBackend backend = {};
    backend.backend_name = backend_name;
    backend.surface_cleanup = surface_cleanup;
    backend.image_cleanup = image_cleanup;
    backend.surface_get_size = nullptr;  // Optional
    backend.image_get_size = nullptr;    // Optional
    backend.surface_get_dimensions = nullptr;  // Optional
    backend.image_get_dimensions = nullptr;    // Optional
    
    return SAMM_register_graphics_backend(&backend);
}

extern "C" int HeapManager_trackGraphicsSurface(void* native_handle, const char* backend_name,
                                     int width, int height, size_t memory_size) {
    if (!native_handle || !backend_name) {
        return 0;
    }
    
    // Look up the backend to get cleanup function
    if (!SAMM_is_graphics_backend_available(backend_name)) {
        return 0;  // Backend not registered
    }
    
    // Create a new GraphicsSurface
    GraphicsSurface* surface = new GraphicsSurface();
    surface->native_handle = native_handle;
    surface->backend_name = backend_name;
    surface->width = width;
    surface->height = height;
    surface->estimated_memory = memory_size;
    surface->handle_id = SAMM_generate_graphics_handle();
    surface->format = 0;  // Default
    surface->user_data = nullptr;
    
    // Set cleanup function from registered backend
    // This is a simplified approach - in practice we'd look up the backend
    surface->cleanup_func = nullptr;  // Will be set by backend lookup
    surface->get_size_func = nullptr;
    
    return SAMM_track_graphics_surface(surface);
}

extern "C" int HeapManager_trackGraphicsImage(void* native_handle, const char* backend_name,
                                  int width, int height, size_t memory_size) {
    if (!native_handle || !backend_name) {
        return 0;
    }
    
    // Look up the backend to get cleanup function
    if (!SAMM_is_graphics_backend_available(backend_name)) {
        return 0;  // Backend not registered
    }
    
    // Create a new GraphicsImage
    GraphicsImage* image = new GraphicsImage();
    image->native_handle = native_handle;
    image->backend_name = backend_name;
    image->width = width;
    image->height = height;
    image->estimated_memory = memory_size;
    image->handle_id = SAMM_generate_graphics_handle();
    image->format = 0;  // Default
    image->user_data = nullptr;
    
    // Set cleanup function from registered backend
    image->cleanup_func = nullptr;  // Will be set by backend lookup
    image->get_size_func = nullptr;
    
    return SAMM_track_graphics_image(image);
}

extern "C" int HeapManager_untrackGraphicsSurface(uint64_t handle_id) {
    return SAMM_untrack_graphics_surface(handle_id);
}

extern "C" int HeapManager_untrackGraphicsImage(uint64_t handle_id) {
    return SAMM_untrack_graphics_image(handle_id);
}

extern "C" void* HeapManager_getGraphicsSurface(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    return surface ? surface->native_handle : nullptr;
}

extern "C" void* HeapManager_getGraphicsImage(uint64_t handle_id) {
    GraphicsImage* image = SAMM_get_graphics_image(handle_id);
    return image ? image->native_handle : nullptr;
}

extern "C" void HeapManager_getGraphicsStats(int64_t* surfaces, int64_t* images, int64_t* memory) {
    SAMM_get_graphics_stats(surfaces, images, memory);
}

// Manual SAMM tracking for custom allocators
extern "C" void HeapManager_trackInCurrentScope(void* ptr) {
    HeapManager::getInstance().trackInCurrentScope(ptr);
}

// String pool SAMM tracking
extern "C" void HeapManager_trackStringPoolAllocation(void* ptr) {
    HeapManager::getInstance().trackStringPoolAllocation(ptr);
}

// C linkage wrapper for BCPL_SET_ERROR - now properly delegates to _BCPL_SET_ERROR
extern "C" void BCPL_SET_ERROR(int code, const char* message, const char* context) {
    // Delegate to the proper error tracking system that logs to the crash report buffer
    _BCPL_SET_ERROR(code, context ? context : "unknown", message ? message : "unknown error");
    
    // Also print to stderr for immediate visibility during debugging
    fprintf(stderr, "BCPL ERROR [%d]: %s (context: %s)\n", code, message ? message : "unknown error", context ? context : "(none)");
}

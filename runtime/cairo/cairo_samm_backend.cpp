#include "cairo_samm_backend.h"
#include "../../HeapManager/graphics_resources.h"
#include "../../HeapManager/heap_c_wrappers.h"
#include "../../HeapManager/Stats.h"
#include "cairo_core.h"
#include <cairo.h>
#include <cstdio>
#include <cstring>

// =============================================================================
// CAIRO BACKEND IMPLEMENTATION
// =============================================================================

namespace {
    bool g_cairo_backend_registered = false;
    bool g_cairo_trace_enabled = false;
    
    void trace_log(const char* format, ...) {
        if (!g_cairo_trace_enabled) return;
        
        va_list args;
        va_start(args, format);
        printf("[Cairo Backend] ");
        vprintf(format, args);
        va_end(args);
    }
    
    // Forward declaration
    size_t cairo_surface_get_size(void* native_handle);
    
    // Cairo surface cleanup function
    void cairo_surface_cleanup(void* native_handle) {
        if (!native_handle) return;
        
        cairo_surface_t* surface = static_cast<cairo_surface_t*>(native_handle);
        size_t surface_size = cairo_surface_get_size(surface);
        trace_log("Cleaning up Cairo surface at %p (size: %zu bytes)\n", surface, surface_size);
        
        // Update stats for surface cleanup
        stats_update_graphics_surface_freed(surface_size);
        
        cairo_surface_destroy(surface);
    }
    
    // Cairo image cleanup function - for now, same as surface
    void cairo_image_cleanup(void* native_handle) {
        if (!native_handle) return;
        
        cairo_surface_t* surface = static_cast<cairo_surface_t*>(native_handle);
        size_t image_size = cairo_surface_get_size(surface);
        trace_log("Cleaning up Cairo image at %p (size: %zu bytes)\n", surface, image_size);
        
        // Update stats for image cleanup
        stats_update_graphics_image_freed(image_size);
        
        cairo_surface_destroy(surface);
    }
    
    // Get Cairo surface memory usage estimate
    size_t cairo_surface_get_size(void* native_handle) {
        if (!native_handle) return 0;
        
        cairo_surface_t* surface = static_cast<cairo_surface_t*>(native_handle);
        
        // Get surface dimensions
        int width = cairo_image_surface_get_width(surface);
        int height = cairo_image_surface_get_height(surface);
        cairo_format_t format = cairo_image_surface_get_format(surface);
        
        // Calculate bytes per pixel based on format
        int bytes_per_pixel = 4; // Default to ARGB32
        switch (format) {
            case CAIRO_FORMAT_ARGB32:
            case CAIRO_FORMAT_RGB24:
                bytes_per_pixel = 4;
                break;
            case CAIRO_FORMAT_A8:
                bytes_per_pixel = 1;
                break;
            case CAIRO_FORMAT_A1:
                bytes_per_pixel = 1; // Simplified
                break;
            case CAIRO_FORMAT_RGB16_565:
                bytes_per_pixel = 2;
                break;
            default:
                bytes_per_pixel = 4;
                break;
        }
        
        size_t estimated_size = static_cast<size_t>(width * height * bytes_per_pixel);
        trace_log("Cairo surface %p estimated size: %dx%d * %d bpp = %zu bytes\n",
                  surface, width, height, bytes_per_pixel, estimated_size);
        
        return estimated_size;
    }
    
    // Get Cairo surface dimensions
    int cairo_surface_get_dimensions(void* native_handle, int* width, int* height) {
        if (!native_handle || !width || !height) return 0;
        
        cairo_surface_t* surface = static_cast<cairo_surface_t*>(native_handle);
        
        *width = cairo_image_surface_get_width(surface);
        *height = cairo_image_surface_get_height(surface);
        
        return 1;
    }
}

// =============================================================================
// CAIRO BACKEND REGISTRATION
// =============================================================================

extern "C" int CairoSAMM_register_backend(void) {
    if (g_cairo_backend_registered) {
        return 1; // Already registered
    }
    
    GraphicsBackend cairo_backend = {};
    cairo_backend.backend_name = "cairo";
    cairo_backend.surface_cleanup = cairo_surface_cleanup;
    cairo_backend.image_cleanup = cairo_image_cleanup;
    cairo_backend.surface_get_size = cairo_surface_get_size;
    cairo_backend.image_get_size = cairo_surface_get_size; // Same for images
    cairo_backend.surface_get_dimensions = cairo_surface_get_dimensions;
    cairo_backend.image_get_dimensions = cairo_surface_get_dimensions; // Same for images
    
    if (SAMM_register_graphics_backend(&cairo_backend)) {
        g_cairo_backend_registered = true;
        trace_log("Cairo backend registered successfully\n");
        return 1;
    } else {
        trace_log("Failed to register Cairo backend\n");
        return 0;
    }
}

extern "C" void CairoSAMM_unregister_backend(void) {
    if (!g_cairo_backend_registered) {
        return;
    }
    
    if (SAMM_unregister_graphics_backend("cairo")) {
        g_cairo_backend_registered = false;
        trace_log("Cairo backend unregistered successfully\n");
    } else {
        trace_log("Failed to unregister Cairo backend\n");
    }
}

extern "C" int CairoSAMM_is_backend_registered(void) {
    return g_cairo_backend_registered ? 1 : 0;
}

extern "C" void CairoSAMM_set_trace_enabled(int enabled) {
    g_cairo_trace_enabled = (enabled != 0);
}

// =============================================================================
// CAIRO SURFACE TRACKING HELPERS
// =============================================================================

extern "C" uint64_t CairoSAMM_track_surface(cairo_surface_t* surface, int width, int height) {
    if (!surface || !g_cairo_backend_registered) {
        return 0;
    }
    
    // Estimate memory usage
    size_t memory_size = cairo_surface_get_size(surface);
    
    // Create GraphicsSurface
    GraphicsSurface* graphics_surface = new GraphicsSurface();
    graphics_surface->native_handle = surface;
    graphics_surface->cleanup_func = cairo_surface_cleanup;
    graphics_surface->get_size_func = cairo_surface_get_size;
    graphics_surface->estimated_memory = memory_size;
    graphics_surface->backend_name = "cairo";
    graphics_surface->handle_id = SAMM_generate_graphics_handle();
    graphics_surface->width = width;
    graphics_surface->height = height;
    graphics_surface->format = static_cast<uint32_t>(cairo_image_surface_get_format(surface));
    graphics_surface->user_data = nullptr;
    
    if (SAMM_track_graphics_surface(graphics_surface)) {
        trace_log("Tracked Cairo surface %p as handle %llu (%dx%d, %zu bytes)\n",
                  surface, graphics_surface->handle_id, width, height, memory_size);
        return graphics_surface->handle_id;
    } else {
        trace_log("Failed to track Cairo surface %p\n", surface);
        delete graphics_surface;
        return 0;
    }
}

extern "C" uint64_t CairoSAMM_track_image(cairo_surface_t* image, int width, int height) {
    if (!image || !g_cairo_backend_registered) {
        return 0;
    }
    
    // Estimate memory usage
    size_t memory_size = cairo_surface_get_size(image);
    
    // Create GraphicsImage
    GraphicsImage* graphics_image = new GraphicsImage();
    graphics_image->native_handle = image;
    graphics_image->cleanup_func = cairo_image_cleanup;
    graphics_image->get_size_func = cairo_surface_get_size;
    graphics_image->estimated_memory = memory_size;
    graphics_image->backend_name = "cairo";
    graphics_image->handle_id = SAMM_generate_graphics_handle();
    graphics_image->width = width;
    graphics_image->height = height;
    graphics_image->format = static_cast<uint32_t>(cairo_image_surface_get_format(image));
    graphics_image->user_data = nullptr;
    
    if (SAMM_track_graphics_image(graphics_image)) {
        trace_log("Tracked Cairo image %p as handle %llu (%dx%d, %zu bytes)\n",
                  image, graphics_image->handle_id, width, height, memory_size);
        return graphics_image->handle_id;
    } else {
        trace_log("Failed to track Cairo image %p\n", image);
        delete graphics_image;
        return 0;
    }
}

extern "C" cairo_surface_t* CairoSAMM_get_surface(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "cairo") != 0) {
        return nullptr;
    }
    
    return static_cast<cairo_surface_t*>(surface->native_handle);
}

extern "C" cairo_surface_t* CairoSAMM_get_image(uint64_t handle_id) {
    GraphicsImage* image = SAMM_get_graphics_image(handle_id);
    if (!image || strcmp(image->backend_name, "cairo") != 0) {
        return nullptr;
    }
    
    return static_cast<cairo_surface_t*>(image->native_handle);
}

extern "C" int CairoSAMM_untrack_surface(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "cairo") != 0) {
        return 0;
    }
    
    trace_log("Untracking Cairo surface handle %llu\n", handle_id);
    return SAMM_untrack_graphics_surface(handle_id);
}

extern "C" int CairoSAMM_untrack_image(uint64_t handle_id) {
    GraphicsImage* image = SAMM_get_graphics_image(handle_id);
    if (!image || strcmp(image->backend_name, "cairo") != 0) {
        return 0;
    }
    
    trace_log("Untracking Cairo image handle %llu\n", handle_id);
    return SAMM_untrack_graphics_image(handle_id);
}

// =============================================================================
// CAIRO SURFACE CREATION WITH AUTOMATIC TRACKING
// =============================================================================

extern "C" uint64_t CairoSAMM_create_surface(int width, int height) {
    if (!g_cairo_backend_registered) {
        trace_log("Cannot create surface - Cairo backend not registered\n");
        return 0;
    }
    
    // Create Cairo surface using standard C++ allocation
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    if (!surface || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        trace_log("Failed to create Cairo surface %dx%d\n", width, height);
        if (surface) {
            cairo_surface_destroy(surface);
        }
        return 0;
    }
    
    trace_log("Created Cairo surface %p (%dx%d)\n", surface, width, height);
    
    // Track in SAMM graphics system
    uint64_t handle_id = CairoSAMM_track_surface(surface, width, height);
    if (handle_id == 0) {
        trace_log("Failed to track Cairo surface %p\n", surface);
        cairo_surface_destroy(surface);
        return 0;
    }
    
    return handle_id;
}

extern "C" uint64_t CairoSAMM_load_png(const char* filename) {
    if (!g_cairo_backend_registered) {
        trace_log("Cannot load PNG - Cairo backend not registered\n");
        return 0;
    }
    
    // Load PNG using Cairo
    cairo_surface_t* surface = cairo_image_surface_create_from_png(filename);
    if (!surface || cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        trace_log("Failed to load PNG from %s\n", filename);
        if (surface) {
            cairo_surface_destroy(surface);
        }
        return 0;
    }
    
    int width = cairo_image_surface_get_width(surface);
    int height = cairo_image_surface_get_height(surface);
    
    trace_log("Loaded PNG %s as Cairo surface %p (%dx%d)\n", filename, surface, width, height);
    
    // Track as image in SAMM graphics system
    uint64_t handle_id = CairoSAMM_track_image(surface, width, height);
    if (handle_id == 0) {
        trace_log("Failed to track Cairo image %p\n", surface);
        cairo_surface_destroy(surface);
        return 0;
    }
    
    return handle_id;
}

// =============================================================================
// INTEGRATION AND INITIALIZATION
// =============================================================================

extern "C" void CairoSAMM_initialize(void) {
    trace_log("Initializing Cairo SAMM backend\n");
    
    // Graphics system should already be initialized by HeapManager
    if (!SAMM_is_graphics_initialized()) {
        trace_log("Warning: Graphics system not initialized\n");
        return;
    }
    
    // Register Cairo backend
    if (!CairoSAMM_register_backend()) {
        trace_log("Failed to register Cairo backend\n");
        return;
    }
    
    // Enable graphics tracing if heap tracing is enabled
    // This can be controlled independently later
    SAMM_set_graphics_trace_enabled(1);
    
    trace_log("Cairo SAMM backend initialization complete\n");
}

extern "C" void CairoSAMM_shutdown(void) {
    trace_log("Shutting down Cairo SAMM backend\n");
    
    // Unregister backend
    CairoSAMM_unregister_backend();
    
    trace_log("Cairo SAMM backend shutdown complete\n");
}

extern "C" int CairoSAMM_is_enabled(void) {
    return (SAMM_is_graphics_initialized() && g_cairo_backend_registered) ? 1 : 0;
}

// =============================================================================
// LEGACY COMPATIBILITY FUNCTIONS
// =============================================================================

// These functions provide compatibility with the old Cairo SAMM interface
extern "C" void CairoSAMM_enterScope(void) {
    // No-op - scope tracking is now handled automatically by HeapManager
    trace_log("CairoSAMM_enterScope called (no-op with new system)\n");
}

extern "C" void CairoSAMM_exitScope(void) {
    // No-op - scope cleanup is now handled automatically by HeapManager
    trace_log("CairoSAMM_exitScope called (no-op with new system)\n");
}

extern "C" void CairoSAMM_trackSurface(uint64_t handle_id) {
    // No-op - tracking is done automatically during creation
    trace_log("CairoSAMM_trackSurface called for handle %llu (no-op with new system)\n", handle_id);
}
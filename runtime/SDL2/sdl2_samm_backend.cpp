#include "sdl2_samm_backend.h"
#include "../../HeapManager/graphics_resources.h"
#include "../../HeapManager/heap_c_wrappers.h"
#include "../../HeapManager/Stats.h"
#include <SDL.h>
#include <cstdio>
#include <cstring>
#include <stdarg.h>

// =============================================================================
// SDL2 BACKEND IMPLEMENTATION
// =============================================================================

namespace {
    bool g_sdl2_backend_registered = false;
    bool g_sdl2_trace_enabled = false;
    
    void trace_log(const char* format, ...) {
        if (!g_sdl2_trace_enabled) return;
        
        va_list args;
        va_start(args, format);
        printf("[SDL2 Backend] ");
        vprintf(format, args);
        va_end(args);
    }
    
    // Forward declarations
    size_t sdl2_window_get_size(void* native_handle);
    size_t sdl2_renderer_get_size(void* native_handle);
    size_t sdl2_texture_get_size(void* native_handle);
    
    // SDL2 window cleanup function
    void sdl2_window_cleanup(void* native_handle) {
        if (!native_handle) return;
        
        SDL_Window* window = static_cast<SDL_Window*>(native_handle);
        size_t window_size = sdl2_window_get_size(window);
        trace_log("Cleaning up SDL2 window at %p (size: %zu bytes)\n", window, window_size);
        
        // Update stats for window cleanup
        stats_update_graphics_surface_freed(window_size);
        
        SDL_DestroyWindow(window);
    }
    
    // SDL2 renderer cleanup function
    void sdl2_renderer_cleanup(void* native_handle) {
        if (!native_handle) return;
        
        SDL_Renderer* renderer = static_cast<SDL_Renderer*>(native_handle);
        size_t renderer_size = sdl2_renderer_get_size(renderer);
        trace_log("Cleaning up SDL2 renderer at %p (size: %zu bytes)\n", renderer, renderer_size);
        
        // Update stats for renderer cleanup
        stats_update_graphics_surface_freed(renderer_size);
        
        SDL_DestroyRenderer(renderer);
    }
    
    // SDL2 texture cleanup function
    void sdl2_texture_cleanup(void* native_handle) {
        if (!native_handle) return;
        
        SDL_Texture* texture = static_cast<SDL_Texture*>(native_handle);
        size_t texture_size = sdl2_texture_get_size(texture);
        trace_log("Cleaning up SDL2 texture at %p (size: %zu bytes)\n", texture, texture_size);
        
        // Update stats for texture cleanup
        stats_update_graphics_image_freed(texture_size);
        
        SDL_DestroyTexture(texture);
    }
    
    // Get SDL2 window memory usage estimate
    size_t sdl2_window_get_size(void* native_handle) {
        if (!native_handle) return 0;
        
        SDL_Window* window = static_cast<SDL_Window*>(native_handle);
        
        // Get window dimensions
        int width, height;
        SDL_GetWindowSize(window, &width, &height);
        
        // Estimate window memory usage
        // Base window object + potential framebuffer for software rendering
        size_t estimated_size = SDL2SAMM_estimate_window_memory(width, height);
        
        trace_log("SDL2 window %p estimated size: %dx%d = %zu bytes\n",
                  window, width, height, estimated_size);
        
        return estimated_size;
    }
    
    // Get SDL2 renderer memory usage estimate
    size_t sdl2_renderer_get_size(void* native_handle) {
        if (!native_handle) return 0;
        
        SDL_Renderer* renderer = static_cast<SDL_Renderer*>(native_handle);
        
        // Get renderer info to estimate memory usage
        SDL_RendererInfo info;
        if (SDL_GetRendererInfo(renderer, &info) != 0) {
            // Fallback estimate if we can't get info
            return 64 * 1024; // 64KB base estimate
        }
        
        // Get output size from renderer
        int width, height;
        if (SDL_GetRendererOutputSize(renderer, &width, &height) != 0) {
            // Fallback to reasonable default if we can't get size
            width = 640;
            height = 480;
        }
        
        size_t estimated_size = SDL2SAMM_estimate_renderer_memory(width, height, info.flags);
        
        trace_log("SDL2 renderer %p estimated size: %dx%d (flags: 0x%X) = %zu bytes\n",
                  renderer, width, height, info.flags, estimated_size);
        
        return estimated_size;
    }
    
    // Get SDL2 texture memory usage estimate
    size_t sdl2_texture_get_size(void* native_handle) {
        if (!native_handle) return 0;
        
        SDL_Texture* texture = static_cast<SDL_Texture*>(native_handle);
        
        // Query texture format and dimensions
        uint32_t format;
        int access, width, height;
        if (SDL_QueryTexture(texture, &format, &access, &width, &height) != 0) {
            // Fallback estimate if query fails
            return 4 * 1024; // 4KB base estimate
        }
        
        size_t estimated_size = SDL2SAMM_estimate_texture_memory(width, height, format);
        
        trace_log("SDL2 texture %p estimated size: %dx%d (format: 0x%X) = %zu bytes\n",
                  texture, width, height, format, estimated_size);
        
        return estimated_size;
    }
    
    // Get SDL2 window dimensions
    int sdl2_window_get_dimensions(void* native_handle, int* width, int* height) {
        if (!native_handle || !width || !height) return 0;
        
        SDL_Window* window = static_cast<SDL_Window*>(native_handle);
        SDL_GetWindowSize(window, width, height);
        
        return 1;
    }
    
    // Get SDL2 texture dimensions
    int sdl2_texture_get_dimensions(void* native_handle, int* width, int* height) {
        if (!native_handle || !width || !height) return 0;
        
        SDL_Texture* texture = static_cast<SDL_Texture*>(native_handle);
        
        uint32_t format;
        int access;
        if (SDL_QueryTexture(texture, &format, &access, width, height) != 0) {
            return 0;
        }
        
        return 1;
    }
}

// =============================================================================
// SDL2 BACKEND REGISTRATION
// =============================================================================

extern "C" int SDL2SAMM_register_backend(void) {
    if (g_sdl2_backend_registered) {
        return 1; // Already registered
    }
    
    GraphicsBackend sdl2_backend = {};
    sdl2_backend.backend_name = "sdl2";
    sdl2_backend.surface_cleanup = sdl2_window_cleanup;    // Windows are treated as surfaces
    sdl2_backend.image_cleanup = sdl2_texture_cleanup;     // Textures are treated as images
    sdl2_backend.surface_get_size = sdl2_window_get_size;
    sdl2_backend.image_get_size = sdl2_texture_get_size;
    sdl2_backend.surface_get_dimensions = sdl2_window_get_dimensions;
    sdl2_backend.image_get_dimensions = sdl2_texture_get_dimensions;
    
    if (SAMM_register_graphics_backend(&sdl2_backend)) {
        g_sdl2_backend_registered = true;
        trace_log("SDL2 backend registered successfully\n");
        return 1;
    } else {
        trace_log("Failed to register SDL2 backend\n");
        return 0;
    }
}

extern "C" void SDL2SAMM_unregister_backend(void) {
    if (!g_sdl2_backend_registered) {
        return;
    }
    
    if (SAMM_unregister_graphics_backend("sdl2")) {
        g_sdl2_backend_registered = false;
        trace_log("SDL2 backend unregistered successfully\n");
    } else {
        trace_log("Failed to unregister SDL2 backend\n");
    }
}

extern "C" int SDL2SAMM_is_backend_registered(void) {
    return g_sdl2_backend_registered ? 1 : 0;
}

extern "C" void SDL2SAMM_set_trace_enabled(int enabled) {
    g_sdl2_trace_enabled = (enabled != 0);
}

// =============================================================================
// SDL2 WINDOW TRACKING
// =============================================================================

extern "C" uint64_t SDL2SAMM_track_window(SDL_Window* window, int width, int height) {
    if (!window || !g_sdl2_backend_registered) {
        return 0;
    }
    
    // Estimate memory usage
    size_t memory_size = SDL2SAMM_estimate_window_memory(width, height);
    
    // Create GraphicsSurface (windows are treated as surfaces)
    GraphicsSurface* graphics_surface = new GraphicsSurface();
    graphics_surface->native_handle = window;
    graphics_surface->cleanup_func = sdl2_window_cleanup;
    graphics_surface->get_size_func = sdl2_window_get_size;
    graphics_surface->estimated_memory = memory_size;
    graphics_surface->backend_name = "sdl2";
    graphics_surface->handle_id = SAMM_generate_graphics_handle();
    graphics_surface->width = width;
    graphics_surface->height = height;
    graphics_surface->format = SDL_GetWindowPixelFormat(window);
    graphics_surface->user_data = nullptr;
    
    if (SAMM_track_graphics_surface(graphics_surface)) {
        trace_log("Tracked SDL2 window %p as handle %llu (%dx%d, %zu bytes)\n",
                  window, graphics_surface->handle_id, width, height, memory_size);
        return graphics_surface->handle_id;
    } else {
        trace_log("Failed to track SDL2 window %p\n", window);
        delete graphics_surface;
        return 0;
    }
}

extern "C" SDL_Window* SDL2SAMM_get_window(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "sdl2") != 0) {
        return nullptr;
    }
    
    return static_cast<SDL_Window*>(surface->native_handle);
}

extern "C" int SDL2SAMM_untrack_window(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "sdl2") != 0) {
        return 0;
    }
    
    trace_log("Untracking SDL2 window handle %llu\n", handle_id);
    return SAMM_untrack_graphics_surface(handle_id);
}

// =============================================================================
// SDL2 RENDERER TRACKING
// =============================================================================

extern "C" uint64_t SDL2SAMM_track_renderer(SDL_Renderer* renderer, int window_width, int window_height) {
    if (!renderer || !g_sdl2_backend_registered) {
        return 0;
    }
    
    // Get renderer flags for memory estimation
    SDL_RendererInfo info;
    uint32_t flags = 0;
    if (SDL_GetRendererInfo(renderer, &info) == 0) {
        flags = info.flags;
    }
    
    // Estimate memory usage
    size_t memory_size = SDL2SAMM_estimate_renderer_memory(window_width, window_height, flags);
    
    // Create GraphicsSurface (renderers are treated as surfaces)
    GraphicsSurface* graphics_surface = new GraphicsSurface();
    graphics_surface->native_handle = renderer;
    graphics_surface->cleanup_func = sdl2_renderer_cleanup;
    graphics_surface->get_size_func = sdl2_renderer_get_size;
    graphics_surface->estimated_memory = memory_size;
    graphics_surface->backend_name = "sdl2";
    graphics_surface->handle_id = SAMM_generate_graphics_handle();
    graphics_surface->width = window_width;
    graphics_surface->height = window_height;
    graphics_surface->format = flags;
    graphics_surface->user_data = nullptr;
    
    if (SAMM_track_graphics_surface(graphics_surface)) {
        trace_log("Tracked SDL2 renderer %p as handle %llu (%dx%d, flags: 0x%X, %zu bytes)\n",
                  renderer, graphics_surface->handle_id, window_width, window_height, flags, memory_size);
        return graphics_surface->handle_id;
    } else {
        trace_log("Failed to track SDL2 renderer %p\n", renderer);
        delete graphics_surface;
        return 0;
    }
}

extern "C" SDL_Renderer* SDL2SAMM_get_renderer(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "sdl2") != 0) {
        return nullptr;
    }
    
    return static_cast<SDL_Renderer*>(surface->native_handle);
}

extern "C" int SDL2SAMM_untrack_renderer(uint64_t handle_id) {
    GraphicsSurface* surface = SAMM_get_graphics_surface(handle_id);
    if (!surface || strcmp(surface->backend_name, "sdl2") != 0) {
        return 0;
    }
    
    trace_log("Untracking SDL2 renderer handle %llu\n", handle_id);
    return SAMM_untrack_graphics_surface(handle_id);
}

// =============================================================================
// SDL2 TEXTURE TRACKING
// =============================================================================

extern "C" uint64_t SDL2SAMM_track_texture(SDL_Texture* texture, int width, int height, uint32_t format) {
    if (!texture || !g_sdl2_backend_registered) {
        return 0;
    }
    
    // Estimate memory usage
    size_t memory_size = SDL2SAMM_estimate_texture_memory(width, height, format);
    
    // Create GraphicsImage (textures are treated as images)
    GraphicsImage* graphics_image = new GraphicsImage();
    graphics_image->native_handle = texture;
    graphics_image->cleanup_func = sdl2_texture_cleanup;
    graphics_image->get_size_func = sdl2_texture_get_size;
    graphics_image->estimated_memory = memory_size;
    graphics_image->backend_name = "sdl2";
    graphics_image->handle_id = SAMM_generate_graphics_handle();
    graphics_image->width = width;
    graphics_image->height = height;
    graphics_image->format = format;
    graphics_image->user_data = nullptr;
    
    if (SAMM_track_graphics_image(graphics_image)) {
        trace_log("Tracked SDL2 texture %p as handle %llu (%dx%d, format: 0x%X, %zu bytes)\n",
                  texture, graphics_image->handle_id, width, height, format, memory_size);
        return graphics_image->handle_id;
    } else {
        trace_log("Failed to track SDL2 texture %p\n", texture);
        delete graphics_image;
        return 0;
    }
}

extern "C" SDL_Texture* SDL2SAMM_get_texture(uint64_t handle_id) {
    GraphicsImage* image = SAMM_get_graphics_image(handle_id);
    if (!image || strcmp(image->backend_name, "sdl2") != 0) {
        return nullptr;
    }
    
    return static_cast<SDL_Texture*>(image->native_handle);
}

extern "C" int SDL2SAMM_untrack_texture(uint64_t handle_id) {
    GraphicsImage* image = SAMM_get_graphics_image(handle_id);
    if (!image || strcmp(image->backend_name, "sdl2") != 0) {
        return 0;
    }
    
    trace_log("Untracking SDL2 texture handle %llu\n", handle_id);
    return SAMM_untrack_graphics_image(handle_id);
}

// =============================================================================
// SDL2 RESOURCE CREATION WITH AUTOMATIC TRACKING
// =============================================================================

extern "C" uint64_t SDL2SAMM_create_window(const char* title, int x, int y, int width, int height, uint32_t flags) {
    if (!g_sdl2_backend_registered) {
        trace_log("Cannot create window - SDL2 backend not registered\n");
        return 0;
    }
    
    // Create SDL2 window
    SDL_Window* window = SDL_CreateWindow(title, x, y, width, height, flags);
    if (!window) {
        trace_log("Failed to create SDL2 window %dx%d: %s\n", width, height, SDL_GetError());
        return 0;
    }
    
    trace_log("Created SDL2 window %p (%dx%d)\n", window, width, height);
    
    // Track in SAMM graphics system
    uint64_t handle_id = SDL2SAMM_track_window(window, width, height);
    if (handle_id == 0) {
        trace_log("Failed to track SDL2 window %p\n", window);
        SDL_DestroyWindow(window);
        return 0;
    }
    
    return handle_id;
}

extern "C" uint64_t SDL2SAMM_create_renderer(uint64_t window_handle_id, int index, uint32_t flags) {
    if (!g_sdl2_backend_registered) {
        trace_log("Cannot create renderer - SDL2 backend not registered\n");
        return 0;
    }
    
    // Get window from handle
    SDL_Window* window = SDL2SAMM_get_window(window_handle_id);
    if (!window) {
        trace_log("Cannot create renderer - invalid window handle %llu\n", window_handle_id);
        return 0;
    }
    
    // Get window dimensions for memory estimation
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    
    // Create SDL2 renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, index, flags);
    if (!renderer) {
        trace_log("Failed to create SDL2 renderer: %s\n", SDL_GetError());
        return 0;
    }
    
    trace_log("Created SDL2 renderer %p (window %dx%d, flags: 0x%X)\n", renderer, width, height, flags);
    
    // Track in SAMM graphics system
    uint64_t handle_id = SDL2SAMM_track_renderer(renderer, width, height);
    if (handle_id == 0) {
        trace_log("Failed to track SDL2 renderer %p\n", renderer);
        SDL_DestroyRenderer(renderer);
        return 0;
    }
    
    return handle_id;
}

extern "C" uint64_t SDL2SAMM_create_texture(uint64_t renderer_handle_id, uint32_t format, int access, int width, int height) {
    if (!g_sdl2_backend_registered) {
        trace_log("Cannot create texture - SDL2 backend not registered\n");
        return 0;
    }
    
    // Get renderer from handle
    SDL_Renderer* renderer = SDL2SAMM_get_renderer(renderer_handle_id);
    if (!renderer) {
        trace_log("Cannot create texture - invalid renderer handle %llu\n", renderer_handle_id);
        return 0;
    }
    
    // Create SDL2 texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, format, access, width, height);
    if (!texture) {
        trace_log("Failed to create SDL2 texture %dx%d: %s\n", width, height, SDL_GetError());
        return 0;
    }
    
    trace_log("Created SDL2 texture %p (%dx%d, format: 0x%X)\n", texture, width, height, format);
    
    // Track in SAMM graphics system
    uint64_t handle_id = SDL2SAMM_track_texture(texture, width, height, format);
    if (handle_id == 0) {
        trace_log("Failed to track SDL2 texture %p\n", texture);
        SDL_DestroyTexture(texture);
        return 0;
    }
    
    return handle_id;
}

extern "C" uint64_t SDL2SAMM_load_bmp(const char* filename) {
    if (!g_sdl2_backend_registered) {
        trace_log("Cannot load BMP - SDL2 backend not registered\n");
        return 0;
    }
    
    // Load BMP as SDL surface
    SDL_Surface* surface = SDL_LoadBMP(filename);
    if (!surface) {
        trace_log("Failed to load BMP from %s: %s\n", filename, SDL_GetError());
        return 0;
    }
    
    int width = surface->w;
    int height = surface->h;
    uint32_t format = surface->format->format;
    
    trace_log("Loaded BMP %s as SDL surface %p (%dx%d, format: 0x%X)\n", filename, surface, width, height, format);
    
    // For now, we'll treat loaded BMPs as textures/images
    // In a full implementation, you might want separate surface tracking
    uint64_t handle_id = SDL2SAMM_track_texture(reinterpret_cast<SDL_Texture*>(surface), width, height, format);
    if (handle_id == 0) {
        trace_log("Failed to track SDL2 surface %p\n", surface);
        SDL_FreeSurface(surface);
        return 0;
    }
    
    return handle_id;
}

// =============================================================================
// MEMORY ESTIMATION UTILITIES
// =============================================================================

extern "C" size_t SDL2SAMM_estimate_window_memory(int width, int height) {
    // Base window structure + potential software framebuffer
    size_t base_size = 1024; // Base window object overhead
    
    // For software rendering, may need framebuffer
    // Assume RGBA32 format for worst case
    size_t framebuffer_size = static_cast<size_t>(width * height * 4);
    
    // Conservative estimate: base + potential framebuffer
    return base_size + framebuffer_size;
}

extern "C" size_t SDL2SAMM_estimate_renderer_memory(int window_width, int window_height, uint32_t flags) {
    size_t base_size = 4096; // Base renderer object overhead
    
    // Hardware accelerated renderers typically use less system memory
    if (flags & SDL_RENDERER_ACCELERATED) {
        // Minimal system memory, GPU handles most buffers
        return base_size + (window_width * window_height * 1); // 1 byte per pixel estimate
    } else {
        // Software renderer needs full framebuffers
        size_t framebuffer_size = static_cast<size_t>(window_width * window_height * 4);
        
        // May need double buffering
        if (flags & SDL_RENDERER_PRESENTVSYNC) {
            framebuffer_size *= 2;
        }
        
        return base_size + framebuffer_size;
    }
}

extern "C" size_t SDL2SAMM_estimate_texture_memory(int width, int height, uint32_t format) {
    int bytes_per_pixel = SDL2SAMM_get_bytes_per_pixel(format);
    return static_cast<size_t>(width * height * bytes_per_pixel);
}

extern "C" int SDL2SAMM_get_bytes_per_pixel(uint32_t format) {
    switch (format) {
        case SDL_PIXELFORMAT_RGB332:
        case SDL_PIXELFORMAT_INDEX8:
            return 1;
        case SDL_PIXELFORMAT_RGB555:
        case SDL_PIXELFORMAT_BGR555:
        case SDL_PIXELFORMAT_RGB565:
        case SDL_PIXELFORMAT_BGR565:
            return 2;
        case SDL_PIXELFORMAT_RGB24:
        case SDL_PIXELFORMAT_BGR24:
            return 3;
        case SDL_PIXELFORMAT_RGBA8888:
        case SDL_PIXELFORMAT_RGBX8888:
        case SDL_PIXELFORMAT_ARGB8888:
        case SDL_PIXELFORMAT_XRGB8888:
        case SDL_PIXELFORMAT_BGRA8888:
        case SDL_PIXELFORMAT_BGRX8888:
        case SDL_PIXELFORMAT_ABGR8888:
        case SDL_PIXELFORMAT_XBGR8888:
            return 4;
        default:
            return 4; // Default to 32-bit
    }
}

// =============================================================================
// SCOPE MANAGEMENT INTEGRATION
// =============================================================================

extern "C" int SDL2SAMM_move_window_to_scope(uint64_t handle_id, int target_scope_offset) {
    return SAMM_move_graphics_surface_to_scope(handle_id, target_scope_offset);
}

extern "C" int SDL2SAMM_move_renderer_to_scope(uint64_t handle_id, int target_scope_offset) {
    return SAMM_move_graphics_surface_to_scope(handle_id, target_scope_offset);
}

extern "C" int SDL2SAMM_move_texture_to_scope(uint64_t handle_id, int target_scope_offset) {
    return SAMM_move_graphics_image_to_scope(handle_id, target_scope_offset);
}

// =============================================================================
// INTEGRATION AND INITIALIZATION
// =============================================================================

extern "C" void SDL2SAMM_initialize(void) {
    trace_log("Initializing SDL2 SAMM backend\n");
    
    // Graphics system should already be initialized by HeapManager
    if (!SAMM_is_graphics_initialized()) {
        trace_log("Warning: Graphics system not initialized\n");
        return;
    }
    
    // Register SDL2 backend
    if (!SDL2SAMM_register_backend()) {
        trace_log("Failed to register SDL2 backend\n");
        return;
    }
    
    // Enable graphics tracing if heap tracing is enabled
    SAMM_set_graphics_trace_enabled(1);
    
    trace_log("SDL2 SAMM backend initialization complete\n");
}

extern "C" void SDL2SAMM_shutdown(void) {
    trace_log("Shutting down SDL2 SAMM backend\n");
    
    // Unregister backend
    SDL2SAMM_unregister_backend();
    
    trace_log("SDL2 SAMM backend shutdown complete\n");
}

extern "C" int SDL2SAMM_is_enabled(void) {
    return (SAMM_is_graphics_initialized() && g_sdl2_backend_registered) ? 1 : 0;
}

// =============================================================================
// LEGACY COMPATIBILITY FUNCTIONS
// =============================================================================

// These functions provide compatibility with the old interface
extern "C" void SDL2SAMM_enterScope(void) {
    // No-op - scope tracking is now handled automatically by HeapManager
    trace_log("SDL2SAMM_enterScope called (no-op with new system)\n");
}

extern "C" void SDL2SAMM_exitScope(void) {
    // No-op - scope cleanup is now handled automatically by HeapManager
    trace_log("SDL2SAMM_exitScope called (no-op with new system)\n");
}

extern "C" void SDL2SAMM_trackWindow(uint64_t handle_id) {
    // No-op - tracking is done automatically during creation
    trace_log("SDL2SAMM_trackWindow called for handle %llu (no-op with new system)\n", handle_id);
}

extern "C" void SDL2SAMM_trackRenderer(uint64_t handle_id) {
    // No-op - tracking is done automatically during creation
    trace_log("SDL2SAMM_trackRenderer called for handle %llu (no-op with new system)\n", handle_id);
}
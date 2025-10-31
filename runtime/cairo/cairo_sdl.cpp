#include "cairo_runtime.h"
#include "cairo_sdl.h"
#include "../SDL2/sdl2_runtime.h"
#include "../SDL2/sdl2_samm_backend.h"
#include "cairo_samm_backend.h"
#include <cairo.h>
#include <SDL.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

// Error message buffer for this module
static char last_error[512] = {0};

// Internal helper to set error message
static void setLastError(const char* message) {
    strncpy(last_error, message, sizeof(last_error) - 1);
    last_error[sizeof(last_error) - 1] = '\0';
}

// Internal helper to get last error
static const char* getLastError() {
    return last_error;
}

// =============================================================================
// PIXEL FORMAT CONVERSION UTILITIES
// =============================================================================

/**
 * Convert Cairo ARGB32 surface data to SDL2 RGBA format
 * Cairo uses premultiplied ARGB (native byte order)
 * SDL2 expects RGBA format
 */
static void convert_cairo_to_sdl_pixels(const unsigned char* cairo_data, 
                                      unsigned char* sdl_data,
                                      int width, int height, int cairo_stride) {
    for (int y = 0; y < height; y++) {
        const uint32_t* cairo_row = (const uint32_t*)(cairo_data + y * cairo_stride);
        uint32_t* sdl_row = (uint32_t*)(sdl_data + y * width * 4);
        
        for (int x = 0; x < width; x++) {
            uint32_t pixel = cairo_row[x];
            
            // Cairo ARGB32 is premultiplied and in native byte order
            // Extract components (assuming little-endian)
            uint8_t b = (pixel >> 0) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t a = (pixel >> 24) & 0xFF;
            
            // Unpremultiply if alpha > 0
            if (a > 0) {
                r = (r * 255) / a;
                g = (g * 255) / a;
                b = (b * 255) / a;
            }
            
            // Pack as RGBA for SDL2
            sdl_row[x] = (r << 0) | (g << 8) | (b << 16) | (a << 24);
        }
    }
}

// =============================================================================
// CAIRO TO SDL2 TEXTURE CONVERSION
// =============================================================================

int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id) {
    // Clear any previous error
    setLastError("");
    
    // Get the Cairo surface from SAMM
    cairo_surface_t* cairo_surface = CairoSAMM_get_surface(surface_handle);
    if (!cairo_surface) {
        setLastError("Invalid Cairo surface handle");
        return 0;
    }
    
    // Get the SDL2 renderer from SAMM
    SDL_Renderer* renderer = SDL2SAMM_get_renderer(sdl_renderer_id);
    if (!renderer) {
        setLastError("Invalid SDL2 renderer handle");
        return 0;
    }
    
    // Ensure the Cairo surface is an image surface
    if (cairo_surface_get_type(cairo_surface) != CAIRO_SURFACE_TYPE_IMAGE) {
        setLastError("Cairo surface must be an image surface for SDL2 conversion");
        return 0;
    }
    
    // Get surface properties
    int width = cairo_image_surface_get_width(cairo_surface);
    int height = cairo_image_surface_get_height(cairo_surface);
    cairo_format_t format = cairo_image_surface_get_format(cairo_surface);
    
    if (format != CAIRO_FORMAT_ARGB32) {
        setLastError("Only CAIRO_FORMAT_ARGB32 surfaces are supported for SDL2 conversion");
        return 0;
    }
    
    // Create SDL2 texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, 
                                           SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
        snprintf(last_error, sizeof(last_error), "Failed to create SDL2 texture: %s", SDL_GetError());
        return 0;
    }
    
    // Enable alpha blending on the texture
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    
    // Get Cairo surface data
    cairo_surface_flush(cairo_surface);
    unsigned char* cairo_data = cairo_image_surface_get_data(cairo_surface);
    int cairo_stride = cairo_image_surface_get_stride(cairo_surface);
    
    // Allocate temporary buffer for converted pixels
    unsigned char* sdl_data = (unsigned char*)malloc(width * height * 4);
    if (!sdl_data) {
        SDL_DestroyTexture(texture);
        setLastError("Failed to allocate memory for pixel conversion");
        return 0;
    }
    
    // Convert pixel format
    convert_cairo_to_sdl_pixels(cairo_data, sdl_data, width, height, cairo_stride);
    
    // Update SDL2 texture with converted data
    int result = SDL_UpdateTexture(texture, NULL, sdl_data, width * 4);
    free(sdl_data);
    
    if (result != 0) {
        SDL_DestroyTexture(texture);
        snprintf(last_error, sizeof(last_error), "Failed to update SDL2 texture: %s", SDL_GetError());
        return 0;
    }
    
    // Track the texture in SDL2 SAMM backend
    uint64_t texture_handle = SDL2SAMM_track_texture(texture, width, height, SDL_PIXELFORMAT_RGBA32);
    if (texture_handle == 0) {
        SDL_DestroyTexture(texture);
        setLastError("Failed to track SDL2 texture in SAMM");
        return 0;
    }
    
    return (int64_t)texture_handle;
}

// =============================================================================
// CAIRO SURFACE DISPLAY IN SDL2 WINDOW
// =============================================================================

void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id) {
    // Clear any previous error
    setLastError("");
    
    // Get the Cairo surface from SAMM
    cairo_surface_t* cairo_surface = CairoSAMM_get_surface(surface_handle);
    if (!cairo_surface) {
        setLastError("Invalid Cairo surface handle");
        return;
    }
    
    // Get the SDL2 window from SAMM
    SDL_Window* window = SDL2SAMM_get_window(sdl_window_id);
    if (!window) {
        setLastError("Invalid SDL2 window handle");
        return;
    }
    
    // Get or create a renderer for this window
    SDL_Renderer* renderer = SDL_GetRenderer(window);
    uint64_t renderer_handle = 0;
    
    if (!renderer) {
        // Create a new renderer for this window
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
        if (!renderer) {
            // Try software renderer as fallback
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
            if (!renderer) {
                snprintf(last_error, sizeof(last_error), "Failed to create SDL2 renderer: %s", SDL_GetError());
                return;
            }
        }
        
        // Track the renderer in SAMM
        int window_width, window_height;
        SDL_GetWindowSize(window, &window_width, &window_height);
        renderer_handle = SDL2SAMM_track_renderer(renderer, window_width, window_height);
        if (renderer_handle == 0) {
            SDL_DestroyRenderer(renderer);
            setLastError("Failed to track renderer in SAMM");
            return;
        }
    }
    
    // Convert the Cairo surface to an SDL2 texture
    // We need to pass a valid renderer handle, so if we don't have one, we need to find it
    uint64_t actual_renderer_handle = renderer_handle;
    if (actual_renderer_handle == 0) {
        // Try to find existing renderer handle - this is a fallback that shouldn't normally happen
        setLastError("Could not determine renderer handle for texture conversion");
        return;
    }
    
    int64_t texture_id = CAIRO_TO_SDL_TEXTURE(surface_handle, (int64_t)actual_renderer_handle);
    if (texture_id == 0) {
        // Error message already set by CAIRO_TO_SDL_TEXTURE
        return;
    }
    
    // Get the texture for rendering
    SDL_Texture* texture = SDL2SAMM_get_texture(texture_id);
    if (!texture) {
        setLastError("Failed to retrieve converted texture");
        return;
    }
    
    // Clear the renderer and draw the texture
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
    
    // Clean up the temporary texture
    SDL2SAMM_untrack_texture(texture_id);
}

// =============================================================================
// CAIRO-OPTIMIZED SDL2 WINDOW CREATION
// =============================================================================

int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height) {
    printf("DEBUG: CAIRO_CREATE_SDL_WINDOW called with title=%p, width=%lld, height=%lld\n", title, width, height);
    
    // Clear any previous error
    setLastError("");
    printf("DEBUG: Error cleared\n");
    
    // Convert BCPL string to C string
    char c_title[256];
    if (title && title[0] > 0) {
        printf("DEBUG: Converting BCPL string, length=%d\n", (int)title[0]);
        int len = (int)title[0];
        if (len > 255) len = 255;
        
        // Copy characters from BCPL string (assuming 1-byte chars in vector)
        for (int i = 0; i < len; i++) {
            c_title[i] = (char)(title[i + 1] & 0xFF);
        }
        c_title[len] = '\0';
        printf("DEBUG: Converted title: '%s'\n", c_title);
    } else {
        printf("DEBUG: Using default title\n");
        strcpy(c_title, "Cairo Window");
    }
    
    // Validate dimensions
    printf("DEBUG: Validating dimensions: %lld x %lld\n", width, height);
    if (width <= 0 || width > 8192) {
        printf("DEBUG: Invalid width: %lld\n", width);
        setLastError("Invalid window width (must be 1-8192)");
        return 0;
    }
    if (height <= 0 || height > 8192) {
        printf("DEBUG: Invalid height: %lld\n", height);
        setLastError("Invalid window height (must be 1-8192)");
        return 0;
    }
    printf("DEBUG: Dimensions validated successfully\n");
    
    // Check if SDL2 is initialized
    printf("DEBUG: Checking SDL2 initialization\n");
    if (SDL_WasInit(SDL_INIT_VIDEO) == 0) {
        printf("DEBUG: SDL2 not initialized, initializing now\n");
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            printf("DEBUG: SDL2 initialization failed: %s\n", SDL_GetError());
            snprintf(last_error, sizeof(last_error), "SDL2 initialization failed: %s", SDL_GetError());
            return 0;
        }
        printf("DEBUG: SDL2 initialized successfully\n");
    } else {
        printf("DEBUG: SDL2 already initialized\n");
    }
    
    // Create SDL2 window optimized for Cairo rendering
    // Use SDL_WINDOW_SHOWN and position it in the center
    uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE;
    printf("DEBUG: Creating SDL2 window with flags=0x%x\n", flags);
    
    printf("DEBUG: Calling SDL2SAMM_create_window\n");
    uint64_t window_handle = SDL2SAMM_create_window(c_title, 
                                                   SDL_WINDOWPOS_CENTERED, 
                                                   SDL_WINDOWPOS_CENTERED,
                                                   (int)width, (int)height, flags);
    
    printf("DEBUG: SDL2SAMM_create_window returned handle=%llu\n", window_handle);
    
    if (window_handle == 0) {
        printf("DEBUG: Window creation failed, SDL error: %s\n", SDL_GetError());
        snprintf(last_error, sizeof(last_error), "Failed to create SDL2 window: %s", SDL_GetError());
        return 0;
    }
    
    printf("DEBUG: CAIRO_CREATE_SDL_WINDOW completed successfully, returning %lld\n", (int64_t)window_handle);
    return (int64_t)window_handle;
}

// =============================================================================
// CAIRO SURFACE UPDATE IN SDL2 WINDOW
// =============================================================================

void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id) {
    // This is essentially the same as CAIRO_DISPLAY_SDL for now
    // In the future, this could be optimized for animation by reusing textures
    CAIRO_DISPLAY_SDL(surface_handle, sdl_window_id);
}

// =============================================================================
// ERROR HANDLING INTEGRATION
// =============================================================================

/**
 * Get last error from the Cairo-SDL2 bridge
 * This can be called from the main Cairo runtime error system
 */
const char* CairoSDL_get_last_error() {
    return getLastError();
}

/**
 * Clear any pending Cairo-SDL2 bridge error
 */
void CairoSDL_clear_error() {
    setLastError("");
}

// =============================================================================
// MODULE INITIALIZATION
// =============================================================================

/**
 * Initialize the Cairo-SDL2 bridge
 * This should be called during runtime startup
 */
void CairoSDL_initialize() {
    setLastError("");
    // Any additional initialization can go here
}

/**
 * Shutdown the Cairo-SDL2 bridge  
 * This should be called during runtime shutdown
 */
void CairoSDL_shutdown() {
    setLastError("");
    // Any cleanup can go here
}
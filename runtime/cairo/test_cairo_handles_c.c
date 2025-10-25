#include <stdio.h>
#include <stdint.h>

// Forward declarations for Cairo runtime functions
typedef uint64_t CairoSurfaceHandle;

#ifdef __cplusplus
extern "C" {
#endif

extern CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height);
extern void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
extern void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
extern void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
extern void CAIRO_SAVE_PNG_C(CairoSurfaceHandle surface_handle, const char* filename);
extern int64_t CAIRO_IS_AVAILABLE(void);

#ifdef __cplusplus
}
#endif

int main() {
    printf("=== C Handle Test ===\n");
    
    // Check if Cairo is available
    int64_t available = CAIRO_IS_AVAILABLE();
    printf("Cairo available: %lld\n", available);
    
    if (available == 0) {
        printf("Cairo not available\n");
        return 1;
    }
    
    // Create surface
    printf("Creating surface...\n");
    CairoSurfaceHandle surface = CAIRO_CREATE_SURFACE(200, 150);
    printf("Surface handle: 0x%llX\n", surface);
    
    if (surface == 0) {
        printf("Failed to create surface\n");
        return 1;
    }
    
    // Clear surface
    printf("Clearing surface...\n");
    CAIRO_CLEAR(surface, 0xFFFFFFFF);
    
    // Set color to red
    printf("Setting red color...\n");
    CAIRO_SET_COLOR(surface, 0xFF0000FF);
    
    // Draw rectangle
    printf("Drawing rectangle...\n");
    CAIRO_FILL_RECT(surface, 10.0, 10.0, 80.0, 60.0);
    
    // Save PNG
    printf("Saving PNG...\n");
    CAIRO_SAVE_PNG_C(surface, "c_handle_test.png");
    
    printf("Test completed - check c_handle_test.png\n");
    return 0;
}
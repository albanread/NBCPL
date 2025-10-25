#include <stdio.h>
#include <stdint.h>
#include <math.h>

// Forward declarations for Cairo runtime functions
typedef uint64_t CairoSurfaceHandle;

#ifdef __cplusplus
extern "C" {
#endif

extern CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height);
extern void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
extern void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
extern void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
extern void CAIRO_FILL_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);
extern void CAIRO_DRAW_LINE(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2);
extern void CAIRO_SET_LINE_WIDTH(CairoSurfaceHandle surface_handle, double width);
extern void CAIRO_SAVE_PNG_C(CairoSurfaceHandle surface_handle, const char* filename);
extern int64_t CAIRO_IS_AVAILABLE(void);

#ifdef __cplusplus
}
#endif

int main() {
    printf("=== Large C Handle Test ===\n");
    
    // Check if Cairo is available
    int64_t available = CAIRO_IS_AVAILABLE();
    printf("Cairo available: %lld\n", available);
    
    if (available == 0) {
        printf("Cairo not available\n");
        return 1;
    }
    
    // Create a larger surface
    printf("Creating 500x400 surface...\n");
    CairoSurfaceHandle surface = CAIRO_CREATE_SURFACE(500, 400);
    printf("Surface handle: 0x%llX\n", surface);
    
    if (surface == 0) {
        printf("Failed to create surface\n");
        return 1;
    }
    
    // Clear surface to light gray
    printf("Clearing surface to light gray...\n");
    CAIRO_CLEAR(surface, 0xF0F0F0FF);
    
    // Draw multiple colored rectangles
    printf("Drawing colored rectangles...\n");
    
    // Red rectangle
    CAIRO_SET_COLOR(surface, 0xFF0000FF);
    CAIRO_FILL_RECT(surface, 50.0, 50.0, 100.0, 80.0);
    
    // Green rectangle
    CAIRO_SET_COLOR(surface, 0x00FF00FF);
    CAIRO_FILL_RECT(surface, 200.0, 50.0, 100.0, 80.0);
    
    // Blue rectangle
    CAIRO_SET_COLOR(surface, 0x0000FFFF);
    CAIRO_FILL_RECT(surface, 350.0, 50.0, 100.0, 80.0);
    
    // Draw multiple circles
    printf("Drawing colored circles...\n");
    
    // Yellow circle
    CAIRO_SET_COLOR(surface, 0xFFFF00FF);
    CAIRO_FILL_CIRCLE(surface, 100.0, 200.0, 40.0);
    
    // Cyan circle
    CAIRO_SET_COLOR(surface, 0x00FFFFFF);
    CAIRO_FILL_CIRCLE(surface, 250.0, 200.0, 40.0);
    
    // Magenta circle
    CAIRO_SET_COLOR(surface, 0xFF00FFFF);
    CAIRO_FILL_CIRCLE(surface, 400.0, 200.0, 40.0);
    
    // Draw lines with different widths
    printf("Drawing lines...\n");
    
    CAIRO_SET_COLOR(surface, 0x000000FF); // Black
    CAIRO_SET_LINE_WIDTH(surface, 2.0);
    CAIRO_DRAW_LINE(surface, 10.0, 300.0, 490.0, 300.0);
    
    CAIRO_SET_LINE_WIDTH(surface, 4.0);
    CAIRO_DRAW_LINE(surface, 10.0, 320.0, 490.0, 320.0);
    
    CAIRO_SET_LINE_WIDTH(surface, 6.0);
    CAIRO_DRAW_LINE(surface, 10.0, 340.0, 490.0, 340.0);
    
    // Draw a pattern of small rectangles
    printf("Drawing pattern...\n");
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 8; j++) {
            // Alternate colors
            uint32_t color = ((i + j) % 2 == 0) ? 0x808080FF : 0x404040FF;
            CAIRO_SET_COLOR(surface, color);
            CAIRO_FILL_RECT(surface, 20.0 + i * 45.0, 260.0 + j * 10.0, 40.0, 8.0);
        }
    }
    
    // Draw some diagonal lines to create a crosshatch pattern
    printf("Drawing crosshatch pattern...\n");
    CAIRO_SET_COLOR(surface, 0x800000FF); // Dark red
    CAIRO_SET_LINE_WIDTH(surface, 1.0);
    
    for (int i = 0; i < 20; i++) {
        double x = 25.0 + i * 20.0;
        CAIRO_DRAW_LINE(surface, x, 150.0, x + 50.0, 180.0);
        CAIRO_DRAW_LINE(surface, x + 50.0, 150.0, x, 180.0);
    }
    
    // Save PNG
    printf("Saving large PNG...\n");
    CAIRO_SAVE_PNG_C(surface, "c_handle_large_test.png");
    
    printf("Large test completed - check c_handle_large_test.png\n");
    printf("Expected: A 500x400 image with colorful rectangles, circles, lines and patterns\n");
    return 0;
}
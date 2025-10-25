#include <iostream>
#include <cairo.h>

// Minimal Cairo test without any BCPL runtime dependencies
// This tests the core Cairo functionality directly

int main() {
    std::cout << "=== Minimal Cairo Test ===" << std::endl;
    
    // Test 1: Check Cairo version
    std::cout << "Cairo version: " << cairo_version_string() << std::endl;
    
    // Test 2: Create a basic surface
    int width = 400;
    int height = 300;
    cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    
    if (cairo_surface_status(surface) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "❌ Failed to create surface: " << cairo_status_to_string(cairo_surface_status(surface)) << std::endl;
        return 1;
    }
    std::cout << "✅ Created " << width << "x" << height << " surface" << std::endl;
    
    // Test 3: Create context
    cairo_t* ctx = cairo_create(surface);
    if (cairo_status(ctx) != CAIRO_STATUS_SUCCESS) {
        std::cerr << "❌ Failed to create context: " << cairo_status_to_string(cairo_status(ctx)) << std::endl;
        cairo_surface_destroy(surface);
        return 1;
    }
    std::cout << "✅ Created Cairo context" << std::endl;
    
    // Test 4: Clear surface with white
    cairo_set_source_rgba(ctx, 1.0, 1.0, 1.0, 1.0);  // White
    cairo_paint(ctx);
    std::cout << "✅ Cleared surface to white" << std::endl;
    
    // Test 5: Draw a red rectangle
    cairo_set_source_rgba(ctx, 1.0, 0.0, 0.0, 1.0);  // Red
    cairo_rectangle(ctx, 50.0, 50.0, 100.0, 80.0);
    cairo_fill(ctx);
    std::cout << "✅ Drew red rectangle" << std::endl;
    
    // Test 6: Draw a blue circle
    cairo_set_source_rgba(ctx, 0.0, 0.0, 1.0, 1.0);  // Blue
    cairo_arc(ctx, 250.0, 150.0, 40.0, 0.0, 2.0 * 3.14159);
    cairo_fill(ctx);
    std::cout << "✅ Drew blue circle" << std::endl;
    
    // Test 7: Draw a green line
    cairo_set_source_rgba(ctx, 0.0, 1.0, 0.0, 1.0);  // Green
    cairo_set_line_width(ctx, 3.0);
    cairo_move_to(ctx, 100.0, 250.0);
    cairo_line_to(ctx, 350.0, 280.0);
    cairo_stroke(ctx);
    std::cout << "✅ Drew green line" << std::endl;
    
    // Test 8: Draw text
    cairo_set_source_rgba(ctx, 0.0, 0.0, 0.0, 1.0);  // Black
    cairo_select_font_face(ctx, "Arial", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
    cairo_set_font_size(ctx, 16.0);
    cairo_move_to(ctx, 50.0, 30.0);
    cairo_show_text(ctx, "Cairo Test - Basic Drawing");
    std::cout << "✅ Drew text" << std::endl;
    
    // Test 9: Save to PNG
    const char* filename = "cairo_minimal_test.png";
    cairo_surface_write_to_png(surface, filename);
    if (cairo_surface_status(surface) == CAIRO_STATUS_SUCCESS) {
        std::cout << "✅ Saved image to " << filename << std::endl;
    } else {
        std::cerr << "❌ Failed to save PNG: " << cairo_status_to_string(cairo_surface_status(surface)) << std::endl;
    }
    
    // Test 10: Check for any Cairo errors
    cairo_status_t ctx_status = cairo_status(ctx);
    cairo_status_t surface_status = cairo_surface_status(surface);
    
    if (ctx_status != CAIRO_STATUS_SUCCESS) {
        std::cerr << "❌ Context error: " << cairo_status_to_string(ctx_status) << std::endl;
    }
    if (surface_status != CAIRO_STATUS_SUCCESS) {
        std::cerr << "❌ Surface error: " << cairo_status_to_string(surface_status) << std::endl;
    }
    
    // Cleanup
    cairo_destroy(ctx);
    cairo_surface_destroy(surface);
    std::cout << "✅ Cleaned up resources" << std::endl;
    
    if (ctx_status == CAIRO_STATUS_SUCCESS && surface_status == CAIRO_STATUS_SUCCESS) {
        std::cout << "\n🎉 All Cairo tests passed! Image saved as " << filename << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ Some tests failed" << std::endl;
        return 1;
    }
}
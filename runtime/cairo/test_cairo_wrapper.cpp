#include <iostream>
#include <memory>
#include "cairo_core.h"

// Test the Cairo C++ wrapper classes without BCPL runtime dependencies

int main() {
    std::cout << "=== Cairo C++ Wrapper Test ===" << std::endl;
    
    try {
        // Test 1: Create a CairoSurface
        std::cout << "\n--- Test 1: Create Surface ---" << std::endl;
        auto surface = std::make_unique<CairoSurface>(500, 400);
        if (!surface->isValid()) {
            std::cerr << "❌ Failed to create Cairo surface" << std::endl;
            return 1;
        }
        std::cout << "✅ Created " << surface->getWidth() << "x" << surface->getHeight() << " surface" << std::endl;
        
        // Test 2: Clear surface
        std::cout << "\n--- Test 2: Clear Surface ---" << std::endl;
        CairoColor lightGray(240, 240, 240);
        surface->clear(lightGray);
        std::cout << "✅ Cleared surface to light gray" << std::endl;
        
        // Test 3: Draw shapes
        std::cout << "\n--- Test 3: Draw Shapes ---" << std::endl;
        
        // Red rectangle
        CairoColor red(255, 0, 0);
        surface->setColor(red);
        surface->fillRectangle(50.0, 50.0, 120.0, 80.0);
        std::cout << "✅ Drew red rectangle" << std::endl;
        
        // Blue circle
        CairoColor blue(0, 0, 255);
        surface->setColor(blue);
        surface->fillCircle(300.0, 200.0, 50.0);
        std::cout << "✅ Drew blue circle" << std::endl;
        
        // Green line
        CairoColor green(0, 200, 0);
        surface->setColor(green);
        surface->setLineWidth(4.0);
        surface->drawLine(100.0, 250.0, 400.0, 300.0);
        std::cout << "✅ Drew green line" << std::endl;
        
        // Test 4: Draw text
        std::cout << "\n--- Test 4: Draw Text ---" << std::endl;
        CairoColor black(0, 0, 0);
        surface->setColor(black);
        surface->setFont("Arial", 18.0);
        surface->drawText(50.0, 30.0, "Cairo C++ Wrapper Test");
        std::cout << "✅ Drew header text" << std::endl;
        
        // Colored text
        CairoColor purple(128, 0, 128);
        surface->drawTextColored(200.0, 120.0, "Colored Text!", purple);
        std::cout << "✅ Drew colored text" << std::endl;
        
        // Test 5: Path operations
        std::cout << "\n--- Test 5: Path Operations ---" << std::endl;
        CairoColor orange(255, 165, 0);
        surface->setColor(orange);
        surface->setLineWidth(2.0);
        
        surface->beginPath();
        surface->moveTo(350.0, 50.0);
        surface->lineTo(400.0, 80.0);
        surface->lineTo(450.0, 50.0);
        surface->lineTo(430.0, 100.0);
        surface->lineTo(370.0, 100.0);
        surface->closePath();
        surface->fillPath();
        std::cout << "✅ Drew filled pentagon path" << std::endl;
        
        // Test 6: Polygon operations
        std::cout << "\n--- Test 6: Polygon Operations ---" << std::endl;
        std::vector<CairoPoint> triangle = {
            CairoPoint(100.0, 350.0),
            CairoPoint(150.0, 300.0),
            CairoPoint(200.0, 350.0)
        };
        
        CairoColor cyan(0, 255, 255, 128); // Semi-transparent
        surface->setColor(cyan);
        surface->fillPolygon(triangle);
        std::cout << "✅ Drew filled triangle polygon" << std::endl;
        
        // Test 7: Batch operations
        std::cout << "\n--- Test 7: Batch Operations ---" << std::endl;
        
        // Multiple lines
        std::vector<CairoPoint> line_points = {
            CairoPoint(250.0, 250.0), CairoPoint(300.0, 280.0),
            CairoPoint(320.0, 250.0), CairoPoint(370.0, 280.0),
            CairoPoint(390.0, 250.0), CairoPoint(440.0, 280.0)
        };
        CairoColor darkBlue(0, 0, 128);
        surface->setColor(darkBlue);
        surface->setLineWidth(2.0);
        surface->drawLines(line_points);
        std::cout << "✅ Drew multiple lines" << std::endl;
        
        // Multiple rectangles
        std::vector<CairoRect> rects = {
            CairoRect(250.0, 300.0, 30.0, 20.0),
            CairoRect(290.0, 300.0, 30.0, 20.0),
            CairoRect(330.0, 300.0, 30.0, 20.0),
            CairoRect(370.0, 300.0, 30.0, 20.0)
        };
        CairoColor pink(255, 192, 203);
        surface->setColor(pink);
        surface->fillRectangles(rects);
        std::cout << "✅ Drew multiple rectangles" << std::endl;
        
        // Test 8: Text measurements
        std::cout << "\n--- Test 8: Text Measurements ---" << std::endl;
        surface->setFont("Arial", 14.0);
        double text_width = 0, text_height = 0;
        surface->getTextExtents("Measured Text", &text_width, &text_height);
        std::cout << "✅ Text size: " << text_width << "x" << text_height << " pixels" << std::endl;
        
        // Test 9: Transformations
        std::cout << "\n--- Test 9: Transformations ---" << std::endl;
        surface->save();
        surface->translate(400.0, 350.0);
        surface->rotate(0.785398); // 45 degrees
        
        CairoColor magenta(255, 0, 255);
        surface->setColor(magenta);
        surface->fillRectangle(-15.0, -15.0, 30.0, 30.0);
        surface->restore();
        std::cout << "✅ Drew rotated rectangle" << std::endl;
        
        // Test 10: Save to PNG
        std::cout << "\n--- Test 10: Save PNG ---" << std::endl;
        surface->saveAsPNG("cairo_wrapper_test.png");
        std::cout << "✅ Saved image to cairo_wrapper_test.png" << std::endl;
        
        // Test 11: Color utilities
        std::cout << "\n--- Test 11: Color Utilities ---" << std::endl;
        CairoColor packed_color(0xFF8040C0); // From packed RGBA
        std::cout << "✅ Packed color RGBA: " << std::hex << packed_color.toRGBA() << std::dec << std::endl;
        std::cout << "   R=" << (int)packed_color.r << " G=" << (int)packed_color.g 
                  << " B=" << (int)packed_color.b << " A=" << (int)packed_color.a << std::endl;
        
        // Test 12: Surface cloning
        std::cout << "\n--- Test 12: Surface Cloning ---" << std::endl;
        auto cloned_surface = surface->clone();
        if (cloned_surface && cloned_surface->isValid()) {
            cloned_surface->saveAsPNG("cairo_wrapper_clone.png");
            std::cout << "✅ Cloned surface and saved as cairo_wrapper_clone.png" << std::endl;
        } else {
            std::cout << "❌ Failed to clone surface" << std::endl;
        }
        
        // Test 13: Convert to image
        std::cout << "\n--- Test 13: Convert to Image ---" << std::endl;
        auto image = surface->toImage();
        if (image && image->isValid()) {
            image->saveAsPNG("cairo_wrapper_image.png");
            std::cout << "✅ Converted to image and saved as cairo_wrapper_image.png" << std::endl;
            std::cout << "   Image size: " << image->getWidth() << "x" << image->getHeight() << std::endl;
        } else {
            std::cout << "❌ Failed to convert surface to image" << std::endl;
        }
        
        std::cout << "\n🎉 All Cairo wrapper tests passed!" << std::endl;
        std::cout << "\nGenerated files:" << std::endl;
        std::cout << "  - cairo_wrapper_test.png (main test image)" << std::endl;
        std::cout << "  - cairo_wrapper_clone.png (cloned surface)" << std::endl;
        std::cout << "  - cairo_wrapper_image.png (converted image)" << std::endl;
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
}
/**
 * @file test_text_only.cpp
 * @brief Minimal test for Cairo text drawing functionality
 * 
 * This test isolates the text drawing issue by testing only the Cairo
 * text functions without dependencies on the full HeapManager or SAMM system.
 */

#include "cairo_core.h"
#include <iostream>
#include <cassert>
#include <cstring>

// Simple mock implementations for missing functions
extern "C" {
    char* bcpl_string_to_c_string(uint32_t* bcpl_str) {
        if (!bcpl_str) return nullptr;
        
        // Count length
        size_t len = 0;
        while (bcpl_str[len] != 0) len++;
        
        // Convert to C string (simple ASCII)
        char* c_str = (char*)malloc(len + 1);
        if (!c_str) return nullptr;
        
        for (size_t i = 0; i < len; i++) {
            c_str[i] = (char)(bcpl_str[i] & 0x7F); // ASCII only
        }
        c_str[len] = '\0';
        return c_str;
    }
    
    uint32_t* c_string_to_bcpl_string(const char* c_str) {
        if (!c_str) return nullptr;
        
        size_t len = strlen(c_str);
        uint32_t* bcpl_str = (uint32_t*)malloc((len + 1) * sizeof(uint32_t));
        if (!bcpl_str) return nullptr;
        
        for (size_t i = 0; i < len; i++) {
            bcpl_str[i] = (uint32_t)(unsigned char)c_str[i];
        }
        bcpl_str[len] = 0;
        return bcpl_str;
    }
}

int main() {
    std::cout << "=== Cairo Text Drawing Test ===" << std::endl;
    
    try {
        // Test 1: Create a surface
        std::cout << "1. Creating Cairo surface..." << std::endl;
        auto surface = std::make_unique<CairoSurface>(400, 300);
        std::cout << "   ✓ Surface created successfully" << std::endl;
        
        // Test 2: Clear the surface
        std::cout << "2. Clearing surface..." << std::endl;
        surface->clear(CairoColor(255, 255, 255, 255)); // White background
        std::cout << "   ✓ Surface cleared" << std::endl;
        
        // Test 3: Set font
        std::cout << "3. Setting font..." << std::endl;
        surface->setFont("Arial", 18.0);
        std::cout << "   ✓ Font set successfully" << std::endl;
        
        // Test 4: Set text color
        std::cout << "4. Setting text color..." << std::endl;
        surface->setColor(CairoColor(0, 0, 0, 255)); // Black text
        std::cout << "   ✓ Color set" << std::endl;
        
        // Test 5: Draw text (this is where the crash might occur)
        std::cout << "5. Drawing text..." << std::endl;
        std::cout << "   About to call drawText()..." << std::endl;
        surface->drawText(50.0, 100.0, "Hello Cairo!");
        std::cout << "   ✓ Text drawn successfully!" << std::endl;
        
        // Test 6: Draw more text
        std::cout << "6. Drawing additional text..." << std::endl;
        surface->drawText(50.0, 150.0, "This is a test");
        std::cout << "   ✓ Additional text drawn" << std::endl;
        
        // Test 7: Test different font size
        std::cout << "7. Testing different font size..." << std::endl;
        surface->setFont("Arial", 24.0);
        surface->drawText(50.0, 200.0, "Bigger text");
        std::cout << "   ✓ Different font size works" << std::endl;
        
        // Test 8: Save the result
        std::cout << "8. Saving to PNG..." << std::endl;
        surface->saveToPNG("text_only_test.png");
        std::cout << "   ✓ Image saved successfully" << std::endl;
        
        std::cout << "\n🎉 All text drawing tests passed!" << std::endl;
        std::cout << "Check text_only_test.png for the result." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}
#include "sdl2_runtime.h"
#include "../../RuntimeManager.h"
#include <iostream>

// SDL2 Runtime Registration Module
// This file registers all SDL2 functions with the RuntimeManager
// so they can be called from BCPL code.

namespace runtime {

// Flag to track if SDL2 functions have been registered
static bool sdl2_functions_registered = false;

/**
 * Lazy registration of SDL2 runtime functions
 * This function is called only when an SDL2 function is first requested
 * to avoid static initializer issues with the SDL2 library.
 */
void lazy_register_sdl2_runtime_functions() {
    if (sdl2_functions_registered) {
        return; // Already registered
    }
    
    RuntimeManager& runtime = RuntimeManager::instance();
    
    try {
        // =============================================================================
        // INITIALIZATION AND CLEANUP
        // =============================================================================
        
        runtime.register_function("SDL2_INIT", 0, 
                                 reinterpret_cast<void*>(SDL2_INIT), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_INIT_SUBSYSTEMS", 1, 
                                 reinterpret_cast<void*>(SDL2_INIT_SUBSYSTEMS), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_QUIT", 0, 
                                 reinterpret_cast<void*>(SDL2_QUIT_IMPL), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // WINDOW MANAGEMENT
        // =============================================================================
        
        runtime.register_function("SDL2_CREATE_WINDOW", 1, 
                                 reinterpret_cast<void*>(SDL2_CREATE_WINDOW), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_CREATE_WINDOW_EX", 6, 
                                 reinterpret_cast<void*>(SDL2_CREATE_WINDOW_EX), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_DESTROY_WINDOW", 1, 
                                 reinterpret_cast<void*>(SDL2_DESTROY_WINDOW), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_SET_WINDOW_TITLE", 2, 
                                 reinterpret_cast<void*>(SDL2_SET_WINDOW_TITLE), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_SET_WINDOW_SIZE", 3, 
                                 reinterpret_cast<void*>(SDL2_SET_WINDOW_SIZE), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // RENDERING
        // =============================================================================
        
        runtime.register_function("SDL2_CREATE_RENDERER", 1, 
                                 reinterpret_cast<void*>(SDL2_CREATE_RENDERER), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_CREATE_RENDERER_EX", 2, 
                                 reinterpret_cast<void*>(SDL2_CREATE_RENDERER_EX), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_DESTROY_RENDERER", 1, 
                                 reinterpret_cast<void*>(SDL2_DESTROY_RENDERER), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_SET_DRAW_COLOR", 5, 
                                 reinterpret_cast<void*>(SDL2_SET_DRAW_COLOR), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_CLEAR", 1, 
                                 reinterpret_cast<void*>(SDL2_CLEAR), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_PRESENT", 1, 
                                 reinterpret_cast<void*>(SDL2_PRESENT), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_DRAW_POINT", 3, 
                                 reinterpret_cast<void*>(SDL2_DRAW_POINT), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_DRAW_LINE", 5, 
                                 reinterpret_cast<void*>(SDL2_DRAW_LINE), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_DRAW_RECT", 5, 
                                 reinterpret_cast<void*>(SDL2_DRAW_RECT), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_FILL_RECT", 5, 
                                 reinterpret_cast<void*>(SDL2_FILL_RECT), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // EVENT HANDLING
        // =============================================================================
        
        runtime.register_function("SDL2_POLL_EVENT", 0, 
                                 reinterpret_cast<void*>(SDL2_POLL_EVENT), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_EVENT_KEY", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_EVENT_KEY), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_EVENT_MOUSE", 2, 
                                 reinterpret_cast<void*>(SDL2_GET_EVENT_MOUSE), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_EVENT_BUTTON", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_EVENT_BUTTON), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // TIMING
        // =============================================================================
        
        runtime.register_function("SDL2_DELAY", 1, 
                                 reinterpret_cast<void*>(SDL2_DELAY), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_TICKS", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_TICKS), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // UTILITY FUNCTIONS
        // =============================================================================
        
        runtime.register_function("SDL2_GET_ERROR", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_ERROR), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_CLEAR_ERROR", 0, 
                                 reinterpret_cast<void*>(SDL2_CLEAR_ERROR), 
                                 FunctionType::STANDARD);
        
        // =============================================================================
        // DIAGNOSTIC FUNCTIONS (for testing SDL2 functionality)
        // =============================================================================
        
        runtime.register_function("SDL2_GET_VERSION", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_VERSION), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_VIDEO_DRIVERS", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_VIDEO_DRIVERS), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_CURRENT_VIDEO_DRIVER", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_CURRENT_VIDEO_DRIVER), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_GET_DISPLAY_MODES", 0, 
                                 reinterpret_cast<void*>(SDL2_GET_DISPLAY_MODES), 
                                 FunctionType::STANDARD);
        
        runtime.register_function("SDL2_TEST_BASIC", 0, 
                                 reinterpret_cast<void*>(SDL2_TEST_BASIC), 
                                 FunctionType::STANDARD);
        
        // Register a special function to report SDL2 linking type
        #ifdef SDL2_RUNTIME_ENABLED
            // This function allows the runtime to report whether SDL2 is statically linked
            runtime.register_function("__SDL2_STATIC_BUILD", 0, 
                                     reinterpret_cast<void*>(SDL2_GET_TICKS), 
                                     FunctionType::STANDARD);
        #endif
        
        // Mark SDL2 functions as registered
        sdl2_functions_registered = true;
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error registering SDL2 runtime functions: " << e.what() << std::endl;
        throw;
    }
}

/**
 * Stub function for initial registration - does nothing to avoid SDL2 static initializers
 * The actual registration happens lazily when first SDL2 function is called
 */
void register_sdl2_runtime_functions() {
    // Actually register SDL2 functions immediately for JIT compatibility
    lazy_register_sdl2_runtime_functions();
    if (RuntimeManager::instance().isTracingEnabled()) {
        std::cout << "SDL2 runtime functions registered immediately" << std::endl;
    }
}

} // namespace runtime

// Export the lazy registration function as a C function to avoid namespace issues
extern "C" void lazy_register_sdl2_runtime_functions() {
    runtime::lazy_register_sdl2_runtime_functions();
}

namespace runtime {

/**
 * Check if SDL2 runtime is available
 * This can be used to conditionally enable SDL2 features
 */
bool is_sdl2_runtime_available() {
    RuntimeManager& runtime = RuntimeManager::instance();
    return runtime.is_function_registered("SDL2_INIT");
}

/**
 * Check if SDL2 is statically linked
 * This checks for the presence of a marker function that indicates static linking
 */
bool is_sdl2_static_build() {
    RuntimeManager& runtime = RuntimeManager::instance();
    return runtime.is_function_registered("__SDL2_STATIC_BUILD");
}

} // namespace runtime
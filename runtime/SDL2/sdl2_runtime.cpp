#include "sdl2_runtime.h"
#include "../runtime.h"
#include "../BCPLError.h"
#include "../../HeapManager/HeapManager.h"
#include <SDL.h>
#include <unordered_map>
#include <string>
#include <cstring>
#include <iostream>

// Define missing error constants
#ifndef ERROR_GENERAL
#define ERROR_GENERAL ERROR_INVALID_ARGUMENT
#endif

// Define macro for error setting
#define BCPL_SET_ERROR(code, func, msg) _BCPL_SET_ERROR(code, func, msg)

// =============================================================================
// INTERNAL STATE MANAGEMENT
// =============================================================================

namespace {
    // Resource management - map IDs to SDL objects
    std::unordered_map<int64_t, SDL_Window*> windows;
    std::unordered_map<int64_t, SDL_Renderer*> renderers;
    
    // ID generators
    int64_t next_window_id = 1;
    int64_t next_renderer_id = 1;
    
    // Event state
    SDL_Event last_event;
    bool has_pending_event = false;
    
    // Utility function to convert BCPL string to C string
    char* bcpl_to_c_string(bcpl_string_t bcpl_str) {
        if (!bcpl_str) return nullptr;
        
        // BCPL strings are stored as [length, char, char, char, ...]
        uint32_t length = bcpl_str[0];
        char* c_str = static_cast<char*>(malloc(length + 1));
        if (!c_str) return nullptr;
        
        // Extract characters from BCPL string
        for (uint32_t i = 0; i < length; i++) {
            c_str[i] = static_cast<char>(bcpl_str[i + 1]);
        }
        c_str[length] = '\0';
        
        return c_str;
    }
    
    // Utility function to create BCPL string from C string
    bcpl_string_t c_to_bcpl_string(const char* c_str) {
        if (!c_str) return nullptr;
        
        size_t length = strlen(c_str);
        if (length > UINT32_MAX - 1) return nullptr;
        
        // Allocate BCPL string through HeapManager
        HeapManager& heap = HeapManager::getInstance();
        bcpl_string_t bcpl_str = reinterpret_cast<bcpl_string_t>(
            heap.allocString(static_cast<uint32_t>(length))
        );
        
        if (!bcpl_str) return nullptr;
        
        // Set length and copy characters
        bcpl_str[0] = static_cast<uint32_t>(length);
        for (size_t i = 0; i < length; i++) {
            bcpl_str[i + 1] = static_cast<uint32_t>(c_str[i]);
        }
        
        return bcpl_str;
    }
    
    // Error handling helper
    void set_sdl_error(const char* function_name) {
        const char* sdl_error = SDL_GetError();
        std::string error_msg = std::string(function_name) + ": " + sdl_error;
        BCPL_SET_ERROR(ERROR_GENERAL, function_name, error_msg.c_str());
    }
}

// =============================================================================
// INITIALIZATION AND CLEANUP
// =============================================================================

extern "C" {

int64_t SDL2_INIT() {
    int result = SDL_Init(SDL_INIT_VIDEO);
    if (result < 0) {
        set_sdl_error("SDL2_INIT");
        return static_cast<int64_t>(result);
    }
    return 0;
}

void SDL2_INIT_SUBSYSTEMS(int64_t flags) {
    if (SDL_Init(static_cast<Uint32>(flags)) < 0) {
        set_sdl_error("SDL2_INIT_SUBSYSTEMS");
    }
}

void SDL2_QUIT_IMPL() {
    // Clean up all windows and renderers
    for (auto& pair : renderers) {
        if (pair.second) {
            SDL_DestroyRenderer(pair.second);
        }
    }
    renderers.clear();
    
    for (auto& pair : windows) {
        if (pair.second) {
            SDL_DestroyWindow(pair.second);
        }
    }
    windows.clear();
    
    SDL_Quit();
}

// Alias for BCPL/assembly linkage
extern "C" void SDL2_QUIT(void) {
    SDL2_QUIT_IMPL();
}

// =============================================================================
// WINDOW MANAGEMENT
// =============================================================================

int64_t SDL2_CREATE_WINDOW(bcpl_string_t title) {
    return SDL2_CREATE_WINDOW_EX(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                640, 480, SDL_WINDOW_SHOWN);
}

int64_t SDL2_CREATE_WINDOW_EX(bcpl_string_t title, int64_t x, int64_t y, 
                               int64_t width, int64_t height, int64_t flags) {
    char* c_title = bcpl_to_c_string(title);
    if (!c_title) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "SDL2_CREATE_WINDOW_EX", "Failed to convert title");
        return 0;
    }
    
    SDL_Window* window = SDL_CreateWindow(
        c_title,
        static_cast<int>(x),
        static_cast<int>(y),
        static_cast<int>(width),
        static_cast<int>(height),
        static_cast<Uint32>(flags)
    );
    
    free(c_title);
    
    if (!window) {
        set_sdl_error("SDL2_CREATE_WINDOW_EX");
        return 0;
    }
    
    int64_t window_id = next_window_id++;
    windows[window_id] = window;
    
    return window_id;
}

void SDL2_DESTROY_WINDOW(int64_t window_id) {
    auto it = windows.find(window_id);
    if (it != windows.end()) {
        SDL_DestroyWindow(it->second);
        windows.erase(it);
    }
}

void SDL2_SET_WINDOW_TITLE(int64_t window_id, bcpl_string_t title) {
    auto it = windows.find(window_id);
    if (it == windows.end()) return;
    
    char* c_title = bcpl_to_c_string(title);
    if (c_title) {
        SDL_SetWindowTitle(it->second, c_title);
        free(c_title);
    }
}

void SDL2_SET_WINDOW_SIZE(int64_t window_id, int64_t width, int64_t height) {
    auto it = windows.find(window_id);
    if (it != windows.end()) {
        SDL_SetWindowSize(it->second, static_cast<int>(width), static_cast<int>(height));
    }
}

// =============================================================================
// RENDERING
// =============================================================================

int64_t SDL2_CREATE_RENDERER(int64_t window_id) {
    return SDL2_CREATE_RENDERER_EX(window_id, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

int64_t SDL2_CREATE_RENDERER_EX(int64_t window_id, int64_t flags) {
    auto it = windows.find(window_id);
    if (it == windows.end()) {
        BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "SDL2_CREATE_RENDERER_EX", "Invalid window ID");
        return 0;
    }
    
    SDL_Renderer* renderer = SDL_CreateRenderer(it->second, -1, static_cast<Uint32>(flags));
    if (!renderer) {
        set_sdl_error("SDL2_CREATE_RENDERER_EX");
        return 0;
    }
    
    int64_t renderer_id = next_renderer_id++;
    renderers[renderer_id] = renderer;
    
    return renderer_id;
}

void SDL2_DESTROY_RENDERER(int64_t renderer_id) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_DestroyRenderer(it->second);
        renderers.erase(it);
    }
}

void SDL2_SET_DRAW_COLOR(int64_t renderer_id, int64_t r, int64_t g, int64_t b, int64_t a) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_SetRenderDrawColor(it->second, 
                              static_cast<Uint8>(r), 
                              static_cast<Uint8>(g), 
                              static_cast<Uint8>(b), 
                              static_cast<Uint8>(a));
    }
}

void SDL2_CLEAR(int64_t renderer_id) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_RenderClear(it->second);
    }
}

void SDL2_PRESENT(int64_t renderer_id) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_RenderPresent(it->second);
    }
}

void SDL2_DRAW_POINT(int64_t renderer_id, int64_t x, int64_t y) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_RenderDrawPoint(it->second, static_cast<int>(x), static_cast<int>(y));
    }
}

void SDL2_DRAW_LINE(int64_t renderer_id, int64_t x1, int64_t y1, int64_t x2, int64_t y2) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_RenderDrawLine(it->second, 
                          static_cast<int>(x1), static_cast<int>(y1),
                          static_cast<int>(x2), static_cast<int>(y2));
    }
}

void SDL2_DRAW_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_Rect rect = {
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h)
        };
        SDL_RenderDrawRect(it->second, &rect);
    }
}

void SDL2_FILL_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h) {
    auto it = renderers.find(renderer_id);
    if (it != renderers.end()) {
        SDL_Rect rect = {
            static_cast<int>(x), static_cast<int>(y),
            static_cast<int>(w), static_cast<int>(h)
        };
        SDL_RenderFillRect(it->second, &rect);
    }
}

// =============================================================================
// EVENT HANDLING
// =============================================================================

int64_t SDL2_POLL_EVENT() {
    if (SDL_PollEvent(&last_event)) {
        has_pending_event = true;
        return static_cast<int64_t>(last_event.type);
    }
    has_pending_event = false;
    return 0;
}

int64_t SDL2_GET_EVENT_KEY() {
    if (has_pending_event && 
        (last_event.type == SDL_KEYDOWN || last_event.type == SDL_KEYUP)) {
        return static_cast<int64_t>(last_event.key.keysym.sym);
    }
    return 0;
}

void SDL2_GET_EVENT_MOUSE(int64_t* x, int64_t* y) {
    if (!x || !y) return;
    
    if (has_pending_event && 
        (last_event.type == SDL_MOUSEBUTTONDOWN || 
         last_event.type == SDL_MOUSEBUTTONUP ||
         last_event.type == SDL_MOUSEMOTION)) {
        
        if (last_event.type == SDL_MOUSEMOTION) {
            *x = static_cast<int64_t>(last_event.motion.x);
            *y = static_cast<int64_t>(last_event.motion.y);
        } else {
            *x = static_cast<int64_t>(last_event.button.x);
            *y = static_cast<int64_t>(last_event.button.y);
        }
    } else {
        *x = 0;
        *y = 0;
    }
}

int64_t SDL2_GET_EVENT_BUTTON() {
    if (has_pending_event && 
        (last_event.type == SDL_MOUSEBUTTONDOWN || last_event.type == SDL_MOUSEBUTTONUP)) {
        return static_cast<int64_t>(last_event.button.button);
    }
    return 0;
}

// =============================================================================
// TIMING
// =============================================================================

void SDL2_DELAY(int64_t milliseconds) {
    if (milliseconds > 0) {
        SDL_Delay(static_cast<Uint32>(milliseconds));
    }
}

int64_t SDL2_GET_TICKS() {
    return static_cast<int64_t>(SDL_GetTicks());
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

bcpl_string_t SDL2_GET_ERROR() {
    const char* error = SDL_GetError();
    if (error && strlen(error) > 0) {
        return c_to_bcpl_string(error);
    }
    return c_to_bcpl_string("");
}

void SDL2_CLEAR_ERROR() {
    SDL_ClearError();
}

// =============================================================================
// DIAGNOSTIC FUNCTIONS (for testing SDL2 functionality)
// =============================================================================

int64_t SDL2_GET_VERSION() {
    SDL_version version;
    SDL_GetVersion(&version);
    return static_cast<int64_t>(version.major * 1000 + version.minor * 100 + version.patch);
}

int64_t SDL2_GET_VIDEO_DRIVERS() {
    return static_cast<int64_t>(SDL_GetNumVideoDrivers());
}

bcpl_string_t SDL2_GET_CURRENT_VIDEO_DRIVER() {
    const char* driver = SDL_GetCurrentVideoDriver();
    if (driver) {
        return c_to_bcpl_string(driver);
    }
    return c_to_bcpl_string("");
}

int64_t SDL2_GET_DISPLAY_MODES() {
    // First check if video is initialized
    if (!SDL_WasInit(SDL_INIT_VIDEO)) {
        return -1; // Video not initialized
    }
    
    int num_displays = SDL_GetNumVideoDisplays();
    if (num_displays <= 0) {
        return -2; // No displays available
    }
    
    // Get display modes for display 0
    return static_cast<int64_t>(SDL_GetNumDisplayModes(0));
}

int64_t SDL2_TEST_BASIC() {
    // Test basic SDL2 functionality without creating windows
    
    // 1. Test version info
    SDL_version version;
    SDL_GetVersion(&version);
    if (version.major < 2) {
        return -1; // SDL version too old
    }
    
    // 2. Test getting video drivers (should work even without init)
    int num_drivers = SDL_GetNumVideoDrivers();
    if (num_drivers <= 0) {
        return -2; // No video drivers available
    }
    
    // 3. Test SDL_Init with timer (minimal subsystem)
    if (SDL_Init(SDL_INIT_TIMER) < 0) {
        return -3; // Failed to initialize timer subsystem
    }
    
    // 4. Test getting ticks (should work after timer init)
    Uint32 ticks = SDL_GetTicks();
    
    // 5. Test platform info
    const char* platform = SDL_GetPlatform();
    if (!platform) {
        SDL_Quit();
        return -4; // Failed to get platform info
    }
    
    // Clean up timer subsystem
    SDL_QuitSubSystem(SDL_INIT_TIMER);
    
    return 0; // All basic tests passed
}

} // extern "C"
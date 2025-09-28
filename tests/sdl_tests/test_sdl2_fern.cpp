#include <iostream>
#include <random>
#include <cstdint>
#include <thread>
#include <chrono>
#include <cstring>

// Define missing global for HeapManager linkage
extern "C" int g_enable_heap_trace = 0;

// Include the SDL2 runtime API
extern "C" {
    #include "../../runtime/SDL2/sdl2_runtime.h"
}

// Helper to create BCPL string from C string (null-terminated, uint32_t per char)
uint32_t* create_bcpl_string(const char* str) {
    size_t len = strlen(str);
    uint32_t* bcpl = new uint32_t[len + 1];
    for (size_t i = 0; i < len; ++i) bcpl[i] = (uint32_t)str[i];
    bcpl[len] = 0;
    return bcpl;
}

int main() {
    if (SDL2_INIT() != 0) {
        std::cerr << "SDL2_INIT failed: " << (const char*)SDL2_GET_ERROR() << std::endl;
        return 1;
    }

    uint32_t* title = create_bcpl_string("IFS Fern - SDL2 Runtime Test");
    int64_t window_id = SDL2_CREATE_WINDOW(title);
    delete[] title;
    if (window_id == 0) {
        std::cerr << "SDL2_CREATE_WINDOW failed: " << (const char*)SDL2_GET_ERROR() << std::endl;
        SDL2_QUIT_IMPL();
        return 1;
    }

    int64_t renderer_id = SDL2_CREATE_RENDERER(window_id);
    if (renderer_id == 0) {
        std::cerr << "SDL2_CREATE_RENDERER failed: " << (const char*)SDL2_GET_ERROR() << std::endl;
        SDL2_DESTROY_WINDOW(window_id);
        SDL2_QUIT_IMPL();
        return 1;
    }

    // Clear background
    SDL2_SET_DRAW_COLOR(renderer_id, 0, 0, 0, 255);
    SDL2_CLEAR(renderer_id);

    // Draw IFS fern
    SDL2_SET_DRAW_COLOR(renderer_id, 0, 255, 0, 255);
    double x = 0.0, y = 0.0;
    std::mt19937 rng((unsigned)time(nullptr));
    std::uniform_real_distribution<> dist(0.0, 1.0);
    int width = 640, height = 480;
    for (int i = 0; i < 100000; ++i) {
        double next_x, next_y;
        double r = dist(rng);
        if (r < 0.01) {
            next_x = 0.0;
            next_y = 0.16 * y;
        } else if (r < 0.86) {
            next_x = 0.85 * x + 0.04 * y;
            next_y = -0.04 * x + 0.85 * y + 1.6;
        } else if (r < 0.93) {
            next_x = 0.20 * x - 0.26 * y;
            next_y = 0.23 * x + 0.22 * y + 1.6;
        } else {
            next_x = -0.15 * x + 0.28 * y;
            next_y = 0.26 * x + 0.24 * y + 0.44;
        }
        x = next_x;
        y = next_y;
        int px = (int)(width / 2 + x * width / 11);
        int py = (int)(height - y * height / 12);
        SDL2_DRAW_POINT(renderer_id, px, py);
    }
    SDL2_PRESENT(renderer_id);

    // Wait for user to close window
    bool running = true;
    while (running) {
        int64_t event = SDL2_POLL_EVENT();
        if (event == SDL2_EVENT_QUIT) running = false;
        SDL2_DELAY(16);
    }

    SDL2_DESTROY_RENDERER(renderer_id);
    SDL2_DESTROY_WINDOW(window_id);
    SDL2_QUIT_IMPL();
    std::cout << "IFS fern test completed." << std::endl;
    return 0;
}
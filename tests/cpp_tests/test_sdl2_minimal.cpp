#include <SDL.h>
#include <iostream>

int main(int argc, char* argv[]) {
    std::cout << "Testing minimal SDL2 functionality..." << std::endl;
    
    // Test 1: Get SDL2 version
    SDL_version version;
    SDL_GetVersion(&version);
    std::cout << "SDL2 version: " << (int)version.major << "." 
              << (int)version.minor << "." << (int)version.patch << std::endl;
    
    // Test 2: Initialize SDL2
    std::cout << "Initializing SDL2..." << std::endl;
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    std::cout << "SDL2 initialized successfully!" << std::endl;
    
    // Test 3: Get video drivers
    int num_drivers = SDL_GetNumVideoDrivers();
    std::cout << "Number of video drivers: " << num_drivers << std::endl;
    
    // Test 4: Get current video driver
    const char* driver = SDL_GetCurrentVideoDriver();
    if (driver) {
        std::cout << "Current video driver: " << driver << std::endl;
    } else {
        std::cout << "No current video driver" << std::endl;
    }
    
    // Test 5: Clean up
    std::cout << "Cleaning up..." << std::endl;
    SDL_Quit();
    std::cout << "SDL2 test completed successfully!" << std::endl;
    
    return 0;
}
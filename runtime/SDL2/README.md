# SDL2 Runtime Module for NewBCPL

This module provides SDL2 graphics and input functionality for BCPL programs through a runtime-based approach. Instead of hardcoded keywords, SDL2 features are exposed as runtime functions that can be called from BCPL code.

## Overview

The SDL2 runtime module follows the pluggable runtime architecture, allowing SDL2 functionality to be conditionally included when building the runtime. This design provides several advantages:

- **Flexible deployment**: SDL2 can be included or excluded based on requirements
- **Clean separation**: SDL2 code is isolated in its own module
- **Easy maintenance**: SDL2 functions can be updated independently
- **Runtime discovery**: BCPL programs can check for SDL2 availability at runtime

## Building with SDL2 Support

### Prerequisites

Install SDL2 development libraries on your system:

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev
```

**macOS:**
```bash
brew install sdl2
```

**Fedora/RHEL:**
```bash
sudo dnf install SDL2-devel
```

### Building the Runtime

#### Dynamic SDL2 Linking (Standard)

To build the unified runtime with SDL2 support using dynamic linking:

```bash
./buildruntime --with-sdl2
```

This will:
1. Check for SDL2 availability using `pkg-config`
2. Compile the SDL2 runtime module with proper flags
3. Create `libbcpl_runtime_sdl2.a` containing all runtime functions including SDL2
4. Display SDL2 linking requirements for your applications

#### Static SDL2 Linking (Self-Contained)

To build a fully self-contained runtime with SDL2 statically embedded:

```bash
./buildruntime --with-sdl2-static
```

This will:
1. Check for SDL2 availability using `pkg-config`
2. Compile the SDL2 runtime module with proper flags
3. Extract and embed the SDL2 static library objects
4. Create `libbcpl_runtime_sdl2_static.a` - a fully self-contained runtime
5. Display system framework linking requirements (no SDL2.dylib needed)

**Benefits of Static Linking:**
- **Easy Deployment**: No need for SDL2.dylib on target systems
- **JIT Isolation**: JIT compiler doesn't need SDL2 dependencies
- **Self-Contained**: Single library file contains everything needed

### Linking Applications

#### Dynamic SDL2 Linking

When linking your BCPL application with dynamic SDL2 support:

```bash
clang++ -o myapp myapp.o libbcpl_runtime_sdl2.a $(pkg-config --libs sdl2)
```

**Note**: Requires SDL2.dylib to be installed on target systems.

#### Static SDL2 Linking

When linking your BCPL application with static SDL2 support:

```bash
clang++ -o myapp myapp.o libbcpl_runtime_sdl2_static.a $(pkg-config --libs --static sdl2 | sed 's/-lSDL2//g')
```

Or more simply on macOS:

```bash
clang++ -o myapp myapp.o libbcpl_runtime_sdl2_static.a -lm \
    -framework CoreAudio -framework AudioToolbox -framework CoreHaptics \
    -framework GameController -framework ForceFeedback -lobjc \
    -framework CoreVideo -framework Cocoa -framework Carbon \
    -framework IOKit -framework QuartzCore -framework Metal
```

**Benefits**: 
- No SDL2 installation required on target systems
- Easier deployment and distribution
- Perfect for standalone applications

## Available Functions

### Initialization and Cleanup

- `SDL2_INIT()` - Initialize SDL2 video subsystem
- `SDL2_INIT_SUBSYSTEMS(flags)` - Initialize specific SDL2 subsystems
- `SDL2_QUIT()` - Quit SDL2 and cleanup all resources

### Window Management

- `SDL2_CREATE_WINDOW(title)` - Create a 640x480 window
- `SDL2_CREATE_WINDOW_EX(title, x, y, width, height, flags)` - Create custom window
- `SDL2_DESTROY_WINDOW(window_id)` - Destroy a window
- `SDL2_SET_WINDOW_TITLE(window_id, title)` - Set window title
- `SDL2_SET_WINDOW_SIZE(window_id, width, height)` - Resize window

### Rendering

- `SDL2_CREATE_RENDERER(window_id)` - Create renderer with default settings
- `SDL2_CREATE_RENDERER_EX(window_id, flags)` - Create renderer with custom flags
- `SDL2_DESTROY_RENDERER(renderer_id)` - Destroy a renderer
- `SDL2_SET_DRAW_COLOR(renderer_id, r, g, b, a)` - Set drawing color
- `SDL2_CLEAR(renderer_id)` - Clear screen with current color
- `SDL2_PRESENT(renderer_id)` - Present the rendered frame

### Drawing Functions

- `SDL2_DRAW_POINT(renderer_id, x, y)` - Draw a point
- `SDL2_DRAW_LINE(renderer_id, x1, y1, x2, y2)` - Draw a line
- `SDL2_DRAW_RECT(renderer_id, x, y, w, h)` - Draw rectangle outline
- `SDL2_FILL_RECT(renderer_id, x, y, w, h)` - Fill rectangle

### Event Handling

- `SDL2_POLL_EVENT()` - Poll for events (returns event type)
- `SDL2_GET_EVENT_KEY()` - Get key code from keyboard events
- `SDL2_GET_EVENT_MOUSE(x_ptr, y_ptr)` - Get mouse coordinates
- `SDL2_GET_EVENT_BUTTON()` - Get mouse button from mouse events

### Timing

- `SDL2_DELAY(milliseconds)` - Delay execution
- `SDL2_GET_TICKS()` - Get milliseconds since SDL initialization

### Utility

- `SDL2_GET_ERROR()` - Get SDL error message
- `SDL2_CLEAR_ERROR()` - Clear SDL error state

## Constants

The following constants are available for use in BCPL code:

### Event Types
- `0x100` - SDL2_QUIT
- `0x300` - SDL2_KEYDOWN  
- `0x301` - SDL2_KEYUP
- `0x400` - SDL2_MOUSEMOTION
- `0x401` - SDL2_MOUSEBUTTONDOWN
- `0x402` - SDL2_MOUSEBUTTONUP

### Window Flags
- `0x00000001` - SDL2_WINDOW_FULLSCREEN
- `0x00000020` - SDL2_WINDOW_RESIZABLE
- `0x00000004` - SDL2_WINDOW_SHOWN

### Mouse Buttons
- `1` - SDL2_BUTTON_LEFT
- `2` - SDL2_BUTTON_MIDDLE
- `3` - SDL2_BUTTON_RIGHT

## Example Usage

```bcpl
LET START() BE
$(
    LET window_id = 0
    LET renderer_id = 0
    LET running = TRUE
    
    // Initialize SDL2
    SDL2_INIT()
    
    // Create window and renderer
    window_id := SDL2_CREATE_WINDOW("My BCPL Game")
    renderer_id := SDL2_CREATE_RENDERER(window_id)
    
    // Main loop
    WHILE running DO
    $(
        LET event_type = SDL2_POLL_EVENT()
        
        SWITCHON event_type INTO
        $(
            CASE 0x100:  // Quit
                running := FALSE
                ENDCASE
                
            CASE 0x300:  // Key press
                LET key = SDL2_GET_EVENT_KEY()
                // Handle key press
                ENDCASE
        $)
        
        // Clear and draw
        SDL2_SET_DRAW_COLOR(renderer_id, 0, 0, 0, 255)  // Black
        SDL2_CLEAR(renderer_id)
        
        SDL2_SET_DRAW_COLOR(renderer_id, 255, 0, 0, 255)  // Red
        SDL2_FILL_RECT(renderer_id, 100, 100, 50, 50)
        
        SDL2_PRESENT(renderer_id)
        SDL2_DELAY(16)  // ~60 FPS
    $)
    
    // Cleanup
    SDL2_DESTROY_RENDERER(renderer_id)
    SDL2_DESTROY_WINDOW(window_id)
    SDL2_QUIT()
$)
```

## Resource Management

The SDL2 module automatically manages SDL resources using ID-based handles:

- **Window IDs**: Unique identifiers for SDL windows
- **Renderer IDs**: Unique identifiers for SDL renderers
- **Automatic cleanup**: `SDL2_QUIT()` automatically destroys all remaining resources

## Error Handling

SDL2 errors are integrated with the BCPL runtime error system:

- SDL errors are automatically converted to BCPL runtime errors
- Use `SDL2_GET_ERROR()` to retrieve human-readable error messages
- Always check return values (0 typically indicates failure)

## Architecture Notes

### String Handling

The module converts between BCPL strings and C strings automatically:
- BCPL strings: `[length, char1, char2, ...]`
- Automatic memory management for string conversions
- Uses HeapManager for BCPL string allocation

### Thread Safety

The SDL2 module is designed for single-threaded use, consistent with BCPL's execution model.

### Memory Management

- All SDL resources are tracked internally
- Automatic cleanup on `SDL2_QUIT()`
- Integration with BCPL's HeapManager for string operations

## Extending the Module

To add new SDL2 functions:

1. Add the C-linkage function to `sdl2_runtime.cpp`
2. Add the function declaration to `sdl2_runtime.h`
3. Register the function in `sdl2_registration.cpp`
4. Update this README with the new function

## Limitations

- Currently supports basic 2D graphics and input
- No support for audio, textures, or advanced SDL2 features
- Single window/renderer workflows are most efficient
- No direct access to SDL2 structures from BCPL code

## Future Enhancements

Potential areas for expansion:
- Texture and sprite support
- Audio playback functions
- Joystick/gamepad input
- Network functionality
- File I/O extensions
- Image loading (SDL2_image integration)

## Troubleshooting

### Build Issues

**"SDL2 development libraries not found"**
- Install SDL2 development packages for your system
- Ensure `pkg-config` can find SDL2: `pkg-config --exists sdl2`

**"SDL2 static library not found"** (for `--with-sdl2-static`)
- Ensure SDL2 static library is installed: check for `libSDL2.a`
- On some systems, you may need a separate SDL2 static development package

**Linking errors (Dynamic)**
- Make sure to link with SDL2 libraries: `$(pkg-config --libs sdl2)`
- Use the correct runtime library: `libbcpl_runtime_sdl2.a`

**Linking errors (Static)**
- Use the static runtime library: `libbcpl_runtime_sdl2_static.a`
- Include all required system frameworks (see linking examples above)
- Don't include `-lSDL2` - it's already embedded in the static library

**Large binary size (Static)**
- Static linking includes the entire SDL2 library (~2.5MB additional size)
- This is normal and expected for self-contained deployment

### Runtime Issues

**Window not appearing**
- Check return value of `SDL2_CREATE_WINDOW()`
- Verify SDL2 initialization succeeded
- Check for SDL errors with `SDL2_GET_ERROR()`

**Poor performance**
- Use `SDL2_DELAY()` in your main loop
- Consider reducing drawing operations
- Check for SDL2 hardware acceleration

**Static vs Dynamic Runtime Issues**
- Ensure you're using the correct runtime library for your build type
- Static: `libbcpl_runtime_sdl2_static.a` (self-contained)
- Dynamic: `libbcpl_runtime_sdl2.a` (requires SDL2.dylib)
- Check that `SDL2_RUNTIME_ENABLED` is defined when compiling applications
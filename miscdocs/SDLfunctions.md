# NewBCPL SDL2 Functions Reference

This document provides a comprehensive reference for all SDL2 functions available in NewBCPL. SDL2 functionality is provided through the runtime system, allowing BCPL programs to create graphics applications, games, and interactive programs.

## Table of Contents

- [Overview](#overview)
- [Setup and Requirements](#setup-and-requirements)
- [Initialization and Cleanup](#initialization-and-cleanup)
- [Window Management](#window-management)
- [Rendering](#rendering)
- [Drawing Functions](#drawing-functions)
- [Event Handling](#event-handling)
- [Timing Functions](#timing-functions)
- [Utility Functions](#utility-functions)
- [Diagnostic Functions](#diagnostic-functions)
- [Constants Reference](#constants-reference)
- [Complete Example](#complete-example)
- [Error Handling](#error-handling)
- [Performance Tips](#performance-tips)

---

## Overview

NewBCPL provides SDL2 integration through **27 runtime functions** that cover:
- Window creation and management
- 2D graphics rendering
- Event handling (keyboard, mouse)
- Timing and animation
- Error reporting and diagnostics

All SDL2 functions return integer values and follow consistent naming conventions with the `SDL2_` prefix.

---

## Setup and Requirements

### Building with SDL2 Support

```bash
# Standard dynamic linking
./buildruntime --with-sdl2

# Self-contained static linking (recommended)
./buildruntime --with-sdl2-static
```

### Checking SDL2 Availability

```bcpl
LET START() = VALOF
$(
  // Check if SDL2 is available at runtime
  LET has_sdl2 = SDL2_TEST_BASIC()
  IF has_sdl2 = 0 THEN
  $(
    WRITES("SDL2 not available*N")
    RESULTIS 1
  $)
  
  WRITES("SDL2 is available*N")
  RESULTIS 0
$)
```

---

## Initialization and Cleanup

### SDL2_INIT()
- **Purpose**: Initialize SDL2 video subsystem
- **Parameters**: None
- **Returns**: INTEGER - 0 on success, -1 on failure
- **Example**: 
```bcpl
IF SDL2_INIT() < 0 THEN
$(
  WRITES("Failed to initialize SDL2*N")
  FINISH()
$)
```

### SDL2_INIT_SUBSYSTEMS(flags)
- **Purpose**: Initialize specific SDL2 subsystems
- **Parameters**: `flags` (INTEGER) - Subsystem flags to initialize
- **Returns**: INTEGER - 0 on success, -1 on failure
- **Example**: `SDL2_INIT_SUBSYSTEMS(0x20)` // Initialize video subsystem

### SDL2_QUIT()
- **Purpose**: Quit SDL2 and cleanup all resources
- **Parameters**: None
- **Returns**: Nothing (automatically cleans up all windows, renderers)
- **Example**: `SDL2_QUIT()` // Call at program end

---

## Window Management

### SDL2_CREATE_WINDOW(title)
- **Purpose**: Create a 640x480 window with default settings
- **Parameters**: `title` (STRING) - Window title
- **Returns**: INTEGER - Window ID (>0) on success, 0 on failure
- **Example**: 
```bcpl
LET window_id = SDL2_CREATE_WINDOW("My Game")
IF window_id = 0 THEN
$(
  WRITES("Failed to create window*N")
  FINISH()
$)
```

### SDL2_CREATE_WINDOW_EX(title, x, y, width, height, flags)
- **Purpose**: Create a window with custom properties
- **Parameters**: 
  - `title` (STRING) - Window title
  - `x` (INTEGER) - X position (or `0x1FFF0000` for centered)
  - `y` (INTEGER) - Y position (or `0x1FFF0000` for centered) 
  - `width` (INTEGER) - Window width
  - `height` (INTEGER) - Window height
  - `flags` (INTEGER) - Window flags (see constants)
- **Returns**: INTEGER - Window ID on success, 0 on failure
- **Example**:
```bcpl
LET window_id = SDL2_CREATE_WINDOW_EX("Custom Window", 
                                      0x1FFF0000, 0x1FFF0000,  // Centered
                                      800, 600,                 // 800x600
                                      0x00000020)               // Resizable
```

### SDL2_DESTROY_WINDOW(window_id)
- **Purpose**: Destroy a window and free its resources
- **Parameters**: `window_id` (INTEGER) - Window ID to destroy
- **Returns**: Nothing
- **Example**: `SDL2_DESTROY_WINDOW(window_id)`

### SDL2_SET_WINDOW_TITLE(window_id, title)
- **Purpose**: Change a window's title
- **Parameters**: 
  - `window_id` (INTEGER) - Target window ID
  - `title` (STRING) - New title
- **Returns**: Nothing
- **Example**: `SDL2_SET_WINDOW_TITLE(window_id, "New Title")`

### SDL2_SET_WINDOW_SIZE(window_id, width, height)
- **Purpose**: Resize a window
- **Parameters**:
  - `window_id` (INTEGER) - Target window ID
  - `width` (INTEGER) - New width
  - `height` (INTEGER) - New height
- **Returns**: Nothing
- **Example**: `SDL2_SET_WINDOW_SIZE(window_id, 1024, 768)`

---

## Rendering

### SDL2_CREATE_RENDERER(window_id)
- **Purpose**: Create a renderer for a window with default settings
- **Parameters**: `window_id` (INTEGER) - Target window ID
- **Returns**: INTEGER - Renderer ID on success, 0 on failure
- **Example**:
```bcpl
LET renderer_id = SDL2_CREATE_RENDERER(window_id)
IF renderer_id = 0 THEN
$(
  WRITES("Failed to create renderer*N")
  FINISH()
$)
```

### SDL2_CREATE_RENDERER_EX(window_id, flags)
- **Purpose**: Create a renderer with custom flags
- **Parameters**:
  - `window_id` (INTEGER) - Target window ID
  - `flags` (INTEGER) - Renderer flags
- **Returns**: INTEGER - Renderer ID on success, 0 on failure
- **Example**: `LET renderer_id = SDL2_CREATE_RENDERER_EX(window_id, 2)` // Hardware accelerated

### SDL2_DESTROY_RENDERER(renderer_id)
- **Purpose**: Destroy a renderer and free its resources
- **Parameters**: `renderer_id` (INTEGER) - Renderer ID to destroy
- **Returns**: Nothing
- **Example**: `SDL2_DESTROY_RENDERER(renderer_id)`

### SDL2_SET_DRAW_COLOR(renderer_id, r, g, b, a)
- **Purpose**: Set the color for drawing operations
- **Parameters**:
  - `renderer_id` (INTEGER) - Target renderer ID
  - `r` (INTEGER) - Red component (0-255)
  - `g` (INTEGER) - Green component (0-255)
  - `b` (INTEGER) - Blue component (0-255)
  - `a` (INTEGER) - Alpha component (0-255)
- **Returns**: Nothing
- **Example**: `SDL2_SET_DRAW_COLOR(renderer_id, 255, 0, 0, 255)` // Solid red

### SDL2_CLEAR(renderer_id)
- **Purpose**: Clear the screen with current draw color
- **Parameters**: `renderer_id` (INTEGER) - Target renderer ID
- **Returns**: Nothing
- **Example**: 
```bcpl
SDL2_SET_DRAW_COLOR(renderer_id, 0, 0, 0, 255)  // Black
SDL2_CLEAR(renderer_id)                          // Clear screen
```

### SDL2_PRESENT(renderer_id)
- **Purpose**: Display the rendered frame (swap buffers)
- **Parameters**: `renderer_id` (INTEGER) - Target renderer ID
- **Returns**: Nothing
- **Example**: `SDL2_PRESENT(renderer_id)` // Show what was drawn

---

## Drawing Functions

### SDL2_DRAW_POINT(renderer_id, x, y)
- **Purpose**: Draw a single point
- **Parameters**:
  - `renderer_id` (INTEGER) - Target renderer ID
  - `x` (INTEGER) - X coordinate
  - `y` (INTEGER) - Y coordinate
- **Returns**: Nothing
- **Example**: `SDL2_DRAW_POINT(renderer_id, 100, 50)`

### SDL2_DRAW_LINE(renderer_id, x1, y1, x2, y2)
- **Purpose**: Draw a line between two points
- **Parameters**:
  - `renderer_id` (INTEGER) - Target renderer ID
  - `x1` (INTEGER) - Start X coordinate
  - `y1` (INTEGER) - Start Y coordinate
  - `x2` (INTEGER) - End X coordinate
  - `y2` (INTEGER) - End Y coordinate
- **Returns**: Nothing
- **Example**: `SDL2_DRAW_LINE(renderer_id, 0, 0, 100, 100)` // Diagonal line

### SDL2_DRAW_RECT(renderer_id, x, y, w, h)
- **Purpose**: Draw a rectangle outline
- **Parameters**:
  - `renderer_id` (INTEGER) - Target renderer ID
  - `x` (INTEGER) - X coordinate of top-left corner
  - `y` (INTEGER) - Y coordinate of top-left corner
  - `w` (INTEGER) - Width
  - `h` (INTEGER) - Height
- **Returns**: Nothing
- **Example**: `SDL2_DRAW_RECT(renderer_id, 50, 50, 100, 75)` // Rectangle outline

### SDL2_FILL_RECT(renderer_id, x, y, w, h)
- **Purpose**: Draw a filled rectangle
- **Parameters**: Same as SDL2_DRAW_RECT
- **Returns**: Nothing
- **Example**: `SDL2_FILL_RECT(renderer_id, 50, 50, 100, 75)` // Filled rectangle

---

## Event Handling

### SDL2_POLL_EVENT()
- **Purpose**: Check for pending events
- **Parameters**: None
- **Returns**: INTEGER - Event type code, or 0 if no events
- **Example**:
```bcpl
LET event_type = SDL2_POLL_EVENT()
SWITCHON event_type INTO
$(
  CASE 0x100:  // Quit event
    running := FALSE
    ENDCASE
  CASE 0x300:  // Key down
    // Handle key press
    ENDCASE
$)
```

### SDL2_GET_EVENT_KEY()
- **Purpose**: Get the key code from the last keyboard event
- **Parameters**: None
- **Returns**: INTEGER - Key code (SDL scancode)
- **Example**:
```bcpl
IF event_type = 0x300 THEN  // Key down
$(
  LET key = SDL2_GET_EVENT_KEY()
  IF key = 4 THEN           // 'A' key
    WRITES("A pressed*N")
$)
```

### SDL2_GET_EVENT_MOUSE(x_ptr, y_ptr)
- **Purpose**: Get mouse coordinates from the last mouse event
- **Parameters**:
  - `x_ptr` (INTEGER) - Pointer to store X coordinate
  - `y_ptr` (INTEGER) - Pointer to store Y coordinate
- **Returns**: Nothing (coordinates stored at provided addresses)
- **Example**:
```bcpl
LET mouse_x = 0
LET mouse_y = 0
SDL2_GET_EVENT_MOUSE(@mouse_x, @mouse_y)
WRITEF("Mouse at: %n, %n*N", mouse_x, mouse_y)
```

### SDL2_GET_EVENT_BUTTON()
- **Purpose**: Get the mouse button from the last mouse button event
- **Parameters**: None
- **Returns**: INTEGER - Button number (1=left, 2=middle, 3=right)
- **Example**:
```bcpl
IF event_type = 0x401 THEN  // Mouse button down
$(
  LET button = SDL2_GET_EVENT_BUTTON()
  IF button = 1 THEN
    WRITES("Left mouse button pressed*N")
$)
```

---

## Timing Functions

### SDL2_DELAY(milliseconds)
- **Purpose**: Pause execution for specified time
- **Parameters**: `milliseconds` (INTEGER) - Time to wait in milliseconds
- **Returns**: Nothing
- **Example**: `SDL2_DELAY(16)` // ~60 FPS (16.67ms per frame)

### SDL2_GET_TICKS()
- **Purpose**: Get milliseconds since SDL initialization
- **Parameters**: None
- **Returns**: INTEGER - Milliseconds elapsed
- **Example**:
```bcpl
LET start_time = SDL2_GET_TICKS()
// ... do work ...
LET elapsed = SDL2_GET_TICKS() - start_time
WRITEF("Operation took %n ms*N", elapsed)
```

---

## Utility Functions

### SDL2_GET_ERROR()
- **Purpose**: Get the last SDL error message
- **Parameters**: None
- **Returns**: INTEGER - Pointer to error string (BCPL string)
- **Example**:
```bcpl
IF SDL2_INIT() < 0 THEN
$(
  LET error_msg = SDL2_GET_ERROR()
  WRITEF("SDL Error: %s*N", error_msg)
  FINISH()
$)
```

### SDL2_CLEAR_ERROR()
- **Purpose**: Clear the SDL error state
- **Parameters**: None
- **Returns**: Nothing
- **Example**: `SDL2_CLEAR_ERROR()`

---

## Diagnostic Functions

### SDL2_GET_VERSION()
- **Purpose**: Get SDL2 version information
- **Parameters**: None
- **Returns**: INTEGER - Pointer to version string
- **Example**:
```bcpl
LET version = SDL2_GET_VERSION()
WRITEF("SDL2 Version: %s*N", version)
```

### SDL2_GET_VIDEO_DRIVERS()
- **Purpose**: Get list of available video drivers
- **Parameters**: None
- **Returns**: INTEGER - Number of video drivers available
- **Example**: `LET driver_count = SDL2_GET_VIDEO_DRIVERS()`

### SDL2_GET_CURRENT_VIDEO_DRIVER()
- **Purpose**: Get the name of the current video driver
- **Parameters**: None
- **Returns**: INTEGER - Pointer to driver name string
- **Example**:
```bcpl
LET driver = SDL2_GET_CURRENT_VIDEO_DRIVER()
WRITEF("Video Driver: %s*N", driver)
```

### SDL2_GET_DISPLAY_MODES()
- **Purpose**: Get information about available display modes
- **Parameters**: None
- **Returns**: INTEGER - Number of display modes
- **Example**: `LET mode_count = SDL2_GET_DISPLAY_MODES()`

### SDL2_TEST_BASIC()
- **Purpose**: Test basic SDL2 functionality
- **Parameters**: None
- **Returns**: INTEGER - 1 if SDL2 is working, 0 if not available
- **Example**:
```bcpl
IF SDL2_TEST_BASIC() = 0 THEN
$(
  WRITES("SDL2 not available - graphics disabled*N")
  RESULTIS 1
$)
```

---

## Constants Reference

### Event Types
```bcpl
MANIFEST $(
  SDL2_QUIT = 0x100,
  SDL2_KEYDOWN = 0x300,
  SDL2_KEYUP = 0x301,
  SDL2_MOUSEMOTION = 0x400,
  SDL2_MOUSEBUTTONDOWN = 0x401,
  SDL2_MOUSEBUTTONUP = 0x402
$)
```

### Window Flags
```bcpl
MANIFEST $(
  SDL2_WINDOW_FULLSCREEN = 0x00000001,
  SDL2_WINDOW_RESIZABLE = 0x00000020,
  SDL2_WINDOW_SHOWN = 0x00000004,
  SDL2_WINDOW_HIDDEN = 0x00000008,
  SDL2_WINDOW_BORDERLESS = 0x00000010
$)
```

### Renderer Flags
```bcpl
MANIFEST $(
  SDL2_RENDERER_SOFTWARE = 0x00000001,
  SDL2_RENDERER_ACCELERATED = 0x00000002,
  SDL2_RENDERER_PRESENTVSYNC = 0x00000004
$)
```

### Mouse Buttons
```bcpl
MANIFEST $(
  SDL2_BUTTON_LEFT = 1,
  SDL2_BUTTON_MIDDLE = 2,
  SDL2_BUTTON_RIGHT = 3
$)
```

### Common Key Codes (SDL Scancodes)
```bcpl
MANIFEST $(
  SDL2_KEY_A = 4,
  SDL2_KEY_B = 5,
  SDL2_KEY_SPACE = 44,
  SDL2_KEY_ESCAPE = 41,
  SDL2_KEY_RETURN = 40,
  SDL2_KEY_UP = 82,
  SDL2_KEY_DOWN = 81,
  SDL2_KEY_LEFT = 80,
  SDL2_KEY_RIGHT = 79
$)
```

### Special Window Positions
```bcpl
MANIFEST $(
  SDL2_WINDOWPOS_CENTERED = 0x1FFF0000,
  SDL2_WINDOWPOS_UNDEFINED = 0x1FFF0000
$)
```

---

## Complete Example

Here's a complete SDL2 program that creates a window, handles events, and draws animated graphics:

```bcpl
GET "libhdr.h"

MANIFEST $(
  // Event types
  SDL2_QUIT = 0x100,
  SDL2_KEYDOWN = 0x300,
  SDL2_MOUSEBUTTONDOWN = 0x401,
  
  // Keys
  SDL2_KEY_ESCAPE = 41,
  SDL2_KEY_SPACE = 44,
  
  // Colors
  BLACK_R = 0, BLACK_G = 0, BLACK_B = 0,
  RED_R = 255, RED_G = 0, RED_B = 0,
  GREEN_R = 0, GREEN_G = 255, GREEN_B = 0,
  BLUE_R = 0, BLUE_G = 0, BLUE_B = 255,
  WHITE_R = 255, WHITE_G = 255, WHITE_B = 255
$)

LET START() = VALOF
$(
  LET window_id = 0
  LET renderer_id = 0
  LET running = TRUE
  LET rect_x = 100
  LET rect_y = 100
  LET dx = 2
  LET dy = 2
  LET color_r = RED_R
  LET color_g = RED_G
  LET color_b = RED_B
  
  // Check SDL2 availability
  IF SDL2_TEST_BASIC() = 0 THEN
  $(
    WRITES("SDL2 not available*N")
    RESULTIS 1
  $)
  
  // Initialize SDL2
  IF SDL2_INIT() < 0 THEN
  $(
    WRITES("Failed to initialize SDL2*N")
    LET error = SDL2_GET_ERROR()
    WRITEF("Error: %s*N", error)
    RESULTIS 1
  $)
  
  // Create window
  window_id := SDL2_CREATE_WINDOW("Bouncing Rectangle Demo")
  IF window_id = 0 THEN
  $(
    WRITES("Failed to create window*N")
    SDL2_QUIT()
    RESULTIS 1
  $)
  
  // Create renderer
  renderer_id := SDL2_CREATE_RENDERER(window_id)
  IF renderer_id = 0 THEN
  $(
    WRITES("Failed to create renderer*N")
    SDL2_DESTROY_WINDOW(window_id)
    SDL2_QUIT()
    RESULTIS 1
  $)
  
  WRITES("SDL2 Demo running. Press SPACE to change color, ESC to quit.*N")
  
  // Main game loop
  WHILE running DO
  $(
    LET event_type = 0
    
    // Handle events
    event_type := SDL2_POLL_EVENT()
    WHILE event_type <> 0 DO
    $(
      SWITCHON event_type INTO
      $(
        CASE SDL2_QUIT:
          running := FALSE
          ENDCASE
          
        CASE SDL2_KEYDOWN:
          LET key = SDL2_GET_EVENT_KEY()
          SWITCHON key INTO
          $(
            CASE SDL2_KEY_ESCAPE:
              running := FALSE
              ENDCASE
              
            CASE SDL2_KEY_SPACE:
              // Change color randomly
              color_r := RAND(255)
              color_g := RAND(255)
              color_b := RAND(255)
              ENDCASE
          $)
          ENDCASE
          
        CASE SDL2_MOUSEBUTTONDOWN:
          LET mouse_x = 0
          LET mouse_y = 0
          SDL2_GET_EVENT_MOUSE(@mouse_x, @mouse_y)
          WRITEF("Mouse clicked at: %n, %n*N", mouse_x, mouse_y)
          // Move rectangle to mouse position
          rect_x := mouse_x - 25
          rect_y := mouse_y - 25
          ENDCASE
      $)
      
      event_type := SDL2_POLL_EVENT()
    $)
    
    // Update rectangle position
    rect_x := rect_x + dx
    rect_y := rect_y + dy
    
    // Bounce off walls
    IF rect_x <= 0 | rect_x >= 590 THEN dx := -dx
    IF rect_y <= 0 | rect_y >= 430 THEN dy := -dy
    
    // Keep rectangle in bounds
    IF rect_x < 0 THEN rect_x := 0
    IF rect_x > 590 THEN rect_x := 590
    IF rect_y < 0 THEN rect_y := 0
    IF rect_y > 430 THEN rect_y := 430
    
    // Clear screen (black background)
    SDL2_SET_DRAW_COLOR(renderer_id, BLACK_R, BLACK_G, BLACK_B, 255)
    SDL2_CLEAR(renderer_id)
    
    // Draw bouncing rectangle
    SDL2_SET_DRAW_COLOR(renderer_id, color_r, color_g, color_b, 255)
    SDL2_FILL_RECT(renderer_id, rect_x, rect_y, 50, 50)
    
    // Draw border
    SDL2_SET_DRAW_COLOR(renderer_id, WHITE_R, WHITE_G, WHITE_B, 255)
    SDL2_DRAW_RECT(renderer_id, rect_x, rect_y, 50, 50)
    
    // Present the frame
    SDL2_PRESENT(renderer_id)
    
    // Control frame rate (~60 FPS)
    SDL2_DELAY(16)
  $)
  
  // Cleanup
  WRITES("Cleaning up...*N")
  SDL2_DESTROY_RENDERER(renderer_id)
  SDL2_DESTROY_WINDOW(window_id)
  SDL2_QUIT()
  
  WRITES("Demo completed successfully.*N")
  RESULTIS 0
$)
```

---

## Error Handling

### Best Practices

1. **Always check return values**:
```bcpl
IF SDL2_INIT() < 0 THEN
$(
  LET error = SDL2_GET_ERROR()
  WRITEF("SDL Init failed: %s*N", error)
  FINISH()
$)
```

2. **Check for zero window/renderer IDs**:
```bcpl
LET window_id = SDL2_CREATE_WINDOW("Test")
IF window_id = 0 THEN
$(
  WRITES("Failed to create window*N")
  SDL2_QUIT()
  RESULTIS 1
$)
```

3. **Always cleanup resources**:
```bcpl
// Cleanup in reverse order of creation
SDL2_DESTROY_RENDERER(renderer_id)
SDL2_DESTROY_WINDOW(window_id)
SDL2_QUIT()
```

### Common Error Codes

- **Return value 0**: Usually indicates failure for creation functions
- **Return value -1**: Usually indicates failure for initialization functions
- **Empty string**: `SDL2_GET_ERROR()` returns empty string if no error

---

## Performance Tips

### Frame Rate Control
```bcpl
// Target 60 FPS
WHILE running DO
$(
  LET frame_start = SDL2_GET_TICKS()
  
  // ... game logic and rendering ...
  
  LET frame_time = SDL2_GET_TICKS() - frame_start
  IF frame_time < 16 THEN
    SDL2_DELAY(16 - frame_time)
$)
```

### Efficient Rendering
```bcpl
// Group drawing operations
SDL2_SET_DRAW_COLOR(renderer_id, red, green, blue, 255)
SDL2_FILL_RECT(renderer_id, x1, y1, w, h)
SDL2_FILL_RECT(renderer_id, x2, y2, w, h)
SDL2_FILL_RECT(renderer_id, x3, y3, w, h)
// Single present call
SDL2_PRESENT(renderer_id)
```

### Event Handling
```bcpl
// Process all pending events each frame
LET event_type = SDL2_POLL_EVENT()
WHILE event_type <> 0 DO
$(
  // Handle event
  event_type := SDL2_POLL_EVENT()
$)
```

### Memory Management
- SDL2 automatically manages window and renderer resources
- `SDL2_QUIT()` automatically cleans up all remaining resources
- Always call `SDL2_QUIT()` before program termination

---

## Building and Deployment

### Static vs Dynamic Linking

**Static Linking (Recommended)**:
```bash
./buildruntime --with-sdl2-static
# Creates self-contained binary - no SDL2.dylib needed
```

**Dynamic Linking**:
```bash
./buildruntime --with-sdl2
# Requires SDL2.dylib on target systems
```

### Platform Support

- **macOS**: Full support with both static and dynamic linking
- **Linux**: Full support (requires SDL2 development packages)
- **Windows**: Should work with appropriate SDL2 setup

---

*This reference covers all 27 SDL2 functions available in NewBCPL Runtime. For technical details about building and deployment, see `runtime/SDL2/README.md`.*
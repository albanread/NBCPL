NewBCPL/book/runtime_sdl.md
# NewBCPL SDL2 Runtime Module

This document provides a comprehensive overview of the SDL2 runtime module in NewBCPL. It covers the purpose, available commands, usage patterns, integration details, and best practices for using SDL2 features from BCPL code.

---

## Overview

The SDL2 runtime module enables BCPL programs to create interactive graphics applications, games, and multimedia tools by exposing a set of routines that wrap the SDL2 C library. All SDL2 functionality is accessed through the runtime, ensuring portability and consistent API design.

- **Access**: All SDL2 features are available via routines prefixed with `SDL2_`.
- **Integration**: The module is conditionally included at build time (`--with-sdl2` or `--with-sdl2-static`).
- **Design**: The interface is designed for simplicity, hiding C/SDL2 details and providing integer-based handles for windows, renderers, and events.

---

## Building and Enabling SDL2 Support

To use SDL2 features, build the runtime with SDL2 support:

```sh
# Dynamic linking
./buildruntime --with-sdl2

# Static linking (recommended for deployment)
./buildruntime --with-sdl2-static
```

---

## Initialization and Cleanup

### `SDL2_INIT()`
- Initializes the SDL2 video subsystem.
- Returns `0` on success, negative on failure.

### `SDL2_INIT_SUBSYSTEMS(flags)`
- Initializes specific SDL2 subsystems (e.g., video, audio).
- `flags`: Bitmask of subsystem flags (see constants).

### `SDL2_QUIT()`
- Cleans up all SDL2 resources and shuts down subsystems.

---

## Window Management

### `SDL2_CREATE_WINDOW(title)`
- Creates a 640x480 window centered on the screen.
- `title`: BCPL string for the window title.
- Returns a window ID (`>0` on success, `0` on failure).

### `SDL2_CREATE_WINDOW_EX(title, x, y, width, height, flags)`
- Creates a window with custom position, size, and flags.
- `x`, `y`: Position (use `SDL2_WINDOWPOS_CENTERED` for centered).
- `flags`: Window flags (e.g., resizable, fullscreen).

### `SDL2_DESTROY_WINDOW(window_id)`
- Destroys the specified window and frees resources.

### `SDL2_SET_WINDOW_TITLE(window_id, title)`
- Changes the title of an existing window.

### `SDL2_SET_WINDOW_SIZE(window_id, width, height)`
- Resizes the specified window.

---

## Rendering

### `SDL2_CREATE_RENDERER(window_id)`
- Creates a renderer for the given window.
- Returns a renderer ID (`>0` on success, `0` on failure).

### `SDL2_CREATE_RENDERER_EX(window_id, flags)`
- Creates a renderer with custom flags (e.g., accelerated, vsync).

### `SDL2_DESTROY_RENDERER(renderer_id)`
- Destroys the specified renderer.

### `SDL2_SET_DRAW_COLOR(renderer_id, r, g, b, a)`
- Sets the color for subsequent drawing operations.

### `SDL2_CLEAR(renderer_id)`
- Clears the screen with the current draw color.

### `SDL2_PRESENT(renderer_id)`
- Presents the rendered frame (swaps buffers).

---

## Drawing Functions

### `SDL2_DRAW_POINT(renderer_id, x, y)`
- Draws a single point.

### `SDL2_DRAW_LINE(renderer_id, x1, y1, x2, y2)`
- Draws a line between two points.

### `SDL2_DRAW_RECT(renderer_id, x, y, w, h)`
- Draws a rectangle outline.

### `SDL2_FILL_RECT(renderer_id, x, y, w, h)`
- Draws a filled rectangle.

---

## Event Handling

### `SDL2_POLL_EVENT()`
- Polls for pending events.
- Returns an event type code (see constants), or `0` if no events.

### `SDL2_GET_EVENT_KEY()`
- Returns the key code from the last keyboard event.

### `SDL2_GET_EVENT_MOUSE(x_ptr, y_ptr)`
- Stores the mouse coordinates from the last mouse event at the provided addresses.

### `SDL2_GET_EVENT_BUTTON()`
- Returns the mouse button from the last mouse button event.

---

## Timing Functions

### `SDL2_DELAY(milliseconds)`
- Pauses execution for the specified number of milliseconds.

### `SDL2_GET_TICKS()`
- Returns the number of milliseconds since SDL initialization.

---

## Utility and Diagnostic Functions

### `SDL2_GET_ERROR()`
- Returns the last SDL error message as a BCPL string.

### `SDL2_CLEAR_ERROR()`
- Clears the SDL error state.

### `SDL2_GET_VERSION()`
- Returns SDL2 version information as an integer.

### `SDL2_GET_VIDEO_DRIVERS()`
- Returns the number of available video drivers.

### `SDL2_GET_CURRENT_VIDEO_DRIVER()`
- Returns the name of the current video driver.

### `SDL2_GET_DISPLAY_MODES()`
- Returns the number of available display modes.

### `SDL2_TEST_BASIC()`
- Returns `1` if SDL2 is working, `0` if not available.

---

## Constants Reference

- **Event Types**: `SDL2_QUIT`, `SDL2_KEYDOWN`, `SDL2_MOUSEBUTTONDOWN`, etc.
- **Window Flags**: `SDL2_WINDOW_FULLSCREEN`, `SDL2_WINDOW_RESIZABLE`, etc.
- **Renderer Flags**: `SDL2_RENDERER_SOFTWARE`, `SDL2_RENDERER_ACCELERATED`, etc.
- **Mouse Buttons**: `SDL2_BUTTON_LEFT`, `SDL2_BUTTON_MIDDLE`, `SDL2_BUTTON_RIGHT`
- **Key Codes**: `SDL2_KEY_A`, `SDL2_KEY_ESCAPE`, etc.
- **Special Positions**: `SDL2_WINDOWPOS_CENTERED`, `SDL2_WINDOWPOS_UNDEFINED`

See the BCPL manifest or the runtime header for full lists.

---

## Example Usage

```bcpl
LET window_id = SDL2_CREATE_WINDOW("Demo")
LET renderer_id = SDL2_CREATE_RENDERER(window_id)
SDL2_SET_DRAW_COLOR(renderer_id, 255, 0, 0, 255) // Red
SDL2_FILL_RECT(renderer_id, 50, 50, 100, 100)
SDL2_PRESENT(renderer_id)
SDL2_DELAY(1000)
SDL2_DESTROY_RENDERER(renderer_id)
SDL2_DESTROY_WINDOW(window_id)
SDL2_QUIT()
```

---

## Error Handling and Best Practices

- Always check return values for initialization and creation functions.
- Use `SDL2_GET_ERROR()` to retrieve error messages.
- Clean up resources in reverse order of creation.
- Call `SDL2_QUIT()` before program termination to ensure all resources are freed.

---

## Integration Details

- **Runtime Registration**: All SDL2 functions are registered with the runtime at initialization. BCPL code can call them as ordinary routines.
- **Handles**: Windows and renderers are referenced by integer IDs, not pointers.
- **Strings**: All string parameters and results use BCPL string conventions.
- **Portability**: The runtime abstracts away platform-specific SDL2 details.

---

## Advanced Notes

- The SDL2 module is designed to be extensible. Additional features (e.g., image loading, audio, joystick support) can be added as new routines.
- The runtime manages all SDL2 resources and ensures safe cleanup.
- Event handling is non-blocking and must be polled each frame for responsive applications.

---

## Further Reading

- See `SDLfunctions.md` in the project root for a detailed reference and more examples.
- For information on extending the SDL2 module or integrating with other runtime features, see the runtime source code in `runtime/SDL2/`.

---
#ifndef SDL2_RUNTIME_H
#define SDL2_RUNTIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Forward declarations for BCPL runtime integration
typedef struct BCPLString BCPLString;
typedef uint32_t* bcpl_string_t;

// SDL2 Runtime Module - C-linkage interface for BCPL
// These functions provide a simplified SDL2 interface that can be called from BCPL code

// =============================================================================
// INITIALIZATION AND CLEANUP
// =============================================================================

/**
 * Initialize SDL2 video subsystem
 * BCPL Usage: result = SDL2_INIT()
 * Returns: 0 on success, negative value on failure
 */
int64_t SDL2_INIT(void);

/**
 * Initialize SDL2 with specific subsystems
 * BCPL Usage: SDL2_INIT_SUBSYSTEMS(flags)
 * @param flags: SDL subsystem flags (SDL_INIT_VIDEO, SDL_INIT_AUDIO, etc.)
 */
void SDL2_INIT_SUBSYSTEMS(int64_t flags);

/**
 * Quit SDL2 and cleanup all subsystems
 * BCPL Usage: SDL2_QUIT()
 */
void SDL2_QUIT_IMPL(void);

// =============================================================================
// WINDOW MANAGEMENT
// =============================================================================

/**
 * Create a window with default settings (640x480, centered)
 * BCPL Usage: SDL2_CREATE_WINDOW(title)
 * @param title: BCPL string for window title
 * Returns: Window ID (0 on failure)
 */
int64_t SDL2_CREATE_WINDOW(bcpl_string_t title);

/**
 * Create a window with custom settings
 * BCPL Usage: SDL2_CREATE_WINDOW_EX(title, x, y, width, height, flags)
 */
int64_t SDL2_CREATE_WINDOW_EX(bcpl_string_t title, int64_t x, int64_t y, 
                               int64_t width, int64_t height, int64_t flags);

/**
 * Destroy a window
 * BCPL Usage: SDL2_DESTROY_WINDOW(window_id)
 */
void SDL2_DESTROY_WINDOW(int64_t window_id);

/**
 * Set window title
 * BCPL Usage: SDL2_SET_WINDOW_TITLE(window_id, title)
 */
void SDL2_SET_WINDOW_TITLE(int64_t window_id, bcpl_string_t title);

/**
 * Set window size
 * BCPL Usage: SDL2_SET_WINDOW_SIZE(window_id, width, height)
 */
void SDL2_SET_WINDOW_SIZE(int64_t window_id, int64_t width, int64_t height);

// =============================================================================
// RENDERING
// =============================================================================

/**
 * Create a renderer for a window
 * BCPL Usage: SDL2_CREATE_RENDERER(window_id)
 * Returns: Renderer ID (0 on failure)
 */
int64_t SDL2_CREATE_RENDERER(int64_t window_id);

/**
 * Create a renderer with specific flags
 * BCPL Usage: SDL2_CREATE_RENDERER_EX(window_id, flags)
 */
int64_t SDL2_CREATE_RENDERER_EX(int64_t window_id, int64_t flags);

/**
 * Destroy a renderer
 * BCPL Usage: SDL2_DESTROY_RENDERER(renderer_id)
 */
void SDL2_DESTROY_RENDERER(int64_t renderer_id);

/**
 * Set render draw color
 * BCPL Usage: SDL2_SET_DRAW_COLOR(renderer_id, r, g, b, a)
 */
void SDL2_SET_DRAW_COLOR(int64_t renderer_id, int64_t r, int64_t g, int64_t b, int64_t a);

/**
 * Clear the screen with current draw color
 * BCPL Usage: SDL2_CLEAR(renderer_id)
 */
void SDL2_CLEAR(int64_t renderer_id);

/**
 * Present the rendered frame
 * BCPL Usage: SDL2_PRESENT(renderer_id)
 */
void SDL2_PRESENT(int64_t renderer_id);

/**
 * Draw a point
 * BCPL Usage: SDL2_DRAW_POINT(renderer_id, x, y)
 */
void SDL2_DRAW_POINT(int64_t renderer_id, int64_t x, int64_t y);

/**
 * Draw a line
 * BCPL Usage: SDL2_DRAW_LINE(renderer_id, x1, y1, x2, y2)
 */
void SDL2_DRAW_LINE(int64_t renderer_id, int64_t x1, int64_t y1, int64_t x2, int64_t y2);

/**
 * Draw a rectangle outline
 * BCPL Usage: SDL2_DRAW_RECT(renderer_id, x, y, w, h)
 */
void SDL2_DRAW_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);

/**
 * Fill a rectangle
 * BCPL Usage: SDL2_FILL_RECT(renderer_id, x, y, w, h)
 */
void SDL2_FILL_RECT(int64_t renderer_id, int64_t x, int64_t y, int64_t w, int64_t h);

// =============================================================================
// EVENT HANDLING
// =============================================================================

/**
 * Poll for events
 * BCPL Usage: event_type = SDL2_POLL_EVENT()
 * Returns: Event type (0 if no events)
 */
int64_t SDL2_POLL_EVENT(void);

/**
 * Get the last event's key code (for keyboard events)
 * BCPL Usage: key = SDL2_GET_EVENT_KEY()
 */
int64_t SDL2_GET_EVENT_KEY(void);

/**
 * Get the last event's mouse coordinates
 * BCPL Usage: SDL2_GET_EVENT_MOUSE(x_ptr, y_ptr)
 */
void SDL2_GET_EVENT_MOUSE(int64_t* x, int64_t* y);

/**
 * Get the last event's mouse button
 * BCPL Usage: button = SDL2_GET_EVENT_BUTTON()
 */
int64_t SDL2_GET_EVENT_BUTTON(void);

// =============================================================================
// TIMING
// =============================================================================

/**
 * Delay execution for specified milliseconds
 * BCPL Usage: SDL2_DELAY(milliseconds)
 */
void SDL2_DELAY(int64_t milliseconds);

/**
 * Get ticks since SDL initialization
 * BCPL Usage: ticks = SDL2_GET_TICKS()
 */
int64_t SDL2_GET_TICKS(void);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * Check if there was an SDL error and get error string
 * BCPL Usage: error_string = SDL2_GET_ERROR()
 * Returns: BCPL string with error message (empty if no error)
 */
bcpl_string_t SDL2_GET_ERROR(void);

/**
 * Clear any pending SDL error
 * BCPL Usage: SDL2_CLEAR_ERROR()
 */
void SDL2_CLEAR_ERROR(void);

// =============================================================================
// DIAGNOSTIC FUNCTIONS (for testing SDL2 functionality)
// =============================================================================

/**
 * Get SDL2 version information
 * BCPL Usage: SDL2_GET_VERSION()
 * Returns: Version as integer (major * 1000 + minor * 100 + patch)
 */
int64_t SDL2_GET_VERSION(void);

/**
 * Get number of available video drivers
 * BCPL Usage: SDL2_GET_VIDEO_DRIVERS()
 * Returns: Number of video drivers available
 */
int64_t SDL2_GET_VIDEO_DRIVERS(void);

/**
 * Get current video driver name
 * BCPL Usage: SDL2_GET_CURRENT_VIDEO_DRIVER()
 * Returns: BCPL string with current video driver name (empty if none)
 */
bcpl_string_t SDL2_GET_CURRENT_VIDEO_DRIVER(void);

/**
 * Get number of available display modes for display 0
 * BCPL Usage: SDL2_GET_DISPLAY_MODES()
 * Returns: Number of display modes available
 */
int64_t SDL2_GET_DISPLAY_MODES(void);

/**
 * Test basic SDL2 functionality without creating windows
 * BCPL Usage: SDL2_TEST_BASIC()
 * Returns: 0 if basic functionality works, negative on error
 */
int64_t SDL2_TEST_BASIC(void);

// =============================================================================
// SDL CONSTANTS (for BCPL usage)
// =============================================================================

// SDL_Init flags
#define SDL2_INIT_TIMER          0x00000001u
#define SDL2_INIT_AUDIO          0x00000010u
#define SDL2_INIT_VIDEO          0x00000020u
#define SDL2_INIT_JOYSTICK       0x00000200u
#define SDL2_INIT_HAPTIC         0x00001000u
#define SDL2_INIT_GAMECONTROLLER 0x00002000u
#define SDL2_INIT_EVENTS         0x00004000u
#define SDL2_INIT_EVERYTHING     0x0000FFFFu

// Window flags
#define SDL2_WINDOW_FULLSCREEN    0x00000001
#define SDL2_WINDOW_RESIZABLE     0x00000020
#define SDL2_WINDOW_MINIMIZED     0x00000040
#define SDL2_WINDOW_MAXIMIZED     0x00000080
#define SDL2_WINDOW_SHOWN         0x00000004
#define SDL2_WINDOW_HIDDEN        0x00000008

// Renderer flags
#define SDL2_RENDERER_SOFTWARE      0x00000001
#define SDL2_RENDERER_ACCELERATED   0x00000002
#define SDL2_RENDERER_PRESENTVSYNC  0x00000004

// Event types
#define SDL2_EVENT_QUIT           0x100
#define SDL2_KEYDOWN        0x300
#define SDL2_KEYUP          0x301
#define SDL2_MOUSEBUTTONDOWN 0x401
#define SDL2_MOUSEBUTTONUP   0x402
#define SDL2_MOUSEMOTION     0x400

// Mouse buttons
#define SDL2_BUTTON_LEFT    1
#define SDL2_BUTTON_MIDDLE  2
#define SDL2_BUTTON_RIGHT   3

// Special window positions
#define SDL2_WINDOWPOS_UNDEFINED 0x1FFF0000u
#define SDL2_WINDOWPOS_CENTERED  0x2FFF0000u

#ifdef __cplusplus
}
#endif

#endif // SDL2_RUNTIME_H
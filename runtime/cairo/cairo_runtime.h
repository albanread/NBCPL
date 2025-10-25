#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Forward declarations for BCPL integration
typedef uint32_t* bcpl_string_t;
typedef uint32_t* bcpl_vector_t;

// Handle types for SAMM integration
typedef uint64_t CairoSurfaceHandle;
typedef uint64_t CairoImageHandle;
typedef uint64_t CairoPatternHandle;

// =============================================================================
// SURFACE MANAGEMENT
// =============================================================================

/**
 * Create new Cairo drawing surface
 * BCPL Usage: surface = CAIRO_CREATE_SURFACE(width, height)
 * @param width Surface width in pixels
 * @param height Surface height in pixels
 * @return Surface handle (0 on failure)
 */
CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height);

/**
 * Load PNG image as Cairo surface
 * BCPL Usage: surface = CAIRO_LOAD_PNG(filename)
 * @param filename BCPL string path to PNG file
 * @return Surface handle (0 on failure)
 */
CairoSurfaceHandle CAIRO_LOAD_PNG(bcpl_string_t filename);

/**
 * Save Cairo surface to PNG file
 * BCPL Usage: CAIRO_SAVE_PNG(surface, filename)
 * @param surface_handle Handle to surface
 * @param filename BCPL string path for output file
 */
void CAIRO_SAVE_PNG(CairoSurfaceHandle surface_handle, bcpl_string_t filename);

/**
 * Get surface dimensions
 * BCPL Usage: CAIRO_GET_SIZE(surface, width_ptr, height_ptr)
 * @param surface_handle Handle to surface
 * @param width Pointer to receive width
 * @param height Pointer to receive height
 */
void CAIRO_GET_SIZE(CairoSurfaceHandle surface_handle, int64_t* width, int64_t* height);

/**
 * Clear surface with solid color
 * BCPL Usage: CAIRO_CLEAR(surface, rgba_color)
 * @param surface_handle Handle to surface
 * @param rgba_color Color in RGBA format (0xRRGGBBAA)
 */
void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);

/**
 * Clone surface for double-buffering
 * BCPL Usage: new_surface = CAIRO_CLONE_SURFACE(source_surface)
 * @param source_handle Handle to source surface
 * @return Handle to cloned surface (0 on failure)
 */
CairoSurfaceHandle CAIRO_CLONE_SURFACE(CairoSurfaceHandle source_handle);

// =============================================================================
// DRAWING STATE
// =============================================================================

/**
 * Set drawing color for fill and stroke operations
 * BCPL Usage: CAIRO_SET_COLOR(surface, rgba_color)
 * @param surface_handle Handle to surface
 * @param rgba_color Color in RGBA format (0xRRGGBBAA)
 */
void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);

/**
 * Set line width for stroke operations
 * BCPL Usage: CAIRO_SET_LINE_WIDTH(surface, width)
 * @param surface_handle Handle to surface
 * @param width Line width in pixels
 */
void CAIRO_SET_LINE_WIDTH(CairoSurfaceHandle surface_handle, double width);

/**
 * Set line join style
 * BCPL Usage: CAIRO_SET_LINE_JOIN(surface, join_style)
 * @param surface_handle Handle to surface
 * @param join_style Join style (0=miter, 1=round, 2=bevel)
 */
void CAIRO_SET_LINE_JOIN(CairoSurfaceHandle surface_handle, int64_t join_style);

/**
 * Set line cap style
 * BCPL Usage: CAIRO_SET_LINE_CAP(surface, cap_style)
 * @param surface_handle Handle to surface
 * @param cap_style Cap style (0=butt, 1=round, 2=square)
 */
void CAIRO_SET_LINE_CAP(CairoSurfaceHandle surface_handle, int64_t cap_style);

/**
 * Set opacity for all drawing operations
 * BCPL Usage: CAIRO_SET_OPACITY(surface, opacity)
 * @param surface_handle Handle to surface
 * @param opacity Opacity value (0.0 to 1.0)
 */
void CAIRO_SET_OPACITY(CairoSurfaceHandle surface_handle, double opacity);

// =============================================================================
// BASIC SHAPES
// =============================================================================

/**
 * Fill rectangle with current color
 * BCPL Usage: CAIRO_FILL_RECT(surface, x, y, width, height)
 */
void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);

/**
 * Draw rectangle outline with current color
 * BCPL Usage: CAIRO_STROKE_RECT(surface, x, y, width, height)
 */
void CAIRO_STROKE_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);

/**
 * Fill circle with current color
 * BCPL Usage: CAIRO_FILL_CIRCLE(surface, cx, cy, radius)
 */
void CAIRO_FILL_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);

/**
 * Draw circle outline with current color
 * BCPL Usage: CAIRO_STROKE_CIRCLE(surface, cx, cy, radius)
 */
void CAIRO_STROKE_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);

/**
 * Draw line between two points
 * BCPL Usage: CAIRO_DRAW_LINE(surface, x1, y1, x2, y2)
 */
void CAIRO_DRAW_LINE(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2);

/**
 * Draw single point (small filled square)
 * BCPL Usage: CAIRO_DRAW_POINT(surface, x, y)
 */
void CAIRO_DRAW_POINT(CairoSurfaceHandle surface_handle, double x, double y);

// =============================================================================
// VECTOR OPERATIONS (BATCH PROCESSING)
// =============================================================================

/**
 * Draw multiple lines from vector of point pairs
 * BCPL Usage: CAIRO_DRAW_LINES_VEC(surface, point_vector)
 * Vector format: [x1, y1, x2, y2, x3, y3, x4, y4, ...] (pairs of endpoints)
 */
void CAIRO_DRAW_LINES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t point_pairs);

/**
 * Fill multiple rectangles from vector of rect quads
 * BCPL Usage: CAIRO_FILL_RECTS_VEC(surface, rect_vector)
 * Vector format: [x1, y1, w1, h1, x2, y2, w2, h2, ...] (quads of x,y,width,height)
 */
void CAIRO_FILL_RECTS_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t rect_quads);

/**
 * Fill multiple circles from vector of circle data
 * BCPL Usage: CAIRO_FILL_CIRCLES_VEC(surface, circle_vector)
 * Vector format: [cx1, cy1, r1, cx2, cy2, r2, ...] (triples of center_x,center_y,radius)
 */
void CAIRO_FILL_CIRCLES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t circle_triples);

/**
 * Draw polyline from vector of points
 * BCPL Usage: CAIRO_DRAW_POLYLINE(surface, point_vector, close_path)
 * Vector format: [x1, y1, x2, y2, x3, y3, ...] (pairs of coordinates)
 */
void CAIRO_DRAW_POLYLINE(CairoSurfaceHandle surface_handle, bcpl_vector_t points, int64_t close_path);

/**
 * Fill polygon from vector of points
 * BCPL Usage: CAIRO_FILL_POLYGON(surface, point_vector)
 * Vector format: [x1, y1, x2, y2, x3, y3, ...] (pairs of coordinates)
 */
void CAIRO_FILL_POLYGON(CairoSurfaceHandle surface_handle, bcpl_vector_t points);

// =============================================================================
// PATH OPERATIONS
// =============================================================================

/**
 * Begin new path (clear current path)
 * BCPL Usage: CAIRO_BEGIN_PATH(surface)
 */
void CAIRO_BEGIN_PATH(CairoSurfaceHandle surface_handle);

/**
 * Move to point without drawing
 * BCPL Usage: CAIRO_MOVE_TO(surface, x, y)
 */
void CAIRO_MOVE_TO(CairoSurfaceHandle surface_handle, double x, double y);

/**
 * Draw line to point from current position
 * BCPL Usage: CAIRO_LINE_TO(surface, x, y)
 */
void CAIRO_LINE_TO(CairoSurfaceHandle surface_handle, double x, double y);

/**
 * Draw cubic Bezier curve
 * BCPL Usage: CAIRO_CURVE_TO(surface, x1, y1, x2, y2, x3, y3)
 */
void CAIRO_CURVE_TO(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2, double x3, double y3);

/**
 * Close current path
 * BCPL Usage: CAIRO_CLOSE_PATH(surface)
 */
void CAIRO_CLOSE_PATH(CairoSurfaceHandle surface_handle);

/**
 * Fill current path
 * BCPL Usage: CAIRO_FILL_PATH(surface)
 */
void CAIRO_FILL_PATH(CairoSurfaceHandle surface_handle);

/**
 * Stroke current path
 * BCPL Usage: CAIRO_STROKE_PATH(surface)
 */
void CAIRO_STROKE_PATH(CairoSurfaceHandle surface_handle);

// =============================================================================
// TEXT RENDERING
// =============================================================================

/**
 * Set font family and size
 * BCPL Usage: CAIRO_SET_FONT(surface, font_name, font_size)
 */
void CAIRO_SET_FONT(CairoSurfaceHandle surface_handle, bcpl_string_t font_name, double font_size);

/**
 * Draw text at position with current color
 * BCPL Usage: CAIRO_DRAW_TEXT(surface, x, y, text)
 */
void CAIRO_DRAW_TEXT(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text);

/**
 * Get text dimensions for layout planning
 * BCPL Usage: CAIRO_TEXT_SIZE(surface, text, width_ptr, height_ptr)
 */
void CAIRO_TEXT_SIZE(CairoSurfaceHandle surface_handle, bcpl_string_t text, double* width, double* height);

/**
 * Draw text with specific color (override current color)
 * BCPL Usage: CAIRO_DRAW_TEXT_COLORED(surface, x, y, text, rgba_color)
 */
void CAIRO_DRAW_TEXT_COLORED(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text, uint32_t rgba_color);

// =============================================================================
// IMAGE OPERATIONS
// =============================================================================

/**
 * Load image from PNG file
 * BCPL Usage: image = CAIRO_LOAD_IMAGE(filename)
 * @param filename BCPL string path to PNG file
 * @return Image handle (0 on failure)
 */
CairoImageHandle CAIRO_LOAD_IMAGE(bcpl_string_t filename);

/**
 * Draw image onto surface at position
 * BCPL Usage: CAIRO_DRAW_IMAGE(surface, image, x, y)
 */
void CAIRO_DRAW_IMAGE(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y);

/**
 * Draw image with scaling
 * BCPL Usage: CAIRO_DRAW_IMAGE_SCALED(surface, image, x, y, scale_x, scale_y)
 */
void CAIRO_DRAW_IMAGE_SCALED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double scale_x, double scale_y);

/**
 * Draw image with rotation (angle in radians)
 * BCPL Usage: CAIRO_DRAW_IMAGE_ROTATED(surface, image, x, y, angle)
 */
void CAIRO_DRAW_IMAGE_ROTATED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double angle);

/**
 * Get image dimensions
 * BCPL Usage: CAIRO_IMAGE_SIZE(image, width_ptr, height_ptr)
 */
void CAIRO_IMAGE_SIZE(CairoImageHandle image_handle, int64_t* width, int64_t* height);

// =============================================================================
// SDL2 DISPLAY INTEGRATION
// =============================================================================

/**
 * Display Cairo surface in existing SDL2 window
 * BCPL Usage: CAIRO_DISPLAY_SDL(surface, sdl_window_id)
 */
void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);

/**
 * Create SDL2 window optimized for Cairo display
 * BCPL Usage: window = CAIRO_CREATE_SDL_WINDOW(title, width, height)
 * @return SDL2 window ID (0 on failure)
 */
int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height);

/**
 * Update SDL2 window with Cairo surface (for animation)
 * BCPL Usage: CAIRO_UPDATE_SDL_WINDOW(surface, sdl_window_id)
 */
void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);

/**
 * Convert Cairo surface to SDL2 texture for efficient blitting
 * BCPL Usage: texture = CAIRO_TO_SDL_TEXTURE(surface, sdl_renderer_id)
 * @return SDL2 texture ID (0 on failure)
 */
int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id);

// =============================================================================
// TRANSFORMATION OPERATIONS
// =============================================================================

/**
 * Save current transformation state
 * BCPL Usage: CAIRO_SAVE(surface)
 */
void CAIRO_SAVE(CairoSurfaceHandle surface_handle);

/**
 * Restore previous transformation state
 * BCPL Usage: CAIRO_RESTORE(surface)
 */
void CAIRO_RESTORE(CairoSurfaceHandle surface_handle);

/**
 * Translate coordinate system
 * BCPL Usage: CAIRO_TRANSLATE(surface, tx, ty)
 */
void CAIRO_TRANSLATE(CairoSurfaceHandle surface_handle, double tx, double ty);

/**
 * Scale coordinate system
 * BCPL Usage: CAIRO_SCALE(surface, sx, sy)
 */
void CAIRO_SCALE(CairoSurfaceHandle surface_handle, double sx, double sy);

/**
 * Rotate coordinate system (angle in radians)
 * BCPL Usage: CAIRO_ROTATE(surface, angle)
 */
void CAIRO_ROTATE(CairoSurfaceHandle surface_handle, double angle);

// =============================================================================
// RESOURCE MANAGEMENT
// =============================================================================

/**
 * Manually release surface (normally handled by SAMM)
 * BCPL Usage: CAIRO_RELEASE_SURFACE(surface)
 */
void CAIRO_RELEASE_SURFACE(CairoSurfaceHandle surface_handle);

/**
 * Manually release image (normally handled by SAMM)
 * BCPL Usage: CAIRO_RELEASE_IMAGE(image)
 */
void CAIRO_RELEASE_IMAGE(CairoImageHandle image_handle);

/**
 * Get number of active surfaces (debugging)
 * BCPL Usage: count = CAIRO_SURFACE_COUNT()
 */
int64_t CAIRO_SURFACE_COUNT(void);

/**
 * Get number of active images (debugging)
 * BCPL Usage: count = CAIRO_IMAGE_COUNT()
 */
int64_t CAIRO_IMAGE_COUNT(void);

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

/**
 * Check if Cairo is properly initialized
 * BCPL Usage: result = CAIRO_IS_AVAILABLE()
 * @return 1 if available, 0 if not
 */
int64_t CAIRO_IS_AVAILABLE(void);

/**
 * Get Cairo version string
 * BCPL Usage: version = CAIRO_GET_VERSION()
 * @return BCPL string with version info
 */
bcpl_string_t CAIRO_GET_VERSION(void);

/**
 * Get last Cairo error message
 * BCPL Usage: error = CAIRO_GET_ERROR()
 * @return BCPL string with error message (empty if no error)
 */
bcpl_string_t CAIRO_GET_ERROR(void);

/**
 * Clear any pending Cairo error
 * BCPL Usage: CAIRO_CLEAR_ERROR()
 */
void CAIRO_CLEAR_ERROR(void);

#ifdef __cplusplus
}
#endif
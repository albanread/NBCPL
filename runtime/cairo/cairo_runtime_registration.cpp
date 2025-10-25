// Cairo Graphics Runtime Registration for BCPL
// This file contains the runtime registration entries that should be added to runtime_api.cpp

#include "cairo_runtime.h"

// Add these entries to the g_runtime_manifest array in runtime_api.cpp:

// =============================================================================
// SURFACE MANAGEMENT
// =============================================================================

{
    "CAIRO_CREATE_SURFACE", "_CAIRO_CREATE_SURFACE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SURFACE), 2,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Create new Cairo drawing surface (width, height)", "GRAPHICS"
},

{
    "CAIRO_LOAD_PNG", "_CAIRO_LOAD_PNG",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_PNG), 1,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Load PNG image as Cairo surface", "GRAPHICS"
},

{
    "CAIRO_SAVE_PNG", "_CAIRO_SAVE_PNG",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAVE_PNG), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Save Cairo surface to PNG file (surface, filename)", "GRAPHICS"
},

{
    "CAIRO_GET_SIZE", "_CAIRO_GET_SIZE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_SIZE), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Get surface dimensions (surface, width_ptr, height_ptr)", "GRAPHICS"
},

{
    "CAIRO_CLEAR", "_CAIRO_CLEAR",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLEAR), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Clear surface with solid color (surface, rgba_color)", "GRAPHICS"
},

{
    "CAIRO_CLONE_SURFACE", "_CAIRO_CLONE_SURFACE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLONE_SURFACE), 1,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Clone surface for double-buffering", "GRAPHICS"
},

// =============================================================================
// DRAWING STATE
// =============================================================================

{
    "CAIRO_SET_COLOR", "_CAIRO_SET_COLOR",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_COLOR), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set drawing color (surface, rgba_color)", "GRAPHICS"
},

{
    "CAIRO_SET_LINE_WIDTH", "_CAIRO_SET_LINE_WIDTH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_WIDTH), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set line width for strokes (surface, width)", "GRAPHICS"
},

{
    "CAIRO_SET_LINE_JOIN", "_CAIRO_SET_LINE_JOIN",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_JOIN), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set line join style (surface, join_style)", "GRAPHICS"
},

{
    "CAIRO_SET_LINE_CAP", "_CAIRO_SET_LINE_CAP",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_LINE_CAP), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set line cap style (surface, cap_style)", "GRAPHICS"
},

{
    "CAIRO_SET_OPACITY", "_CAIRO_SET_OPACITY",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_OPACITY), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set opacity for drawing operations (surface, opacity)", "GRAPHICS"
},

// =============================================================================
// BASIC SHAPES
// =============================================================================

{
    "CAIRO_FILL_RECT", "_CAIRO_FILL_RECT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECT), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill rectangle (surface, x, y, width, height)", "GRAPHICS"
},

{
    "CAIRO_STROKE_RECT", "_CAIRO_STROKE_RECT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_RECT), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw rectangle outline (surface, x, y, width, height)", "GRAPHICS"
},

{
    "CAIRO_FILL_CIRCLE", "_CAIRO_FILL_CIRCLE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_CIRCLE), 4,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill circle (surface, cx, cy, radius)", "GRAPHICS"
},

{
    "CAIRO_STROKE_CIRCLE", "_CAIRO_STROKE_CIRCLE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_CIRCLE), 4,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw circle outline (surface, cx, cy, radius)", "GRAPHICS"
},

{
    "CAIRO_DRAW_LINE", "_CAIRO_DRAW_LINE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINE), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw line between points (surface, x1, y1, x2, y2)", "GRAPHICS"
},

{
    "CAIRO_DRAW_POINT", "_CAIRO_DRAW_POINT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_POINT), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw single point (surface, x, y)", "GRAPHICS"
},

// =============================================================================
// VECTOR OPERATIONS
// =============================================================================

{
    "CAIRO_DRAW_LINES_VEC", "_CAIRO_DRAW_LINES_VEC",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINES_VEC), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw multiple lines from vector of point pairs", "GRAPHICS"
},

{
    "CAIRO_FILL_RECTS_VEC", "_CAIRO_FILL_RECTS_VEC",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECTS_VEC), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill multiple rectangles from vector of rect quads", "GRAPHICS"
},

{
    "CAIRO_FILL_CIRCLES_VEC", "_CAIRO_FILL_CIRCLES_VEC",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_CIRCLES_VEC), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill multiple circles from vector of circle data", "GRAPHICS"
},

{
    "CAIRO_DRAW_POLYLINE", "_CAIRO_DRAW_POLYLINE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_POLYLINE), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw polyline from vector of points (surface, points, close)", "GRAPHICS"
},

{
    "CAIRO_FILL_POLYGON", "_CAIRO_FILL_POLYGON",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_POLYGON), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill polygon from vector of points", "GRAPHICS"
},

// =============================================================================
// PATH OPERATIONS
// =============================================================================

{
    "CAIRO_BEGIN_PATH", "_CAIRO_BEGIN_PATH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_BEGIN_PATH), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Begin new path (surface)", "GRAPHICS"
},

{
    "CAIRO_MOVE_TO", "_CAIRO_MOVE_TO",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_MOVE_TO), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Move to point without drawing (surface, x, y)", "GRAPHICS"
},

{
    "CAIRO_LINE_TO", "_CAIRO_LINE_TO",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LINE_TO), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw line to point (surface, x, y)", "GRAPHICS"
},

{
    "CAIRO_CURVE_TO", "_CAIRO_CURVE_TO",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CURVE_TO), 7,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw cubic Bezier curve (surface, x1, y1, x2, y2, x3, y3)", "GRAPHICS"
},

{
    "CAIRO_CLOSE_PATH", "_CAIRO_CLOSE_PATH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLOSE_PATH), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Close current path (surface)", "GRAPHICS"
},

{
    "CAIRO_FILL_PATH", "_CAIRO_FILL_PATH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_PATH), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill current path (surface)", "GRAPHICS"
},

{
    "CAIRO_STROKE_PATH", "_CAIRO_STROKE_PATH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_STROKE_PATH), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Stroke current path (surface)", "GRAPHICS"
},

// =============================================================================
// TEXT RENDERING
// =============================================================================

{
    "CAIRO_SET_FONT", "_CAIRO_SET_FONT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_FONT), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Set font family and size (surface, font_name, font_size)", "GRAPHICS"
},

{
    "CAIRO_DRAW_TEXT", "_CAIRO_DRAW_TEXT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_TEXT), 4,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw text at position (surface, x, y, text)", "GRAPHICS"
},

{
    "CAIRO_TEXT_SIZE", "_CAIRO_TEXT_SIZE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TEXT_SIZE), 4,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Get text dimensions (surface, text, width_ptr, height_ptr)", "GRAPHICS"
},

{
    "CAIRO_DRAW_TEXT_COLORED", "_CAIRO_DRAW_TEXT_COLORED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_TEXT_COLORED), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw text with specific color (surface, x, y, text, rgba_color)", "GRAPHICS"
},

// =============================================================================
// IMAGE OPERATIONS
// =============================================================================

{
    "CAIRO_LOAD_IMAGE", "_CAIRO_LOAD_IMAGE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_IMAGE), 1,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Load PNG image file", "GRAPHICS"
},

{
    "CAIRO_DRAW_IMAGE", "_CAIRO_DRAW_IMAGE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE), 4,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw image on surface (surface, image, x, y)", "GRAPHICS"
},

{
    "CAIRO_DRAW_IMAGE_SCALED", "_CAIRO_DRAW_IMAGE_SCALED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE_SCALED), 6,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw scaled image (surface, image, x, y, scale_x, scale_y)", "GRAPHICS"
},

{
    "CAIRO_DRAW_IMAGE_ROTATED", "_CAIRO_DRAW_IMAGE_ROTATED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_IMAGE_ROTATED), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw rotated image (surface, image, x, y, angle)", "GRAPHICS"
},

{
    "CAIRO_IMAGE_SIZE", "_CAIRO_IMAGE_SIZE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IMAGE_SIZE), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Get image dimensions (image, width_ptr, height_ptr)", "GRAPHICS"
},

// =============================================================================
// SDL2 DISPLAY INTEGRATION
// =============================================================================

{
    "CAIRO_DISPLAY_SDL", "_CAIRO_DISPLAY_SDL",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DISPLAY_SDL), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Display Cairo surface in SDL2 window", "GRAPHICS"
},

{
    "CAIRO_CREATE_SDL_WINDOW", "_CAIRO_CREATE_SDL_WINDOW",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SDL_WINDOW), 3,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Create SDL2 window for Cairo display (title, width, height)", "GRAPHICS"
},

{
    "CAIRO_UPDATE_SDL_WINDOW", "_CAIRO_UPDATE_SDL_WINDOW",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_UPDATE_SDL_WINDOW), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Update SDL2 window with Cairo surface", "GRAPHICS"
},

{
    "CAIRO_TO_SDL_TEXTURE", "_CAIRO_TO_SDL_TEXTURE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TO_SDL_TEXTURE), 2,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Convert Cairo surface to SDL2 texture", "GRAPHICS"
},

// =============================================================================
// TRANSFORMATION OPERATIONS
// =============================================================================

{
    "CAIRO_SAVE", "_CAIRO_SAVE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAVE), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Save current transformation state", "GRAPHICS"
},

{
    "CAIRO_RESTORE", "_CAIRO_RESTORE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RESTORE), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Restore previous transformation state", "GRAPHICS"
},

{
    "CAIRO_TRANSLATE", "_CAIRO_TRANSLATE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_TRANSLATE), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Translate coordinate system (surface, tx, ty)", "GRAPHICS"
},

{
    "CAIRO_SCALE", "_CAIRO_SCALE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SCALE), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Scale coordinate system (surface, sx, sy)", "GRAPHICS"
},

{
    "CAIRO_ROTATE", "_CAIRO_ROTATE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_ROTATE), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Rotate coordinate system (surface, angle)", "GRAPHICS"
},

// =============================================================================
// RESOURCE MANAGEMENT
// =============================================================================

{
    "CAIRO_RELEASE_SURFACE", "_CAIRO_RELEASE_SURFACE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RELEASE_SURFACE), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Manually release surface (normally handled by SAMM)", "GRAPHICS"
},

{
    "CAIRO_RELEASE_IMAGE", "_CAIRO_RELEASE_IMAGE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_RELEASE_IMAGE), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Manually release image (normally handled by SAMM)", "GRAPHICS"
},

{
    "CAIRO_SURFACE_COUNT", "_CAIRO_SURFACE_COUNT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SURFACE_COUNT), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Get number of active surfaces (debugging)", "GRAPHICS"
},

{
    "CAIRO_IMAGE_COUNT", "_CAIRO_IMAGE_COUNT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IMAGE_COUNT), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Get number of active images (debugging)", "GRAPHICS"
},

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

{
    "CAIRO_IS_AVAILABLE", "_CAIRO_IS_AVAILABLE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_IS_AVAILABLE), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Check if Cairo graphics is available", "GRAPHICS"
},

{
    "CAIRO_VERSION", "_CAIRO_VERSION",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_VERSION), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
    "Get Cairo version string", "GRAPHICS"
},

{
    "CAIRO_GET_ERROR", "_CAIRO_GET_ERROR",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_GET_ERROR), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::STRING,
    "Get last Cairo error message", "GRAPHICS"
},

{
    "CAIRO_CLEAR_ERROR", "_CAIRO_CLEAR_ERROR",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLEAR_ERROR), 0,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Clear any pending Cairo error", "GRAPHICS"
},

// =============================================================================
// SAMM INTEGRATION FUNCTIONS
// =============================================================================

{
    "CAIRO_CREATE_SURFACE_RETAINED", "_CAIRO_CREATE_SURFACE_RETAINED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SURFACE_RETAINED), 3,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Create surface in parent scope (width, height, parent_offset)", "GRAPHICS"
},

{
    "CAIRO_LOAD_PNG_RETAINED", "_CAIRO_LOAD_PNG_RETAINED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_PNG_RETAINED), 2,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Load PNG in parent scope (filename, parent_offset)", "GRAPHICS"
},

{
    "CAIRO_LOAD_IMAGE_RETAINED", "_CAIRO_LOAD_IMAGE_RETAINED",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_LOAD_IMAGE_RETAINED), 2,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Load image in parent scope (filename, parent_offset)", "GRAPHICS"
},

{
    "CAIRO_SAMM_SCOPE_DEPTH", "_CAIRO_SAMM_SCOPE_DEPTH",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_SCOPE_DEPTH), 0,
    RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
    "Get current SAMM scope depth for Cairo", "GRAPHICS"
},

{
    "CAIRO_SAMM_FORCE_CLEANUP", "_CAIRO_SAMM_FORCE_CLEANUP",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_FORCE_CLEANUP), 0,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Force immediate Cairo resource cleanup", "GRAPHICS"
},

{
    "CAIRO_SAMM_GET_STATS", "_CAIRO_SAMM_GET_STATS",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_GET_STATS), 3,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Get SAMM statistics (surfaces_ptr, images_ptr, memory_ptr)", "GRAPHICS"
},

{
    "CAIRO_SAMM_SET_TRACE", "_CAIRO_SAMM_SET_TRACE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_SET_TRACE), 1,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Enable/disable SAMM debug tracing (enabled)", "GRAPHICS"
},

{
    "CAIRO_SAMM_DUMP_STATE", "_CAIRO_SAMM_DUMP_STATE",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_DUMP_STATE), 0,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Dump current SAMM state to stdout", "GRAPHICS"
},

// Add corresponding forward declarations to the extern "C" block at the top of runtime_api.cpp:

/*
extern "C" {
    // ... existing declarations ...

    // Cairo Graphics Functions
    CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height);
    CairoSurfaceHandle CAIRO_LOAD_PNG(bcpl_string_t filename);
    void CAIRO_SAVE_PNG(CairoSurfaceHandle surface_handle, bcpl_string_t filename);
    void CAIRO_GET_SIZE(CairoSurfaceHandle surface_handle, int64_t* width, int64_t* height);
    void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
    CairoSurfaceHandle CAIRO_CLONE_SURFACE(CairoSurfaceHandle source_handle);
    
    void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color);
    void CAIRO_SET_LINE_WIDTH(CairoSurfaceHandle surface_handle, double width);
    void CAIRO_SET_LINE_JOIN(CairoSurfaceHandle surface_handle, int64_t join_style);
    void CAIRO_SET_LINE_CAP(CairoSurfaceHandle surface_handle, int64_t cap_style);
    void CAIRO_SET_OPACITY(CairoSurfaceHandle surface_handle, double opacity);
    
    void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
    void CAIRO_STROKE_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height);
    void CAIRO_FILL_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);
    void CAIRO_STROKE_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius);
    void CAIRO_DRAW_LINE(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2);
    void CAIRO_DRAW_POINT(CairoSurfaceHandle surface_handle, double x, double y);
    
    void CAIRO_DRAW_LINES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t point_pairs);
    void CAIRO_FILL_RECTS_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t rect_quads);
    void CAIRO_FILL_CIRCLES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t circle_triples);
    void CAIRO_DRAW_POLYLINE(CairoSurfaceHandle surface_handle, bcpl_vector_t points, int64_t close_path);
    void CAIRO_FILL_POLYGON(CairoSurfaceHandle surface_handle, bcpl_vector_t points);
    
    void CAIRO_BEGIN_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_MOVE_TO(CairoSurfaceHandle surface_handle, double x, double y);
    void CAIRO_LINE_TO(CairoSurfaceHandle surface_handle, double x, double y);
    void CAIRO_CURVE_TO(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2, double x3, double y3);
    void CAIRO_CLOSE_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_FILL_PATH(CairoSurfaceHandle surface_handle);
    void CAIRO_STROKE_PATH(CairoSurfaceHandle surface_handle);
    
    void CAIRO_SET_FONT(CairoSurfaceHandle surface_handle, bcpl_string_t font_name, double font_size);
    void CAIRO_DRAW_TEXT(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text);
    void CAIRO_TEXT_SIZE(CairoSurfaceHandle surface_handle, bcpl_string_t text, double* width, double* height);
    void CAIRO_DRAW_TEXT_COLORED(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text, uint32_t rgba_color);
    
    CairoImageHandle CAIRO_LOAD_IMAGE(bcpl_string_t filename);
    void CAIRO_DRAW_IMAGE(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y);
    void CAIRO_DRAW_IMAGE_SCALED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double scale_x, double scale_y);
    void CAIRO_DRAW_IMAGE_ROTATED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double angle);
    void CAIRO_IMAGE_SIZE(CairoImageHandle image_handle, int64_t* width, int64_t* height);
    
    void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);
    int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height);
    void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id);
    int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id);
    
    void CAIRO_SAVE(CairoSurfaceHandle surface_handle);
    void CAIRO_RESTORE(CairoSurfaceHandle surface_handle);
    void CAIRO_TRANSLATE(CairoSurfaceHandle surface_handle, double tx, double ty);
    void CAIRO_SCALE(CairoSurfaceHandle surface_handle, double sx, double sy);
    void CAIRO_ROTATE(CairoSurfaceHandle surface_handle, double angle);
    
    void CAIRO_RELEASE_SURFACE(CairoSurfaceHandle surface_handle);
    void CAIRO_RELEASE_IMAGE(CairoImageHandle image_handle);
    int64_t CAIRO_SURFACE_COUNT(void);
    int64_t CAIRO_IMAGE_COUNT(void);
    
    int64_t CAIRO_IS_AVAILABLE(void);
    bcpl_string_t CAIRO_VERSION(void);
    bcpl_string_t CAIRO_GET_ERROR(void);
    void CAIRO_CLEAR_ERROR(void);
    
    // SAMM integration functions
    CairoSurfaceHandle CAIRO_CREATE_SURFACE_RETAINED(int64_t width, int64_t height, int64_t parent_scope_offset);
    CairoSurfaceHandle CAIRO_LOAD_PNG_RETAINED(bcpl_string_t filename, int64_t parent_scope_offset);
    CairoImageHandle CAIRO_LOAD_IMAGE_RETAINED(bcpl_string_t filename, int64_t parent_scope_offset);
    int64_t CAIRO_SAMM_SCOPE_DEPTH(void);
    void CAIRO_SAMM_FORCE_CLEANUP(void);
    void CAIRO_SAMM_GET_STATS(int64_t* active_surfaces, int64_t* active_images, int64_t* memory_usage);
    void CAIRO_SAMM_SET_TRACE(int64_t enabled);
    void CAIRO_SAMM_DUMP_STATE(void);
}
*/
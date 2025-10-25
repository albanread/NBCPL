# Cairo Graphics Integration for BCPL Runtime

## Overview

This document describes the integration of Cairo 2D graphics library into the NewBCPL runtime system. The design provides a modern, RAII-managed C++ API for Cairo functionality while exposing simple, efficient functions to BCPL code through the existing runtime API registration system.

## 🎨 Design Philosophy

### Core Principles

1. **SAMM Integration**: All Cairo surfaces and images are managed by the SAMM (Scope Aware Memory Management) system for automatic cleanup
2. **Runtime API Consistency**: Follow the established `runtime_api.cpp` registration pattern
3. **SDL2 Display Integration**: Bonus SDL2 integration for displaying Cairo images
4. **Vector-Friendly API**: Support both scalar arguments and vector operations for batch processing
5. **Resource Safety**: RAII resource management with automatic cleanup on scope exit

### Architecture Overview

The Cairo integration consists of four main components:

1. **Core Cairo Wrapper** (`runtime/cairo/cairo_core.h/.cpp`) - C++ RAII wrappers around Cairo
2. **BCPL Runtime Interface** (`runtime/cairo/cairo_runtime.h/.cpp`) - C-linkage functions for BCPL
3. **SAMM Integration** (`runtime/cairo/cairo_samm.h/.cpp`) - Memory management integration
4. **SDL2 Display Module** (`runtime/cairo/cairo_sdl.h/.cpp`) - Optional SDL2 surface display

## 📐 API Design

### Data Structures

```cpp
// Color representation (32-bit RGBA)
struct CairoColor {
    uint8_t r, g, b, a;
    
    // Constructor for BCPL compatibility
    CairoColor(uint32_t rgba) : 
        r((rgba >> 24) & 0xFF), 
        g((rgba >> 16) & 0xFF), 
        b((rgba >> 8) & 0xFF), 
        a(rgba & 0xFF) {}
};

// Point for path operations
struct CairoPoint {
    double x, y;
};

// Rectangle for clipping and drawing
struct CairoRect {
    double x, y, width, height;
};
```

### SAMM-Managed Resources

All Cairo resources are tracked by SAMM using smart handle system:

```cpp
// Handle-based system for SAMM integration
typedef uint64_t CairoSurfaceHandle;
typedef uint64_t CairoImageHandle;
typedef uint64_t CairoPatternHandle;

// SAMM resource tracker
class CairoResourceManager {
    static std::unordered_map<uint64_t, std::unique_ptr<CairoSurface>> surfaces_;
    static std::unordered_map<uint64_t, std::unique_ptr<CairoImage>> images_;
    static std::atomic<uint64_t> next_handle_;
    
public:
    static CairoSurfaceHandle registerSurface(std::unique_ptr<CairoSurface> surface);
    static CairoImageHandle registerImage(std::unique_ptr<CairoImage> image);
    static void cleanup(); // Called by SAMM on scope exit
};
```

## 🔧 Runtime Function Categories

### Surface Management

```cpp
// Create new drawing surface
int64_t CAIRO_CREATE_SURFACE(int64_t width, int64_t height);

// Create surface from PNG file
int64_t CAIRO_LOAD_PNG(bcpl_string_t filename);

// Save surface to PNG
void CAIRO_SAVE_PNG(int64_t surface_handle, bcpl_string_t filename);

// Get surface dimensions
void CAIRO_GET_SIZE(int64_t surface_handle, int64_t* width, int64_t* height);

// Clear surface with color
void CAIRO_CLEAR(int64_t surface_handle, uint32_t rgba_color);

// Clone surface for double-buffering
int64_t CAIRO_CLONE_SURFACE(int64_t source_handle);
```

### Drawing State

```cpp
// Set drawing color (RGBA packed as uint32)
void CAIRO_SET_COLOR(int64_t surface_handle, uint32_t rgba_color);

// Set line width for strokes
void CAIRO_SET_LINE_WIDTH(int64_t surface_handle, double width);

// Set line join style (CAIRO_LINE_JOIN_MITER, ROUND, BEVEL)
void CAIRO_SET_LINE_JOIN(int64_t surface_handle, int64_t join_style);

// Set line cap style (CAIRO_LINE_CAP_BUTT, ROUND, SQUARE)
void CAIRO_SET_LINE_CAP(int64_t surface_handle, int64_t cap_style);

// Set opacity for all drawing operations
void CAIRO_SET_OPACITY(int64_t surface_handle, double opacity);
```

### Basic Shapes

```cpp
// Draw filled rectangle
void CAIRO_FILL_RECT(int64_t surface_handle, double x, double y, double width, double height);

// Draw rectangle outline
void CAIRO_STROKE_RECT(int64_t surface_handle, double x, double y, double width, double height);

// Draw filled circle
void CAIRO_FILL_CIRCLE(int64_t surface_handle, double cx, double cy, double radius);

// Draw circle outline
void CAIRO_STROKE_CIRCLE(int64_t surface_handle, double cx, double cy, double radius);

// Draw line between two points
void CAIRO_DRAW_LINE(int64_t surface_handle, double x1, double y1, double x2, double y2);

// Draw single point (filled square)
void CAIRO_DRAW_POINT(int64_t surface_handle, double x, double y);
```

### Vector Operations (Batch Processing)

```cpp
// Draw multiple lines from vector of point pairs
void CAIRO_DRAW_LINES_VEC(int64_t surface_handle, bcpl_vector_t point_pairs);

// Draw multiple rectangles from vector of rect quads (x, y, width, height)
void CAIRO_FILL_RECTS_VEC(int64_t surface_handle, bcpl_vector_t rect_quads);

// Draw multiple circles from vector of circle triples (cx, cy, radius)
void CAIRO_FILL_CIRCLES_VEC(int64_t surface_handle, bcpl_vector_t circle_triples);

// Set multiple colors for gradient operations
void CAIRO_SET_COLORS_VEC(int64_t surface_handle, bcpl_vector_t color_stops);

// Draw polyline from vector of points
void CAIRO_DRAW_POLYLINE(int64_t surface_handle, bcpl_vector_t points, int64_t close_path);

// Fill polygon from vector of points
void CAIRO_FILL_POLYGON(int64_t surface_handle, bcpl_vector_t points);
```

### Path Operations

```cpp
// Begin new path
void CAIRO_BEGIN_PATH(int64_t surface_handle);

// Move to point without drawing
void CAIRO_MOVE_TO(int64_t surface_handle, double x, double y);

// Draw line to point
void CAIRO_LINE_TO(int64_t surface_handle, double x, double y);

// Draw quadratic curve
void CAIRO_CURVE_TO(int64_t surface_handle, double x1, double y1, double x2, double y2, double x3, double y3);

// Close current path
void CAIRO_CLOSE_PATH(int64_t surface_handle);

// Fill current path
void CAIRO_FILL_PATH(int64_t surface_handle);

// Stroke current path
void CAIRO_STROKE_PATH(int64_t surface_handle);
```

### Text Rendering

```cpp
// Set font family and size
void CAIRO_SET_FONT(int64_t surface_handle, bcpl_string_t font_name, double font_size);

// Draw text at position
void CAIRO_DRAW_TEXT(int64_t surface_handle, double x, double y, bcpl_string_t text);

// Get text dimensions for layout
void CAIRO_TEXT_SIZE(int64_t surface_handle, bcpl_string_t text, double* width, double* height);

// Draw text with color override
void CAIRO_DRAW_TEXT_COLORED(int64_t surface_handle, double x, double y, bcpl_string_t text, uint32_t rgba_color);
```

### Image Operations

```cpp
// Draw image onto surface
void CAIRO_DRAW_IMAGE(int64_t surface_handle, int64_t image_handle, double x, double y);

// Draw image with scaling
void CAIRO_DRAW_IMAGE_SCALED(int64_t surface_handle, int64_t image_handle, double x, double y, double scale_x, double scale_y);

// Draw image with rotation (angle in radians)
void CAIRO_DRAW_IMAGE_ROTATED(int64_t surface_handle, int64_t image_handle, double x, double y, double angle);

// Create pattern from image for tiling
int64_t CAIRO_CREATE_PATTERN(int64_t image_handle);

// Fill rectangle with pattern
void CAIRO_FILL_WITH_PATTERN(int64_t surface_handle, int64_t pattern_handle, double x, double y, double width, double height);
```

### SDL2 Display Integration

```cpp
// Display Cairo surface in SDL2 window
void CAIRO_DISPLAY_SDL(int64_t surface_handle, int64_t sdl_window_id);

// Create SDL2 window optimized for Cairo display
int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height);

// Update SDL2 window with Cairo surface (for animation)
void CAIRO_UPDATE_SDL_WINDOW(int64_t surface_handle, int64_t sdl_window_id);

// Convert Cairo surface to SDL2 texture for efficient blitting
int64_t CAIRO_TO_SDL_TEXTURE(int64_t surface_handle, int64_t sdl_renderer_id);
```

## 🏗️ Implementation Structure

### File Organization

```
runtime/cairo/
├── cairo_core.h         # C++ RAII wrappers
├── cairo_core.cpp       # Core Cairo functionality
├── cairo_runtime.h      # C-linkage BCPL interface
├── cairo_runtime.cpp    # Runtime function implementations
├── cairo_samm.h         # SAMM integration
├── cairo_samm.cpp       # Memory management
├── cairo_sdl.h          # SDL2 display integration
├── cairo_sdl.cpp        # SDL2 implementation
└── cairo_vectors.cpp    # Vector operation implementations
```

### SAMM Integration Details

```cpp
// SAMM resource tracking for automatic cleanup
class CairoSAMM {
private:
    static std::unordered_set<CairoSurfaceHandle> current_scope_surfaces_;
    static std::unordered_set<CairoImageHandle> current_scope_images_;
    
public:
    // Called by SAMM on scope entry
    static void enterScope();
    
    // Called by SAMM on scope exit - cleanup all resources
    static void exitScope();
    
    // Track resource in current scope
    static void trackSurface(CairoSurfaceHandle handle);
    static void trackImage(CairoImageHandle handle);
    
    // Manual cleanup for retained resources
    static void releaseSurface(CairoSurfaceHandle handle);
    static void releaseImage(CairoImageHandle handle);
};
```

### Vector Processing Implementation

```cpp
// Efficient batch processing for vector operations
class CairoVectorProcessor {
public:
    // Process vector of point pairs for line drawing
    static void processLinePairs(cairo_t* ctx, const std::vector<CairoPoint>& points);
    
    // Process vector of rectangles
    static void processRectQuads(cairo_t* ctx, const std::vector<CairoRect>& rects);
    
    // Process vector of color stops for gradients
    static void processColorStops(cairo_t* ctx, const std::vector<CairoColor>& colors);
    
    // Convert BCPL vector to native vector types
    static std::vector<CairoPoint> convertPointVector(bcpl_vector_t bcpl_vec);
    static std::vector<CairoRect> convertRectVector(bcpl_vector_t bcpl_vec);
};
```

## 📋 Runtime Registration

All functions are registered in `runtime_api.cpp` following the established pattern:

```cpp
// Surface Management
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

// Drawing Operations
{
    "CAIRO_FILL_RECT", "_CAIRO_FILL_RECT",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECT), 5,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Fill rectangle on surface (surface, x, y, width, height)", "GRAPHICS"
},

// Vector Operations
{
    "CAIRO_DRAW_LINES_VEC", "_CAIRO_DRAW_LINES_VEC",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINES_VEC), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Draw multiple lines from vector of point pairs", "GRAPHICS"
},

// SDL2 Integration
{
    "CAIRO_DISPLAY_SDL", "_CAIRO_DISPLAY_SDL",
    reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DISPLAY_SDL), 2,
    RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
    "Display Cairo surface in SDL2 window", "GRAPHICS"
},
```

## 🔗 Build Integration

### Compilation Flags

```makefile
# Cairo compilation
CAIRO_CFLAGS = -I/opt/homebrew/include/cairo -I/opt/homebrew/include/pixman-1 -I/opt/homebrew/include/freetype2 -I/opt/homebrew/include/libpng16 -I/opt/homebrew/include/harfbuzz -I/opt/homebrew/include/glib-2.0 -I/opt/homebrew/lib/glib-2.0/include

CAIRO_LIBS = -L/opt/homebrew/lib -lcairo -lpixman-1 -lfreetype -lpng16 -lz -lbz2 -lharfbuzz -lglib-2.0 -lgobject-2.0

# Combined with existing SDL2 flags
GRAPHICS_LIBS = $(CAIRO_LIBS) $(SDL2_LIBS)
```

### Runtime Library Variants

Create new runtime library variants with Cairo support:

- `libbcpl_runtime_cairo.a` - Cairo-only support
- `libbcpl_runtime_cairo_sdl2.a` - Cairo + SDL2 integrated
- `libbcpl_runtime_full.a` - All graphics features

## 📖 BCPL Usage Examples

### Basic Drawing

```bcpl
LET START() BE
$(
    // Create a 800x600 surface
    LET surface = CAIRO_CREATE_SURFACE(800, 600)
    
    // Clear to white background
    CAIRO_CLEAR(surface, #xFFFFFFFF)
    
    // Set red color and draw rectangle
    CAIRO_SET_COLOR(surface, #xFF0000FF)
    CAIRO_FILL_RECT(surface, 100.0, 100.0, 200.0, 150.0)
    
    // Set blue color and draw circle
    CAIRO_SET_COLOR(surface, #x0000FFFF)
    CAIRO_FILL_CIRCLE(surface, 400.0, 300.0, 80.0)
    
    // Save to file
    CAIRO_SAVE_PNG(surface, "my_drawing.png")
    
    // Surface automatically cleaned up by SAMM
$)
```

### Vector Operations

```bcpl
LET DRAW_MANY_LINES() BE
$(
    LET surface = CAIRO_CREATE_SURFACE(800, 600)
    CAIRO_CLEAR(surface, #xFFFFFFFF)
    
    // Create vector of line endpoints (x1, y1, x2, y2 pairs)
    LET line_points = VEC 100
    line_points!0 := 0.0;   line_points!1 := 0.0     // Start point
    line_points!2 := 100.0; line_points!3 := 100.0   // End point
    line_points!4 := 100.0; line_points!5 := 0.0     // Next start
    line_points!6 := 200.0; line_points!7 := 100.0   // Next end
    // ... more points
    
    // Draw all lines in one efficient call
    LET point_vector = CREATE_VECTOR_FROM_ARRAY(line_points, 8)
    CAIRO_SET_COLOR(surface, #x000000FF)
    CAIRO_DRAW_LINES_VEC(surface, point_vector)
    
    CAIRO_SAVE_PNG(surface, "many_lines.png")
$)
```

### SDL2 Display Integration

```bcpl
LET GRAPHICS_DEMO() BE
$(
    // Create Cairo surface
    LET surface = CAIRO_CREATE_SURFACE(640, 480)
    
    // Create SDL2 window optimized for Cairo
    LET window = CAIRO_CREATE_SDL_WINDOW("Cairo Graphics Demo", 640, 480)
    
    // Animation loop
    FOR frame = 0 TO 100 DO
    $(
        // Clear and draw frame
        CAIRO_CLEAR(surface, #x000000FF)
        CAIRO_SET_COLOR(surface, #xFF0000FF)
        CAIRO_FILL_CIRCLE(surface, 320.0 + frame * 2.0, 240.0, 20.0)
        
        // Display in SDL2 window
        CAIRO_UPDATE_SDL_WINDOW(surface, window)
        
        SDL2_DELAY(16) // ~60 FPS
        
        // Check for quit event
        IF SDL2_POLL_EVENT() = SDL2_QUIT THEN BREAK
    $)
$)
```

### Image Loading and Manipulation

```bcpl
LET IMAGE_PROCESSING() BE
$(
    // Load background image
    LET background = CAIRO_LOAD_PNG("background.png")
    LET overlay = CAIRO_LOAD_PNG("overlay.png")
    
    // Create compositing surface
    LET composite = CAIRO_CREATE_SURFACE(800, 600)
    
    // Draw background
    CAIRO_DRAW_IMAGE(composite, background, 0.0, 0.0)
    
    // Draw overlay with rotation and scaling
    CAIRO_DRAW_IMAGE_ROTATED(composite, overlay, 400.0, 300.0, 0.785) // 45 degrees
    
    // Add text
    CAIRO_SET_FONT(composite, "Arial", 24.0)
    CAIRO_SET_COLOR(composite, #xFFFFFFFF)
    CAIRO_DRAW_TEXT(composite, 50.0, 50.0, "Composed Image")
    
    CAIRO_SAVE_PNG(composite, "result.png")
$)
```

## 🚀 Performance Considerations

### Memory Management

1. **SAMM Integration**: All surfaces automatically cleaned up on scope exit
2. **Handle-Based System**: Lightweight handles avoid copying large image data
3. **Background Cleanup**: SAMM's background worker handles expensive cleanup
4. **Resource Pooling**: Common surface sizes cached for reuse

### Rendering Optimization

1. **Vector Operations**: Batch processing reduces Cairo context switches
2. **Path Optimization**: Complex paths built once and reused
3. **Surface Caching**: Frequently used images cached in memory
4. **SDL2 Textures**: Cairo surfaces converted to GPU textures for fast display

### Threading Considerations

1. **Thread-Safe Handles**: Handle-based system avoids shared resource issues
2. **Per-Thread Contexts**: Each BCPL thread gets isolated Cairo contexts
3. **SAMM Coordination**: Background cleanup coordinated with rendering threads

## 🔧 Development and Testing

### Test Programs

Create comprehensive test suite in `tests/cairo/`:

```
tests/cairo/
├── basic_shapes.bcl      # Test rectangles, circles, lines
├── vector_operations.bcl # Test batch drawing functions
├── image_loading.bcl     # Test PNG load/save
├── text_rendering.bcl    # Test font rendering
├── sdl_integration.bcl   # Test SDL2 display
├── performance_test.bcl  # Benchmark drawing operations
└── memory_test.bcl       # Test SAMM integration
```

### Debugging Support

1. **Debug Surface Export**: Intermediate surfaces can be saved for inspection
2. **Performance Metrics**: Built-in timing for expensive operations
3. **Memory Tracking**: SAMM provides detailed allocation statistics
4. **Error Reporting**: Clear error messages for invalid operations

## 📈 Future Extensions

### Phase 2 Features

1. **Gradient Support**: Linear and radial gradients
2. **Pattern Fills**: Image-based pattern fills
3. **Clipping Paths**: Complex clipping regions
4. **Transformations**: Rotate, scale, translate operations
5. **Animation Helpers**: Easing functions and tweening

### Phase 3 Features

1. **3D Integration**: Cairo surfaces as textures in 3D scenes
2. **Vector Graphics**: SVG import/export capabilities
3. **Advanced Text**: Rich text layout and formatting
4. **Filters**: Blur, drop shadow, color effects
5. **GPU Acceleration**: OpenGL/Metal backend integration

## 🎯 Benefits

### For BCPL Developers

1. **Simple API**: Easy-to-use graphics functions
2. **Automatic Cleanup**: SAMM prevents memory leaks
3. **Fast Development**: No manual resource management
4. **Flexible Display**: Output to files or SDL2 windows
5. **Vector-Friendly**: Efficient batch operations

### For Runtime System

1. **Consistent Architecture**: Follows established patterns
2. **SAMM Integration**: Leverages existing memory management
3. **Modular Design**: Optional Cairo support
4. **Performance**: Optimized for BCPL use cases
5. **Extensible**: Easy to add new graphics features

This design provides a comprehensive, well-integrated graphics system that enhances BCPL's capabilities while maintaining the runtime's architectural principles and performance characteristics.
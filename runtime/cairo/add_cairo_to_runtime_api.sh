#!/bin/bash
# Helper script to add Cairo function declarations to runtime_api.cpp
# This script safely adds Cairo functions to the existing runtime API

set -e

RUNTIME_API_FILE="../../runtime_api.cpp"
BACKUP_FILE="../../runtime_api.cpp.backup"

echo "=== Adding Cairo Functions to runtime_api.cpp ==="

# Check if runtime_api.cpp exists
if [ ! -f "$RUNTIME_API_FILE" ]; then
    echo "❌ Error: $RUNTIME_API_FILE not found"
    echo "   Run this script from the runtime/cairo directory"
    exit 1
fi

# Create backup
echo "📁 Creating backup: $BACKUP_FILE"
cp "$RUNTIME_API_FILE" "$BACKUP_FILE"

# Check if Cairo functions are already added
if grep -q "CAIRO_CREATE_SURFACE" "$RUNTIME_API_FILE"; then
    echo "✅ Cairo functions already present in runtime_api.cpp"
    exit 0
fi

echo "🔧 Adding Cairo function declarations..."

# Create temporary file for the new runtime_api.cpp
TEMP_FILE=$(mktemp)

# Process the runtime_api.cpp file
while IFS= read -r line; do
    echo "$line" >> "$TEMP_FILE"

    # Add Cairo forward declarations after existing extern "C" block
    if [[ "$line" == *"extern \"C\" {"* ]] && ! grep -q "// Cairo Graphics Functions" "$TEMP_FILE"; then
        cat >> "$TEMP_FILE" << 'EOF'

    // Cairo Graphics Functions
    typedef uint64_t CairoSurfaceHandle;
    typedef uint64_t CairoImageHandle;

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
EOF
    fi

    # Add Cairo manifest entries before the closing brace of g_runtime_manifest
    if [[ "$line" == *"};"* ]] && [[ "$line" == *"g_runtime_manifest"* ]]; then
        # Remove the closing brace and semicolon
        sed -i '' '$d' "$TEMP_FILE"

        cat >> "$TEMP_FILE" << 'EOF'

    // === CAIRO GRAPHICS FUNCTIONS ===

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
    {
        "CAIRO_SAVE_PNG", "_CAIRO_SAVE_PNG",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAVE_PNG), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Save Cairo surface to PNG file (surface, filename)", "GRAPHICS"
    },
    {
        "CAIRO_CLEAR", "_CAIRO_CLEAR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CLEAR), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Clear surface with solid color (surface, rgba_color)", "GRAPHICS"
    },

    // Drawing Operations
    {
        "CAIRO_SET_COLOR", "_CAIRO_SET_COLOR",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SET_COLOR), 2,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Set drawing color (surface, rgba_color)", "GRAPHICS"
    },
    {
        "CAIRO_FILL_RECT", "_CAIRO_FILL_RECT",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_RECT), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill rectangle (surface, x, y, width, height)", "GRAPHICS"
    },
    {
        "CAIRO_FILL_CIRCLE", "_CAIRO_FILL_CIRCLE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_FILL_CIRCLE), 4,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Fill circle (surface, cx, cy, radius)", "GRAPHICS"
    },
    {
        "CAIRO_DRAW_LINE", "_CAIRO_DRAW_LINE",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_DRAW_LINE), 5,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Draw line between points (surface, x1, y1, x2, y2)", "GRAPHICS"
    },

    // Vector Operations
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

    // Text Operations
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

    // Image Operations
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

    // Utility Functions
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

    // SAMM Integration
    {
        "CAIRO_CREATE_SURFACE_RETAINED", "_CAIRO_CREATE_SURFACE_RETAINED",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_CREATE_SURFACE_RETAINED), 3,
        RuntimeFunctionType::STANDARD, RuntimeReturnType::INTEGER,
        "Create surface in parent scope (width, height, parent_offset)", "GRAPHICS"
    },
    {
        "CAIRO_SAMM_GET_STATS", "_CAIRO_SAMM_GET_STATS",
        reinterpret_cast<RuntimeFunctionPtr>(CAIRO_SAMM_GET_STATS), 3,
        RuntimeFunctionType::ROUTINE, RuntimeReturnType::VOID,
        "Get SAMM statistics (surfaces_ptr, images_ptr, memory_ptr)", "GRAPHICS"
    }

};
EOF
    fi

done < "$RUNTIME_API_FILE"

# Replace the original file
mv "$TEMP_FILE" "$RUNTIME_API_FILE"

echo "✅ Successfully added Cairo functions to runtime_api.cpp!"
echo ""
echo "Added:"
echo "  • Forward declarations for all Cairo functions"
echo "  • Runtime manifest entries for core Cairo operations"
echo "  • SAMM integration functions"
echo "  • Vector operation support"
echo ""
echo "📁 Backup saved as: $BACKUP_FILE"
echo "🔧 Ready to build Cairo-enabled runtime!"
echo ""
echo "Next steps:"
echo "  1. Run: cd .. && ./buildruntime --with-cairo-static"
echo "  2. Test with: ./NewBCPL --list-runtime | grep CAIRO"

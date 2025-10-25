#include "cairo_runtime.h"
#include "cairo_core.h"
#include "cairo_samm.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <unordered_map>
#include <mutex>

// Forward declarations for BCPL string conversion
extern "C" {
    // These should be defined elsewhere in the runtime
    char* bcpl_string_to_c_string(bcpl_string_t bcpl_str);
    bcpl_string_t c_string_to_bcpl_string(const char* c_str);
    double* bcpl_vector_to_double_array(bcpl_vector_t bcpl_vec, size_t* count);
    uint32_t* bcpl_vector_to_uint32_array(bcpl_vector_t bcpl_vec, size_t* count);
}

// Global error state
static std::string g_last_error;
static std::mutex g_error_mutex;

// Helper functions
namespace {
    void setLastError(const std::string& error) {
        std::lock_guard<std::mutex> lock(g_error_mutex);
        g_last_error = error;
    }
    
    void clearLastError() {
        std::lock_guard<std::mutex> lock(g_error_mutex);
        g_last_error.clear();
    }
    
    std::string getLastError() {
        std::lock_guard<std::mutex> lock(g_error_mutex);
        return g_last_error;
    }
    
    std::string bcplStringToStd(bcpl_string_t bcpl_str) {
        if (!bcpl_str) return "";
        char* c_str = bcpl_string_to_c_string(bcpl_str);
        if (!c_str) return "";
        std::string result(c_str);
        free(c_str); // Free the allocated C string copy we made
        return result;
    }
}

// =============================================================================
// SURFACE MANAGEMENT
// =============================================================================

extern "C" {

CairoSurfaceHandle CAIRO_CREATE_SURFACE(int64_t width, int64_t height) {
    try {
        clearLastError();
        printf("DEBUG: CAIRO_CREATE_SURFACE called with width=%lld, height=%lld\n", width, height);
        
        if (width <= 0 || height <= 0) {
            printf("DEBUG: Invalid dimensions, returning 0\n");
            setLastError("Invalid surface dimensions");
            return 0;
        }
        
        auto surface = std::make_unique<CairoSurface>(static_cast<int>(width), static_cast<int>(height));
        printf("DEBUG: Created CairoSurface object at %p\n", surface.get());
        printf("DEBUG: Surface isValid: %s\n", surface->isValid() ? "true" : "false");
        
        CairoSurfaceHandle handle = CairoResourceManager::registerSurface(std::move(surface));
        printf("DEBUG: Registered surface with handle=%llu\n", handle);
        
        // Track in SAMM if enabled
        CairoSAMM_trackSurface(handle);
        
        printf("DEBUG: Returning handle=%llu\n", handle);
        return handle;
    } catch (const std::exception& e) {
        printf("DEBUG: Exception in CAIRO_CREATE_SURFACE: %s\n", e.what());
        setLastError(e.what());
        return 0;
    }
}

CairoSurfaceHandle CAIRO_LOAD_PNG(bcpl_string_t filename) {
    try {
        clearLastError();
        std::string filepath = bcplStringToStd(filename);
        if (filepath.empty()) {
            setLastError("Invalid filename");
            return 0;
        }
        
        auto image = std::make_unique<CairoImage>(filepath);
        auto surface = std::make_unique<CairoSurface>(*image);
        CairoSurfaceHandle handle = CairoResourceManager::registerSurface(std::move(surface));
        
        // Track in SAMM if enabled
        CairoSAMM_trackSurface(handle);
        
        return handle;
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

void CAIRO_SAVE_PNG(CairoSurfaceHandle surface_handle, bcpl_string_t filename) {
    try {
        clearLastError();
        printf("DEBUG: CAIRO_SAVE_PNG called with handle=%llu, filename ptr=%p\n", surface_handle, filename);
        
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            printf("DEBUG: CAIRO_SAVE_PNG - getSurface returned NULL\n");
            setLastError("Invalid surface handle");
            return;
        }
        
        printf("DEBUG: CAIRO_SAVE_PNG - got surface at %p\n", surface);
        
        std::string filepath = bcplStringToStd(filename);
        printf("DEBUG: CAIRO_SAVE_PNG - converted filename to: '%s'\n", filepath.c_str());
        
        if (filepath.empty()) {
            printf("DEBUG: CAIRO_SAVE_PNG - filename is empty\n");
            setLastError("Invalid filename");
            return;
        }
        
        printf("DEBUG: CAIRO_SAVE_PNG - calling surface->saveAsPNG\n");
        surface->saveAsPNG(filepath);
        printf("DEBUG: CAIRO_SAVE_PNG - saveAsPNG completed\n");
    } catch (const std::exception& e) {
        printf("DEBUG: CAIRO_SAVE_PNG - exception: %s\n", e.what());
        setLastError(e.what());
    }
}

void CAIRO_GET_SIZE(CairoSurfaceHandle surface_handle, int64_t* width, int64_t* height) {
    try {
        clearLastError();
        if (!width || !height) {
            setLastError("Null pointer for dimensions");
            return;
        }
        
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            *width = 0;
            *height = 0;
            return;
        }
        
        *width = surface->getWidth();
        *height = surface->getHeight();
    } catch (const std::exception& e) {
        setLastError(e.what());
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

void CAIRO_CLEAR(CairoSurfaceHandle surface_handle, uint32_t rgba_color) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        CairoColor color(rgba_color);
        surface->clear(color);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

CairoSurfaceHandle CAIRO_CLONE_SURFACE(CairoSurfaceHandle source_handle) {
    try {
        clearLastError();
        CairoSurface* source = CairoResourceManager::getSurface(source_handle);
        if (!source) {
            setLastError("Invalid source surface handle");
            return 0;
        }
        
        auto cloned = source->clone();
        if (!cloned) {
            setLastError("Failed to clone surface");
            return 0;
        }
        
        CairoSurfaceHandle handle = CairoResourceManager::registerSurface(std::move(cloned));
        CairoSAMM_trackSurface(handle);
        
        return handle;
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

// =============================================================================
// DRAWING STATE
// =============================================================================

void CAIRO_SET_COLOR(CairoSurfaceHandle surface_handle, uint32_t rgba_color) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        CairoColor color(rgba_color);
        surface->setColor(color);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_SET_LINE_WIDTH(CairoSurfaceHandle surface_handle, double width) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->setLineWidth(width);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_SET_LINE_JOIN(CairoSurfaceHandle surface_handle, int64_t join_style) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        cairo_line_join_t join = static_cast<cairo_line_join_t>(join_style);
        surface->setLineJoin(join);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_SET_LINE_CAP(CairoSurfaceHandle surface_handle, int64_t cap_style) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        cairo_line_cap_t cap = static_cast<cairo_line_cap_t>(cap_style);
        surface->setLineCap(cap);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_SET_OPACITY(CairoSurfaceHandle surface_handle, double opacity) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->setOpacity(opacity);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// BASIC SHAPES
// =============================================================================

void CAIRO_FILL_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height) {
    try {
        clearLastError();
        printf("DEBUG: CAIRO_FILL_RECT called with handle=%lu, x=%f, y=%f, w=%f, h=%f\n", 
               surface_handle, x, y, width, height);
        
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            printf("DEBUG: getSurface returned NULL for handle %lu\n", surface_handle);
            setLastError("Invalid surface handle");
            return;
        }
        
        printf("DEBUG: Got valid surface pointer: %p\n", surface);
        printf("DEBUG: Surface dimensions: %dx%d\n", surface->getWidth(), surface->getHeight());
        printf("DEBUG: Surface context: %p\n", surface->getContext());
        
        surface->fillRectangle(x, y, width, height);
        printf("DEBUG: fillRectangle completed\n");
    } catch (const std::exception& e) {
        printf("DEBUG: Exception in CAIRO_FILL_RECT: %s\n", e.what());
        setLastError(e.what());
    }
}

void CAIRO_STROKE_RECT(CairoSurfaceHandle surface_handle, double x, double y, double width, double height) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->strokeRectangle(x, y, width, height);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_FILL_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->fillCircle(cx, cy, radius);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_STROKE_CIRCLE(CairoSurfaceHandle surface_handle, double cx, double cy, double radius) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->strokeCircle(cx, cy, radius);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_LINE(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->drawLine(x1, y1, x2, y2);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_POINT(CairoSurfaceHandle surface_handle, double x, double y) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->drawPoint(x, y);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// VECTOR OPERATIONS
// =============================================================================

void CAIRO_DRAW_LINES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t point_pairs) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        size_t count;
        double* data = bcpl_vector_to_double_array(point_pairs, &count);
        if (!data || count < 4) {
            setLastError("Invalid or insufficient point data");
            return;
        }
        
        std::vector<CairoPoint> points = CairoUtils::pointsFromFloatArray(data, count);
        surface->drawLines(points);
        
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_FILL_RECTS_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t rect_quads) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        size_t count;
        double* data = bcpl_vector_to_double_array(rect_quads, &count);
        if (!data || count < 4) {
            setLastError("Invalid or insufficient rectangle data");
            return;
        }
        
        std::vector<CairoRect> rects = CairoUtils::rectsFromFloatArray(data, count);
        surface->fillRectangles(rects);
        
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_FILL_CIRCLES_VEC(CairoSurfaceHandle surface_handle, bcpl_vector_t circle_triples) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        size_t count;
        double* data = bcpl_vector_to_double_array(circle_triples, &count);
        if (!data || count < 3) {
            setLastError("Invalid or insufficient circle data");
            return;
        }
        
        std::vector<CairoPoint> centers;
        std::vector<double> radii;
        
        for (size_t i = 0; i < count - 2; i += 3) {
            centers.emplace_back(data[i], data[i + 1]);
            radii.push_back(data[i + 2]);
        }
        
        surface->fillCircles(centers, radii);
        
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_POLYLINE(CairoSurfaceHandle surface_handle, bcpl_vector_t points, int64_t close_path) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        size_t count;
        double* data = bcpl_vector_to_double_array(points, &count);
        if (!data || count < 4) {
            setLastError("Invalid or insufficient point data");
            return;
        }
        
        std::vector<CairoPoint> point_list = CairoUtils::pointsFromFloatArray(data, count);
        surface->strokePolygon(point_list, close_path != 0);
        
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_FILL_POLYGON(CairoSurfaceHandle surface_handle, bcpl_vector_t points) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        size_t count;
        double* data = bcpl_vector_to_double_array(points, &count);
        if (!data || count < 6) {  // At least 3 points (6 coordinates)
            setLastError("Invalid or insufficient point data for polygon");
            return;
        }
        
        std::vector<CairoPoint> point_list = CairoUtils::pointsFromFloatArray(data, count);
        surface->fillPolygon(point_list);
        
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// PATH OPERATIONS
// =============================================================================

void CAIRO_BEGIN_PATH(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->beginPath();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_MOVE_TO(CairoSurfaceHandle surface_handle, double x, double y) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->moveTo(x, y);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_LINE_TO(CairoSurfaceHandle surface_handle, double x, double y) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->lineTo(x, y);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_CURVE_TO(CairoSurfaceHandle surface_handle, double x1, double y1, double x2, double y2, double x3, double y3) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->curveTo(x1, y1, x2, y2, x3, y3);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_CLOSE_PATH(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->closePath();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_FILL_PATH(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->fillPath();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_STROKE_PATH(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->strokePath();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// TEXT RENDERING
// =============================================================================

void CAIRO_SET_FONT(CairoSurfaceHandle surface_handle, bcpl_string_t font_name, double font_size) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        std::string font = bcplStringToStd(font_name);
        surface->setFont(font, font_size);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_TEXT(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        std::string text_str = bcplStringToStd(text);
        surface->drawText(x, y, text_str);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_TEXT_SIZE(CairoSurfaceHandle surface_handle, bcpl_string_t text, double* width, double* height) {
    try {
        clearLastError();
        if (!width || !height) {
            setLastError("Null pointer for text dimensions");
            return;
        }
        
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            *width = 0;
            *height = 0;
            return;
        }
        
        std::string text_str = bcplStringToStd(text);
        surface->getTextExtents(text_str, width, height);
    } catch (const std::exception& e) {
        setLastError(e.what());
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

void CAIRO_DRAW_TEXT_COLORED(CairoSurfaceHandle surface_handle, double x, double y, bcpl_string_t text, uint32_t rgba_color) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        std::string text_str = bcplStringToStd(text);
        CairoColor color(rgba_color);
        surface->drawTextColored(x, y, text_str, color);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// IMAGE OPERATIONS
// =============================================================================

CairoImageHandle CAIRO_LOAD_IMAGE(bcpl_string_t filename) {
    try {
        clearLastError();
        std::string filepath = bcplStringToStd(filename);
        if (filepath.empty()) {
            setLastError("Invalid filename");
            return 0;
        }
        
        auto image = std::make_unique<CairoImage>(filepath);
        CairoImageHandle handle = CairoResourceManager::registerImage(std::move(image));
        
        // Track in SAMM if enabled
        CairoSAMM_trackImage(handle);
        
        return handle;
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

void CAIRO_DRAW_IMAGE(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        CairoImage* image = CairoResourceManager::getImage(image_handle);
        if (!image) {
            setLastError("Invalid image handle");
            return;
        }
        
        surface->drawImage(*image, x, y);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_IMAGE_SCALED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double scale_x, double scale_y) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        CairoImage* image = CairoResourceManager::getImage(image_handle);
        if (!image) {
            setLastError("Invalid image handle");
            return;
        }
        
        surface->drawImageScaled(*image, x, y, scale_x, scale_y);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_DRAW_IMAGE_ROTATED(CairoSurfaceHandle surface_handle, CairoImageHandle image_handle, double x, double y, double angle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        CairoImage* image = CairoResourceManager::getImage(image_handle);
        if (!image) {
            setLastError("Invalid image handle");
            return;
        }
        
        surface->drawImageRotated(*image, x, y, angle);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_IMAGE_SIZE(CairoImageHandle image_handle, int64_t* width, int64_t* height) {
    try {
        clearLastError();
        if (!width || !height) {
            setLastError("Null pointer for image dimensions");
            return;
        }
        
        CairoImage* image = CairoResourceManager::getImage(image_handle);
        if (!image) {
            setLastError("Invalid image handle");
            *width = 0;
            *height = 0;
            return;
        }
        
        *width = image->getWidth();
        *height = image->getHeight();
    } catch (const std::exception& e) {
        setLastError(e.what());
        if (width) *width = 0;
        if (height) *height = 0;
    }
}

// =============================================================================
// TRANSFORMATION OPERATIONS
// =============================================================================

void CAIRO_SAVE(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->save();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_RESTORE(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->restore();
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_TRANSLATE(CairoSurfaceHandle surface_handle, double tx, double ty) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->translate(tx, ty);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_SCALE(CairoSurfaceHandle surface_handle, double sx, double sy) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->scale(sx, sy);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_ROTATE(CairoSurfaceHandle surface_handle, double angle) {
    try {
        clearLastError();
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            setLastError("Invalid surface handle");
            return;
        }
        
        surface->rotate(angle);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

// =============================================================================
// RESOURCE MANAGEMENT
// =============================================================================

void CAIRO_RELEASE_SURFACE(CairoSurfaceHandle surface_handle) {
    try {
        clearLastError();
        CairoResourceManager::releaseSurface(surface_handle);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

void CAIRO_RELEASE_IMAGE(CairoImageHandle image_handle) {
    try {
        clearLastError();
        CairoResourceManager::releaseImage(image_handle);
    } catch (const std::exception& e) {
        setLastError(e.what());
    }
}

int64_t CAIRO_SURFACE_COUNT(void) {
    try {
        clearLastError();
        return static_cast<int64_t>(CairoResourceManager::getSurfaceCount());
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

int64_t CAIRO_IMAGE_COUNT(void) {
    try {
        clearLastError();
        return static_cast<int64_t>(CairoResourceManager::getImageCount());
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

// =============================================================================
// UTILITY FUNCTIONS
// =============================================================================

int64_t CAIRO_IS_AVAILABLE(void) {
    try {
        clearLastError();
        return 1; // If we can call this function, Cairo is available
    } catch (const std::exception& e) {
        setLastError(e.what());
        return 0;
    }
}

bcpl_string_t CAIRO_GET_VERSION(void) {
    try {
        clearLastError();
        const char* version_str = cairo_version_string();
        return c_string_to_bcpl_string(version_str);
    } catch (const std::exception& e) {
        setLastError(e.what());
        return c_string_to_bcpl_string("");
    }
}

bcpl_string_t CAIRO_GET_ERROR(void) {
    try {
        std::string error = getLastError();
        return c_string_to_bcpl_string(error.c_str());
    } catch (const std::exception& e) {
        return c_string_to_bcpl_string("Error getting error message");
    }
}

void CAIRO_CLEAR_ERROR(void) {
    try {
        clearLastError();
    } catch (const std::exception& e) {
        // Can't really handle an exception in clear error
    }
}

// =============================================================================
// SDL2 DISPLAY INTEGRATION (Stub implementations)
// =============================================================================

void CAIRO_DISPLAY_SDL(CairoSurfaceHandle surface_handle, int64_t sdl_window_id) {
    setLastError("SDL2 integration not implemented in this module");
}

int64_t CAIRO_CREATE_SDL_WINDOW(bcpl_string_t title, int64_t width, int64_t height) {
    setLastError("SDL2 integration not implemented in this module"); 
    return 0;
}

void CAIRO_UPDATE_SDL_WINDOW(CairoSurfaceHandle surface_handle, int64_t sdl_window_id) {
    setLastError("SDL2 integration not implemented in this module");
}

int64_t CAIRO_TO_SDL_TEXTURE(CairoSurfaceHandle surface_handle, int64_t sdl_renderer_id) {
    setLastError("SDL2 integration not implemented in this module");
    return 0;
}

// C-compatible wrapper for testing
void CAIRO_SAVE_PNG_C(CairoSurfaceHandle surface_handle, const char* filename) {
    try {
        clearLastError();
        printf("DEBUG: CAIRO_SAVE_PNG_C called with handle=%llu, filename='%s'\n", surface_handle, filename);
        
        CairoSurface* surface = CairoResourceManager::getSurface(surface_handle);
        if (!surface) {
            printf("DEBUG: CAIRO_SAVE_PNG_C - getSurface returned NULL\n");
            setLastError("Invalid surface handle");
            return;
        }
        
        printf("DEBUG: CAIRO_SAVE_PNG_C - got surface at %p\n", surface);
        
        if (!filename || strlen(filename) == 0) {
            printf("DEBUG: CAIRO_SAVE_PNG_C - filename is null or empty\n");
            setLastError("Invalid filename");
            return;
        }
        
        std::string filepath(filename);
        printf("DEBUG: CAIRO_SAVE_PNG_C - calling surface->saveAsPNG with '%s'\n", filepath.c_str());
        surface->saveAsPNG(filepath);
        printf("DEBUG: CAIRO_SAVE_PNG_C - saveAsPNG completed successfully\n");
    } catch (const std::exception& e) {
        printf("DEBUG: CAIRO_SAVE_PNG_C - exception: %s\n", e.what());
        setLastError(e.what());
    }
}

} // extern "C"
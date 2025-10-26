#pragma once

#include <cairo.h>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <atomic>

// Forward declaration for SAMM allocators
namespace CairoSAMM {
    template<typename T>
    struct BCPLDeleter;
    
    template<typename T>
    using unique_ptr = std::unique_ptr<T, BCPLDeleter<T>>;
}

// Forward declarations
struct CairoColor;
struct CairoPoint;
struct CairoRect;

/**
 * @brief RGBA color structure for Cairo operations
 * Values range from 0 to 255 for each component
 */
struct CairoColor {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;

    // Constructors
    CairoColor() = default;
    CairoColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) : r(r), g(g), b(b), a(a) {}
    
    // Constructor from packed RGBA uint32 (RGBA format: 0xRRGGBBAA)
    explicit CairoColor(uint32_t rgba) : 
        r((rgba >> 24) & 0xFF), 
        g((rgba >> 16) & 0xFF), 
        b((rgba >> 8) & 0xFF), 
        a(rgba & 0xFF) {}

    // Convert to Cairo's 0.0-1.0 float format
    void applytoCairo(cairo_t* ctx) const {
        cairo_set_source_rgba(ctx, r / 255.0, g / 255.0, b / 255.0, a / 255.0);
    }

    // Convert to packed RGBA format
    uint32_t toRGBA() const {
        return (static_cast<uint32_t>(r) << 24) | 
               (static_cast<uint32_t>(g) << 16) | 
               (static_cast<uint32_t>(b) << 8) | 
               static_cast<uint32_t>(a);
    }
};

/**
 * @brief 2D point structure for path operations
 */
struct CairoPoint {
    double x = 0.0;
    double y = 0.0;

    CairoPoint() = default;
    CairoPoint(double x, double y) : x(x), y(y) {}
};

/**
 * @brief Rectangle structure for drawing operations
 */
struct CairoRect {
    double x = 0.0;
    double y = 0.0;
    double width = 0.0;
    double height = 0.0;

    CairoRect() = default;
    CairoRect(double x, double y, double width, double height) 
        : x(x), y(y), width(width), height(height) {}
};

/**
 * @brief RAII wrapper for Cairo images loaded from files
 * Manages cairo_surface_t lifecycle automatically
 */
class CairoImage {
public:
    /**
     * @brief Load image from PNG file
     * @param filepath Path to PNG file
     * @throws std::runtime_error if file cannot be loaded
     */
    explicit CairoImage(const std::string& filepath);

    /**
     * @brief Create image from existing Cairo surface (takes ownership)
     * @param surface Cairo surface to wrap (will be destroyed in destructor)
     */
    explicit CairoImage(cairo_surface_t* surface);

    ~CairoImage();

    // Disable copy to prevent double-free
    CairoImage(const CairoImage&) = delete;
    CairoImage& operator=(const CairoImage&) = delete;

    // Allow move
    CairoImage(CairoImage&& other) noexcept;
    CairoImage& operator=(CairoImage&& other) noexcept;

    // Accessors
    int getWidth() const;
    int getHeight() const;
    cairo_surface_t* getSurface() const { return surface_; }
    bool isValid() const { return surface_ != nullptr; }

    // Image operations
    void saveAsPNG(const std::string& filepath) const;
    CairoSAMM::unique_ptr<CairoImage> clone() const;

private:
    cairo_surface_t* surface_ = nullptr;
    void cleanup();
};

/**
 * @brief RAII wrapper for Cairo drawing surfaces
 * Combines cairo_surface_t and cairo_t for simplified API
 */
class CairoSurface {
public:
    /**
     * @brief Create new blank drawing surface
     * @param width Surface width in pixels
     * @param height Surface height in pixels
     */
    CairoSurface(int width, int height);

    /**
     * @brief Create surface from existing image
     * @param image Source image to copy
     */
    explicit CairoSurface(const CairoImage& image);

    ~CairoSurface();

    // Disable copy and move to keep ownership simple
    CairoSurface(const CairoSurface&) = delete;
    CairoSurface& operator=(const CairoSurface&) = delete;
    CairoSurface(CairoSurface&&) = delete;
    CairoSurface& operator=(CairoSurface&&) = delete;

    // Accessors
    int getWidth() const;
    int getHeight() const;
    cairo_t* getContext() const { return context_; }
    cairo_surface_t* getSurface() const { return surface_; }
    bool isValid() const { return surface_ != nullptr && context_ != nullptr; }

    // File operations
    void saveAsPNG(const std::string& filepath) const;
    CairoSAMM::unique_ptr<CairoImage> toImage() const;

    // Surface operations
    void clear(const CairoColor& color);
    CairoSAMM::unique_ptr<CairoSurface> clone() const;

    // --- Drawing State Management ---
    void setColor(const CairoColor& color);
    void setLineWidth(double width);
    void setLineJoin(cairo_line_join_t join);
    void setLineCap(cairo_line_cap_t cap);
    void setOpacity(double opacity);
    void setAntialias(cairo_antialias_t antialias);
    cairo_antialias_t getAntialias() const;
    void setFont(const std::string& font_name, double font_size);

    // --- Basic Shape Drawing ---
    void fillRectangle(double x, double y, double width, double height);
    void strokeRectangle(double x, double y, double width, double height);
    void fillCircle(double cx, double cy, double radius);
    void strokeCircle(double cx, double cy, double radius);
    void drawLine(double x1, double y1, double x2, double y2);
    void drawPoint(double x, double y, double size = 1.0);

    // --- Path Operations ---
    void beginPath();
    void moveTo(double x, double y);
    void lineTo(double x, double y);
    void curveTo(double x1, double y1, double x2, double y2, double x3, double y3);
    void closePath();
    void fillPath();
    void strokePath();

    // --- Polygon Operations ---
    void fillPolygon(const std::vector<CairoPoint>& points);
    void strokePolygon(const std::vector<CairoPoint>& points, bool close = true);
    void drawPolyline(const std::vector<CairoPoint>& points);

    // --- Text Operations ---
    void drawText(double x, double y, const std::string& text);
    void drawTextColored(double x, double y, const std::string& text, const CairoColor& color);
    void getTextExtents(const std::string& text, double* width, double* height) const;

    // --- Image Operations ---
    void drawImage(const CairoImage& image, double x, double y);
    void drawImageScaled(const CairoImage& image, double x, double y, double scale_x, double scale_y);
    void drawImageRotated(const CairoImage& image, double x, double y, double angle);

    // --- Batch Operations (Vector Processing) ---
    void drawLines(const std::vector<CairoPoint>& line_endpoints);
    void fillRectangles(const std::vector<CairoRect>& rectangles);
    void fillCircles(const std::vector<CairoPoint>& centers, const std::vector<double>& radii);

    // --- Advanced Operations ---
    void save();
    void restore();
    void translate(double tx, double ty);
    void scale(double sx, double sy);
    void rotate(double angle);

private:
    cairo_surface_t* surface_ = nullptr;
    cairo_t* context_ = nullptr;
    int width_ = 0;
    int height_ = 0;

    void cleanup();
    void initializeContext();
    void setPathFromPoints(const std::vector<CairoPoint>& points, bool close = false);
};

/**
 * @brief Resource manager for SAMM integration
 * Handles automatic cleanup of Cairo resources on scope exit
 */
class CairoResourceManager {
public:
    using SurfaceHandle = uint64_t;
    using ImageHandle = uint64_t;

    // Handle management
    static SurfaceHandle registerSurface(CairoSAMM::unique_ptr<CairoSurface> surface);
    static ImageHandle registerImage(CairoSAMM::unique_ptr<CairoImage> image);
    
    // Resource access
    static CairoSurface* getSurface(SurfaceHandle handle);
    static CairoImage* getImage(ImageHandle handle);
    
    // Resource cleanup
    static void releaseSurface(SurfaceHandle handle);
    static void releaseImage(ImageHandle handle);
    static void cleanup(); // Called by SAMM on scope exit
    
    // Statistics
    static size_t getSurfaceCount();
    static size_t getImageCount();
    
private:
    static std::unordered_map<SurfaceHandle, CairoSAMM::unique_ptr<CairoSurface>> surfaces_;
    static std::unordered_map<ImageHandle, CairoSAMM::unique_ptr<CairoImage>> images_;
    static std::atomic<uint64_t> next_handle_;
    static std::mutex resource_mutex_;
    
    static uint64_t generateHandle();
};

/**
 * @brief Utility functions for vector processing
 */
namespace CairoUtils {
    // Convert BCPL vector data to native types
    std::vector<CairoPoint> pointsFromFloatArray(const double* data, size_t count);
    std::vector<CairoRect> rectsFromFloatArray(const double* data, size_t count);
    std::vector<CairoColor> colorsFromIntArray(const uint32_t* data, size_t count);
    
    // Batch processing helpers
    void drawLinesOptimized(cairo_t* ctx, const std::vector<CairoPoint>& endpoints);
    void fillRectsOptimized(cairo_t* ctx, const std::vector<CairoRect>& rects);
    void fillCirclesOptimized(cairo_t* ctx, const std::vector<CairoPoint>& centers, 
                              const std::vector<double>& radii);
}

/**
 * @brief Error handling utilities
 */
namespace CairoError {
    void checkStatus(cairo_status_t status);
    void checkSurface(cairo_surface_t* surface);
    void checkContext(cairo_t* context);
    std::string getErrorString(cairo_status_t status);
}
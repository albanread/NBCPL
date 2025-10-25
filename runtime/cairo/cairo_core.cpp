#include "cairo_core.h"
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <mutex>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// =============================================================================
// CairoImage Implementation
// =============================================================================

CairoImage::CairoImage(const std::string& filepath) {
    surface_ = cairo_image_surface_create_from_png(filepath.c_str());
    if (cairo_surface_status(surface_) != CAIRO_STATUS_SUCCESS) {
        cleanup();
        throw std::runtime_error("Failed to load PNG: " + filepath + " - " + 
                                 cairo_status_to_string(cairo_surface_status(surface_)));
    }
}

CairoImage::CairoImage(cairo_surface_t* surface) : surface_(surface) {
    if (surface_ && cairo_surface_status(surface_) != CAIRO_STATUS_SUCCESS) {
        cleanup();
        throw std::runtime_error("Invalid Cairo surface provided");
    }
}

CairoImage::~CairoImage() {
    cleanup();
}

CairoImage::CairoImage(CairoImage&& other) noexcept : surface_(other.surface_) {
    other.surface_ = nullptr;
}

CairoImage& CairoImage::operator=(CairoImage&& other) noexcept {
    if (this != &other) {
        cleanup();
        surface_ = other.surface_;
        other.surface_ = nullptr;
    }
    return *this;
}

int CairoImage::getWidth() const {
    if (!surface_) return 0;
    return cairo_image_surface_get_width(surface_);
}

int CairoImage::getHeight() const {
    if (!surface_) return 0;
    return cairo_image_surface_get_height(surface_);
}

void CairoImage::saveAsPNG(const std::string& filepath) const {
    if (!surface_) {
        throw std::runtime_error("Cannot save invalid image");
    }
    cairo_status_t status = cairo_surface_write_to_png(surface_, filepath.c_str());
    if (status != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error("Failed to save PNG: " + filepath + " - " + 
                                 cairo_status_to_string(status));
    }
}

std::unique_ptr<CairoImage> CairoImage::clone() const {
    if (!surface_) return nullptr;
    
    int width = getWidth();
    int height = getHeight();
    cairo_surface_t* new_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    cairo_t* cr = cairo_create(new_surface);
    
    cairo_set_source_surface(cr, surface_, 0, 0);
    cairo_paint(cr);
    
    cairo_destroy(cr);
    return std::make_unique<CairoImage>(new_surface);
}

void CairoImage::cleanup() {
    if (surface_) {
        cairo_surface_destroy(surface_);
        surface_ = nullptr;
    }
}

// =============================================================================
// CairoSurface Implementation
// =============================================================================

CairoSurface::CairoSurface(int width, int height) : width_(width), height_(height) {
    printf("DEBUG: CairoSurface constructor - this=%p, width=%d, height=%d\n", this, width, height);
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
    printf("DEBUG: CairoSurface constructor - surface_ created at %p\n", surface_);
    
    cairo_status_t status = cairo_surface_status(surface_);
    printf("DEBUG: CairoSurface constructor - surface status: %s\n", cairo_status_to_string(status));
    
    if (status != CAIRO_STATUS_SUCCESS) {
        printf("DEBUG: CairoSurface constructor - ERROR: surface creation failed\n");
        cleanup();
        throw std::runtime_error("Failed to create Cairo surface: " + 
                                 std::string(cairo_status_to_string(cairo_surface_status(surface_))));
    }
    
    printf("DEBUG: CairoSurface constructor - calling initializeContext\n");
    initializeContext();
    printf("DEBUG: CairoSurface constructor - completed, context_=%p\n", context_);
}

CairoSurface::CairoSurface(const CairoImage& image) {
    width_ = image.getWidth();
    height_ = image.getHeight();
    
    surface_ = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
    if (cairo_surface_status(surface_) != CAIRO_STATUS_SUCCESS) {
        cleanup();
        throw std::runtime_error("Failed to create Cairo surface from image");
    }
    
    initializeContext();
    
    // Copy image content
    cairo_set_source_surface(context_, image.getSurface(), 0, 0);
    cairo_paint(context_);
}

CairoSurface::~CairoSurface() {
    cleanup();
}

int CairoSurface::getWidth() const {
    return width_;
}

int CairoSurface::getHeight() const {
    return height_;
}

void CairoSurface::saveAsPNG(const std::string& filepath) const {
    if (!surface_) {
        throw std::runtime_error("Cannot save invalid surface");
    }
    cairo_status_t status = cairo_surface_write_to_png(surface_, filepath.c_str());
    if (status != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error("Failed to save PNG: " + filepath + " - " + 
                                 cairo_status_to_string(status));
    }
}

std::unique_ptr<CairoImage> CairoSurface::toImage() const {
    if (!surface_) return nullptr;
    
    cairo_surface_t* image_surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width_, height_);
    cairo_t* cr = cairo_create(image_surface);
    
    cairo_set_source_surface(cr, surface_, 0, 0);
    cairo_paint(cr);
    
    cairo_destroy(cr);
    return std::make_unique<CairoImage>(image_surface);
}

void CairoSurface::clear(const CairoColor& color) {
    if (!context_) return;
    
    cairo_save(context_);
    cairo_set_operator(context_, CAIRO_OPERATOR_SOURCE);
    color.applytoCairo(context_);
    cairo_paint(context_);
    cairo_restore(context_);
}

std::unique_ptr<CairoSurface> CairoSurface::clone() const {
    if (!surface_) return nullptr;
    
    auto new_surface = std::make_unique<CairoSurface>(width_, height_);
    cairo_set_source_surface(new_surface->context_, surface_, 0, 0);
    cairo_paint(new_surface->context_);
    
    return new_surface;
}

// --- Drawing State Management ---

void CairoSurface::setColor(const CairoColor& color) {
    if (context_) {
        color.applytoCairo(context_);
    }
}

void CairoSurface::setLineWidth(double width) {
    if (context_) {
        cairo_set_line_width(context_, width);
    }
}

void CairoSurface::setLineJoin(cairo_line_join_t join) {
    if (context_) {
        cairo_set_line_join(context_, join);
    }
}

void CairoSurface::setLineCap(cairo_line_cap_t cap) {
    if (context_) {
        cairo_set_line_cap(context_, cap);
    }
}

void CairoSurface::setOpacity(double opacity) {
    // Store opacity for use in subsequent operations
    // Cairo doesn't have a global opacity, so we'll need to apply it per operation
    if (context_) {
        cairo_paint_with_alpha(context_, opacity);
    }
}

void CairoSurface::setFont(const std::string& font_name, double font_size) {
    if (context_) {
        cairo_select_font_face(context_, font_name.c_str(), 
                               CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
        cairo_set_font_size(context_, font_size);
    }
}

// --- Basic Shape Drawing ---

void CairoSurface::fillRectangle(double x, double y, double width, double height) {
    printf("DEBUG: fillRectangle - this=%p, context_=%p, surface_=%p\n", this, context_, surface_);
    printf("DEBUG: fillRectangle - params: x=%.1f, y=%.1f, w=%.1f, h=%.1f\n", x, y, width, height);
    
    if (context_) {
        printf("DEBUG: fillRectangle - context is valid, calling cairo functions\n");
        cairo_rectangle(context_, x, y, width, height);
        cairo_status_t status1 = cairo_status(context_);
        printf("DEBUG: fillRectangle - cairo_rectangle status: %s\n", cairo_status_to_string(status1));
        
        cairo_fill(context_);
        cairo_status_t status2 = cairo_status(context_);
        printf("DEBUG: fillRectangle - cairo_fill status: %s\n", cairo_status_to_string(status2));
    } else {
        printf("DEBUG: fillRectangle - ERROR: context_ is NULL!\n");
    }
}

void CairoSurface::strokeRectangle(double x, double y, double width, double height) {
    if (context_) {
        cairo_rectangle(context_, x, y, width, height);
        cairo_stroke(context_);
    }
}

void CairoSurface::fillCircle(double cx, double cy, double radius) {
    if (context_) {
        cairo_arc(context_, cx, cy, radius, 0, 2 * M_PI);
        cairo_fill(context_);
    }
}

void CairoSurface::strokeCircle(double cx, double cy, double radius) {
    if (context_) {
        cairo_arc(context_, cx, cy, radius, 0, 2 * M_PI);
        cairo_stroke(context_);
    }
}

void CairoSurface::drawLine(double x1, double y1, double x2, double y2) {
    if (context_) {
        cairo_move_to(context_, x1, y1);
        cairo_line_to(context_, x2, y2);
        cairo_stroke(context_);
    }
}

void CairoSurface::drawPoint(double x, double y, double size) {
    if (context_) {
        cairo_rectangle(context_, x - size/2, y - size/2, size, size);
        cairo_fill(context_);
    }
}

// --- Path Operations ---

void CairoSurface::beginPath() {
    if (context_) {
        cairo_new_path(context_);
    }
}

void CairoSurface::moveTo(double x, double y) {
    if (context_) {
        cairo_move_to(context_, x, y);
    }
}

void CairoSurface::lineTo(double x, double y) {
    if (context_) {
        cairo_line_to(context_, x, y);
    }
}

void CairoSurface::curveTo(double x1, double y1, double x2, double y2, double x3, double y3) {
    if (context_) {
        cairo_curve_to(context_, x1, y1, x2, y2, x3, y3);
    }
}

void CairoSurface::closePath() {
    if (context_) {
        cairo_close_path(context_);
    }
}

void CairoSurface::fillPath() {
    if (context_) {
        cairo_fill(context_);
    }
}

void CairoSurface::strokePath() {
    if (context_) {
        cairo_stroke(context_);
    }
}

// --- Polygon Operations ---

void CairoSurface::fillPolygon(const std::vector<CairoPoint>& points) {
    if (context_ && !points.empty()) {
        setPathFromPoints(points, true);
        cairo_fill(context_);
    }
}

void CairoSurface::strokePolygon(const std::vector<CairoPoint>& points, bool close) {
    if (context_ && !points.empty()) {
        setPathFromPoints(points, close);
        cairo_stroke(context_);
    }
}

void CairoSurface::drawPolyline(const std::vector<CairoPoint>& points) {
    strokePolygon(points, false);
}

// --- Text Operations ---

void CairoSurface::drawText(double x, double y, const std::string& text) {
    if (context_) {
        cairo_move_to(context_, x, y);
        cairo_show_text(context_, text.c_str());
    }
}

void CairoSurface::drawTextColored(double x, double y, const std::string& text, const CairoColor& color) {
    if (context_) {
        cairo_save(context_);
        color.applytoCairo(context_);
        cairo_move_to(context_, x, y);
        cairo_show_text(context_, text.c_str());
        cairo_restore(context_);
    }
}

void CairoSurface::getTextExtents(const std::string& text, double* width, double* height) const {
    if (context_ && width && height) {
        cairo_text_extents_t extents;
        cairo_text_extents(context_, text.c_str(), &extents);
        *width = extents.width;
        *height = extents.height;
    }
}

// --- Image Operations ---

void CairoSurface::drawImage(const CairoImage& image, double x, double y) {
    if (context_ && image.getSurface()) {
        cairo_set_source_surface(context_, image.getSurface(), x, y);
        cairo_paint(context_);
    }
}

void CairoSurface::drawImageScaled(const CairoImage& image, double x, double y, double scale_x, double scale_y) {
    if (context_ && image.getSurface()) {
        cairo_save(context_);
        cairo_translate(context_, x, y);
        cairo_scale(context_, scale_x, scale_y);
        cairo_set_source_surface(context_, image.getSurface(), 0, 0);
        cairo_paint(context_);
        cairo_restore(context_);
    }
}

void CairoSurface::drawImageRotated(const CairoImage& image, double x, double y, double angle) {
    if (context_ && image.getSurface()) {
        cairo_save(context_);
        cairo_translate(context_, x, y);
        cairo_rotate(context_, angle);
        cairo_translate(context_, -image.getWidth()/2.0, -image.getHeight()/2.0);
        cairo_set_source_surface(context_, image.getSurface(), 0, 0);
        cairo_paint(context_);
        cairo_restore(context_);
    }
}

// --- Batch Operations ---

void CairoSurface::drawLines(const std::vector<CairoPoint>& line_endpoints) {
    if (context_ && line_endpoints.size() >= 2) {
        for (size_t i = 0; i < line_endpoints.size() - 1; i += 2) {
            cairo_move_to(context_, line_endpoints[i].x, line_endpoints[i].y);
            cairo_line_to(context_, line_endpoints[i+1].x, line_endpoints[i+1].y);
        }
        cairo_stroke(context_);
    }
}

void CairoSurface::fillRectangles(const std::vector<CairoRect>& rectangles) {
    if (context_) {
        for (const auto& rect : rectangles) {
            cairo_rectangle(context_, rect.x, rect.y, rect.width, rect.height);
        }
        cairo_fill(context_);
    }
}

void CairoSurface::fillCircles(const std::vector<CairoPoint>& centers, const std::vector<double>& radii) {
    if (context_ && centers.size() == radii.size()) {
        for (size_t i = 0; i < centers.size(); ++i) {
            cairo_arc(context_, centers[i].x, centers[i].y, radii[i], 0, 2 * M_PI);
        }
        cairo_fill(context_);
    }
}

// --- Advanced Operations ---

void CairoSurface::save() {
    if (context_) {
        cairo_save(context_);
    }
}

void CairoSurface::restore() {
    if (context_) {
        cairo_restore(context_);
    }
}

void CairoSurface::translate(double tx, double ty) {
    if (context_) {
        cairo_translate(context_, tx, ty);
    }
}

void CairoSurface::scale(double sx, double sy) {
    if (context_) {
        cairo_scale(context_, sx, sy);
    }
}

void CairoSurface::rotate(double angle) {
    if (context_) {
        cairo_rotate(context_, angle);
    }
}

// --- Private Methods ---

void CairoSurface::cleanup() {
    if (context_) {
        cairo_destroy(context_);
        context_ = nullptr;
    }
    if (surface_) {
        cairo_surface_destroy(surface_);
        surface_ = nullptr;
    }
}

void CairoSurface::initializeContext() {
    printf("DEBUG: initializeContext - this=%p, surface_=%p\n", this, surface_);
    
    if (surface_) {
        printf("DEBUG: initializeContext - surface is valid, creating context\n");
        context_ = cairo_create(surface_);
        printf("DEBUG: initializeContext - context created at %p\n", context_);
        
        cairo_status_t status = cairo_status(context_);
        printf("DEBUG: initializeContext - context status: %s\n", cairo_status_to_string(status));
        
        if (status != CAIRO_STATUS_SUCCESS) {
            printf("DEBUG: initializeContext - ERROR: context creation failed\n");
            cleanup();
            throw std::runtime_error("Failed to create Cairo context: " + 
                                     std::string(cairo_status_to_string(cairo_status(context_))));
        }
        
        // Set high-quality antialiasing
        printf("DEBUG: initializeContext - setting antialiasing\n");
        cairo_set_antialias(context_, CAIRO_ANTIALIAS_DEFAULT);
        cairo_set_antialias(context_, CAIRO_ANTIALIAS_GOOD);
        printf("DEBUG: initializeContext - initialization complete\n");
    } else {
        printf("DEBUG: initializeContext - ERROR: surface_ is NULL\n");
    }
}

void CairoSurface::setPathFromPoints(const std::vector<CairoPoint>& points, bool close) {
    if (context_ && !points.empty()) {
        cairo_new_path(context_);
        cairo_move_to(context_, points[0].x, points[0].y);
        
        for (size_t i = 1; i < points.size(); ++i) {
            cairo_line_to(context_, points[i].x, points[i].y);
        }
        
        if (close) {
            cairo_close_path(context_);
        }
    }
}

// =============================================================================
// CairoResourceManager Implementation
// =============================================================================

std::unordered_map<CairoResourceManager::SurfaceHandle, std::unique_ptr<CairoSurface>> CairoResourceManager::surfaces_;
std::unordered_map<CairoResourceManager::ImageHandle, std::unique_ptr<CairoImage>> CairoResourceManager::images_;
std::atomic<uint64_t> CairoResourceManager::next_handle_{1};
std::mutex CairoResourceManager::resource_mutex_;

CairoResourceManager::SurfaceHandle CairoResourceManager::registerSurface(std::unique_ptr<CairoSurface> surface) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    SurfaceHandle handle = generateHandle();
    printf("DEBUG: registerSurface - generated handle=%llu, surface ptr=%p\n", handle, surface.get());
    surfaces_[handle] = std::move(surface);
    printf("DEBUG: registerSurface - stored surface, map size now=%zu\n", surfaces_.size());
    return handle;
}

CairoResourceManager::ImageHandle CairoResourceManager::registerImage(std::unique_ptr<CairoImage> image) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    ImageHandle handle = generateHandle();
    images_[handle] = std::move(image);
    return handle;
}

CairoSurface* CairoResourceManager::getSurface(SurfaceHandle handle) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    printf("DEBUG: getSurface - looking for handle=%llu in map of size %zu\n", handle, surfaces_.size());
    auto it = surfaces_.find(handle);
    if (it != surfaces_.end()) {
        printf("DEBUG: getSurface - found surface at %p\n", it->second.get());
        return it->second.get();
    } else {
        printf("DEBUG: getSurface - handle %llu NOT FOUND\n", handle);
        return nullptr;
    }
}

CairoImage* CairoResourceManager::getImage(ImageHandle handle) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    auto it = images_.find(handle);
    return (it != images_.end()) ? it->second.get() : nullptr;
}

void CairoResourceManager::releaseSurface(SurfaceHandle handle) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    surfaces_.erase(handle);
}

void CairoResourceManager::releaseImage(ImageHandle handle) {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    images_.erase(handle);
}

void CairoResourceManager::cleanup() {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    surfaces_.clear();
    images_.clear();
}

size_t CairoResourceManager::getSurfaceCount() {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    return surfaces_.size();
}

size_t CairoResourceManager::getImageCount() {
    std::lock_guard<std::mutex> lock(resource_mutex_);
    return images_.size();
}

uint64_t CairoResourceManager::generateHandle() {
    return next_handle_.fetch_add(1);
}

// =============================================================================
// CairoUtils Implementation
// =============================================================================

namespace CairoUtils {

std::vector<CairoPoint> pointsFromFloatArray(const double* data, size_t count) {
    std::vector<CairoPoint> points;
    points.reserve(count / 2);
    
    for (size_t i = 0; i < count - 1; i += 2) {
        points.emplace_back(data[i], data[i + 1]);
    }
    
    return points;
}

std::vector<CairoRect> rectsFromFloatArray(const double* data, size_t count) {
    std::vector<CairoRect> rects;
    rects.reserve(count / 4);
    
    for (size_t i = 0; i < count - 3; i += 4) {
        rects.emplace_back(data[i], data[i + 1], data[i + 2], data[i + 3]);
    }
    
    return rects;
}

std::vector<CairoColor> colorsFromIntArray(const uint32_t* data, size_t count) {
    std::vector<CairoColor> colors;
    colors.reserve(count);
    
    for (size_t i = 0; i < count; ++i) {
        colors.emplace_back(data[i]);
    }
    
    return colors;
}

void drawLinesOptimized(cairo_t* ctx, const std::vector<CairoPoint>& endpoints) {
    if (!ctx || endpoints.size() < 2) return;
    
    for (size_t i = 0; i < endpoints.size() - 1; i += 2) {
        cairo_move_to(ctx, endpoints[i].x, endpoints[i].y);
        cairo_line_to(ctx, endpoints[i + 1].x, endpoints[i + 1].y);
    }
    cairo_stroke(ctx);
}

void fillRectsOptimized(cairo_t* ctx, const std::vector<CairoRect>& rects) {
    if (!ctx) return;
    
    for (const auto& rect : rects) {
        cairo_rectangle(ctx, rect.x, rect.y, rect.width, rect.height);
    }
    cairo_fill(ctx);
}

void fillCirclesOptimized(cairo_t* ctx, const std::vector<CairoPoint>& centers, 
                          const std::vector<double>& radii) {
    if (!ctx || centers.size() != radii.size()) return;
    
    for (size_t i = 0; i < centers.size(); ++i) {
        cairo_arc(ctx, centers[i].x, centers[i].y, radii[i], 0, 2 * M_PI);
    }
    cairo_fill(ctx);
}

} // namespace CairoUtils

// =============================================================================
// CairoError Implementation
// =============================================================================

namespace CairoError {

void checkStatus(cairo_status_t status) {
    if (status != CAIRO_STATUS_SUCCESS) {
        throw std::runtime_error("Cairo error: " + getErrorString(status));
    }
}

void checkSurface(cairo_surface_t* surface) {
    if (!surface) {
        throw std::runtime_error("Null Cairo surface");
    }
    checkStatus(cairo_surface_status(surface));
}

void checkContext(cairo_t* context) {
    if (!context) {
        throw std::runtime_error("Null Cairo context");
    }
    checkStatus(cairo_status(context));
}

std::string getErrorString(cairo_status_t status) {
    return std::string(cairo_status_to_string(status));
}

} // namespace CairoError
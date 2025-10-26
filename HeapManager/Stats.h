#ifndef STATS_H
#define STATS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Allocation types for the stats system
typedef enum {
    STATS_ALLOC_UNKNOWN = 0,
    STATS_ALLOC_VEC,
    STATS_ALLOC_STRING,
    STATS_ALLOC_OBJECT,
    STATS_ALLOC_GENERIC,
    STATS_ALLOC_LIST,
    STATS_ALLOC_GRAPHICS_SURFACE,
    STATS_ALLOC_GRAPHICS_IMAGE
} StatsAllocType;

// C API for the Stats class (for use from C code and runtime)
void stats_init(void);
void stats_shutdown(void);
void stats_update_alloc(size_t bytes, StatsAllocType type);
void stats_update_free(size_t bytes);
void stats_update_double_free(void);
void stats_update_io_read(size_t bytes);
void stats_update_io_write(size_t bytes);
void stats_update_file_opened(void);
void stats_update_file_closed(void);
void stats_update_graphics_surface_created(size_t bytes);
void stats_update_graphics_surface_freed(size_t bytes);
void stats_update_graphics_image_created(size_t bytes);
void stats_update_graphics_image_freed(size_t bytes);
void stats_print_metrics(void);

// Getter functions for metrics
size_t stats_get_total_allocs(void);
size_t stats_get_total_bytes_allocated(void);
size_t stats_get_total_frees(void);
size_t stats_get_total_bytes_freed(void);
size_t stats_get_vec_allocs(void);
size_t stats_get_string_allocs(void);
size_t stats_get_double_free_attempts(void);
size_t stats_get_bytes_read(void);
size_t stats_get_bytes_written(void);
size_t stats_get_files_opened(void);
size_t stats_get_files_closed(void);
size_t stats_get_graphics_surfaces_created(void);
size_t stats_get_graphics_surfaces_freed(void);
size_t stats_get_graphics_surface_bytes_allocated(void);
size_t stats_get_graphics_surface_bytes_freed(void);
size_t stats_get_graphics_images_created(void);
size_t stats_get_graphics_images_freed(void);
size_t stats_get_graphics_image_bytes_allocated(void);
size_t stats_get_graphics_image_bytes_freed(void);

#ifdef __cplusplus
}

// C++ Stats class implementation
class Stats {
private:
    // Memory allocation metrics
    size_t total_bytes_allocated;
    size_t total_bytes_freed;
    size_t total_allocs;
    size_t total_frees;
    size_t vec_allocs;
    size_t string_allocs;
    size_t double_free_attempts;
    
    // File I/O metrics
    size_t bytes_read;
    size_t bytes_written;
    size_t files_opened;
    size_t files_closed;
    
    // Graphics resource metrics
    size_t graphics_surfaces_created;
    size_t graphics_surfaces_freed;
    size_t graphics_surface_bytes_allocated;
    size_t graphics_surface_bytes_freed;
    size_t graphics_images_created;
    size_t graphics_images_freed;
    size_t graphics_image_bytes_allocated;
    size_t graphics_image_bytes_freed;
    
    // Singleton instance
    static Stats* instance;
    
    // Private constructor for singleton
    Stats();
    
public:
    // Singleton access
    static Stats& getInstance();
    static void destroyInstance();
    
    // Memory allocation tracking
    void updateAlloc(size_t bytes, StatsAllocType type);
    void updateFree(size_t bytes);
    void updateDoubleFree();
    
    // File I/O tracking
    void updateIORead(size_t bytes);
    void updateIOWrite(size_t bytes);
    void updateFileOpened();
    void updateFileClosed();
    
    // Graphics resource tracking
    void updateGraphicsSurfaceCreated(size_t bytes);
    void updateGraphicsSurfaceFreed(size_t bytes);
    void updateGraphicsImageCreated(size_t bytes);
    void updateGraphicsImageFreed(size_t bytes);
    
    // Getter methods
    size_t getTotalAllocs() const { return total_allocs; }
    size_t getTotalBytesAllocated() const { return total_bytes_allocated; }
    size_t getTotalFrees() const { return total_frees; }
    size_t getTotalBytesFreed() const { return total_bytes_freed; }
    size_t getVecAllocs() const { return vec_allocs; }
    size_t getStringAllocs() const { return string_allocs; }
    size_t getDoubleFreeAttempts() const { return double_free_attempts; }
    size_t getBytesRead() const { return bytes_read; }
    size_t getBytesWritten() const { return bytes_written; }
    size_t getFilesOpened() const { return files_opened; }
    size_t getFilesClosed() const { return files_closed; }
    size_t getGraphicsSurfacesCreated() const { return graphics_surfaces_created; }
    size_t getGraphicsSurfacesFreed() const { return graphics_surfaces_freed; }
    size_t getGraphicsSurfaceBytesAllocated() const { return graphics_surface_bytes_allocated; }
    size_t getGraphicsSurfaceBytesFreed() const { return graphics_surface_bytes_freed; }
    size_t getGraphicsImagesCreated() const { return graphics_images_created; }
    size_t getGraphicsImagesFreed() const { return graphics_images_freed; }
    size_t getGraphicsImageBytesAllocated() const { return graphics_image_bytes_allocated; }
    size_t getGraphicsImageBytesFreed() const { return graphics_image_bytes_freed; }
    
    // Metrics display
    void printMetrics() const;
    
    // Reset all metrics (for testing)
    void reset();
};

#endif // __cplusplus

#endif // STATS_H
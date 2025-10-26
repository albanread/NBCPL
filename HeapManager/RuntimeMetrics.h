#ifndef RUNTIME_METRICS_H
#define RUNTIME_METRICS_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Simple global metrics structure - visible across all compilation units
typedef struct {
    // Memory allocation metrics
    volatile size_t total_bytes_allocated;
    volatile size_t total_bytes_freed;
    volatile size_t total_allocs;
    volatile size_t total_frees;
    volatile size_t vec_allocs;
    volatile size_t string_allocs;
    volatile size_t object_allocs;
    volatile size_t double_free_attempts;
    
    // Graphics resource metrics
    volatile size_t graphics_surfaces_created;
    volatile size_t graphics_surfaces_freed;
    volatile size_t graphics_surface_bytes_allocated;
    volatile size_t graphics_surface_bytes_freed;
    volatile size_t graphics_images_created;
    volatile size_t graphics_images_freed;
    volatile size_t graphics_image_bytes_allocated;
    volatile size_t graphics_image_bytes_freed;
    
    // File I/O metrics
    volatile size_t bytes_read;
    volatile size_t bytes_written;
    volatile size_t files_opened;
    volatile size_t files_closed;
} RuntimeMetrics;

// Global metrics instance - declared here, defined in RuntimeMetrics.c
extern RuntimeMetrics g_runtime_metrics;

// Thread-safe update functions
void metrics_update_alloc(size_t bytes, int alloc_type);
void metrics_update_free(size_t bytes);
void metrics_update_double_free(void);
void metrics_update_io_read(size_t bytes);
void metrics_update_io_write(size_t bytes);
void metrics_update_file_opened(void);
void metrics_update_file_closed(void);
void metrics_update_graphics_surface_created(size_t bytes);
void metrics_update_graphics_surface_freed(size_t bytes);
void metrics_update_graphics_image_created(size_t bytes);
void metrics_update_graphics_image_freed(size_t bytes);

// Metrics display
void metrics_print_all(void);

// Reset all metrics (for testing)
void metrics_reset_all(void);

// Allocation types for metrics_update_alloc
#define METRICS_ALLOC_UNKNOWN 0
#define METRICS_ALLOC_VEC     1
#define METRICS_ALLOC_STRING  2
#define METRICS_ALLOC_OBJECT  3
#define METRICS_ALLOC_GENERIC 4

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_METRICS_H
#include "Stats.h"
#include "HeapManager.h"
#include <stdio.h>
#include <cstdio>

// Initialize static instance
Stats* Stats::instance = nullptr;

// Constructor
Stats::Stats() : 
    total_bytes_allocated(0),
    total_bytes_freed(0),
    total_allocs(0),
    total_frees(0),
    vec_allocs(0),
    string_allocs(0),
    double_free_attempts(0),
    bytes_read(0),
    bytes_written(0),
    files_opened(0),
    files_closed(0),
    graphics_surfaces_created(0),
    graphics_surfaces_freed(0),
    graphics_surface_bytes_allocated(0),
    graphics_surface_bytes_freed(0),
    graphics_images_created(0),
    graphics_images_freed(0),
    graphics_image_bytes_allocated(0),
    graphics_image_bytes_freed(0) {
    printf("DEBUG: Stats singleton instance created at %p\n", this);
}

// Singleton access
Stats& Stats::getInstance() {
    if (!instance) {
        instance = new Stats();
        printf("DEBUG: Stats singleton getInstance() - created new instance at %p\n", instance);
    } else {
        printf("DEBUG: Stats singleton getInstance() - returning existing instance at %p\n", instance);
    }
    return *instance;
}

void Stats::destroyInstance() {
    printf("DEBUG: Stats singleton destroyInstance() - destroying instance at %p\n", instance);
    delete instance;
    instance = nullptr;
}

// Memory allocation tracking
void Stats::updateAlloc(size_t bytes, StatsAllocType type) {
    printf("DEBUG: Stats::updateAlloc called - bytes=%zu, type=%d\n", bytes, (int)type);
    printf("DEBUG: Before update - total_allocs=%zu, total_bytes_allocated=%zu\n", 
           total_allocs, total_bytes_allocated);
    
    total_bytes_allocated += bytes;
    total_allocs++;
    
    switch (type) {
        case STATS_ALLOC_VEC:
            vec_allocs++;
            break;
        case STATS_ALLOC_STRING:
            string_allocs++;
            break;
        case STATS_ALLOC_GRAPHICS_SURFACE:
            graphics_surfaces_created++;
            graphics_surface_bytes_allocated += bytes;
            break;
        case STATS_ALLOC_GRAPHICS_IMAGE:
            graphics_images_created++;
            graphics_image_bytes_allocated += bytes;
            break;
        default:
            // Generic allocation, no specific counter
            break;
    }
    
    printf("DEBUG: After update - total_allocs=%zu, total_bytes_allocated=%zu\n", 
           total_allocs, total_bytes_allocated);
}

void Stats::updateFree(size_t bytes) {
    total_bytes_freed += bytes;
    total_frees++;
}

void Stats::updateDoubleFree() {
    double_free_attempts++;
}

// File I/O tracking
void Stats::updateIORead(size_t bytes) {
    bytes_read += bytes;
}

void Stats::updateIOWrite(size_t bytes) {
    bytes_written += bytes;
}

void Stats::updateFileOpened() {
    files_opened++;
}

void Stats::updateFileClosed() {
    files_closed++;
}

// Graphics resource tracking
void Stats::updateGraphicsSurfaceCreated(size_t bytes) {
    graphics_surfaces_created++;
    graphics_surface_bytes_allocated += bytes;
    total_bytes_allocated += bytes;
    total_allocs++;
}

void Stats::updateGraphicsSurfaceFreed(size_t bytes) {
    graphics_surfaces_freed++;
    graphics_surface_bytes_freed += bytes;
    total_bytes_freed += bytes;
    total_frees++;
}

void Stats::updateGraphicsImageCreated(size_t bytes) {
    graphics_images_created++;
    graphics_image_bytes_allocated += bytes;
    total_bytes_allocated += bytes;
    total_allocs++;
}

void Stats::updateGraphicsImageFreed(size_t bytes) {
    graphics_images_freed++;
    graphics_image_bytes_freed += bytes;
    total_bytes_freed += bytes;
    total_frees++;
}

// Reset all metrics (for testing)
void Stats::reset() {
    total_bytes_allocated = 0;
    total_bytes_freed = 0;
    total_allocs = 0;
    total_frees = 0;
    vec_allocs = 0;
    string_allocs = 0;
    double_free_attempts = 0;
    bytes_read = 0;
    bytes_written = 0;
    files_opened = 0;
    files_closed = 0;
    graphics_surfaces_created = 0;
    graphics_surfaces_freed = 0;
    graphics_surface_bytes_allocated = 0;
    graphics_surface_bytes_freed = 0;
    graphics_images_created = 0;
    graphics_images_freed = 0;
    graphics_image_bytes_allocated = 0;
    graphics_image_bytes_freed = 0;
}

// Metrics display
void Stats::printMetrics() const {
    printf("DEBUG: Stats::printMetrics called on instance at %p - total_allocs=%zu, total_bytes_allocated=%zu\n", 
           this, total_allocs, total_bytes_allocated);
    
    printf("\n--- BCPL Runtime Metrics ---\n");
    printf("Memory allocations: %zu (%zu bytes)\n", total_allocs, total_bytes_allocated);
    printf("Memory frees: %zu (%zu bytes)\n", total_frees, total_bytes_freed);
    printf("Vector allocations: %zu\n", vec_allocs);
    printf("String allocations: %zu\n", string_allocs);
    printf("Double-free attempts: %zu\n", double_free_attempts);
    printf("Current active allocations: %zu (%zu bytes)\n", 
           total_allocs - total_frees, 
           total_bytes_allocated - total_bytes_freed);
    
    // Graphics resource metrics
    printf("Graphics Resources:\n");
    printf("  Surfaces created: %zu (%zu bytes)\n", 
           graphics_surfaces_created, graphics_surface_bytes_allocated);
    printf("  Surfaces freed: %zu (%zu bytes)\n", 
           graphics_surfaces_freed, graphics_surface_bytes_freed);
    printf("  Active surfaces: %zu (%zu bytes)\n", 
           graphics_surfaces_created - graphics_surfaces_freed,
           graphics_surface_bytes_allocated - graphics_surface_bytes_freed);
    printf("  Images created: %zu (%zu bytes)\n", 
           graphics_images_created, graphics_image_bytes_allocated);
    printf("  Images freed: %zu (%zu bytes)\n", 
           graphics_images_freed, graphics_image_bytes_freed);
    printf("  Active images: %zu (%zu bytes)\n", 
           graphics_images_created - graphics_images_freed,
           graphics_image_bytes_allocated - graphics_image_bytes_freed);
    
    // Bloom filter metrics
    HeapManager& heap_mgr = HeapManager::getInstance();
    printf("Bloom filter statistics:\n");
    printf("  Items tracked: %zu\n", heap_mgr.getBloomFilterItemsAdded());
    printf("  Memory usage: %zu bytes\n", heap_mgr.getBloomFilterMemoryUsage());
    printf("  False positives: %zu\n", heap_mgr.getBloomFilterFalsePositives());
    printf("  Est. false positive rate: %.4f%%\n", heap_mgr.getBloomFilterFalsePositiveRate() * 100.0);
    
    printf("File I/O operations:\n");
    printf("  Files opened: %zu\n", files_opened);
    printf("  Files closed: %zu\n", files_closed);
    printf("  Bytes read: %zu\n", bytes_read);
    printf("  Bytes written: %zu\n", bytes_written);
    printf("  Open files: %zu\n", files_opened - files_closed);
    printf("--------------------------\n");
}

// C API implementations
extern "C" {
    void stats_init(void) {
        Stats::getInstance(); // Initialize singleton
    }
    
    void stats_shutdown(void) {
        Stats::destroyInstance();
    }
    
    void stats_update_alloc(size_t bytes, StatsAllocType type) {
        printf("DEBUG: stats_update_alloc C API called - bytes=%zu, type=%d\n", bytes, (int)type);
        Stats::getInstance().updateAlloc(bytes, type);
    }
    
    void stats_update_free(size_t bytes) {
        Stats::getInstance().updateFree(bytes);
    }
    
    void stats_update_double_free(void) {
        Stats::getInstance().updateDoubleFree();
    }
    
    void stats_update_io_read(size_t bytes) {
        Stats::getInstance().updateIORead(bytes);
    }
    
    void stats_update_io_write(size_t bytes) {
        Stats::getInstance().updateIOWrite(bytes);
    }
    
    void stats_update_file_opened(void) {
        Stats::getInstance().updateFileOpened();
    }
    
    void stats_update_file_closed(void) {
        Stats::getInstance().updateFileClosed();
    }
    
    void stats_update_graphics_surface_created(size_t bytes) {
        Stats::getInstance().updateGraphicsSurfaceCreated(bytes);
    }
    
    void stats_update_graphics_surface_freed(size_t bytes) {
        Stats::getInstance().updateGraphicsSurfaceFreed(bytes);
    }
    
    void stats_update_graphics_image_created(size_t bytes) {
        Stats::getInstance().updateGraphicsImageCreated(bytes);
    }
    
    void stats_update_graphics_image_freed(size_t bytes) {
        Stats::getInstance().updateGraphicsImageFreed(bytes);
    }
    
    void stats_print_metrics(void) {
        printf("DEBUG: stats_print_metrics C API called\n");
        Stats::getInstance().printMetrics();
    }
    
    // Getter functions
    size_t stats_get_total_allocs(void) {
        return Stats::getInstance().getTotalAllocs();
    }
    
    size_t stats_get_total_bytes_allocated(void) {
        return Stats::getInstance().getTotalBytesAllocated();
    }
    
    size_t stats_get_total_frees(void) {
        return Stats::getInstance().getTotalFrees();
    }
    
    size_t stats_get_total_bytes_freed(void) {
        return Stats::getInstance().getTotalBytesFreed();
    }
    
    size_t stats_get_vec_allocs(void) {
        return Stats::getInstance().getVecAllocs();
    }
    
    size_t stats_get_string_allocs(void) {
        return Stats::getInstance().getStringAllocs();
    }
    
    size_t stats_get_double_free_attempts(void) {
        return Stats::getInstance().getDoubleFreeAttempts();
    }
    
    size_t stats_get_bytes_read(void) {
        return Stats::getInstance().getBytesRead();
    }
    
    size_t stats_get_bytes_written(void) {
        return Stats::getInstance().getBytesWritten();
    }
    
    size_t stats_get_files_opened(void) {
        return Stats::getInstance().getFilesOpened();
    }
    
    size_t stats_get_files_closed(void) {
        return Stats::getInstance().getFilesClosed();
    }
    
    size_t stats_get_graphics_surfaces_created(void) {
        return Stats::getInstance().getGraphicsSurfacesCreated();
    }
    
    size_t stats_get_graphics_surfaces_freed(void) {
        return Stats::getInstance().getGraphicsSurfacesFreed();
    }
    
    size_t stats_get_graphics_surface_bytes_allocated(void) {
        return Stats::getInstance().getGraphicsSurfaceBytesAllocated();
    }
    
    size_t stats_get_graphics_surface_bytes_freed(void) {
        return Stats::getInstance().getGraphicsSurfaceBytesFreed();
    }
    
    size_t stats_get_graphics_images_created(void) {
        return Stats::getInstance().getGraphicsImagesCreated();
    }
    
    size_t stats_get_graphics_images_freed(void) {
        return Stats::getInstance().getGraphicsImagesFreed();
    }
    
    size_t stats_get_graphics_image_bytes_allocated(void) {
        return Stats::getInstance().getGraphicsImageBytesAllocated();
    }
    
    size_t stats_get_graphics_image_bytes_freed(void) {
        return Stats::getInstance().getGraphicsImageBytesFreed();
    }
}
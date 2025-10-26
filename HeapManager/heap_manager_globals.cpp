#include "heap_manager_defs.h"
#include "HeapManager.h"
#include "Stats.h"
#include <stdio.h>
#include <stdbool.h>

// A signal-safe shadow copy of heap metadata.
// 'volatile' is crucial to prevent compiler optimizations that could hide updates from the signal handler.
volatile HeapBlock g_shadow_heap_blocks[MAX_HEAP_BLOCKS];
volatile size_t g_shadow_heap_index = 0;

// Global flag for signal handler to check if tracing is enabled.
volatile bool g_is_heap_tracing_enabled = false;

// (Global heap tracking array removed for JIT refactor)

// Legacy global variables - now just wrappers for Stats class
// These are kept for compatibility but redirect to the Stats singleton
size_t g_total_bytes_allocated = 0;
size_t g_total_bytes_freed = 0;
size_t g_total_allocs = 0;
size_t g_total_frees = 0;
size_t g_vec_allocs = 0;
size_t g_string_allocs = 0;
size_t g_double_free_attempts = 0;
size_t g_bytes_read = 0;
size_t g_bytes_written = 0;
size_t g_files_opened = 0;
size_t g_files_closed = 0;

// Functions to update metrics (for internal use)
void update_alloc_metrics(size_t bytes, AllocType type) {
    // Convert AllocType to StatsAllocType
    StatsAllocType stats_type;
    switch (type) {
        case ALLOC_VEC:
            stats_type = STATS_ALLOC_VEC;
            break;
        case ALLOC_STRING:
            stats_type = STATS_ALLOC_STRING;
            break;
        case ALLOC_OBJECT:
            stats_type = STATS_ALLOC_OBJECT;
            break;
        case ALLOC_LIST:
            stats_type = STATS_ALLOC_LIST;
            break;
        default:
            stats_type = STATS_ALLOC_GENERIC;
            break;
    }
    
    // Use Stats singleton
    stats_update_alloc(bytes, stats_type);
    
    // IMPORTANT: Also update HeapManager counters for cross-process metrics
    HeapManager& heap_mgr = HeapManager::getInstance();
    switch (type) {
        case ALLOC_VEC:
            heap_mgr.incrementVectorAllocations(bytes);
            break;
        case ALLOC_STRING:
            heap_mgr.incrementStringAllocations(bytes);
            break;
        case ALLOC_OBJECT:
            heap_mgr.incrementObjectAllocations(bytes);
            break;
        default:
            heap_mgr.incrementObjectAllocations(bytes); // Treat unknown as object
            break;
    }
    
    // Update legacy globals for compatibility
    g_total_bytes_allocated = stats_get_total_bytes_allocated();
    g_total_allocs = stats_get_total_allocs();
    g_vec_allocs = stats_get_vec_allocs();
    g_string_allocs = stats_get_string_allocs();
}

void update_free_metrics(size_t bytes) {
    stats_update_free(bytes);
    
    // Update HeapManager counters as well
    HeapManager& heap_mgr = HeapManager::getInstance();
    heap_mgr.incrementFrees(bytes, ALLOC_GENERIC); // Generic type for frees
    
    // Update legacy globals for compatibility
    g_total_bytes_freed = stats_get_total_bytes_freed();
    g_total_frees = stats_get_total_frees();
}

void update_double_free_metrics(void) {
    stats_update_double_free();
    
    // Update HeapManager counters as well
    HeapManager& heap_mgr = HeapManager::getInstance();
    heap_mgr.incrementDoubleFreeAttempts();
    
    // Update legacy globals for compatibility
    g_double_free_attempts = stats_get_double_free_attempts();
}

// File I/O metrics functions
void update_io_metrics_read(size_t bytes) {
    stats_update_io_read(bytes);
    g_bytes_read = stats_get_bytes_read();
}

void update_io_metrics_write(size_t bytes) {
    stats_update_io_write(bytes);
    g_bytes_written = stats_get_bytes_written();
}

void update_io_metrics_file_opened(void) {
    stats_update_file_opened();
    g_files_opened = stats_get_files_opened();
}

void update_io_metrics_file_closed(void) {
    stats_update_file_closed();
    g_files_closed = stats_get_files_closed();
}

// Public API: Print runtime memory metrics
void print_runtime_metrics(void) {
    HeapManager& heap_mgr = HeapManager::getInstance();
    
    printf("\n--- BCPL Runtime Metrics ---\n");
    printf("Memory allocations: %zu (%zu bytes)\n", 
           heap_mgr.getTotalObjectsAllocated() + heap_mgr.getTotalVectorsAllocated() + heap_mgr.getTotalStringsAllocated(),
           heap_mgr.getTotalBytesAllocated());
    printf("Memory frees: %zu (%zu bytes)\n", 
           heap_mgr.getTotalVectorsFreed() + heap_mgr.getTotalStringsFreed(), 
           heap_mgr.getTotalBytesFreed());
    printf("Vector allocations: %zu\n", heap_mgr.getTotalVectorsAllocated());
    printf("String allocations: %zu\n", heap_mgr.getTotalStringsAllocated());
    printf("Object allocations: %zu\n", heap_mgr.getTotalObjectsAllocated());
    printf("Double-free attempts: %zu\n", heap_mgr.getTotalDoubleFreeAttempts());
    printf("Current active allocations: %zu (%zu bytes)\n", 
           (heap_mgr.getTotalObjectsAllocated() + heap_mgr.getTotalVectorsAllocated() + heap_mgr.getTotalStringsAllocated()) -
           (heap_mgr.getTotalVectorsFreed() + heap_mgr.getTotalStringsFreed()),
           heap_mgr.getTotalBytesAllocated() - heap_mgr.getTotalBytesFreed());
    
    // Bloom filter metrics
    printf("Bloom filter statistics:\n");
    printf("  Items tracked: %zu\n", heap_mgr.getBloomFilterItemsAdded());
    printf("  Memory usage: %zu bytes\n", heap_mgr.getBloomFilterMemoryUsage());
    printf("  False positives: %zu\n", heap_mgr.getBloomFilterFalsePositives());
    printf("  Est. false positive rate: %.4f%%\n", heap_mgr.getBloomFilterFalsePositiveRate() * 100.0);
    
    printf("--------------------------\n");
}

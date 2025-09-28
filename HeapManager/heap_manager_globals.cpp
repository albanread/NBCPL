#include "heap_manager_defs.h"
#include "HeapManager.h"
#include <stdio.h>
#include <stdbool.h>

// A signal-safe shadow copy of heap metadata.
// 'volatile' is crucial to prevent compiler optimizations that could hide updates from the signal handler.
volatile HeapBlock g_shadow_heap_blocks[MAX_HEAP_BLOCKS];
volatile size_t g_shadow_heap_index = 0;

// Global flag for signal handler to check if tracing is enabled.
volatile bool g_is_heap_tracing_enabled = false;

// (Global heap tracking array removed for JIT refactor)

// Runtime metrics tracking
static size_t g_total_bytes_allocated = 0;
static size_t g_total_bytes_freed = 0;
static size_t g_total_allocs = 0;
static size_t g_total_frees = 0;
static size_t g_vec_allocs = 0;
static size_t g_string_allocs = 0;
static size_t g_double_free_attempts = 0;

// File I/O metrics tracking
static size_t g_bytes_read = 0;
static size_t g_bytes_written = 0;
static size_t g_files_opened = 0;
static size_t g_files_closed = 0;

// Functions to update metrics (for internal use)
void update_alloc_metrics(size_t bytes, AllocType type) {
    g_total_bytes_allocated += bytes;
    g_total_allocs++;
    
    if (type == ALLOC_VEC) {
        g_vec_allocs++;
    } else if (type == ALLOC_STRING) {
        g_string_allocs++;
    }
}

void update_free_metrics(size_t bytes) {
    g_total_bytes_freed += bytes;
    g_total_frees++;
}

void update_double_free_metrics(void) {
    g_double_free_attempts++;
}

// File I/O metrics functions
void update_io_metrics_read(size_t bytes) {
    g_bytes_read += bytes;
}

void update_io_metrics_write(size_t bytes) {
    g_bytes_written += bytes;
}

void update_io_metrics_file_opened(void) {
    g_files_opened++;
}

void update_io_metrics_file_closed(void) {
    g_files_closed++;
}

// Public API: Print runtime memory metrics
void print_runtime_metrics(void) {
    printf("\n--- BCPL Runtime Metrics ---\n");
    printf("Memory allocations: %zu (%zu bytes)\n", g_total_allocs, g_total_bytes_allocated);
    printf("Memory frees: %zu (%zu bytes)\n", g_total_frees, g_total_bytes_freed);
    printf("Vector allocations: %zu\n", g_vec_allocs);
    printf("String allocations: %zu\n", g_string_allocs);
    printf("Double-free attempts: %zu\n", g_double_free_attempts);
    printf("Current active allocations: %zu (%zu bytes)\n", 
           g_total_allocs - g_total_frees, 
           g_total_bytes_allocated - g_total_bytes_freed);
    
    // Bloom filter metrics
    HeapManager& heap_mgr = HeapManager::getInstance();
    printf("Bloom filter statistics:\n");
    printf("  Items tracked: %zu\n", heap_mgr.getBloomFilterItemsAdded());
    printf("  Memory usage: %zu bytes\n", heap_mgr.getBloomFilterMemoryUsage());
    printf("  False positives: %zu\n", heap_mgr.getBloomFilterFalsePositives());
    printf("  Est. false positive rate: %.4f%%\n", heap_mgr.getBloomFilterFalsePositiveRate() * 100.0);
    
    printf("File I/O operations:\n");
    printf("  Files opened: %zu\n", g_files_opened);
    printf("  Files closed: %zu\n", g_files_closed);
    printf("  Bytes read: %zu\n", g_bytes_read);
    printf("  Bytes written: %zu\n", g_bytes_written);
    printf("  Open files: %zu\n", g_files_opened - g_files_closed);
    printf("--------------------------\n");
}

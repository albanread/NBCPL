/*
 * fast_string_allocator.h
 * High-performance string allocator for BCPL runtime
 * 
 * This allocator provides a drop-in replacement for bcpl_alloc_chars() with
 * significant performance improvements through size-class pooling, similar
 * to the successful freelist allocator for ListAtoms.
 * 
 * Key features:
 * - Size-class based pools (8, 16, 32, 64, 128, 256, 512, 1024+ chars)
 * - Bulk allocation with adaptive scaling under pressure
 * - Thread-safe with minimal locking overhead
 * - Integration with existing SAMM scope management
 * - Zero API changes - drop-in replacement for bcpl_alloc_chars()
 */

#ifndef FAST_STRING_ALLOCATOR_H
#define FAST_STRING_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Configuration Constants ---
#define FAST_STRING_SIZE_CLASSES 8
#define FAST_STRING_INITIAL_CHUNK_SIZE 512
#define FAST_STRING_MAX_CHUNK_SIZE 32768
#define FAST_STRING_SCALING_FACTOR 4

// String size classes (in characters, not including null terminator)
static const size_t FAST_STRING_SIZE_CLASSES_ARRAY[FAST_STRING_SIZE_CLASSES] = {
    8, 16, 32, 64, 128, 256, 512, 1024
};

// --- Internal Types ---

// Pool entry for free string slots
typedef struct FastStringEntry {
    struct FastStringEntry* next;
    size_t capacity;  // Actual string capacity in characters
} FastStringEntry;

// Size class pool (similar to freelist design)
typedef struct FastStringPool {
    FastStringEntry* free_head;
    size_t class_size;           // Size class in characters
    size_t current_chunk_size;   // Current bulk allocation size
    size_t total_allocated;      // Total strings allocated from heap
    size_t total_requests;       // Total allocation requests
    size_t reuse_count;          // Strings reused from pool
    size_t scaling_events;       // Number of times we scaled up
} FastStringPool;

// Main allocator structure
typedef struct FastStringAllocator {
    FastStringPool size_pools[FAST_STRING_SIZE_CLASSES];
    
    // Statistics
    size_t total_strings_allocated;
    size_t total_strings_freed;
    size_t total_bytes_allocated;
    size_t pool_hits;            // Allocations satisfied by pool
    size_t heap_fallbacks;       // Direct heap allocations (oversized)
    
    // Threading (uses void* to avoid pthread dependency in header)
    void* mutex;
    bool initialized;
} FastStringAllocator;

// Performance statistics
typedef struct FastStringStats {
    size_t total_allocated;
    size_t total_freed;
    size_t bytes_allocated;
    size_t pool_hits;
    size_t heap_fallbacks;
    double pool_hit_rate;        // Percentage of requests satisfied by pool
    size_t size_class_requests[FAST_STRING_SIZE_CLASSES];
    size_t size_class_reuse_rates[FAST_STRING_SIZE_CLASSES];
} FastStringStats;

// --- Public API ---

/**
 * Initialize the fast string allocator
 * Called automatically on first allocation, but can be called explicitly
 * @return true on success, false on failure
 */
bool fast_string_allocator_init(void);

/**
 * Shutdown and cleanup the allocator
 * Frees all internal structures (but not allocated strings)
 */
void fast_string_allocator_shutdown(void);

/**
 * Optimized replacement for bcpl_alloc_chars()
 * 
 * This is the main optimization entry point. It provides identical semantics
 * to the original bcpl_alloc_chars() but with much better performance through
 * pooling.
 * 
 * @param num_chars Number of characters needed (excluding null terminator)
 * @return Pointer to string payload (uint32_t array), or NULL on failure
 */
void* fast_bcpl_alloc_chars(int64_t num_chars);

/**
 * Return a string to the pool for reuse
 * 
 * @param string_payload Pointer returned by fast_bcpl_alloc_chars()
 */
void fast_bcpl_free_chars(void* string_payload);

/**
 * Get allocator performance statistics
 */
FastStringStats fast_string_get_stats(void);

/**
 * Print detailed performance metrics
 */
void fast_string_print_metrics(void);

/**
 * Force replenishment of a specific size class
 * Useful for preallocation or testing
 * 
 * @param size_class_index Index into FAST_STRING_SIZE_CLASSES_ARRAY
 */
void fast_string_replenish_size_class(size_t size_class_index);

/**
 * Integration with SAMM - track allocation in current scope
 * This maintains compatibility with the existing SAMM system
 * 
 * @param string_payload Pointer to track
 */
void fast_string_samm_track(void* string_payload);

/**
 * SAMM cleanup - return strings in scope to pool
 * Called by SAMM during scope exit
 * 
 * @param scope_strings Array of string pointers to free
 * @param count Number of strings in array
 */
void fast_string_samm_cleanup_scope(void** scope_strings, size_t count);

// --- Utility Functions ---

/**
 * Determine which size class a string belongs to
 * 
 * @param num_chars Number of characters
 * @return Size class index, or FAST_STRING_SIZE_CLASSES for oversized
 */
size_t fast_string_get_size_class(size_t num_chars);

/**
 * Get the actual capacity for a size class
 * 
 * @param size_class_index Index into size classes array
 * @return Capacity in characters
 */
size_t fast_string_get_class_capacity(size_t size_class_index);

/**
 * Check if allocator is initialized and healthy
 */
bool fast_string_allocator_is_healthy(void);

/**
 * Reset statistics (useful for benchmarking)
 */
void fast_string_reset_stats(void);

#ifdef __cplusplus
}
#endif

#endif // FAST_STRING_ALLOCATOR_H
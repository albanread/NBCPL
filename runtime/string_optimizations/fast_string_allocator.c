/*
 * fast_string_allocator.c
 * High-performance string allocator implementation for BCPL runtime
 * 
 * This provides a drop-in replacement for bcpl_alloc_chars() with significant
 * performance improvements through size-class pooling, inspired by the
 * successful freelist allocator design.
 */

#include "fast_string_allocator.h"
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>
#include <stdint.h>

// --- Global Allocator Instance ---
static FastStringAllocator g_fast_string_allocator = {0};
static pthread_mutex_t g_allocator_mutex = PTHREAD_MUTEX_INITIALIZER;

// --- Internal Helper Functions ---

static size_t get_size_class_index(size_t num_chars) {
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        if (num_chars <= FAST_STRING_SIZE_CLASSES_ARRAY[i]) {
            return i;
        }
    }
    return FAST_STRING_SIZE_CLASSES; // Oversized - use heap fallback
}

// Calculate total memory needed for a string entry
static size_t calculate_entry_size(size_t string_capacity) {
    // Entry header + length metadata + string data + null terminator
    return sizeof(FastStringEntry) + sizeof(uint64_t) + (string_capacity + 1) * sizeof(uint32_t);
}

// Get pointer to string data from pool entry
static uint32_t* get_string_data_from_entry(FastStringEntry* entry) {
    char* entry_ptr = (char*)entry;
    uint64_t* length_ptr = (uint64_t*)(entry_ptr + sizeof(FastStringEntry));
    return (uint32_t*)(length_ptr + 1);
}

// Get pool entry from string data pointer
static FastStringEntry* get_entry_from_string_data(uint32_t* string_data) {
    uint64_t* length_ptr = ((uint64_t*)string_data) - 1;
    char* entry_ptr = ((char*)length_ptr) - sizeof(FastStringEntry);
    return (FastStringEntry*)entry_ptr;
}

// Replenish a size class pool with more string slots
static void replenish_size_class_pool(FastStringPool* pool, size_t size_class_index) {
    size_t string_capacity = pool->class_size;
    size_t chunk_size = pool->current_chunk_size;
    size_t entry_size = calculate_entry_size(string_capacity);
    size_t total_size = chunk_size * entry_size;
    
    // Allocate one large chunk
    char* chunk = (char*)malloc(total_size);
    if (!chunk) {
        fprintf(stderr, "FastStringAllocator: Failed to allocate chunk for size class %zu (%zu bytes)\n", 
                size_class_index, total_size);
        return;
    }
    
    // Initialize all entries in the chunk and link them
    FastStringEntry* prev = NULL;
    for (size_t i = 0; i < chunk_size; i++) {
        char* entry_ptr = chunk + (i * entry_size);
        FastStringEntry* entry = (FastStringEntry*)entry_ptr;
        
        entry->capacity = string_capacity;
        entry->next = prev;
        
        // Initialize the string metadata
        uint64_t* length_ptr = (uint64_t*)(entry_ptr + sizeof(FastStringEntry));
        *length_ptr = 0; // Will be set when allocated
        
        prev = entry;
    }
    
    // Add entire chunk to the pool's free list
    pool->free_head = prev;
    pool->total_allocated += chunk_size;
    
    // Adaptive scaling under pressure (similar to freelist design)
    if (pool->current_chunk_size < FAST_STRING_MAX_CHUNK_SIZE) {
        pool->current_chunk_size *= FAST_STRING_SCALING_FACTOR;
        if (pool->current_chunk_size > FAST_STRING_MAX_CHUNK_SIZE) {
            pool->current_chunk_size = FAST_STRING_MAX_CHUNK_SIZE;
        }
        pool->scaling_events++;
        printf("FAST_STRING: Scaled up size class %zu chunk size to %zu (pressure detected)\n", 
               size_class_index, pool->current_chunk_size);
    }
}

// --- Public API Implementation ---

bool fast_string_allocator_init(void) {
    pthread_mutex_lock(&g_allocator_mutex);
    
    if (g_fast_string_allocator.initialized) {
        pthread_mutex_unlock(&g_allocator_mutex);
        return true;
    }
    
    // Initialize all size class pools
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        FastStringPool* pool = &g_fast_string_allocator.size_pools[i];
        pool->free_head = NULL;
        pool->class_size = FAST_STRING_SIZE_CLASSES_ARRAY[i];
        pool->current_chunk_size = FAST_STRING_INITIAL_CHUNK_SIZE;
        pool->total_allocated = 0;
        pool->total_requests = 0;
        pool->reuse_count = 0;
        pool->scaling_events = 0;
    }
    
    // Initialize global statistics
    g_fast_string_allocator.total_strings_allocated = 0;
    g_fast_string_allocator.total_strings_freed = 0;
    g_fast_string_allocator.total_bytes_allocated = 0;
    g_fast_string_allocator.pool_hits = 0;
    g_fast_string_allocator.heap_fallbacks = 0;
    g_fast_string_allocator.mutex = &g_allocator_mutex;
    g_fast_string_allocator.initialized = true;
    
    pthread_mutex_unlock(&g_allocator_mutex);
    
    printf("FastStringAllocator: Initialized with %d size classes\n", FAST_STRING_SIZE_CLASSES);
    printf("Size classes: ");
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        printf("%zu%s", FAST_STRING_SIZE_CLASSES_ARRAY[i], 
               (i < FAST_STRING_SIZE_CLASSES - 1) ? ", " : " chars\n");
    }
    
    return true;
}

void fast_string_allocator_shutdown(void) {
    pthread_mutex_lock(&g_allocator_mutex);
    
    if (!g_fast_string_allocator.initialized) {
        pthread_mutex_unlock(&g_allocator_mutex);
        return;
    }
    
    // Print final statistics before shutdown
    printf("FastStringAllocator: Shutting down...\n");
    fast_string_print_metrics();
    
    // Note: We don't free the pool chunks here as they may contain active strings
    // In a production system, you'd want proper reference counting or cleanup tracking
    
    g_fast_string_allocator.initialized = false;
    
    pthread_mutex_unlock(&g_allocator_mutex);
}

void* fast_bcpl_alloc_chars(int64_t num_chars) {
    if (num_chars < 0) {
        return NULL;
    }
    
    // Auto-initialize on first use
    if (!g_fast_string_allocator.initialized) {
        if (!fast_string_allocator_init()) {
            return NULL;
        }
    }
    
    size_t char_count = (size_t)num_chars;
    size_t size_class_index = get_size_class_index(char_count);
    
    pthread_mutex_lock(&g_allocator_mutex);
    
    g_fast_string_allocator.total_strings_allocated++;
    
    // Handle oversized strings with direct heap allocation
    if (size_class_index >= FAST_STRING_SIZE_CLASSES) {
        g_fast_string_allocator.heap_fallbacks++;
        pthread_mutex_unlock(&g_allocator_mutex);
        
        // Fallback to direct allocation for very large strings
        size_t total_size = sizeof(uint64_t) + (char_count + 1) * sizeof(uint32_t);
        uint64_t* ptr = (uint64_t*)malloc(total_size);
        if (!ptr) return NULL;
        
        ptr[0] = char_count; // Store length
        uint32_t* payload = (uint32_t*)(ptr + 1);
        payload[char_count] = 0; // Null terminator
        
        return payload;
    }
    
    // Use size class pool
    FastStringPool* pool = &g_fast_string_allocator.size_pools[size_class_index];
    pool->total_requests++;
    
    // Replenish pool if empty
    if (pool->free_head == NULL) {
        replenish_size_class_pool(pool, size_class_index);
    }
    
    if (pool->free_head == NULL) {
        // Replenishment failed
        pthread_mutex_unlock(&g_allocator_mutex);
        return NULL;
    }
    
    // Get entry from pool
    FastStringEntry* entry = pool->free_head;
    pool->free_head = entry->next;
    pool->reuse_count++;
    g_fast_string_allocator.pool_hits++;
    
    pthread_mutex_unlock(&g_allocator_mutex);
    
    // Initialize string for use
    uint32_t* string_data = get_string_data_from_entry(entry);
    uint64_t* length_ptr = ((uint64_t*)string_data) - 1;
    *length_ptr = char_count; // Store actual requested length
    string_data[char_count] = 0; // Null terminator
    
    return string_data;
}

void fast_bcpl_free_chars(void* string_payload) {
    if (!string_payload || !g_fast_string_allocator.initialized) {
        return;
    }
    
    uint32_t* string_data = (uint32_t*)string_payload;
    uint64_t* length_ptr = ((uint64_t*)string_data) - 1;
    size_t string_length = *length_ptr;
    
    pthread_mutex_lock(&g_allocator_mutex);
    
    // Check if this was a heap fallback allocation
    size_t size_class_index = get_size_class_index(string_length);
    if (size_class_index >= FAST_STRING_SIZE_CLASSES) {
        // Direct heap allocation - free directly
        free(length_ptr);
        g_fast_string_allocator.total_strings_freed++;
        pthread_mutex_unlock(&g_allocator_mutex);
        return;
    }
    
    // Return to appropriate size class pool
    FastStringEntry* entry = get_entry_from_string_data(string_data);
    FastStringPool* pool = &g_fast_string_allocator.size_pools[size_class_index];
    
    entry->next = pool->free_head;
    pool->free_head = entry;
    
    g_fast_string_allocator.total_strings_freed++;
    
    pthread_mutex_unlock(&g_allocator_mutex);
}

FastStringStats fast_string_get_stats(void) {
    FastStringStats stats = {0};
    
    if (!g_fast_string_allocator.initialized) {
        return stats;
    }
    
    pthread_mutex_lock(&g_allocator_mutex);
    
    stats.total_allocated = g_fast_string_allocator.total_strings_allocated;
    stats.total_freed = g_fast_string_allocator.total_strings_freed;
    stats.bytes_allocated = g_fast_string_allocator.total_bytes_allocated;
    stats.pool_hits = g_fast_string_allocator.pool_hits;
    stats.heap_fallbacks = g_fast_string_allocator.heap_fallbacks;
    
    if (stats.total_allocated > 0) {
        stats.pool_hit_rate = (double)stats.pool_hits / stats.total_allocated * 100.0;
    }
    
    // Collect per-size-class statistics
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        FastStringPool* pool = &g_fast_string_allocator.size_pools[i];
        stats.size_class_requests[i] = pool->total_requests;
        if (pool->total_requests > 0) {
            stats.size_class_reuse_rates[i] = pool->reuse_count * 100 / pool->total_requests;
        }
    }
    
    pthread_mutex_unlock(&g_allocator_mutex);
    
    return stats;
}

void fast_string_print_metrics(void) {
    FastStringStats stats = fast_string_get_stats();
    
    printf("\n=== Fast String Allocator Metrics ===\n");
    printf("Total strings allocated: %zu\n", stats.total_allocated);
    printf("Total strings freed: %zu\n", stats.total_freed);
    printf("Pool hit rate: %.2f%%\n", stats.pool_hit_rate);
    printf("Heap fallbacks (oversized): %zu\n", stats.heap_fallbacks);
    
    printf("\nSize class performance:\n");
    pthread_mutex_lock(&g_allocator_mutex);
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        FastStringPool* pool = &g_fast_string_allocator.size_pools[i];
        if (pool->total_requests > 0) {
            double reuse_rate = (double)pool->reuse_count / pool->total_requests * 100.0;
            printf("  Class %zu (%3zu chars): %6zu requests, %6zu reused (%5.1f%%), %3zu scalings, chunk: %zu\n",
                   i, pool->class_size, pool->total_requests, pool->reuse_count, 
                   reuse_rate, pool->scaling_events, pool->current_chunk_size);
        }
    }
    pthread_mutex_unlock(&g_allocator_mutex);
    
    printf("==========================================\n");
}

void fast_string_replenish_size_class(size_t size_class_index) {
    if (size_class_index >= FAST_STRING_SIZE_CLASSES || !g_fast_string_allocator.initialized) {
        return;
    }
    
    pthread_mutex_lock(&g_allocator_mutex);
    FastStringPool* pool = &g_fast_string_allocator.size_pools[size_class_index];
    replenish_size_class_pool(pool, size_class_index);
    pthread_mutex_unlock(&g_allocator_mutex);
}

size_t fast_string_get_size_class(size_t num_chars) {
    return get_size_class_index(num_chars);
}

size_t fast_string_get_class_capacity(size_t size_class_index) {
    if (size_class_index >= FAST_STRING_SIZE_CLASSES) {
        return 0;
    }
    return FAST_STRING_SIZE_CLASSES_ARRAY[size_class_index];
}

bool fast_string_allocator_is_healthy(void) {
    if (!g_fast_string_allocator.initialized) {
        return false;
    }
    
    pthread_mutex_lock(&g_allocator_mutex);
    
    // Check that all pools are in reasonable state
    bool healthy = true;
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        FastStringPool* pool = &g_fast_string_allocator.size_pools[i];
        if (pool->class_size != FAST_STRING_SIZE_CLASSES_ARRAY[i]) {
            healthy = false;
            break;
        }
    }
    
    pthread_mutex_unlock(&g_allocator_mutex);
    
    return healthy;
}

void fast_string_reset_stats(void) {
    if (!g_fast_string_allocator.initialized) {
        return;
    }
    
    pthread_mutex_lock(&g_allocator_mutex);
    
    g_fast_string_allocator.total_strings_allocated = 0;
    g_fast_string_allocator.total_strings_freed = 0;
    g_fast_string_allocator.total_bytes_allocated = 0;
    g_fast_string_allocator.pool_hits = 0;
    g_fast_string_allocator.heap_fallbacks = 0;
    
    for (size_t i = 0; i < FAST_STRING_SIZE_CLASSES; i++) {
        FastStringPool* pool = &g_fast_string_allocator.size_pools[i];
        pool->total_requests = 0;
        pool->reuse_count = 0;
        pool->scaling_events = 0;
    }
    
    pthread_mutex_unlock(&g_allocator_mutex);
    
    printf("FastStringAllocator: Statistics reset\n");
}

// SAMM integration functions (stubs for now - would integrate with actual SAMM)
void fast_string_samm_track(void* string_payload) {
    // In a full implementation, this would integrate with the SAMM system
    // to track the allocation in the current scope
    (void)string_payload; // Suppress unused parameter warning
}

void fast_string_samm_cleanup_scope(void** scope_strings, size_t count) {
    // In a full implementation, this would be called by SAMM during scope exit
    // to return all strings in the scope to their respective pools
    for (size_t i = 0; i < count; i++) {
        if (scope_strings[i]) {
            fast_bcpl_free_chars(scope_strings[i]);
        }
    }
}
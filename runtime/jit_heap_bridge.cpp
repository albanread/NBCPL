#include "runtime.h"
#include "ListDataTypes.h"
#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../HeapManager/HeapManager.h" // Make sure this is included
#include "../HeapManager/heap_manager_defs.h" // Include for HeapBlock definition
#include "runtime_freelist.h"
#include "../include/compiler_interface.h"
#include "string_optimizations_embedded.cpp"

// Import the global heap tracking structures from the main program
extern "C" {
    extern HeapBlock g_heap_blocks_array[MAX_HEAP_BLOCKS];
    extern size_t g_heap_blocks_index;
}

#ifdef __cplusplus
extern "C" {
#endif

// Bridge for allocating 64-bit word vectors
void* bcpl_alloc_words(int64_t num_words, const char* func, const char* var) {
    // Note: Ignoring func and var in JIT mode as HeapManager handles tracking
    void* result = HeapManager::getInstance().allocVec(num_words);

    // Debug print to verify heap tracking in JIT context
    printf("[JIT Heap] Allocated vector at %p, current g_heap_blocks_index: %zu\n", 
           result, g_heap_blocks_index);

    return result;
}

// Bridge for allocating 32-bit character strings - OPTIMIZED VERSION
void* bcpl_alloc_chars(int64_t num_chars) {
    // Try optimized string pool allocator first
    void* result = embedded_fast_bcpl_alloc_chars(num_chars);
    if (result) {
        return result;
    }
    
    // Fallback to HeapManager if pool allocation fails
    return HeapManager::getInstance().allocString(num_chars);
}

// Bridge for freeing any allocated memory
void bcpl_free(void* ptr) {
    printf("DEBUG: JIT bcpl_free called with ptr=%p\n", ptr);
    fflush(stdout);
    
    if (ptr == nullptr) {
        printf("DEBUG: JIT bcpl_free - ptr is null, returning\n");
        fflush(stdout);
        return;
    }
    
    printf("DEBUG: JIT bcpl_free - calling HeapManager::getInstance().free()\n");
    fflush(stdout);
    
    HeapManager::getInstance().free(ptr);
    
    printf("DEBUG: JIT bcpl_free completed for ptr=%p\n", ptr);
    fflush(stdout);
}

// Wrapper for GETVEC that takes only 1 parameter (traditional BCPL interface)
void* bcpl_getvec(int64_t num_words) {
    return bcpl_alloc_words(num_words, "GETVEC", "vector");
}

// Note: Freelist interface implementations moved to runtime_freelist.c
// to avoid duplicate symbol definitions

#ifdef __cplusplus
}
#endif

// Remove ListHeader alias; use explicit struct from ListDataTypes.h

// All freelist logic is now handled by runtime_freelist.c
// Use the API from runtime_freelist.h

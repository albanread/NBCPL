// heap_interface.c
// Pure C heap allocation interface for standalone BCPL runtime

#include <stdlib.h>
#include <stdint.h>
#include "BCPLError.h"

// Forward declaration for error tracking
extern void _BCPL_SET_ERROR(BCPLErrorCode code, const char* func, const char* msg);

/**
 * Allocate a vector of 64-bit words for BCPL.
 * The returned pointer points to the payload (not the length prefix).
 */
void* BCPL_ALLOC_WORDS(int64_t num_words) {
    if (num_words <= 0) {
        _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BCPL_ALLOC_WORDS", "Attempted to allocate negative or zero words");
        return NULL;
    }
    size_t total_size = sizeof(uint64_t) + num_words * sizeof(uint64_t);
    uint64_t* ptr = (uint64_t*)malloc(total_size);
    if (!ptr) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "BCPL_ALLOC_WORDS", "System malloc failed for word vector");
        return NULL;
    }
    ptr[0] = num_words;
    return (void*)(ptr + 1);
}

/**
 * Allocate a vector of 32-bit characters for BCPL strings.
 * The returned pointer points to the payload (not the length prefix).
 * The string is null-terminated.
 */
void* BCPL_ALLOC_CHARS(int64_t num_chars) {
    if (num_chars < 0) {
        _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BCPL_ALLOC_CHARS", "Attempted to allocate negative number of chars");
        return NULL;
    }
    size_t total_size = sizeof(uint64_t) + (num_chars + 1) * sizeof(uint32_t);
    uint64_t* ptr = (uint64_t*)malloc(total_size);
    if (!ptr) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "BCPL_ALLOC_CHARS", "System malloc failed for char buffer");
        return NULL;
    }
    ptr[0] = num_chars;
    uint32_t* payload = (uint32_t*)(ptr + 1);
    payload[num_chars] = 0;
    return (void*)payload;
}

/**
 * Free a BCPL-allocated vector or string.
 * The pointer must be the payload pointer returned by BCPL_ALLOC_WORDS/CHARS.
 */
void BCPL_FREE(void* payload) {
    if (!payload) {
        _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BCPL_FREE", "Attempted to free NULL pointer");
        return;
    }
    uint64_t* original_ptr = ((uint64_t*)payload) - 1;
    free(original_ptr);
}
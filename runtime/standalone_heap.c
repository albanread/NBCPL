/*
 * standalone_heap.c
 * Pure C heap implementation for BCPL runtime (no C++ dependencies).
 * This file should NOT include any .inc files. Only heap logic here.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "runtime.h"
#include "ListDataTypes.h"
#include "BCPLError.h"
extern void _BCPL_SET_ERROR(BCPLErrorCode, const char*, const char*);
#include "runtime_freelist.h"

// Allocates a vector of 64-bit words (BCPL vector).
void* BCPL_ALLOC_WORDS(int64_t num_words, const char* func, const char* var) {
    if (num_words <= 0) {
        _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BCPL_ALLOC_WORDS", "Attempted to allocate negative or zero words");
        return NULL;
    }
    size_t total_size = sizeof(uint64_t) + (num_words * sizeof(uint64_t));
    uint64_t* block = (uint64_t*)malloc(total_size);
    if (!block) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "BCPL_ALLOC_WORDS", "System malloc failed for word vector");
        return NULL;
    }
    block[0] = num_words;
    return (void*)(block + 1);
}

// Allocates a character string buffer (BCPL string).
void* BCPL_ALLOC_CHARS(int64_t num_chars) {
    if (num_chars < 0) {
        _BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "BCPL_ALLOC_CHARS", "Attempted to allocate negative number of chars");
        return NULL;
    }
    size_t total_size = sizeof(uint64_t) + ((num_chars + 1) * sizeof(uint32_t)); // +1 for null terminator
    uint64_t* block = (uint64_t*)malloc(total_size);
    if (!block) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "BCPL_ALLOC_CHARS", "System malloc failed for char buffer");
        return NULL;
    }
    block[0] = num_chars;
    uint32_t* payload = (uint32_t*)(block + 1);
    payload[num_chars] = 0; // Null terminate
    return (void*)payload;
}

// Frees memory allocated by BCPL_ALLOC_WORDS or BCPL_ALLOC_CHARS.
void FREEVEC(void* ptr) {
    if (!ptr) return;
    uint64_t* block_start = ((uint64_t*)ptr) - 1;
    free(block_start);
}

// List allocation/freelist logic for standalone mode
// Freelist logic is now handled by runtime_freelist.c

// All freelist logic is now handled by runtime_freelist.c
// Use the API from runtime_freelist.h
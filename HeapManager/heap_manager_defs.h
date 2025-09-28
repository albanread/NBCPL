#ifndef HEAP_MANAGER_DEFS_H
#define HEAP_MANAGER_DEFS_H

#include <stddef.h> // For size_t
#include <stdint.h> // For uint64_t, uint32_t
#include <stdbool.h> // For bool in C

// Allocation types for the heap manager
typedef enum {
    ALLOC_UNKNOWN = 0, // Default/uninitialized
    ALLOC_VEC,         // Vector allocation (64-bit words)
    ALLOC_STRING,      // String allocation (32-bit chars)
    ALLOC_OBJECT,      // Object allocation (vtable + members, no prefix)
    ALLOC_GENERIC,     // Generic allocation
    ALLOC_FREE,        // Marker for freed blocks
    ALLOC_LIST         // List allocation (BCPL list header/node)
} AllocType;

// Structure to track allocated heap blocks
// All members are basic types for signal safety
// type: ALLOC_VEC for vectors, ALLOC_OBJECT for objects, etc.
typedef struct {
    AllocType type;            // Type of allocation
    void* address;             // 64-bit pointer to the allocation
    size_t size;               // Size in bytes (64-bit length)
    const char* function_name; // Name of the function where allocation occurred
    const char* variable_name; // Name of the variable being allocated
} HeapBlock;

// Constants for heap management
#define MAX_HEAP_BLOCKS 128    // Maximum number of tracked heap blocks


// External declarations for globals
#ifdef __cplusplus
extern "C" {
#endif

// Signal-safe shadow heap tracking for crash diagnostics
extern volatile HeapBlock g_shadow_heap_blocks[MAX_HEAP_BLOCKS];
extern volatile size_t g_shadow_heap_index;
extern volatile bool g_is_heap_tracing_enabled;

// Functions to update metrics (for internal use)
void update_alloc_metrics(size_t bytes, AllocType type);
void update_free_metrics(size_t bytes);

// Double-free tracking function
void update_double_free_metrics(void);

// File I/O metrics functions
void update_io_metrics_read(size_t bytes);
void update_io_metrics_write(size_t bytes);
void update_io_metrics_file_opened(void);
void update_io_metrics_file_closed(void);

// Metrics tracking function
void print_runtime_metrics(void);

#ifdef __cplusplus
}
#endif

#endif // HEAP_MANAGER_DEFS_H
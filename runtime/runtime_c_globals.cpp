/*
 * runtime_c_globals.cpp
 * Provides C linkage definitions for globals and error function required by the JIT/runtime.
 * This file resolves linker errors for missing symbols referenced from C and C++ code.
 */

#include "../HeapManager/heap_manager_defs.h"
#include "BCPLError.h"
#include "ListDataTypes.h"

#ifdef __cplusplus
extern "C" {
#endif

// --- Heap tracking globals ---
HeapBlock g_heap_blocks_array[MAX_HEAP_BLOCKS] = {};
size_t g_heap_blocks_index = 0;

// --- Error logging globals ---
volatile BCPLErrorInfo g_recent_errors[MAX_RECENT_ERRORS];
volatile size_t g_recent_error_index = 0;

// --- Freelist functions (forward to canonical implementation) ---
ListHeader* getHeaderFromFreelist(void);
void returnHeaderToFreelist(ListHeader* header);

// --- Error reporting function (forward to canonical implementation) ---
void _BCPL_SET_ERROR(BCPLErrorCode code, const char* func, const char* msg);

#ifdef __cplusplus
}
#endif

/*
 * NOTE:
 * - The actual implementations for getHeaderFromFreelist, returnHeaderToFreelist, and _BCPL_SET_ERROR
 *   are provided in runtime_freelist.c and BCPLError.c, respectively.
 * - This file ensures that the linker finds the correct C symbols for runtime and JIT builds.
 */

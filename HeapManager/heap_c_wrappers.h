#ifndef HEAP_C_WRAPPERS_H
#define HEAP_C_WRAPPERS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* Heap_allocVec(size_t numElements);
void* Heap_allocObject(size_t size);
void Heap_free(void* payload);

// SAMM: Scope Aware Memory Management C API
void HeapManager_setSAMMEnabled(int enabled);
int HeapManager_isSAMMEnabled(void);
void HeapManager_enterScope(void);
void HeapManager_exitScope(void);
void HeapManager_retainPointer(void* ptr, int parent_scope_offset);
void HeapManager_trackFreelistAllocation(void* ptr);
void HeapManager_handleMemoryPressure(void);
void HeapManager_waitForSAMM(void);
void HeapManager_shutdown(void);

// SAMM: RETAIN allocation variants
void* HeapManager_allocObjectRetained(size_t size, int parent_scope_offset);
void* HeapManager_allocVecRetained(size_t numElements, int parent_scope_offset);
void* HeapManager_allocStringRetained(size_t numChars, int parent_scope_offset);
void* HeapManager_allocListRetained(int parent_scope_offset);

#ifdef __cplusplus
}
#endif

#endif // HEAP_C_WRAPPERS_H
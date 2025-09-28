// NewBCPL/HeapManager/heap_c_wrappers.cpp
// ============================================================================
// MAINTAINER NOTE:
// If you add new HeapManager methods that need to be called from C or assembly
// (the runtime), you MUST add a corresponding extern "C" wrapper function here.
// Otherwise, you will get linker errors!
//
// See also: runtime/README.md for more information on runtime ABI requirements.
// ============================================================================

// C linkage wrappers for Heap_allocVec, Heap_allocObject, Heap_free, and BCPL_SET_ERROR

#include "HeapManager.h"
#include "../include/compiler_interface.h"

extern "C" void* Heap_allocVec(size_t numElements) {
    return HeapManager::getInstance().allocVec(numElements);
}

extern "C" void* Heap_allocObject(size_t size) {
    return HeapManager::getInstance().allocObject(size);
}

extern "C" void Heap_free(void* payload) {
    HeapManager::getInstance().free(payload);
}

// Forward declaration of the proper error tracking function
extern "C" void _BCPL_SET_ERROR(int code, const char* func, const char* msg);

// SAMM: Scope Aware Memory Management C wrapper functions
extern "C" void HeapManager_setSAMMEnabled(int enabled) {
    HeapManager::getInstance().setSAMMEnabled(enabled != 0);
}

extern "C" int HeapManager_isSAMMEnabled(void) {
    return HeapManager::getInstance().isSAMMEnabled() ? 1 : 0;
}

extern "C" void HeapManager_enterScope(void) {
    HeapManager::getInstance().enterScope();
}

// Alias for snake_case expected by BCPL/assembly
extern "C" void HeapManager_enter_scope(void) {
    HeapManager_enterScope();
}

extern "C" void HeapManager_exitScope(void) {
    HeapManager::getInstance().exitScope();
}

// Alias for snake_case expected by BCPL/assembly
extern "C" void HeapManager_exit_scope(void) {
    HeapManager_exitScope();
}

extern "C" void HeapManager_retainPointer(void* ptr, int parent_scope_offset) {
    HeapManager::getInstance().retainPointer(ptr, parent_scope_offset);
}

extern "C" void HeapManager_trackFreelistAllocation(void* ptr) {
    HeapManager::getInstance().trackFreelistAllocation(ptr);
}

extern "C" void HeapManager_handleMemoryPressure(void) {
    HeapManager::getInstance().handleMemoryPressure();
}

extern "C" void HeapManager_waitForSAMM(void) {
    HeapManager::getInstance().waitForSAMM();
}

extern "C" void HeapManager_shutdown(void) {
    HeapManager::getInstance().shutdown();
}

// SAMM: RETAIN allocation variants
extern "C" void* HeapManager_allocObjectRetained(size_t size, int parent_scope_offset) {
    return HeapManager::getInstance().allocObjectRetained(size, parent_scope_offset);
}

extern "C" void* HeapManager_allocVecRetained(size_t numElements, int parent_scope_offset) {
    return HeapManager::getInstance().allocVecRetained(numElements, parent_scope_offset);
}

extern "C" void* HeapManager_allocStringRetained(size_t numChars, int parent_scope_offset) {
    return HeapManager::getInstance().allocStringRetained(numChars, parent_scope_offset);
}

extern "C" void* HeapManager_allocListRetained(int parent_scope_offset) {
    return HeapManager::getInstance().allocListRetained(parent_scope_offset);
}

// Manual SAMM tracking for custom allocators
extern "C" void HeapManager_trackInCurrentScope(void* ptr) {
    HeapManager::getInstance().trackInCurrentScope(ptr);
}

// String pool SAMM tracking
extern "C" void HeapManager_trackStringPoolAllocation(void* ptr) {
    HeapManager::getInstance().trackStringPoolAllocation(ptr);
}

// C linkage wrapper for BCPL_SET_ERROR - now properly delegates to _BCPL_SET_ERROR
extern "C" void BCPL_SET_ERROR(int code, const char* message, const char* context) {
    // Delegate to the proper error tracking system that logs to the crash report buffer
    _BCPL_SET_ERROR(code, context ? context : "unknown", message ? message : "unknown error");
    
    // Also print to stderr for immediate visibility during debugging
    fprintf(stderr, "BCPL ERROR [%d]: %s (context: %s)\n", code, message ? message : "unknown error", context ? context : "(none)");
}

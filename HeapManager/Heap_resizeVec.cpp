#include "HeapManager.h"
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print, int_to_dec
#include <cstdlib>
#include "runtime/BCPLError.h"
#include "include/compiler_interface.h"
#ifdef __cplusplus
extern "C" {
#endif
void BCPL_SET_ERROR(int code, const char* message, const char* context);
#ifdef __cplusplus
}
#endif
#include <cstdint>
#include <mutex>

// Resize a vector
void* resizeVec(void* payload, size_t newNumElements) {
    if (!payload) {
        BCPL_SET_ERROR(ERROR_INVALID_ARGUMENT, "resizeVec", "Cannot resize a NULL vector");
        safe_print("Error: Cannot resize a NULL vector\n");
        return nullptr;
    }

    // Calculate base address for Vec allocations (payload points after metadata)
    void* base_address = static_cast<uint8_t*>(payload) - sizeof(uint64_t);

    HeapManager& mgr = HeapManager::getInstance();
    std::lock_guard<std::mutex> lock(mgr.heap_mutex_);

    auto it = mgr.heap_blocks_.find(base_address);
    if (it != mgr.heap_blocks_.end() && it->second.type == ALLOC_VEC) {
        // Calculate new size
        size_t newTotalSize = sizeof(uint64_t) + newNumElements * sizeof(uint64_t);

        // Resize the memory block
        void* newPtr = realloc(it->second.address, newTotalSize);
        if (!newPtr) {
            BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "resizeVec", "realloc failed for vector resize");
            safe_print("Error: Vector resize failed\n");
            return nullptr;
        }

        // Update metadata
        it->second.address = newPtr;
        it->second.size = newTotalSize;

        // Update the length field in the vector
        uint64_t* vec = static_cast<uint64_t*>(newPtr);
        vec[0] = newNumElements;

        return static_cast<void*>(vec + 1); // Return pointer to the payload
    }

    BCPL_SET_ERROR(ERROR_INVALID_POINTER, "resizeVec", "Vector not found in heap tracking");
    safe_print("Error: Vector not found in heap tracking\n");
    return nullptr;
}

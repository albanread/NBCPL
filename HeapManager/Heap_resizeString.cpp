#include <cstdlib>
#include <stdexcept>
#include <algorithm> // For std::min
#include "HeapManager.h" // Include HeapManager class definition
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print, u64_to_hex, int_to_dec

// Function is defined in SignalSafeUtils.h
extern void safe_print(const char*); // Declaration of safe_print utility

void* resizeString(void* payload, size_t newNumChars) {
    if (!payload) {
        safe_print("Error: Cannot resize a NULL string\n");
        return nullptr;
    }

    // Calculate base address for String allocations (payload points after metadata)
    void* base_address = static_cast<uint8_t*>(payload) - sizeof(uint64_t);

    HeapManager& mgr = HeapManager::getInstance();
    std::lock_guard<std::mutex> lock(mgr.heap_mutex_);

    auto it = mgr.heap_blocks_.find(base_address);
    if (it != mgr.heap_blocks_.end() && it->second.type == ALLOC_STRING) {
        // Calculate new size
        size_t newTotalSize = sizeof(uint64_t) + (newNumChars + 1) * sizeof(uint32_t);

        // Resize the memory block
        void* newPtr = realloc(it->second.address, newTotalSize);
        if (!newPtr) {
            safe_print("Error: String resize failed\n");
            return nullptr;
        }

        // Update metadata
        it->second.address = newPtr;
        it->second.size = newTotalSize;

        // Update the length field in the string
        uint64_t* str = static_cast<uint64_t*>(newPtr);
        str[0] = newNumChars;

        // Ensure null terminator
        uint32_t* payload_ptr = reinterpret_cast<uint32_t*>(str + 1);
        payload_ptr[newNumChars] = 0;

        return static_cast<void*>(payload_ptr); // Return pointer to the payload
    }

    safe_print("Error: String not found in heap tracking\n");
    return nullptr;
}

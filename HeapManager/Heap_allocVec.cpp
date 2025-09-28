#include "HeapManager.h"
#include "heap_manager_defs.h"
#include "../SignalSafeUtils.h" // For safe_print, u64_to_hex, int_to_dec
#include <cstdlib>
#if __has_include("runtime/BCPLError.h")
#include "runtime/BCPLError.h"
#else
#include "../runtime/BCPLError.h"
#endif
#include "include/compiler_interface.h"
#include <cstdint>
#include <cstddef> // For ptrdiff_t
#include <algorithm> // For std::min
#include <mutex>

void* HeapManager::allocVec(size_t numElements) {
    size_t totalSize = sizeof(uint64_t) + numElements * sizeof(uint64_t);
    void* ptr;
    const size_t HEAP_ALIGNMENT = 16;
    if (posix_memalign(&ptr, HEAP_ALIGNMENT, totalSize) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocVec", "System posix_memalign failed");
        safe_print("Error: Vector allocation failed\n");
        return nullptr;
    }

    // Initialize vector metadata
    uint64_t* vec = static_cast<uint64_t*>(ptr);
    vec[0] = numElements; // Store length

    // Track allocation using thread-safe unordered_map
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_VEC, ptr, totalSize, nullptr, nullptr});
        // Conditionally update the signal-safe shadow array if tracing is enabled
        if (traceEnabled) {
            g_shadow_heap_blocks[g_shadow_heap_index].type = ALLOC_VEC;
            g_shadow_heap_blocks[g_shadow_heap_index].address = ptr;
            g_shadow_heap_blocks[g_shadow_heap_index].size = totalSize;
            g_shadow_heap_blocks[g_shadow_heap_index].function_name = nullptr;
            g_shadow_heap_blocks[g_shadow_heap_index].variable_name = nullptr;
            g_shadow_heap_index = (g_shadow_heap_index + 1) % MAX_HEAP_BLOCKS;
        }
    }

    // SAMM: Track allocation in current scope if enabled
    if (samm_enabled_.load() && ptr != nullptr) {
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (!scope_allocations_.empty()) {
            scope_allocations_.back().push_back(ptr);
            if (traceEnabled) {
                printf("SAMM: Tracked vector allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                       ptr, scope_allocations_.size(), scope_allocations_.back().size());
            }
        } else {
            if (traceEnabled) {
                printf("SAMM: ERROR - No scopes available to track vector allocation %p\n", ptr);
            }
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: Not tracking vector allocation %p (enabled: %s, ptr valid: %s)\n", 
                   ptr, samm_enabled_.load() ? "YES" : "NO", (ptr != nullptr) ? "YES" : "NO");
        }
    }

    // Trace log
    traceLog("Allocated vector: Address=%p, Size=%zu, Elements=%zu\n", ptr, totalSize, numElements);

    // Update internal metrics
    totalBytesAllocated += totalSize;
    totalVectorsAllocated++;
    
    // Update global metrics
    update_alloc_metrics(totalSize, ALLOC_VEC);

    return static_cast<void*>(vec + 1); // Return pointer to the payload
}

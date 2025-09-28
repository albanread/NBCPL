#include "HeapManager.h"
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print
#include <cstdlib>
#if __has_include("runtime/BCPLError.h")
#include "runtime/BCPLError.h"
#else
#include "../runtime/BCPLError.h"
#endif
#include "include/compiler_interface.h"
#ifdef __cplusplus
extern "C" {
#endif
void BCPL_SET_ERROR(int code, const char* message, const char* context);
#ifdef __cplusplus
}
#endif
#include <cstring>
#include <mutex>

void* HeapManager::allocString(size_t numChars) {
    size_t totalSize = sizeof(uint64_t) + (numChars + 1) * sizeof(uint32_t);
    void* ptr;
    const size_t HEAP_ALIGNMENT = 16;
    if (posix_memalign(&ptr, HEAP_ALIGNMENT, totalSize) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocString", "System posix_memalign failed");
        safe_print("Error: String allocation failed\n");
        return nullptr;
    }

    // Initialize string metadata
    uint64_t* str = static_cast<uint64_t*>(ptr);
    str[0] = numChars; // Store length
    uint32_t* payload = reinterpret_cast<uint32_t*>(str + 1);
    payload[numChars] = 0; // Null terminator

    // Track allocation using unordered_map and mutex
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_STRING, ptr, totalSize, nullptr, nullptr});
        // Conditionally update the signal-safe shadow array if tracing is enabled
        if (traceEnabled) {
            g_shadow_heap_blocks[g_shadow_heap_index].type = ALLOC_STRING;
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
                printf("SAMM: Tracked string allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                       ptr, scope_allocations_.size(), scope_allocations_.back().size());
            }
        } else {
            if (traceEnabled) {
                printf("SAMM: ERROR - No scopes available to track string allocation %p\n", ptr);
            }
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: Not tracking string allocation %p (enabled: %s, ptr valid: %s)\n", 
                   ptr, samm_enabled_.load() ? "YES" : "NO", (ptr != nullptr) ? "YES" : "NO");
        }
    }

    // Trace log
    traceLog("Allocated string: Address=%p, Size=%zu, Characters=%zu\n", ptr, totalSize, numChars);

    // Update internal metrics
    totalBytesAllocated += totalSize;
    totalStringsAllocated++;
    
    // Update global metrics
    update_alloc_metrics(totalSize, ALLOC_STRING);

    return static_cast<void*>(payload); // Return pointer to the payload
}

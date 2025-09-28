#include "HeapManager.h"
#include "heap_manager_defs.h"
#include "../SignalSafeUtils.h"
#include "../runtime/ListDataTypes.h"
#include <cstdlib> // For posix_memalign
#include <cstring> // For memset
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

void* HeapManager::allocObject(size_t size) {
    const size_t HEAP_ALIGNMENT = 16;
    void* ptr;
    if (posix_memalign(&ptr, HEAP_ALIGNMENT, size) != 0) {
        BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "allocObject", "System posix_memalign failed");
        safe_print("Error: Object allocation failed\n");
        return nullptr;
    }
    memset(ptr, 0, size);
    {
        // Lock the mutex for thread safety
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(ptr, HeapBlock{ALLOC_OBJECT, ptr, size, nullptr, nullptr});
        // Conditionally update the signal-safe shadow array if tracing is enabled
        if (traceEnabled) {
            g_shadow_heap_blocks[g_shadow_heap_index].type = ALLOC_OBJECT;
            g_shadow_heap_blocks[g_shadow_heap_index].address = ptr;
            g_shadow_heap_blocks[g_shadow_heap_index].size = size;
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
                printf("SAMM: Tracked allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                       ptr, scope_allocations_.size(), scope_allocations_.back().size());
            }
        } else {
            if (traceEnabled) {
                printf("SAMM: ERROR - No scopes available to track allocation %p\n", ptr);
            }
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: Not tracking allocation %p (enabled: %s, ptr valid: %s)\n", 
                   ptr, samm_enabled_.load() ? "YES" : "NO", (ptr != nullptr) ? "YES" : "NO");
        }
    }

    // Debug output for allocated object
    if (HeapManager::getInstance().isTracingEnabled()) {
        safe_print("\n=== OBJECT ALLOCATED ===\n");
        safe_print("Address: 0x");
        char addr_buf[20];
        u64_to_hex((uint64_t)(uintptr_t)ptr, addr_buf);
        safe_print(addr_buf);
        safe_print(", Size: ");
        char size_buf[20];
        int_to_dec((int)size, size_buf);
        safe_print(size_buf);
        safe_print("\n=== END OBJECT ALLOCATION ===\n");
    }

    traceLog("Allocated object: Address=%p, Size=%zu\n", ptr, size);
    totalBytesAllocated += size;
    totalObjectsAllocated++;
    
    // Check if this is a ListAtom allocation
    if (size == sizeof(ListAtom)) {
        totalListAtomsAllocated++;
        if (traceEnabled) {
            printf("DEBUG: ListAtom allocated at %p (total atoms: %zu)\n", ptr, totalListAtomsAllocated);
        }
    }
    
    update_alloc_metrics(size, ALLOC_OBJECT);
    return ptr;
}
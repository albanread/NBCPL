#include "HeapManager.h"
#include "heap_manager_defs.h"
#include "../runtime/ListDataTypes.h"
#include "../SignalSafeUtils.h"

// Forward declaration for the freelist allocator (C linkage)
extern "C" ListHeader* getHeaderFromFreelist();

void* HeapManager::allocList() {
    // Allocate a ListHeader from the freelist
    ListHeader* header = getHeaderFromFreelist();
    if (!header) {
        safe_print("Error: List header allocation failed (freelist empty)\n");
        return nullptr;
    }

    // Track this allocation in the HeapManager's map (thread-safe)
    {
        std::lock_guard<std::mutex> lock(heap_mutex_);
        heap_blocks_.emplace(header, HeapBlock{ALLOC_LIST, header, sizeof(ListHeader), nullptr, nullptr});
        // Conditionally update the signal-safe shadow array if tracing is enabled
        if (traceEnabled) {
            g_shadow_heap_blocks[g_shadow_heap_index].type = ALLOC_LIST;
            g_shadow_heap_blocks[g_shadow_heap_index].address = header;
            g_shadow_heap_blocks[g_shadow_heap_index].size = sizeof(ListHeader);
            g_shadow_heap_blocks[g_shadow_heap_index].function_name = nullptr;
            g_shadow_heap_blocks[g_shadow_heap_index].variable_name = nullptr;
            g_shadow_heap_index = (g_shadow_heap_index + 1) % MAX_HEAP_BLOCKS;
        }
    }

    // SAMM: Track allocation in current scope if enabled
    if (samm_enabled_.load() && header != nullptr) {
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (!scope_allocations_.empty()) {
            scope_allocations_.back().push_back(header);
            if (traceEnabled) {
                printf("SAMM: Tracked list allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                       header, scope_allocations_.size(), scope_allocations_.back().size());
            }
        } else {
            if (traceEnabled) {
                printf("SAMM: ERROR - No scopes available to track list allocation %p\n", header);
            }
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: Not tracking list allocation %p (enabled: %s, ptr valid: %s)\n", 
                   header, samm_enabled_.load() ? "YES" : "NO", (header != nullptr) ? "YES" : "NO");
        }
    }

    // Update internal metrics
    totalBytesAllocated += sizeof(ListHeader);
    totalListsAllocated++;

    traceLog("Tracked list header: Address=%p, Size=%zu\n", header, sizeof(ListHeader));
    return header;
}

#include "HeapManager.h"
#include "heap_manager_defs.h"
#include "../SignalSafeUtils.h"
#include "../runtime/ListDataTypes.h" // For ListHeader, ListAtom, ATOM_* types
#include <algorithm>
#include <cstdint>
#include <unistd.h> // For write()

// Removed unsafe safe_format_line and strncat usage. Use only safe_print sequences below.

void HeapManager::dumpHeapSignalSafe() {
    // Check the global flag first. This is a safe read.
    if (!g_is_heap_tracing_enabled) {
        safe_print("\n--- Heap Allocation Report (Signal Handler) ---\n");
        safe_print("NOTE: Heap tracing was not enabled. No heap data to dump.\n");
        safe_print("---------------------------------------------\n");
        return;
    }

    safe_print("\n--- Heap Allocation Report (Signal Handler) ---\n");
    char index_buf[20], addr_buf[20], size_buf[20];

    int active_blocks = 0;
    for (size_t i = 0; i < MAX_HEAP_BLOCKS; ++i) {
        // Use a reference to the volatile data
        const volatile HeapBlock& block = g_shadow_heap_blocks[i];

        if (block.address != nullptr && block.type != ALLOC_FREE) {
            int_to_dec((int64_t)i, index_buf);
            u64_to_hex((uint64_t)(uintptr_t)block.address, addr_buf);
            int_to_dec((int64_t)block.size, size_buf);

            safe_print("Block ");
            safe_print(index_buf);
            safe_print(": Type=");
            switch (block.type) {
                case ALLOC_VEC: safe_print("Vector"); break;
                case ALLOC_OBJECT: safe_print("Object"); break;
                case ALLOC_STRING: safe_print("String"); break;
                case ALLOC_LIST: safe_print("List"); break;
                default: safe_print("Unknown"); break;
            }
            safe_print(", Address=0x");
            safe_print(addr_buf);
            safe_print(", Size=");
            safe_print(size_buf);
            safe_print("\n");
            active_blocks++;
        }
    }
    safe_print("---------------------------------------------\n");
    safe_print("Total blocks tracked: ");
    int_to_dec(active_blocks, index_buf);
    safe_print(index_buf);
    safe_print("\n");
}

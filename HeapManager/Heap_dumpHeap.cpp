#include "HeapManager.h"
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print, u64_to_hex, int_to_dec
#include <algorithm> // For std::min

void HeapManager::dumpHeap() const {
    safe_print("\n=== Heap Allocation Report (v3.1)   =======================\n");
    char addr_buf[20], size_buf[20], type_buf[20];

    size_t count = 0;
    for (const auto& kv : heap_blocks_) {
        const HeapBlock& block = kv.second;
        if (block.address != nullptr) {
            safe_print("Block ");
            int_to_dec((int64_t)count, type_buf);
            safe_print(type_buf);
            safe_print(": Type=");
            const char* type_name = "UNKNOWN";
            switch (block.type) {
                case ALLOC_VEC: type_name = "VECTOR"; break;
                case ALLOC_OBJECT: type_name = "OBJECT"; break;
                case ALLOC_STRING: type_name = "STRING"; break;
                case ALLOC_LIST: type_name = "LIST"; break;
                case ALLOC_FREE: type_name = "FREED"; break;
                case ALLOC_GENERIC: type_name = "GENERIC"; break;
                default: break;
            }
            safe_print(type_name);
            safe_print(", Address=");
            u64_to_hex((uint64_t)(uintptr_t)block.address, addr_buf);
            safe_print(addr_buf);
            safe_print(", Size=");
            int_to_dec((int64_t)block.size, size_buf);
            safe_print(size_buf);
            if (block.type == ALLOC_FREE) {
                safe_print(" [FREED]");
            }
            safe_print("\n");

            if (block.type == ALLOC_VEC) {
                safe_print("  Vector length: ");
                uint64_t* vec = static_cast<uint64_t*>(block.address);
                size_t len = vec[0];
                char len_buf[20];
                int_to_dec((int64_t)len, len_buf);
                safe_print(len_buf);
                safe_print("\n  Elements: ");
                for (size_t j = 0; j < std::min(len, size_t(10)); ++j) {
                    int_to_dec((int64_t)vec[1 + j], addr_buf);
                    safe_print(addr_buf);
                    safe_print(" ");
                }
                if (len > 10) safe_print("...");
                safe_print("\n");
            }
            ++count;
        }
    }
    safe_print("Total active blocks: ");
    int_to_dec((int64_t)count, type_buf);
    safe_print(type_buf);
    safe_print("\n");

    safe_print("\n=== End Allocation Report (v3.1)      =======================\n");
}

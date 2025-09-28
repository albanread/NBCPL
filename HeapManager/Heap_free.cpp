#include <cstdlib>
#include "HeapManager.h" 
#include <cstddef> // For size_t
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print
#include "../runtime/ListDataTypes.h" // For ListHeader
#include "../runtime/BCPLError.h"

// Declare returnHeaderToFreelist with C linkage
extern "C" void returnHeaderToFreelist(ListHeader*);

void HeapManager::free(void* payload) {
    if (!payload) return;

    // SAMM: Check if this pointer was already freed by SAMM
    if (samm_enabled_.load()) {
        std::lock_guard<std::mutex> samm_lock(scope_mutex_);
        if (samm_freed_pointers_.find(payload) != samm_freed_pointers_.end()) {
            // Already freed by SAMM, ignore this free() call
            if (traceEnabled) {
                printf("SAMM: Ignoring free() for %p - already freed by SAMM\n", payload);
            }
            return;
        }
    }

    std::lock_guard<std::mutex> lock(heap_mutex_);
    
    // Debug: Always print what we're trying to free
    if (traceEnabled) {
        printf("DEBUG: HeapManager::free called with payload=%p\n", payload);
    }

    // Check if this payload address was recently freed (double-free detection using Bloom filter)
    if (traceEnabled) {
        printf("DEBUG: Checking payload %p in Bloom filter\n", payload);
    }
    if (recently_freed_addresses_.check(payload)) {
        totalDoubleFreeAttempts++;
        // Note: This could be a false positive, so we increment the counter but mention it in the error
        totalBloomFilterFalsePositives++; // Assume it's a false positive for stats (will be corrected if it's a real double-free)
        update_double_free_metrics();
        if (traceEnabled) {
            printf("DEBUG: POTENTIAL DOUBLE-FREE DETECTED for payload %p (could be false positive)!\n", payload);
            safe_print("\n=== ERROR: POTENTIAL DOUBLE FREE DETECTED (PAYLOAD) ===\n");
            safe_print("Address: 0x");
            char addr_buf[20];
            u64_to_hex((uint64_t)(uintptr_t)payload, addr_buf);
            safe_print(addr_buf);
            safe_print("\nNote: This could be a false positive from Bloom filter\n");
            safe_print("=== END POTENTIAL DOUBLE FREE ERROR ===\n");
        }
        _BCPL_SET_ERROR(ERROR_DOUBLE_FREE, "free", "Potential double-free detected for memory address (Bloom filter detection)");
        if (traceEnabled) {
            traceLog("Potential double-free detected: Address=%p\n", payload);
        }
        return;
    }

    // Calculate potential base address for Vec/String (payload - 8 bytes)
    void* base_address_for_vec_string = static_cast<uint8_t*>(payload) - sizeof(uint64_t);
    
    // Check if the base address was recently freed (for Vec/String double-frees)
    if (traceEnabled) {
        printf("DEBUG: Checking base address %p in Bloom filter\n", base_address_for_vec_string);
    }
    if (recently_freed_addresses_.check(base_address_for_vec_string)) {
        totalDoubleFreeAttempts++;
        // Note: This could be a false positive, so we increment the counter but mention it in the error
        totalBloomFilterFalsePositives++; // Assume it's a false positive for stats (will be corrected if it's a real double-free)
        update_double_free_metrics();
        if (traceEnabled) {
            printf("DEBUG: POTENTIAL DOUBLE-FREE DETECTED for base address %p (payload %p, could be false positive)!\n", base_address_for_vec_string, payload);
            safe_print("\n=== ERROR: POTENTIAL DOUBLE FREE DETECTED (VEC/STRING) ===\n");
            safe_print("Payload address: 0x");
            char addr_buf[20];
            u64_to_hex((uint64_t)(uintptr_t)payload, addr_buf);
            safe_print(addr_buf);
            safe_print(", Base address: 0x");
            u64_to_hex((uint64_t)(uintptr_t)base_address_for_vec_string, addr_buf);
            safe_print(addr_buf);
            safe_print("\nNote: This could be a false positive from Bloom filter\n");
            safe_print("=== END POTENTIAL DOUBLE FREE ERROR ===\n");
        }
        _BCPL_SET_ERROR(ERROR_DOUBLE_FREE, "free", "Potential double-free detected for memory address (vector/string, Bloom filter detection)");
        if (traceEnabled) {
            traceLog("Potential double-free detected (vector/string): Payload=%p, Base=%p\n", payload, base_address_for_vec_string);
        }
        return;
    }

    // First, try to find the block assuming the payload IS the base address (for Objects, Lists)
    auto it = heap_blocks_.find(payload);

    // If not found, it might be a Vec or String where the payload is offset
    if (it == heap_blocks_.end()) {
        it = heap_blocks_.find(base_address_for_vec_string);
    }

    // Now, check if we found it with either method
    if (it != heap_blocks_.end()) {
        const auto& block = it->second;
        void* base_address = block.address; // Use the definitive address from the block

        // Update metrics based on block.type
        totalBytesFreed += block.size;
        update_free_metrics(block.size);
        if (block.type == ALLOC_VEC) totalVectorsFreed++;
        else if (block.type == ALLOC_STRING) totalStringsFreed++;

        // Handle actual deallocation
        if (block.type == ALLOC_LIST) {
            returnHeaderToFreelist(static_cast<ListHeader*>(payload));
        } else {
            std::free(base_address);
        }

        // Add to Bloom filter for double-free detection
        // Always track the base address
        if (traceEnabled) {
            printf("DEBUG: Adding base address %p to Bloom filter\n", base_address);
        }
        recently_freed_addresses_.add(base_address);
        bloom_filter_items_added_++;
        
        // For Vec/String types, also track the payload address to catch double-frees
        // from either the base or payload address
        if (base_address != payload) {
            if (traceEnabled) {
                printf("DEBUG: Adding payload address %p to Bloom filter\n", payload);
            }
            recently_freed_addresses_.add(payload);
            bloom_filter_items_added_++;
        }

        // Print fixed bloom filter statistics
        if (traceEnabled) {
            printf("DEBUG: Fixed Bloom filter (12MB) now has ~%lu items (capacity: 10M, estimated false positive rate: %.4f%%)\n", 
                   bloom_filter_items_added_, 
                   recently_freed_addresses_.estimate_false_positive_rate(bloom_filter_items_added_) * 100.0);
        }
        
        // Reset bloom filter if it gets too saturated (over 8M items to stay well under 10M capacity)
        const size_t MAX_BLOOM_ITEMS = 8000000;
        if (bloom_filter_items_added_ > MAX_BLOOM_ITEMS) {
            printf("DEBUG: Fixed Bloom filter reached 8M items, clearing to maintain low FP rate\n");
            recently_freed_addresses_.clear();
            bloom_filter_items_added_ = 0;
        }
        
        // Remove from tracking map
        heap_blocks_.erase(it);

        // Conditionally update shadow array if tracing
        if (traceEnabled) {
            for (size_t i = 0; i < MAX_HEAP_BLOCKS; ++i) {
                if (g_shadow_heap_blocks[i].address == base_address) {
                    g_shadow_heap_blocks[i].type = ALLOC_FREE;
                    g_shadow_heap_blocks[i].address = nullptr;
                    g_shadow_heap_blocks[i].size = 0;
                    g_shadow_heap_blocks[i].function_name = nullptr;
                    g_shadow_heap_blocks[i].variable_name = nullptr;
                    break;
                }
            }
        }

        // Debug output for freed memory (only when tracing is enabled)
        if (traceEnabled) {
            safe_print("\n=== MEMORY FREED ===\n");
            safe_print("Base address: 0x");
            char addr_buf[20];
            u64_to_hex((uint64_t)(uintptr_t)base_address, addr_buf);
            safe_print(addr_buf);
            safe_print(", Payload address: 0x");
            u64_to_hex((uint64_t)(uintptr_t)payload, addr_buf);
            safe_print(addr_buf);
            safe_print("\n=== END FREED INFO ===\n");
        }

        // Trace log if enabled
        traceLog("Freed memory: Address=%p\n", payload);

        return;
    }

    // If not found in map
    _BCPL_SET_ERROR(ERROR_INVALID_POINTER, "free", "Attempt to free an untracked memory address");
    if (traceEnabled) {
        safe_print("\n=== ERROR: UNTRACKED MEMORY FREE ATTEMPT ===\n");
        safe_print("Address: 0x");
        char addr_buf[20];
        u64_to_hex((uint64_t)(uintptr_t)payload, addr_buf);
        safe_print(addr_buf);
        safe_print("\n=== END UNTRACKED FREE ERROR ===\n");
    }
}


#ifndef HEAP_MANAGER_H
#define HEAP_MANAGER_H

// ============================================================================
// MAINTAINER NOTE:
// If you add new HeapManager methods that need to be called from C or assembly
// (the runtime), you MUST add a corresponding extern "C" wrapper function in
// heap_c_wrappers.cpp. Otherwise, you will get linker errors!
//
// See also: runtime/README.md for more information on runtime ABI requirements.
// ============================================================================

#include <cstddef> // For size_t
#include <cstdint> // For uint64_t, uint32_t
#include <unordered_map>
#include <mutex>
#include <vector>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <unordered_set>

// Include heap manager definitions
#include "heap_manager_defs.h"
#include "BloomFilter.h"

// Global heap trace flag (set early in main.cpp)
extern bool g_enable_heap_trace;

#ifdef __cplusplus
extern "C" {
#endif

void* OBJECT_HEAP_ALLOC(void* class_ptr);
void OBJECT_HEAP_FREE(void* object_ptr);

#ifdef __cplusplus
}
#endif

class HeapManager {
public:
    // The new scalable tracking map. It maps a block's base address to its metadata.
    std::unordered_map<void*, HeapBlock> heap_blocks_;
    
    // Fixed 12MB Bloom filter for efficient double-free detection
    // Can track ~10M freed addresses with <0.1% false positive rate
    // 96M bits (12MB), 10 hash functions - no scaling needed
    BloomFilter<96000000, 10> recently_freed_addresses_;
    
    // Counter to track approximate number of items added to Bloom filter
    size_t bloom_filter_items_added_;

    // A mutex to make heap operations thread-safe.
    // 'mutable' allows const methods like dumpHeap() to lock it.
    mutable std::mutex heap_mutex_;

    // Cleanup timing metrics
    double total_cleanup_time_ms_;
    size_t items_cleaned_count_;

    // SAMM: Scope Aware Memory Management
    // Dual-mutex architecture for minimal contention
    mutable std::mutex scope_mutex_;    // Ultra-fast operations on hot path
    mutable std::mutex cleanup_mutex_;  // Queue coordination between threads
    
    // SAMM: Scope vector for tracking allocations per lexical scope
    std::vector<std::vector<void*>> scope_allocations_;
    
    // SAMM: Track which pointers are ListHeaders from freelist (not individual atoms)
    std::unordered_set<void*> freelist_pointers_;
    
    // SAMM: Track which pointers are from string pool (not HeapManager)
    std::unordered_set<void*> string_pool_pointers_;
    
    // SAMM: Background cleanup infrastructure
    std::queue<std::vector<void*>> cleanup_queue_;
    std::condition_variable cleanup_cv_;
    std::thread cleanup_worker_;
    std::atomic<bool> running_{true};
    std::atomic<bool> samm_enabled_{false};
    
    // SAMM: Track freed pointers to prevent double-free in mixed mode
    std::unordered_set<void*> samm_freed_pointers_;
    
    // SAMM: Performance and debug statistics
    std::atomic<uint64_t> samm_scopes_entered_{0};
    std::atomic<uint64_t> samm_scopes_exited_{0};
    std::atomic<uint64_t> samm_objects_cleaned_{0};
    std::atomic<uint64_t> samm_cleanup_batches_processed_{0};

private:
    // Internal state for metrics
    size_t totalBytesAllocated;
    size_t totalBytesFreed;
    size_t totalVectorsAllocated;
    size_t totalObjectsAllocated;
    size_t totalStringsAllocated;
    size_t totalListsAllocated;
    size_t totalListAtomsAllocated;
    size_t totalVectorsFreed;
    size_t totalStringsFreed;
    size_t totalListsFreed;
    size_t totalListAtomsFreed;
    size_t totalDoubleFreeAttempts;  // Track double-free attempts
    size_t totalBloomFilterFalsePositives; // Track potential false positives

    // Cleanup timing statistics
    double totalCleanupTimeMs;

    // Trace flag
    bool traceEnabled; // Controls whether trace messages are printed

    // Private constructor for singleton pattern
    HeapManager();
    HeapManager(const HeapManager&) = delete;
    HeapManager& operator=(const HeapManager&) = delete;

    // Private helper for logging (if needed outside public interface)

    // Static instance for singleton
    static HeapManager* instance;

    // SAMM: Private helper methods
    void cleanupWorker();
    void cleanupPointersImmediate(const std::vector<void*>& ptrs);
    void* internalAlloc(size_t size, AllocType type);

public:
    // Destructor - ensures proper SAMM shutdown
    ~HeapManager();
    
    // Singleton access
    static HeapManager& getInstance();

    // Allocation functions
    void* allocVec(size_t numElements);
    void* allocObject(size_t size);

    // Allocate and track a ListHeader (for BCPL lists)
    void* allocList();

    // Setter for traceEnabled
    void setTraceEnabled(bool enabled);
    bool isTracingEnabled() const;
    void* allocString(size_t numChars);

    // Deallocation function
    void free(void* payload);

    // Debugging and metrics
    void dumpHeap() const;
    void dumpHeapSignalSafe(); // Must be truly signal-safe
    void printMetrics() const;

    // Public static traceLog for use everywhere
    static void traceLog(const char* format, ...);

    // Public static traceLog for use everywhere
    // static void traceLog(const char* format, ...); // Already declared above

    // Getter for double-free attempts count
    size_t getDoubleFreeAttempts() const { return totalDoubleFreeAttempts; }
    
    // Getter for Bloom filter statistics
    size_t getBloomFilterFalsePositives() const { return totalBloomFilterFalsePositives; }
    size_t getBloomFilterItemsAdded() const { return bloom_filter_items_added_; }
    size_t getBloomFilterMemoryUsage() const { return recently_freed_addresses_.memory_usage(); }
    double getBloomFilterFalsePositiveRate() const { 
        return recently_freed_addresses_.estimate_false_positive_rate(bloom_filter_items_added_); 
    }
    
    // Fixed bloom filter specific getters  
    const char* getBloomFilterTier() const { return "10M-Fixed"; }
    size_t getBloomFilterResetCount() const { return 0; } // No resets with fixed filter
    size_t getBloomFilterCapacity() const { return 10000000; } // 10M capacity

    // Cleanup timing getters
    double getTotalCleanupTimeMs() const { return totalCleanupTimeMs; }

    // --- Added for leak reporting ---
    size_t getTotalBytesAllocated() const { return totalBytesAllocated; }
    size_t getTotalBytesFreed() const { return totalBytesFreed; }
    
    // List-specific metrics
    size_t getTotalListsAllocated() const { return totalListsAllocated; }
    size_t getTotalListsFreed() const { return totalListsFreed; }
    size_t getTotalListAtomsAllocated() const { return totalListAtomsAllocated; }
    size_t getTotalListAtomsFreed() const { return totalListAtomsFreed; }

    // SAMM: Scope Aware Memory Management API
    void setSAMMEnabled(bool enabled);
    bool isSAMMEnabled() const { return samm_enabled_.load(); }
    void startBackgroundWorker();
    void stopBackgroundWorker();
    void enterScope();
    void exitScope();
    void retainPointer(void* ptr, int parent_scope_offset = 1);
    void trackFreelistAllocation(void* ptr);
    void trackStringPoolAllocation(void* ptr);
    void trackInCurrentScope(void* ptr);
    void handleMemoryPressure();
    void waitForSAMM();
    void shutdown();
    
    // SAMM: Statistics and debugging
    struct SAMMStats {
        uint64_t scopes_entered;
        uint64_t scopes_exited;
        uint64_t objects_cleaned;
        uint64_t cleanup_batches_processed;
        size_t current_queue_depth;
        bool background_worker_running;
        size_t current_scope_depth;
    };
    SAMMStats getSAMMStats() const;
    
    // SAMM: RETAIN variants of allocation functions
    void* allocObjectRetained(size_t size, int parent_scope_offset = 1);
    void* allocVecRetained(size_t numElements, int parent_scope_offset = 1);
    void* allocStringRetained(size_t numChars, int parent_scope_offset = 1);
    void* allocListRetained(int parent_scope_offset = 1);
};

#endif // HEAP_MANAGER_H
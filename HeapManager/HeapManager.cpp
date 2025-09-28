#include "HeapManager.h"
#include "heap_c_wrappers.h"
#include "heap_c_wrappers.h"
#include "../SignalSafeUtils.h" // For safe_print
#include <cstdarg> // For va_list, va_start, va_end
#include <cstdio>  // For vsnprintf
#include <cstdlib> // For std::abort, posix_memalign
#include <cstring> // For memset
#include <chrono>  // For cleanup timing
#include "../runtime/ListDataTypes.h" // For ListHeader and ListAtom

// Forward declarations for freelist functions
extern "C" {
    void returnNodeToFreelist(ListAtom* node);
    void returnHeaderToFreelist(ListHeader* header);
    void embedded_fast_bcpl_free_chars(void* ptr);
}

// Static instance for the singleton
HeapManager* HeapManager::instance = nullptr;

// Private constructor
HeapManager::HeapManager()
    : bloom_filter_items_added_(0),
      totalBytesAllocated(0),
      totalBytesFreed(0),
      totalVectorsAllocated(0),
      totalObjectsAllocated(0),
      totalStringsAllocated(0),
      totalListsAllocated(0),
      totalListAtomsAllocated(0),
      totalVectorsFreed(0),
      totalStringsFreed(0),
      totalListsFreed(0),
      totalListAtomsFreed(0),
      totalDoubleFreeAttempts(0),
      totalBloomFilterFalsePositives(0),
      totalCleanupTimeMs(0.0),
      traceEnabled(false) {
    if (g_enable_heap_trace) {
        printf("DEBUG: HeapManager constructor called\n");
    }
    
    // SAMM: Initialize scope stack with global scope
    scope_allocations_.push_back({});
}

// Destructor - ensures proper SAMM shutdown
HeapManager::~HeapManager() {
    shutdown();
}

// Public static trace log helper
void HeapManager::traceLog(const char* format, ...) {
    if (!HeapManager::getInstance().isTracingEnabled()) return;
    char buffer[256];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    fprintf(stderr, "%s", buffer);
}

// Add debug to getInstance to see if it's being called
HeapManager& HeapManager::getInstance() {
    if (!HeapManager::instance) {
        HeapManager::instance = new HeapManager();
        if (instance->traceEnabled) {
            printf("DEBUG: Creating HeapManager singleton instance\n");
        }
    }
    return *HeapManager::instance;
}

bool HeapManager::isTracingEnabled() const {
    return traceEnabled;
}

// SAMM: Background cleanup worker thread
void HeapManager::cleanupWorker() {
    if (traceEnabled) {
        printf("SAMM: Background cleanup worker thread started (DEBUG)\n");
    }
    
    while (running_.load()) {
        std::unique_lock<std::mutex> lock(cleanup_mutex_);
        if (traceEnabled) {
            printf("SAMM: Worker waiting for cleanup queue (queue size: %zu)\n", cleanup_queue_.size());
        }
        
        cleanup_cv_.wait(lock, [this] { 
            return !cleanup_queue_.empty() || !running_.load(); 
        });
        
        if (traceEnabled) {
            printf("SAMM: Worker woke up, queue size: %zu, running: %s\n", 
                   cleanup_queue_.size(), running_.load() ? "YES" : "NO");
        }
        
        while (!cleanup_queue_.empty()) {
            auto ptrs = std::move(cleanup_queue_.front());
            cleanup_queue_.pop();
            if (traceEnabled) {
                printf("SAMM: Processing batch of %zu objects\n", ptrs.size());
            }
            lock.unlock();
            
            // Clean up this batch immediately
            cleanupPointersImmediate(ptrs);
            
            samm_cleanup_batches_processed_.fetch_add(1);
            if (traceEnabled) {
                printf("SAMM: Background worker processed batch of %zu objects\n", ptrs.size());
            }
            
            lock.lock();
        }
    }
    
    if (traceEnabled) {
        printf("SAMM: Background cleanup worker thread stopped (DEBUG)\n");
    }
}

// SAMM: Immediate cleanup of a batch of pointers
void HeapManager::cleanupPointersImmediate(const std::vector<void*>& ptrs) {
    if (traceEnabled) {
        printf("SAMM: cleanupPointersImmediate called with %zu pointers\n", ptrs.size());
    }
    
    // Time the cleanup operation
    auto start_time = std::chrono::high_resolution_clock::now();
    
    for (void* ptr : ptrs) {
        if (ptr != nullptr) {
            if (traceEnabled) {
                printf("SAMM: Cleaning up pointer %p\n", ptr);
            }
            
            // Check if this is a freelist ListHeader, string pool allocation, or regular HeapManager pointer
            bool is_freelist_header = false;
            bool is_string_pool = false;
            {
                std::lock_guard<std::mutex> lock(scope_mutex_);
                is_freelist_header = freelist_pointers_.find(ptr) != freelist_pointers_.end();
                is_string_pool = string_pool_pointers_.find(ptr) != string_pool_pointers_.end();
                if (is_freelist_header) {
                    freelist_pointers_.erase(ptr);  // Remove from tracking set
                }
                if (is_string_pool) {
                    string_pool_pointers_.erase(ptr);  // Remove from tracking set
                }
            }
            
            if (is_freelist_header) {
                // This is a ListHeader - free the list properly
                if (traceEnabled) {
                    printf("SAMM: Freeing list header %p and returning all atoms to freelist\n", ptr);
                }
                

                
                ListHeader* header = (ListHeader*)ptr;
                
                // Walk through all ListAtoms and return them to freelist
                ListAtom* atom = header->head;
                while (atom) {
                    ListAtom* next = atom->next;
                    returnNodeToFreelist(atom);
                    atom = next;
                }
                
                // Return the header to freelist
                returnHeaderToFreelist(header);
                
            } else if (is_string_pool) {
                // This is a string pool allocation - return to pool
                if (traceEnabled) {
                    printf("SAMM: Returning string pool allocation %p to pool for reuse\n", ptr);
                }
                
                // Call our embedded string pool free function
                embedded_fast_bcpl_free_chars(ptr);
                
            } else {
                // Call the standard free function for HeapManager allocations
                free(ptr);
            }
            
            // Mark as freed in SAMM to prevent future double-free
            {
                std::lock_guard<std::mutex> lock(scope_mutex_);
                samm_freed_pointers_.insert(ptr);
            }
            
            samm_objects_cleaned_.fetch_add(1);
            if (traceEnabled) {
                printf("SAMM: Successfully cleaned pointer %p\n", ptr);
            }
        }
    }
    
    // Record cleanup timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration<double, std::milli>(end_time - start_time);
    totalCleanupTimeMs += duration.count();
    
    if (traceEnabled) {
        printf("SAMM: cleanupPointersImmediate completed %zu items in %.3f ms\n", 
               ptrs.size(), duration.count());
    }
}

// SAMM: Internal allocation with scope tracking
void* HeapManager::internalAlloc(size_t size, AllocType type) {
    void* ptr = nullptr;
    
    // Call appropriate allocation method based on type
    switch (type) {
        case ALLOC_OBJECT:
            ptr = allocObject(size);
            break;
        case ALLOC_VEC:
            // For vectors, size is number of elements
            ptr = allocVec(size);
            break;
        case ALLOC_STRING:
            // For strings, size is number of characters
            ptr = allocString(size);
            break;
        case ALLOC_LIST:
            ptr = allocList();
            break;
        default:
            return nullptr;
    }
    
    // If SAMM is enabled, track this allocation in current scope
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
    
    return ptr;
}

// SAMM: Enable/disable SAMM
void HeapManager::setSAMMEnabled(bool enabled) {
    if (enabled && !samm_enabled_.load()) {
        // Enabling SAMM for the first time
        samm_enabled_.store(true);
        startBackgroundWorker();
        if (traceEnabled) {
            printf("SAMM: ENABLED and background worker started\n");
        }
    } else if (!enabled && samm_enabled_.load()) {
        // Disabling SAMM
        samm_enabled_.store(false);
        if (traceEnabled) {
            printf("SAMM: DISABLED\n");
        }
    }
}

// SAMM: Start background cleanup worker
void HeapManager::startBackgroundWorker() {
    if (!cleanup_worker_.joinable()) {
        running_.store(true);
        cleanup_worker_ = std::thread(&HeapManager::cleanupWorker, this);
        if (traceEnabled) {
            printf("SAMM: Background worker thread created and started\n");
        }
    } else if (traceEnabled) {
        printf("SAMM: Background worker already running\n");
    }
}

// SAMM: Stop background cleanup worker
void HeapManager::stopBackgroundWorker() {
    running_.store(false);
    cleanup_cv_.notify_all();
    if (cleanup_worker_.joinable()) {
        cleanup_worker_.join();
        if (traceEnabled) {
            printf("SAMM: Background worker stopped\n");
        }
    }
}

// SAMM: Enter a new lexical scope
void HeapManager::enterScope() {
    if (!samm_enabled_.load()) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(scope_mutex_);
    scope_allocations_.push_back({});
    samm_scopes_entered_.fetch_add(1);
    
    if (traceEnabled) {
        printf("SAMM: Entered scope (depth: %zu)\n", scope_allocations_.size());
    }
}

// SAMM: Exit current lexical scope
void HeapManager::exitScope() {
    if (!samm_enabled_.load()) {
        return;
    }
    
    std::vector<void*> to_cleanup;
    
    {
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (scope_allocations_.size() > 1) { // Don't exit global scope
            to_cleanup = std::move(scope_allocations_.back());
            scope_allocations_.pop_back();
            samm_scopes_exited_.fetch_add(1);
            
            if (traceEnabled) {
                printf("SAMM: Scope exit - found %zu objects to cleanup (remaining depth: %zu)\n", 
                       to_cleanup.size(), scope_allocations_.size());
            }
        } else {
            if (traceEnabled) {
                printf("SAMM: Cannot exit global scope (current depth: %zu)\n", scope_allocations_.size());
            }
        }
    }
    
    if (!to_cleanup.empty()) {
        if (traceEnabled) {
            printf("SAMM: About to queue %zu objects for cleanup\n", to_cleanup.size());
        }
        
        // Try to queue for background cleanup
        {
            std::lock_guard<std::mutex> cleanup_lock(cleanup_mutex_);
            cleanup_queue_.push(std::move(to_cleanup));
            if (traceEnabled) {
                printf("SAMM: Queued objects for background cleanup (queue depth: %zu)\n", cleanup_queue_.size());
            }
        }
        cleanup_cv_.notify_one();
        if (traceEnabled) {
            printf("SAMM: Notified background worker\n");
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: No objects to cleanup in this scope\n");
        }
    }
}

// SAMM: Retain pointer to parent scope
void HeapManager::retainPointer(void* ptr, int parent_scope_offset) {
    if (!samm_enabled_.load() || ptr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(scope_mutex_);
    if (scope_allocations_.size() > static_cast<size_t>(parent_scope_offset)) {
        // Remove from current scope
        auto& current_scope = scope_allocations_.back();
        auto it = std::find(current_scope.begin(), current_scope.end(), ptr);
        if (it != current_scope.end()) {
            current_scope.erase(it);
            
            // Add to parent scope
            size_t parent_index = scope_allocations_.size() - 1 - parent_scope_offset;
            scope_allocations_[parent_index].push_back(ptr);
            
            if (traceEnabled) {
                printf("SAMM: Retained pointer %p to parent scope (offset %d)\n", ptr, parent_scope_offset);
            }
        }
    }
}

// SAMM: Track freelist allocation in current scope
void HeapManager::trackFreelistAllocation(void* ptr) {
    if (!samm_enabled_.load() || ptr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(scope_mutex_);
    
    // Mark this pointer as a freelist allocation
    freelist_pointers_.insert(ptr);
    
    if (!scope_allocations_.empty()) {
        scope_allocations_.back().push_back(ptr);
        if (traceEnabled) {
            printf("SAMM: Tracked freelist allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                   ptr, scope_allocations_.size(), scope_allocations_.back().size());
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: ERROR - No scopes available to track freelist allocation %p\n", ptr);
        }
    }
}

// SAMM: Manually track allocation in current scope (for custom allocators)
void HeapManager::trackInCurrentScope(void* ptr) {
    if (!samm_enabled_.load() || ptr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(scope_mutex_);
    
    if (!scope_allocations_.empty()) {
        scope_allocations_.back().push_back(ptr);
        if (traceEnabled) {
            printf("SAMM: Tracked custom allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                   ptr, scope_allocations_.size(), scope_allocations_.back().size());
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: ERROR - No scopes available to track custom allocation %p\n", ptr);
        }
    }
}

// SAMM: Track string pool allocation for proper cleanup
void HeapManager::trackStringPoolAllocation(void* ptr) {
    if (!samm_enabled_.load() || ptr == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(scope_mutex_);
    
    // Mark this pointer as a string pool allocation
    string_pool_pointers_.insert(ptr);
    
    if (!scope_allocations_.empty()) {
        scope_allocations_.back().push_back(ptr);
        if (traceEnabled) {
            printf("SAMM: Tracked string pool allocation %p in scope (depth: %zu, scope size: %zu)\n", 
                   ptr, scope_allocations_.size(), scope_allocations_.back().size());
        }
    } else {
        if (traceEnabled) {
            printf("SAMM: ERROR - No scopes available to track string pool allocation %p\n", ptr);
        }
    }
}

// SAMM: Handle memory pressure by immediate cleanup
void HeapManager::handleMemoryPressure() {
    if (!samm_enabled_.load()) {
        return;
    }
    
    std::vector<std::vector<void*>> all_queued;
    
    {
        std::unique_lock<std::mutex> lock(cleanup_mutex_);
        while (!cleanup_queue_.empty()) {
            all_queued.push_back(std::move(cleanup_queue_.front()));
            cleanup_queue_.pop();
        }
    }
    
    // Clean up all queued items immediately
    for (const auto& batch : all_queued) {
        cleanupPointersImmediate(batch);
    }
    
    if (traceEnabled && !all_queued.empty()) {
        printf("SAMM: Memory pressure cleanup processed %zu batches\n", all_queued.size());
    }
}

// SAMM: Wait for all background cleanup to complete
void HeapManager::waitForSAMM() {
    if (!samm_enabled_.load()) {
        return;
    }
    
    // Simple approach: just process any remaining cleanup immediately
    handleMemoryPressure();
    
    if (traceEnabled) {
        printf("SAMM: Processed all pending cleanup operations\n");
    }
}

// SAMM: Shutdown and cleanup
void HeapManager::shutdown() {
    if (samm_enabled_.load()) {
        // Process remaining cleanup queue
        handleMemoryPressure();
        
        // Stop background worker
        stopBackgroundWorker();
        
        // --- START OF FIX ---
        std::vector<std::vector<void*>> remaining_scopes_to_clean;
        {
            // Lock only long enough to safely copy the remaining pointers
            std::lock_guard<std::mutex> lock(scope_mutex_);
            remaining_scopes_to_clean = std::move(scope_allocations_);
            scope_allocations_.clear();
        }

        // Now, perform the cleanup on the local copy *without* holding the lock.
        // This allows the free() function to acquire the lock without deadlocking.
        for (auto& scope : remaining_scopes_to_clean) {
            cleanupPointersImmediate(scope);
        }
        // --- END OF FIX ---
        
        if (traceEnabled) {
            printf("SAMM: Shutdown complete\n");
        }
    }
}

// SAMM: Get statistics
HeapManager::SAMMStats HeapManager::getSAMMStats() const {
    std::lock_guard<std::mutex> lock(scope_mutex_);
    std::lock_guard<std::mutex> cleanup_lock(cleanup_mutex_);
    
    return {
        samm_scopes_entered_.load(),
        samm_scopes_exited_.load(),
        samm_objects_cleaned_.load(),
        samm_cleanup_batches_processed_.load(),
        cleanup_queue_.size(),
        cleanup_worker_.joinable() && running_.load(),
        scope_allocations_.size()
    };
}

// SAMM: RETAIN allocation variants
void* HeapManager::allocObjectRetained(size_t size, int parent_scope_offset) {
    void* ptr = allocObject(size);
    if (ptr != nullptr) {
        retainPointer(ptr, parent_scope_offset);
    }
    return ptr;
}

void* HeapManager::allocVecRetained(size_t numElements, int parent_scope_offset) {
    void* ptr = allocVec(numElements);
    if (ptr != nullptr) {
        retainPointer(ptr, parent_scope_offset);
    }
    return ptr;
}

void* HeapManager::allocStringRetained(size_t numChars, int parent_scope_offset) {
    void* ptr = allocString(numChars);
    if (ptr != nullptr) {
        retainPointer(ptr, parent_scope_offset);
    }
    return ptr;
}

void* HeapManager::allocListRetained(int parent_scope_offset) {
    void* ptr = allocList();
    if (ptr != nullptr) {
        retainPointer(ptr, parent_scope_offset);
    }
    return ptr;
}





// Helper: Assume class struct has size at offset +32 (or use a fixed size for now)
static size_t get_object_size_from_class(void* class_ptr) {
    // For now, assume a fixed size (e.g., 64 bytes)
    // In a real system, read from class metadata
    return 64;
}





// Assume class struct layout:
// +0: self pointer
// +8: parent pointer
// +16: vtable pointer
// ... (other metadata)
// vtable is an array of function pointers, indexed by method slot/hash





void HeapManager::setTraceEnabled(bool enabled) {
    traceEnabled = enabled;
    g_is_heap_tracing_enabled = enabled; // Update global flag for signal handler
}

// allocObject implementation moved to Heap_allocObject.cpp
// Forward declaration is not needed here since it's already in the class declaration

extern "C" void* OBJECT_HEAP_ALLOC(void* class_ptr) {
    // Calculate the object size - we need space for:
    // 1. vtable pointer (8 bytes)
    // 2. member variables based on class definition
    size_t object_size = 24;  // Default size: 8 (vtable) + 16 (members)
    
    if (class_ptr != nullptr) {
        HeapManager::traceLog("OBJECT_HEAP_ALLOC: Class pointer provided: %p\n", class_ptr);
        // In a real implementation, we would extract the size from class metadata
    }
    
    HeapManager::traceLog("OBJECT_HEAP_ALLOC: Allocating object of size %zu bytes\n", object_size);
    
    if (HeapManager::getInstance().isTracingEnabled()) {
        safe_print("\n=== OBJECT_HEAP_ALLOC BEGIN ===\n");
        char size_buf[20];
        int_to_dec((int)object_size, size_buf);
        safe_print("Allocating object of size: ");
        safe_print(size_buf);
        if (class_ptr != nullptr) {
            safe_print(", Class ptr: 0x");
            char addr_buf[20];
            u64_to_hex((uint64_t)(uintptr_t)class_ptr, addr_buf);
            safe_print(addr_buf);
        } else {
            safe_print(", Class ptr: NULL");
        }
        safe_print("\n");
    }
    
    // Allocate the object using the correct object allocator
    void* obj = Heap_allocObject(object_size);
    
    if (obj) {
        // Zero-initialize the memory (redundant, but safe)
        memset(obj, 0, object_size);
        if (HeapManager::getInstance().isTracingEnabled()) {
            safe_print("Object allocated at address: 0x");
            char addr_buf[20];
            u64_to_hex((uint64_t)(uintptr_t)obj, addr_buf);
            safe_print(addr_buf);
            safe_print("\n=== OBJECT_HEAP_ALLOC END ===\n");
        }
        HeapManager::traceLog("OBJECT_HEAP_ALLOC: Object allocated at %p\n", obj);
    } else {
        if (HeapManager::getInstance().isTracingEnabled()) {
            safe_print("Object allocation FAILED!\n=== OBJECT_HEAP_ALLOC END ===\n");
        }
    }
    
    return obj;
}

extern "C" void OBJECT_HEAP_FREE(void* object_ptr) {
    printf("OBJECT_HEAP_FREE: Called with object_ptr=%p\n", object_ptr);
    
    if (object_ptr == nullptr) {
        printf("OBJECT_HEAP_FREE: Warning - Attempt to free null pointer.\n");
        HeapManager::traceLog("OBJECT_HEAP_FREE: Warning - Attempt to free null pointer.\n");
        return;
    }
    
    printf("OBJECT_HEAP_FREE: Freeing object at %p\n", object_ptr);
    HeapManager::traceLog("OBJECT_HEAP_FREE: Freeing object at %p.\n", object_ptr);
    
    // Free the memory using the HeapManager's mechanism
    Heap_free(object_ptr);
    
    printf("OBJECT_HEAP_FREE: Successfully freed object at %p\n", object_ptr);
}

extern "C" void* RUNTIME_METHOD_LOOKUP(void* class_ptr, uint64_t method_hash) {
    // This is a placeholder for dynamic method lookup.
    // In a real scenario, this would involve looking up the method in the class's vtable.
    HeapManager::traceLog("RUNTIME_METHOD_LOOKUP: class_ptr=%p, method_hash=%llu\n", class_ptr, method_hash);
    return nullptr; // Placeholder
}

extern "C" void PIC_RUNTIME_HELPER() {
    // This is a placeholder for a runtime helper function that might be called
    // from position-independent code (PIC).
    HeapManager::traceLog("PIC_RUNTIME_HELPER: Called.\n");
    // Actual implementation would depend on the specific helper needed.
}

// Special debug helper for checking heap status during BRK instruction
extern "C" void DEBUG_HEAP_AT_BREAKPOINT() {
    safe_print("\n=== HEAP STATUS AT BREAKPOINT ===\n");

    // Count and report live object allocations using heap_blocks_ map
    int object_count = 0;
    char buf[64];

    // Access singleton instance
    HeapManager& mgr = HeapManager::getInstance();

    {
        std::lock_guard<std::mutex> lock(mgr.heap_mutex_);
        for (const auto& kv : mgr.heap_blocks_) {
            const HeapBlock& block = kv.second;
            if (block.type == ALLOC_OBJECT && block.address != nullptr) {
                object_count++;
                safe_print("Found object: Address: 0x");
                u64_to_hex((uint64_t)(uintptr_t)block.address, buf);
                safe_print(buf);
                safe_print(", Size: ");
                int_to_dec((int)block.size, buf);
                safe_print(buf);
                safe_print("\n");
            }
        }
    }

    safe_print("Total object allocations found: ");
    int_to_dec(object_count, buf);
    safe_print(buf);
    safe_print("\n=== END HEAP STATUS ===\n");
}

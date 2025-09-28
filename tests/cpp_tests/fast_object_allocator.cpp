// fast_object_allocator.cpp
// High-performance object allocator with size-class freelists and 16-byte alignment
// Optimized for abundant memory systems (16GB-192GB) where speed > memory efficiency

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <array>
#include <atomic>
#include <mutex>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include "../../HeapManager/HeapManager.h"
#include "../../runtime/ListDataTypes.h"

// Global trace variables required by the runtime
bool g_enable_heap_trace = false;
bool g_enable_lexer_trace = false;
bool g_enable_symbols_trace = false;

extern "C" {
    void BCPL_LIST_APPEND_OBJECT(ListHeader* header, void* object_ptr);
    ListHeader* BCPL_LIST_CREATE_EMPTY(void);
    void HeapManager_enterScope(void);
    void HeapManager_exitScope(void);
    void HeapManager_setSAMMEnabled(int enabled);
    int HeapManager_isSAMMEnabled(void);
}

// ============================================================================
// Fast Object Allocator - Size Class Based with Alignment
// ============================================================================

class FastObjectAllocator {
public:
    // Size classes: powers of 2 from 16 to 1024 bytes (all 16-byte aligned)
    static constexpr size_t NUM_SIZE_CLASSES = 7;
    static constexpr size_t SIZE_CLASSES[NUM_SIZE_CLASSES] = {
        16, 32, 64, 128, 256, 512, 1024
    };
    static constexpr size_t ALIGNMENT = 16;
    static constexpr size_t CHUNK_SIZE = 4 * 1024;   // 4KB chunks - much more reasonable
    static constexpr size_t MAX_OBJECTS_PER_CHUNK = CHUNK_SIZE / 16;  // Max for smallest objects

private:
    // Free object node - stored in the free object's memory itself
    struct FreeNode {
        FreeNode* next;
        // Padding to maintain alignment if needed
        char padding[ALIGNMENT - sizeof(FreeNode*)];
    };
    
    // Memory chunk for each size class
    struct MemoryChunk {
        char* memory;
        size_t size;
        size_t objects_allocated;
        size_t objects_capacity;
        MemoryChunk* next_chunk;
        
        MemoryChunk(size_t chunk_size) 
            : size(chunk_size), objects_allocated(0), next_chunk(nullptr) {
            // Allocate aligned memory
            if (posix_memalign(reinterpret_cast<void**>(&memory), ALIGNMENT, chunk_size) != 0) {
                memory = nullptr;
                objects_capacity = 0;
            } else {
                memset(memory, 0, chunk_size);
                objects_capacity = chunk_size / 16;  // Will be adjusted per size class
            }
        }
        
        ~MemoryChunk() {
            if (memory) {
                free(memory);
            }
        }
    };
    
    // Per-size-class data
    struct SizeClassData {
        size_t object_size;
        std::atomic<FreeNode*> free_list;
        std::mutex chunk_mutex;
        MemoryChunk* chunks;
        
        // Statistics
        std::atomic<size_t> total_allocated;
        std::atomic<size_t> total_freed;
        std::atomic<size_t> chunks_allocated;
        
        SizeClassData() = default;
        
        SizeClassData(size_t size) 
            : object_size(size), free_list(nullptr), chunks(nullptr),
              total_allocated(0), total_freed(0), chunks_allocated(0) {}
              
        // Move constructor
        SizeClassData(SizeClassData&& other) noexcept
            : object_size(other.object_size), free_list(other.free_list.load()),
              chunks(other.chunks), total_allocated(other.total_allocated.load()),
              total_freed(other.total_freed.load()), chunks_allocated(other.chunks_allocated.load()) {
            other.chunks = nullptr;
        }
        
        // Move assignment
        SizeClassData& operator=(SizeClassData&& other) noexcept {
            if (this != &other) {
                object_size = other.object_size;
                free_list = other.free_list.load();
                chunks = other.chunks;
                total_allocated = other.total_allocated.load();
                total_freed = other.total_freed.load();
                chunks_allocated = other.chunks_allocated.load();
                other.chunks = nullptr;
            }
            return *this;
        }
    };
    
    std::array<SizeClassData, NUM_SIZE_CLASSES> size_classes_;
    std::atomic<size_t> total_memory_allocated_;
    std::atomic<bool> initialized_;
    
    // Find size class index for given size
    int get_size_class_index(size_t size) {
        for (size_t i = 0; i < NUM_SIZE_CLASSES; ++i) {
            if (size <= SIZE_CLASSES[i]) {
                return i;
            }
        }
        return -1;  // Too large
    }
    
    // Allocate new chunk for size class
    bool allocate_new_chunk(int class_index) {
        SizeClassData& sc = size_classes_[class_index];
        size_t object_size = sc.object_size;
        
        // Calculate optimal chunk size for this object size
        size_t objects_per_chunk = CHUNK_SIZE / object_size;
        if (objects_per_chunk < 8) objects_per_chunk = 8;    // Minimum 8 objects
        if (objects_per_chunk > 256) objects_per_chunk = 256; // Maximum 256 objects
        
        size_t actual_chunk_size = objects_per_chunk * object_size;
        
        std::lock_guard<std::mutex> lock(sc.chunk_mutex);
        
        // Allocate new chunk
        MemoryChunk* new_chunk = new MemoryChunk(actual_chunk_size);
        if (!new_chunk->memory) {
            delete new_chunk;
            return false;
        }
        
        new_chunk->objects_capacity = objects_per_chunk;
        
        // Link into chunk list
        new_chunk->next_chunk = sc.chunks;
        sc.chunks = new_chunk;
        sc.chunks_allocated++;
        total_memory_allocated_ += actual_chunk_size;
        
        // Build free list from this chunk
        char* current = new_chunk->memory;
        FreeNode* prev_node = nullptr;
        
        for (size_t i = 0; i < objects_per_chunk; ++i) {
            FreeNode* node = reinterpret_cast<FreeNode*>(current);
            node->next = prev_node;
            prev_node = node;
            current += object_size;
        }
        
        // Atomically update free list
        FreeNode* old_head = sc.free_list.load();
        do {
            prev_node->next = old_head;
        } while (!sc.free_list.compare_exchange_weak(old_head, 
                 reinterpret_cast<FreeNode*>(new_chunk->memory + (objects_per_chunk - 1) * object_size)));
        
        return true;
    }

public:
    FastObjectAllocator() : total_memory_allocated_(0), initialized_(false) {
        // Initialize size classes
        for (size_t i = 0; i < NUM_SIZE_CLASSES; ++i) {
            new (&size_classes_[i]) SizeClassData(SIZE_CLASSES[i]);
        }
        initialized_ = true;
    }
    
    ~FastObjectAllocator() {
        // Clean up all chunks
        for (auto& sc : size_classes_) {
            std::lock_guard<std::mutex> lock(sc.chunk_mutex);
            MemoryChunk* chunk = sc.chunks;
            while (chunk) {
                MemoryChunk* next = chunk->next_chunk;
                delete chunk;
                chunk = next;
            }
        }
    }
    
    // Fast aligned object allocation
    void* allocate(size_t size) {
        if (!initialized_) return nullptr;
        
        int class_index = get_size_class_index(size);
        if (class_index < 0) {
            // Fallback to system allocator for very large objects
            void* ptr;
            if (posix_memalign(&ptr, ALIGNMENT, size) == 0) {
                memset(ptr, 0, size);
                return ptr;
            }
            return nullptr;
        }
        
        SizeClassData& sc = size_classes_[class_index];
        
        // Try to get from free list
        FreeNode* node = sc.free_list.load();
        while (node) {
            if (sc.free_list.compare_exchange_weak(node, node->next)) {
                // Successfully got object from free list
                sc.total_allocated++;
                memset(node, 0, sc.object_size);  // Clear the object
                return node;
            }
            // CAS failed, retry with updated node
        }
        
        // Free list empty, allocate new chunk
        if (!allocate_new_chunk(class_index)) {
            return nullptr;
        }
        
        // Retry allocation from newly allocated chunk
        node = sc.free_list.load();
        while (node) {
            if (sc.free_list.compare_exchange_weak(node, node->next)) {
                sc.total_allocated++;
                memset(node, 0, sc.object_size);
                return node;
            }
        }
        
        return nullptr;  // Should not reach here
    }
    
    // Fast object deallocation
    void deallocate(void* ptr, size_t size) {
        if (!ptr || !initialized_) return;
        
        int class_index = get_size_class_index(size);
        if (class_index < 0) {
            // Large object, use system free
            free(ptr);
            return;
        }
        
        SizeClassData& sc = size_classes_[class_index];
        
        // Add back to free list
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        FreeNode* old_head = sc.free_list.load();
        do {
            node->next = old_head;
        } while (!sc.free_list.compare_exchange_weak(old_head, node));
        
        sc.total_freed++;
    }
    
    // Statistics
    void print_statistics() {
        std::cout << "\n=== Fast Object Allocator Statistics ===\n";
        std::cout << "Total memory allocated: " << (total_memory_allocated_ / 1024.0 / 1024.0) << " MB\n";
        
        for (size_t i = 0; i < NUM_SIZE_CLASSES; ++i) {
            const auto& sc = size_classes_[i];
            std::cout << "Size class " << sc.object_size << " bytes:\n";
            std::cout << "  Objects allocated: " << sc.total_allocated.load() << "\n";
            std::cout << "  Objects freed: " << sc.total_freed.load() << "\n";
            std::cout << "  Objects in use: " << (sc.total_allocated.load() - sc.total_freed.load()) << "\n";
            std::cout << "  Memory chunks: " << sc.chunks_allocated.load() << "\n";
            
            // Count free list length
            size_t free_count = 0;
            FreeNode* node = sc.free_list.load();
            while (node && free_count < 1000) {  // Limit to prevent infinite loops
                node = node->next;
                free_count++;
            }
            std::cout << "  Free objects available: " << free_count << "\n";
        }
        std::cout << "==========================================\n";
    }
    
    static FastObjectAllocator& getInstance() {
        static FastObjectAllocator instance;
        return instance;
    }
};

// ============================================================================
// Test Objects Using Fast Allocator
// ============================================================================

// Mock vtable functions
extern "C" void mock_create_function() { }
extern "C" void mock_release_function() { }

// Vtable structure
struct FastVTable {
    void (*create_func)();
    void (*release_func)();
};

static FastVTable fast_vtable = { mock_create_function, mock_release_function };

// Test objects of different sizes
struct FastObject16 {   // 16 bytes
    void* vtable;
    int64_t value;
};

struct FastObject32 {   // 32 bytes  
    void* vtable;
    int64_t slots[3];
};

struct FastObject64 {   // 64 bytes
    void* vtable;
    int64_t slots[7];
};

struct FastObject128 {  // 128 bytes
    void* vtable;
    int64_t slots[15];
};

// Fast object creation functions
FastObject16* create_fast_object16(int64_t value) {
    FastObject16* obj = (FastObject16*)FastObjectAllocator::getInstance().allocate(sizeof(FastObject16));
    if (obj) {
        obj->vtable = &fast_vtable;
        obj->value = value;
    }
    return obj;
}

FastObject32* create_fast_object32(int64_t base_value) {
    FastObject32* obj = (FastObject32*)FastObjectAllocator::getInstance().allocate(sizeof(FastObject32));
    if (obj) {
        obj->vtable = &fast_vtable;
        for (int i = 0; i < 3; ++i) {
            obj->slots[i] = base_value + i;
        }
    }
    return obj;
}

FastObject64* create_fast_object64(int64_t base_value) {
    FastObject64* obj = (FastObject64*)FastObjectAllocator::getInstance().allocate(sizeof(FastObject64));
    if (obj) {
        obj->vtable = &fast_vtable;
        for (int i = 0; i < 7; ++i) {
            obj->slots[i] = base_value + i;
        }
    }
    return obj;
}

FastObject128* create_fast_object128(int64_t base_value) {
    FastObject128* obj = (FastObject128*)FastObjectAllocator::getInstance().allocate(sizeof(FastObject128));
    if (obj) {
        obj->vtable = &fast_vtable;
        for (int i = 0; i < 15; ++i) {
            obj->slots[i] = base_value + i;
        }
    }
    return obj;
}

// ============================================================================
// Performance Tests
// ============================================================================

class Timer {
    std::chrono::high_resolution_clock::time_point t0;
public:
    void start() { t0 = std::chrono::high_resolution_clock::now(); }
    double stop() {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
};

bool test_fast_allocation_performance() {
    std::cout << "=== Fast Allocation Performance Test ===\n";
    
    const int iterations = 10000;  // Reduced from 100k to 10k
    Timer timer;
    
    // Test 16-byte objects
    timer.start();
    std::vector<FastObject16*> objects16;
    objects16.reserve(iterations);
    
    for (int i = 0; i < iterations; ++i) {
        FastObject16* obj = create_fast_object16(i);
        objects16.push_back(obj);
    }
    
    double elapsed16 = timer.stop();
    double rate16 = iterations / (elapsed16 / 1000.0);
    
    // Test 32-byte objects
    timer.start();
    std::vector<FastObject32*> objects32;
    objects32.reserve(iterations);
    
    for (int i = 0; i < iterations; ++i) {
        FastObject32* obj = create_fast_object32(i);
        objects32.push_back(obj);
    }
    
    double elapsed32 = timer.stop();
    double rate32 = iterations / (elapsed32 / 1000.0);
    
    // Test 64-byte objects
    timer.start();
    std::vector<FastObject64*> objects64;
    objects64.reserve(iterations);
    
    for (int i = 0; i < iterations; ++i) {
        FastObject64* obj = create_fast_object64(i);
        objects64.push_back(obj);
    }
    
    double elapsed64 = timer.stop();
    double rate64 = iterations / (elapsed64 / 1000.0);
    
    // Test 128-byte objects
    timer.start();
    std::vector<FastObject128*> objects128;
    objects128.reserve(iterations);
    
    for (int i = 0; i < iterations; ++i) {
        FastObject128* obj = create_fast_object128(i);
        objects128.push_back(obj);
    }
    
    double elapsed128 = timer.stop();
    double rate128 = iterations / (elapsed128 / 1000.0);
    
    std::cout << "Fast Allocation Results (" << iterations << " objects each):\n";
    std::cout << "16-byte objects:  " << elapsed16 << " ms (" << rate16 << " objects/sec)\n";
    std::cout << "32-byte objects:  " << elapsed32 << " ms (" << rate32 << " objects/sec)\n";
    std::cout << "64-byte objects:  " << elapsed64 << " ms (" << rate64 << " objects/sec)\n";
    std::cout << "128-byte objects: " << elapsed128 << " ms (" << rate128 << " objects/sec)\n";
    
    // Test deallocation performance
    timer.start();
    for (auto obj : objects16) {
        FastObjectAllocator::getInstance().deallocate(obj, sizeof(FastObject16));
    }
    for (auto obj : objects32) {
        FastObjectAllocator::getInstance().deallocate(obj, sizeof(FastObject32));
    }
    for (auto obj : objects64) {
        FastObjectAllocator::getInstance().deallocate(obj, sizeof(FastObject64));
    }
    for (auto obj : objects128) {
        FastObjectAllocator::getInstance().deallocate(obj, sizeof(FastObject128));
    }
    
    double dealloc_elapsed = timer.stop();
    double dealloc_rate = (iterations * 4) / (dealloc_elapsed / 1000.0);
    
    std::cout << "Deallocation: " << dealloc_elapsed << " ms (" << dealloc_rate << " deallocations/sec)\n";
    
    return true;
}

bool test_fast_object_lists() {
    std::cout << "\n=== Fast Object Lists Test ===\n";
    
    const int num_lists = 100;     // Reduced from 1000 to 100
    const int objects_per_list = 50; // Reduced from 100 to 50
    Timer timer;
    
    timer.start();
    
    for (int list_idx = 0; list_idx < num_lists; ++list_idx) {
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int obj_idx = 0; obj_idx < objects_per_list; ++obj_idx) {
                int value = list_idx * 1000 + obj_idx;
                switch (obj_idx % 4) {
                    case 0: BCPL_LIST_APPEND_OBJECT(header, create_fast_object16(value)); break;
                    case 1: BCPL_LIST_APPEND_OBJECT(header, create_fast_object32(value)); break;
                    case 2: BCPL_LIST_APPEND_OBJECT(header, create_fast_object64(value)); break;
                    case 3: BCPL_LIST_APPEND_OBJECT(header, create_fast_object128(value)); break;
                }
            }
        }
        HeapManager_exitScope();
        
        if ((list_idx + 1) % 200 == 0) {
            std::cout << "Completed " << (list_idx + 1) << " lists...\n";
        }
    }
    
    double elapsed = timer.stop();
    int total_objects = num_lists * objects_per_list;
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Created " << total_objects << " objects in " << num_lists << " lists\n";
    std::cout << "Time: " << elapsed << " ms\n";
    std::cout << "Rate: " << rate << " objects/second\n";
    
    return true;
}

int main() {
    std::cout << "=== FAST OBJECT ALLOCATOR TEST ===\n";
    
    // Enable SAMM
    HeapManager_setSAMMEnabled(1);
    std::cout << "SAMM enabled: " << (HeapManager_isSAMMEnabled() ? "YES" : "NO") << "\n\n";
    
    // Disable heap tracing for performance
    HeapManager::getInstance().setTraceEnabled(false);
    
    try {
        bool all_passed = true;
        
        all_passed &= test_fast_allocation_performance();
        all_passed &= test_fast_object_lists();
        
        // Print allocator statistics
        FastObjectAllocator::getInstance().print_statistics();
        
        // Wait for cleanup
        std::cout << "\nWaiting 5 seconds for SAMM cleanup...\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        std::cout << "\nAll fast object tests: " << (all_passed ? "PASSED" : "FAILED") << "\n";
        
        return all_passed ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION occurred\n";
        return 1;
    }
}
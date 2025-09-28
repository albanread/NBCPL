// recycling_allocator.cpp
// Simple recycling allocator focused on reusing freed objects for maximum performance
// Uses simple freelists per object size with minimal memory overhead

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <unordered_map>
#include <mutex>
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
// Simple Recycling Allocator
// ============================================================================

class RecyclingAllocator {
private:
    struct FreeNode {
        FreeNode* next;
    };
    
    // Per-size freelists
    std::unordered_map<size_t, FreeNode*> freelists_;
    std::mutex mutex_;
    
    // Statistics
    size_t total_allocations_ = 0;
    size_t total_deallocations_ = 0;
    size_t total_recycled_ = 0;
    size_t total_malloc_calls_ = 0;
    size_t total_free_calls_ = 0;
    
    static constexpr size_t ALIGNMENT = 16;
    
    size_t align_size(size_t size) {
        return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    }
    
public:
    void* allocate(size_t size) {
        size = align_size(size);
        
        std::lock_guard<std::mutex> lock(mutex_);
        total_allocations_++;
        
        // Try to get from freelist first
        auto it = freelists_.find(size);
        if (it != freelists_.end() && it->second != nullptr) {
            FreeNode* node = it->second;
            it->second = node->next;
            total_recycled_++;
            
            // Clear the memory
            memset(node, 0, size);
            return node;
        }
        
        // No recycled object available, allocate new
        void* ptr;
        if (posix_memalign(&ptr, ALIGNMENT, size) == 0) {
            memset(ptr, 0, size);
            total_malloc_calls_++;
            return ptr;
        }
        
        return nullptr;
    }
    
    void deallocate(void* ptr, size_t size) {
        if (!ptr) return;
        
        size = align_size(size);
        
        std::lock_guard<std::mutex> lock(mutex_);
        total_deallocations_++;
        
        // Add to freelist for recycling
        FreeNode* node = reinterpret_cast<FreeNode*>(ptr);
        auto it = freelists_.find(size);
        if (it != freelists_.end()) {
            node->next = it->second;
            it->second = node;
        } else {
            node->next = nullptr;
            freelists_[size] = node;
        }
    }
    
    void print_stats() {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << "\n=== Recycling Allocator Statistics ===\n";
        std::cout << "Total allocations: " << total_allocations_ << "\n";
        std::cout << "Total deallocations: " << total_deallocations_ << "\n";
        std::cout << "Objects recycled: " << total_recycled_ << "\n";
        std::cout << "New malloc calls: " << total_malloc_calls_ << "\n";
        std::cout << "Recycling efficiency: " << (total_recycled_ * 100.0 / total_allocations_) << "%\n";
        
        std::cout << "Freelist sizes:\n";
        for (const auto& pair : freelists_) {
            size_t count = 0;
            FreeNode* node = pair.second;
            while (node && count < 1000) {  // Safety limit
                node = node->next;
                count++;
            }
            std::cout << "  " << pair.first << " bytes: " << count << " objects\n";
        }
        std::cout << "=======================================\n";
    }
    
    static RecyclingAllocator& getInstance() {
        static RecyclingAllocator instance;
        return instance;
    }
};

// ============================================================================
// Test Objects
// ============================================================================

// Mock vtable functions
extern "C" void mock_create_function() { }
extern "C" void mock_release_function() { }

struct SimpleVTable {
    void (*create_func)();
    void (*release_func)();
};

static SimpleVTable simple_vtable = { mock_create_function, mock_release_function };

// Test objects of different sizes
struct RecycleObject16 {   // 16 bytes
    void* vtable;
    int64_t value;
};

struct RecycleObject32 {   // 32 bytes  
    void* vtable;
    int64_t slots[3];
};

struct RecycleObject64 {   // 64 bytes
    void* vtable;
    int64_t slots[7];
};

struct RecycleObject128 {  // 128 bytes
    void* vtable;
    int64_t slots[15];
};

// Object creation functions using recycling allocator
RecycleObject16* create_recycled_object16(int64_t value) {
    RecycleObject16* obj = (RecycleObject16*)RecyclingAllocator::getInstance().allocate(sizeof(RecycleObject16));
    if (obj) {
        obj->vtable = &simple_vtable;
        obj->value = value;
    }
    return obj;
}

RecycleObject32* create_recycled_object32(int64_t base_value) {
    RecycleObject32* obj = (RecycleObject32*)RecyclingAllocator::getInstance().allocate(sizeof(RecycleObject32));
    if (obj) {
        obj->vtable = &simple_vtable;
        for (int i = 0; i < 3; ++i) {
            obj->slots[i] = base_value + i;
        }
    }
    return obj;
}

RecycleObject64* create_recycled_object64(int64_t base_value) {
    RecycleObject64* obj = (RecycleObject64*)RecyclingAllocator::getInstance().allocate(sizeof(RecycleObject64));
    if (obj) {
        obj->vtable = &simple_vtable;
        for (int i = 0; i < 7; ++i) {
            obj->slots[i] = base_value + i;
        }
    }
    return obj;
}

RecycleObject128* create_recycled_object128(int64_t base_value) {
    RecycleObject128* obj = (RecycleObject128*)RecyclingAllocator::getInstance().allocate(sizeof(RecycleObject128));
    if (obj) {
        obj->vtable = &simple_vtable;
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

bool test_recycling_performance() {
    std::cout << "=== Recycling Performance Test ===\n";
    
    const int iterations = 50000;  // Reasonable test size
    Timer timer;
    
    std::cout << "Testing allocation/deallocation cycles for maximum recycling...\n";
    
    // Test 1: Create and immediately free objects (100% recycling after first round)
    timer.start();
    for (int round = 0; round < 10; ++round) {
        std::vector<RecycleObject32*> objects;
        objects.reserve(iterations);
        
        // Allocate
        for (int i = 0; i < iterations; ++i) {
            RecycleObject32* obj = create_recycled_object32(i);
            objects.push_back(obj);
        }
        
        // Deallocate (feed recycling)
        for (auto obj : objects) {
            RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject32));
        }
        
        if ((round + 1) % 3 == 0) {
            std::cout << "Round " << (round + 1) << " completed...\n";
        }
    }
    double elapsed = timer.stop();
    
    int total_objects = iterations * 10;
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Recycling test: " << total_objects << " objects in " << elapsed << " ms\n";
    std::cout << "Rate: " << rate << " objects/second\n";
    
    return true;
}

bool test_mixed_size_recycling() {
    std::cout << "\n=== Mixed Size Recycling Test ===\n";
    
    const int objects_per_size = 10000;
    Timer timer;
    
    timer.start();
    
    // Create objects of all sizes
    std::vector<RecycleObject16*> objects16;
    std::vector<RecycleObject32*> objects32;
    std::vector<RecycleObject64*> objects64;
    std::vector<RecycleObject128*> objects128;
    
    objects16.reserve(objects_per_size);
    objects32.reserve(objects_per_size);
    objects64.reserve(objects_per_size);
    objects128.reserve(objects_per_size);
    
    // First allocation round (no recycling possible)
    for (int i = 0; i < objects_per_size; ++i) {
        objects16.push_back(create_recycled_object16(i));
        objects32.push_back(create_recycled_object32(i));
        objects64.push_back(create_recycled_object64(i));
        objects128.push_back(create_recycled_object128(i));
    }
    
    // Free all objects (populate freelists)
    for (auto obj : objects16) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject16));
    }
    for (auto obj : objects32) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject32));
    }
    for (auto obj : objects64) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject64));
    }
    for (auto obj : objects128) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject128));
    }
    
    // Second allocation round (should be 100% recycled)
    objects16.clear();
    objects32.clear();
    objects64.clear();
    objects128.clear();
    
    for (int i = 0; i < objects_per_size; ++i) {
        objects16.push_back(create_recycled_object16(i + 10000));
        objects32.push_back(create_recycled_object32(i + 10000));
        objects64.push_back(create_recycled_object64(i + 10000));
        objects128.push_back(create_recycled_object128(i + 10000));
    }
    
    double elapsed = timer.stop();
    int total_objects = objects_per_size * 4 * 2;  // 4 sizes × 2 rounds
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Mixed size test: " << total_objects << " objects in " << elapsed << " ms\n";
    std::cout << "Rate: " << rate << " objects/second\n";
    
    // Clean up
    for (auto obj : objects16) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject16));
    }
    for (auto obj : objects32) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject32));
    }
    for (auto obj : objects64) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject64));
    }
    for (auto obj : objects128) {
        RecyclingAllocator::getInstance().deallocate(obj, sizeof(RecycleObject128));
    }
    
    return true;
}

bool test_recycled_object_lists() {
    std::cout << "\n=== Recycled Object Lists Test ===\n";
    
    const int num_lists = 500;
    const int objects_per_list = 100;
    Timer timer;
    
    timer.start();
    
    for (int list_idx = 0; list_idx < num_lists; ++list_idx) {
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int obj_idx = 0; obj_idx < objects_per_list; ++obj_idx) {
                int value = list_idx * 1000 + obj_idx;
                switch (obj_idx % 4) {
                    case 0: BCPL_LIST_APPEND_OBJECT(header, create_recycled_object16(value)); break;
                    case 1: BCPL_LIST_APPEND_OBJECT(header, create_recycled_object32(value)); break;
                    case 2: BCPL_LIST_APPEND_OBJECT(header, create_recycled_object64(value)); break;
                    case 3: BCPL_LIST_APPEND_OBJECT(header, create_recycled_object128(value)); break;
                }
            }
        }
        HeapManager_exitScope();
        
        if ((list_idx + 1) % 100 == 0) {
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
    std::cout << "=== RECYCLING ALLOCATOR TEST ===\n";
    
    // Enable SAMM
    HeapManager_setSAMMEnabled(1);
    std::cout << "SAMM enabled: " << (HeapManager_isSAMMEnabled() ? "YES" : "NO") << "\n\n";
    
    // Disable heap tracing for performance
    HeapManager::getInstance().setTraceEnabled(false);
    
    try {
        bool all_passed = true;
        
        all_passed &= test_recycling_performance();
        all_passed &= test_mixed_size_recycling();
        all_passed &= test_recycled_object_lists();
        
        // Print allocator statistics
        RecyclingAllocator::getInstance().print_stats();
        
        // Wait for SAMM cleanup
        std::cout << "\nWaiting 3 seconds for SAMM cleanup...\n";
        std::this_thread::sleep_for(std::chrono::seconds(3));
        
        std::cout << "\nAll recycling tests: " << (all_passed ? "PASSED" : "FAILED") << "\n";
        
        return all_passed ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION occurred\n";
        return 1;
    }
}
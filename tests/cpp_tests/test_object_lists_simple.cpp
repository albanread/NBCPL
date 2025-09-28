// test_object_lists_simple.cpp
// Simplified object list test to verify the full pattern works without overwhelming the system

#include <iostream>
#include <chrono>
#include <thread>
#include "../../HeapManager/HeapManager.h"
#include "../../runtime/ListDataTypes.h"

// Global trace variables required by the runtime
bool g_enable_heap_trace = false;
bool g_enable_lexer_trace = false;
bool g_enable_symbols_trace = false;

extern "C" {
    void BCPL_LIST_APPEND_OBJECT(ListHeader* header, void* object_ptr);
    void* Heap_allocObject(size_t size);
    ListHeader* BCPL_LIST_CREATE_EMPTY(void);
    void HeapManager_enterScope(void);
    void HeapManager_exitScope(void);
    void HeapManager_setSAMMEnabled(int enabled);
    int HeapManager_isSAMMEnabled(void);
}

// Mock function implementations matching BCPL calling convention
extern "C" void mock_create_function() {
    // Safe no-op constructor
}

extern "C" void mock_release_function() {
    // Safe no-op destructor - HeapManager handles object memory
}

// Vtable structures for different object types
struct VTable3 {
    void (*create_func)();
    void (*release_func)();
};

struct VTable5 {
    void (*create_func)();
    void (*release_func)();
};

// Global vtable instances
static VTable3 vtable_3slots = { mock_create_function, mock_release_function };
static VTable5 vtable_5slots = { mock_create_function, mock_release_function };

// Object structures with different slot counts
struct TestObject3 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    // Total: 32 bytes
};

struct TestObject5 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    int64_t slot4;     // 8 bytes
    int64_t slot5;     // 8 bytes
    // Total: 48 bytes
};

// Object creation functions
TestObject3* create_test_object3(int64_t base_value) {
    TestObject3* obj = (TestObject3*)Heap_allocObject(sizeof(TestObject3));
    if (obj) {
        obj->vtable = &vtable_3slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
    }
    return obj;
}

TestObject5* create_test_object5(int64_t base_value) {
    TestObject5* obj = (TestObject5*)Heap_allocObject(sizeof(TestObject5));
    if (obj) {
        obj->vtable = &vtable_5slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
        obj->slot4 = base_value + 3;
        obj->slot5 = base_value + 4;
    }
    return obj;
}

// Helper for timing
class Timer {
    std::chrono::high_resolution_clock::time_point t0;
public:
    void start() { t0 = std::chrono::high_resolution_clock::now(); }
    double stop() {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
};

int main() {
    std::cout << "=== Simple Object List Test ===\n";
    
    // Enable SAMM for automatic memory management
    HeapManager_setSAMMEnabled(1);
    std::cout << "SAMM enabled: " << (HeapManager_isSAMMEnabled() ? "YES" : "NO") << "\n";
    
    // Disable HeapManager tracing to reduce noise
    HeapManager::getInstance().setTraceEnabled(false);
    
    // Capture initial memory stats
    auto initial_stats = HeapManager::getInstance().getSAMMStats();
    size_t initial_bytes_allocated = HeapManager::getInstance().getTotalBytesAllocated();
    size_t initial_bytes_freed = HeapManager::getInstance().getTotalBytesFreed();
    
    std::cout << "=== Initial Memory Stats ===\n";
    std::cout << "Objects cleaned (SAMM): " << initial_stats.objects_cleaned << "\n";
    std::cout << "Bytes allocated: " << initial_bytes_allocated << "\n";
    std::cout << "Bytes freed: " << initial_bytes_freed << "\n\n";
    
    try {
        std::cout << "Test 1: Single object list creation...\n";
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            std::cout << "Created empty list\n";
            
            // Create a few objects and add them to the list
            for (int i = 0; i < 5; ++i) {
                if (i % 2 == 0) {
                    TestObject3* obj = create_test_object3(i * 10);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    std::cout << "Added Object3 #" << i << " (value " << (i * 10) << ")\n";
                } else {
                    TestObject5* obj = create_test_object5(i * 10);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    std::cout << "Added Object5 #" << i << " (value " << (i * 10) << ")\n";
                }
            }
            
            std::cout << "Final list length: " << header->length << "\n";
        }
        HeapManager_exitScope();
        std::cout << "Test 1 PASSED: Single object list creation\n\n";
        
        std::cout << "Test 2: Multiple object lists...\n";
        Timer timer;
        timer.start();
        
        const int num_lists = 10;
        const int objects_per_list = 5;
        int total_objects = 0;
        
        for (int list_idx = 0; list_idx < num_lists; ++list_idx) {
            HeapManager_enterScope();
            {
                ListHeader* header = BCPL_LIST_CREATE_EMPTY();
                
                for (int obj_idx = 0; obj_idx < objects_per_list; ++obj_idx) {
                    int value = list_idx * 100 + obj_idx;
                    if (obj_idx % 2 == 0) {
                        TestObject3* obj = create_test_object3(value);
                        BCPL_LIST_APPEND_OBJECT(header, obj);
                    } else {
                        TestObject5* obj = create_test_object5(value);
                        BCPL_LIST_APPEND_OBJECT(header, obj);
                    }
                    total_objects++;
                }
            }
            HeapManager_exitScope();
            
            if ((list_idx + 1) % 5 == 0) {
                std::cout << "Completed " << (list_idx + 1) << " lists...\n";
            }
        }
        
        double elapsed = timer.stop();
        std::cout << "Test 2 PASSED: Created " << num_lists << " lists with " << total_objects << " total objects in " << elapsed << " ms\n\n";
        
        std::cout << "Test 3: Object append performance...\n";
        timer.start();
        
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            const int num_appends = 100;
            
            for (int i = 0; i < num_appends; ++i) {
                if (i % 2 == 0) {
                    TestObject3* obj = create_test_object3(i);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                } else {
                    TestObject5* obj = create_test_object5(i);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                }
            }
            
            std::cout << "Final list length: " << header->length << "\n";
        }
        HeapManager_exitScope();
        
        elapsed = timer.stop();
        double rate = 100 / (elapsed / 1000.0);
        std::cout << "Test 3 PASSED: " << 100 << " object appends in " << elapsed << " ms (" << rate << " appends/sec)\n\n";
        
        std::cout << "Test 4: Mixed object sizes stress test...\n";
        timer.start();
        
        const int stress_lists = 20;
        const int stress_objects_per_list = 10;
        total_objects = 0;
        
        for (int list_idx = 0; list_idx < stress_lists; ++list_idx) {
            HeapManager_enterScope();
            {
                ListHeader* header = BCPL_LIST_CREATE_EMPTY();
                
                for (int obj_idx = 0; obj_idx < stress_objects_per_list; ++obj_idx) {
                    int value = list_idx * 1000 + obj_idx;
                    // Alternate between object types
                    switch (obj_idx % 4) {
                        case 0:
                        case 2: {
                            TestObject3* obj = create_test_object3(value);
                            BCPL_LIST_APPEND_OBJECT(header, obj);
                            break;
                        }
                        case 1:
                        case 3: {
                            TestObject5* obj = create_test_object5(value);
                            BCPL_LIST_APPEND_OBJECT(header, obj);
                            break;
                        }
                    }
                    total_objects++;
                }
            }
            HeapManager_exitScope();
        }
        
        elapsed = timer.stop();
        std::cout << "Test 4 PASSED: Created " << stress_lists << " lists with " << total_objects << " total objects in " << elapsed << " ms\n\n";
        
        // Capture final memory stats
        auto final_stats = HeapManager::getInstance().getSAMMStats();
        size_t final_bytes_allocated = HeapManager::getInstance().getTotalBytesAllocated();
        size_t final_bytes_freed = HeapManager::getInstance().getTotalBytesFreed();
        
        // Calculate differences
        size_t objects_cleaned_during_test = final_stats.objects_cleaned - initial_stats.objects_cleaned;
        size_t bytes_allocated_during_test = final_bytes_allocated - initial_bytes_allocated;
        size_t bytes_freed_during_test = final_bytes_freed - initial_bytes_freed;
        size_t net_bytes_leaked = (final_bytes_allocated > final_bytes_freed) ? 
                                 (final_bytes_allocated - final_bytes_freed) : 0;
        
        // Final heap statistics
        std::cout << "=== Final Memory Stats ===\n";
        std::cout << "Objects cleaned by SAMM: " << objects_cleaned_during_test << "\n";
        std::cout << "Bytes allocated during test: " << bytes_allocated_during_test << "\n";
        std::cout << "Bytes freed during test: " << bytes_freed_during_test << "\n";
        std::cout << "Net bytes leaked: " << net_bytes_leaked << "\n";
        if (bytes_allocated_during_test > 0) {
            std::cout << "Memory cleanup efficiency: " << 
                         (bytes_freed_during_test * 100.0 / bytes_allocated_during_test) << "%\n";
        }
        std::cout << "\n";
        
        // Wait for SAMM cleanup thread to complete
        std::cout << "Waiting 5 seconds for SAMM cleanup thread to finish...\n";
        std::this_thread::sleep_for(std::chrono::seconds(5));
        
        // Check memory stats again after cleanup wait
        auto post_cleanup_stats = HeapManager::getInstance().getSAMMStats();
        size_t post_cleanup_bytes_freed = HeapManager::getInstance().getTotalBytesFreed();
        size_t objects_cleaned_after_wait = post_cleanup_stats.objects_cleaned - initial_stats.objects_cleaned;
        size_t bytes_freed_after_wait = post_cleanup_bytes_freed - initial_bytes_freed;
        
        std::cout << "=== Post-Cleanup Memory Stats ===\n";
        std::cout << "Objects cleaned by SAMM (after wait): " << objects_cleaned_after_wait << "\n";
        std::cout << "Bytes freed (after wait): " << bytes_freed_after_wait << "\n";
        if (bytes_allocated_during_test > 0) {
            std::cout << "Final cleanup efficiency: " << 
                         (bytes_freed_after_wait * 100.0 / bytes_allocated_during_test) << "%\n";
        }
        std::cout << "\n";
        
        std::cout << "=== Test Summary ===\n";
        std::cout << "All object list tests completed successfully!\n";
        std::cout << "Total objects tested: " << (5 + total_objects + 100) << "\n";
        std::cout << "Object sizes tested: 32 bytes (3-slot) and 48 bytes (5-slot)\n";
        std::cout << "Memory management: SAMM automatic cleanup\n";
        
    } catch (const std::exception& e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION occurred\n";
        return 1;
    }
    
    std::cout << "\n=== ALL TESTS PASSED ===\n";
    return 0;
}
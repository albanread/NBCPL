// aggressive_object_stress.cpp
// Aggressive stress test for BCPL object lists with SAMM memory management
// Tests high volume, complex patterns, memory pressure, and cleanup efficiency

#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <random>
#include <algorithm>
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

// Mock function implementations
extern "C" void mock_create_function() { /* no-op */ }
extern "C" void mock_release_function() { /* no-op */ }

// Vtable structures for different object sizes
struct VTable3 { void (*create_func)(); void (*release_func)(); };
struct VTable5 { void (*create_func)(); void (*release_func)(); };
struct VTable7 { void (*create_func)(); void (*release_func)(); };
struct VTable9 { void (*create_func)(); void (*release_func)(); };

// Global vtables
static VTable3 vtable_3slots = { mock_create_function, mock_release_function };
static VTable5 vtable_5slots = { mock_create_function, mock_release_function };
static VTable7 vtable_7slots = { mock_create_function, mock_release_function };
static VTable9 vtable_9slots = { mock_create_function, mock_release_function };

// Variable-sized object structures (3-9 integer slots)
struct TestObject3 {
    void* vtable; int64_t slot1, slot2, slot3;
};

struct TestObject5 {
    void* vtable; int64_t slot1, slot2, slot3, slot4, slot5;
};

struct TestObject7 {
    void* vtable; int64_t slot1, slot2, slot3, slot4, slot5, slot6, slot7;
};

struct TestObject9 {
    void* vtable; int64_t slot1, slot2, slot3, slot4, slot5, slot6, slot7, slot8, slot9;
};

// Object creation functions
TestObject3* create_test_object3(int64_t base) {
    TestObject3* obj = (TestObject3*)Heap_allocObject(sizeof(TestObject3));
    if (obj) {
        obj->vtable = &vtable_3slots;
        obj->slot1 = base; obj->slot2 = base + 1; obj->slot3 = base + 2;
    }
    return obj;
}

TestObject5* create_test_object5(int64_t base) {
    TestObject5* obj = (TestObject5*)Heap_allocObject(sizeof(TestObject5));
    if (obj) {
        obj->vtable = &vtable_5slots;
        obj->slot1 = base; obj->slot2 = base + 1; obj->slot3 = base + 2;
        obj->slot4 = base + 3; obj->slot5 = base + 4;
    }
    return obj;
}

TestObject7* create_test_object7(int64_t base) {
    TestObject7* obj = (TestObject7*)Heap_allocObject(sizeof(TestObject7));
    if (obj) {
        obj->vtable = &vtable_7slots;
        obj->slot1 = base; obj->slot2 = base + 1; obj->slot3 = base + 2;
        obj->slot4 = base + 3; obj->slot5 = base + 4; obj->slot6 = base + 5;
        obj->slot7 = base + 6;
    }
    return obj;
}

TestObject9* create_test_object9(int64_t base) {
    TestObject9* obj = (TestObject9*)Heap_allocObject(sizeof(TestObject9));
    if (obj) {
        obj->vtable = &vtable_9slots;
        obj->slot1 = base; obj->slot2 = base + 1; obj->slot3 = base + 2;
        obj->slot4 = base + 3; obj->slot5 = base + 4; obj->slot6 = base + 5;
        obj->slot7 = base + 6; obj->slot8 = base + 7; obj->slot9 = base + 8;
    }
    return obj;
}

// Timer helper
class Timer {
    std::chrono::high_resolution_clock::time_point t0;
public:
    void start() { t0 = std::chrono::high_resolution_clock::now(); }
    double stop() {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
};

// Memory tracking helper
struct MemorySnapshot {
    size_t objects_cleaned;
    size_t bytes_allocated;
    size_t bytes_freed;
    
    MemorySnapshot() {
        auto stats = HeapManager::getInstance().getSAMMStats();
        objects_cleaned = stats.objects_cleaned;
        bytes_allocated = HeapManager::getInstance().getTotalBytesAllocated();
        bytes_freed = HeapManager::getInstance().getTotalBytesFreed();
    }
    
    void print_delta(const MemorySnapshot& before, const std::string& test_name) {
        size_t objects_delta = objects_cleaned - before.objects_cleaned;
        size_t alloc_delta = bytes_allocated - before.bytes_allocated;
        size_t freed_delta = bytes_freed - before.bytes_freed;
        size_t net_leaked = alloc_delta > freed_delta ? (alloc_delta - freed_delta) : 0;
        
        std::cout << "=== " << test_name << " Memory Results ===\n";
        std::cout << "Objects cleaned: " << objects_delta << "\n";
        std::cout << "Bytes allocated: " << alloc_delta << "\n";
        std::cout << "Bytes freed: " << freed_delta << "\n";
        std::cout << "Net leaked: " << net_leaked << "\n";
        if (alloc_delta > 0) {
            std::cout << "Cleanup efficiency: " << (freed_delta * 100.0 / alloc_delta) << "%\n";
        }
        std::cout << "\n";
    }
};

// Test 1: High Volume Object Creation
bool test_high_volume_creation() {
    std::cout << "TEST 1: High Volume Object Creation (100,000 objects)\n";
    MemorySnapshot before;
    Timer timer;
    
    const int total_objects = 100000;
    timer.start();
    
    for (int batch = 0; batch < 100; ++batch) {
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int i = 0; i < 1000; ++i) {
                int value = batch * 1000 + i;
                switch (i % 4) {
                    case 0: BCPL_LIST_APPEND_OBJECT(header, create_test_object3(value)); break;
                    case 1: BCPL_LIST_APPEND_OBJECT(header, create_test_object5(value)); break;
                    case 2: BCPL_LIST_APPEND_OBJECT(header, create_test_object7(value)); break;
                    case 3: BCPL_LIST_APPEND_OBJECT(header, create_test_object9(value)); break;
                }
            }
        }
        HeapManager_exitScope();
        
        if ((batch + 1) % 20 == 0) {
            std::cout << "Completed " << ((batch + 1) * 1000) << " objects...\n";
        }
    }
    
    double elapsed = timer.stop();
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Created " << total_objects << " objects in " << elapsed << " ms\n";
    std::cout << "Creation rate: " << rate << " objects/second\n";
    
    MemorySnapshot after;
    after.print_delta(before, "High Volume Creation");
    
    return true;
}

// Test 2: Memory Pressure Test
bool test_memory_pressure() {
    std::cout << "TEST 2: Memory Pressure Test (Large objects, rapid allocation)\n";
    MemorySnapshot before;
    Timer timer;
    
    const int pressure_cycles = 50;
    const int objects_per_cycle = 2000;
    timer.start();
    
    for (int cycle = 0; cycle < pressure_cycles; ++cycle) {
        HeapManager_enterScope();
        {
            // Create multiple large lists in same scope to build pressure
            std::vector<ListHeader*> headers;
            for (int list_idx = 0; list_idx < 5; ++list_idx) {
                ListHeader* header = BCPL_LIST_CREATE_EMPTY();
                headers.push_back(header);
                
                for (int obj_idx = 0; obj_idx < objects_per_cycle / 5; ++obj_idx) {
                    int value = cycle * 10000 + list_idx * 1000 + obj_idx;
                    // Bias toward larger objects to increase pressure
                    switch (obj_idx % 8) {
                        case 0: case 1: BCPL_LIST_APPEND_OBJECT(header, create_test_object9(value)); break;
                        case 2: case 3: BCPL_LIST_APPEND_OBJECT(header, create_test_object7(value)); break;
                        case 4: case 5: BCPL_LIST_APPEND_OBJECT(header, create_test_object5(value)); break;
                        case 6: case 7: BCPL_LIST_APPEND_OBJECT(header, create_test_object3(value)); break;
                    }
                }
            }
        }
        HeapManager_exitScope();
        
        if ((cycle + 1) % 10 == 0) {
            std::cout << "Pressure cycle " << (cycle + 1) << " completed...\n";
        }
    }
    
    double elapsed = timer.stop();
    int total_objects = pressure_cycles * objects_per_cycle;
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Created " << total_objects << " objects under pressure in " << elapsed << " ms\n";
    std::cout << "Pressure creation rate: " << rate << " objects/second\n";
    
    MemorySnapshot after;
    after.print_delta(before, "Memory Pressure");
    
    return true;
}

// Test 3: Rapid Scope Cycling
bool test_rapid_scope_cycling() {
    std::cout << "TEST 3: Rapid Scope Cycling (10,000 scope cycles)\n";
    MemorySnapshot before;
    Timer timer;
    
    const int scope_cycles = 10000;
    const int objects_per_scope = 20;
    timer.start();
    
    for (int cycle = 0; cycle < scope_cycles; ++cycle) {
        HeapManager_enterScope();
        {
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int i = 0; i < objects_per_scope; ++i) {
                int value = cycle * 100 + i;
                switch (i % 4) {
                    case 0: BCPL_LIST_APPEND_OBJECT(header, create_test_object3(value)); break;
                    case 1: BCPL_LIST_APPEND_OBJECT(header, create_test_object5(value)); break;
                    case 2: BCPL_LIST_APPEND_OBJECT(header, create_test_object7(value)); break; 
                    case 3: BCPL_LIST_APPEND_OBJECT(header, create_test_object9(value)); break;
                }
            }
        }
        HeapManager_exitScope();
        
        if ((cycle + 1) % 1000 == 0) {
            std::cout << "Scope cycle " << (cycle + 1) << " completed...\n";
        }
    }
    
    double elapsed = timer.stop();
    int total_objects = scope_cycles * objects_per_scope;
    double cycle_rate = scope_cycles / (elapsed / 1000.0);
    
    std::cout << "Completed " << scope_cycles << " scope cycles in " << elapsed << " ms\n";
    std::cout << "Scope cycle rate: " << cycle_rate << " cycles/second\n";
    std::cout << "Total objects: " << total_objects << "\n";
    
    MemorySnapshot after;
    after.print_delta(before, "Rapid Scope Cycling");
    
    return true;
}

// Test 4: Mixed Size Stress Test
bool test_mixed_size_stress() {
    std::cout << "TEST 4: Mixed Size Stress Test (Random object sizes)\n";
    MemorySnapshot before;
    Timer timer;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> size_dist(0, 3);
    std::uniform_int_distribution<> list_size_dist(10, 100);
    
    const int stress_rounds = 1000;
    int total_objects = 0;
    timer.start();
    
    for (int round = 0; round < stress_rounds; ++round) {
        HeapManager_enterScope();
        {
            int objects_in_list = list_size_dist(gen);
            ListHeader* header = BCPL_LIST_CREATE_EMPTY();
            
            for (int i = 0; i < objects_in_list; ++i) {
                int value = round * 1000 + i;
                int size_choice = size_dist(gen);
                
                switch (size_choice) {
                    case 0: BCPL_LIST_APPEND_OBJECT(header, create_test_object3(value)); break;
                    case 1: BCPL_LIST_APPEND_OBJECT(header, create_test_object5(value)); break;
                    case 2: BCPL_LIST_APPEND_OBJECT(header, create_test_object7(value)); break;
                    case 3: BCPL_LIST_APPEND_OBJECT(header, create_test_object9(value)); break;
                }
                total_objects++;
            }
        }
        HeapManager_exitScope();
        
        if ((round + 1) % 200 == 0) {
            std::cout << "Mixed stress round " << (round + 1) << " completed...\n";
        }
    }
    
    double elapsed = timer.stop();
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Created " << total_objects << " mixed-size objects in " << elapsed << " ms\n";
    std::cout << "Mixed creation rate: " << rate << " objects/second\n";
    
    MemorySnapshot after;
    after.print_delta(before, "Mixed Size Stress");
    
    return true;
}

// Test 5: Nested Scope Stress
bool test_nested_scope_stress() {
    std::cout << "TEST 5: Nested Scope Stress Test (Deep nesting)\n";
    MemorySnapshot before;
    Timer timer;
    
    const int depth_levels = 5;
    const int objects_per_level = 100;
    const int iterations = 100;
    int total_objects = 0;
    
    timer.start();
    
    for (int iter = 0; iter < iterations; ++iter) {
        // Level 1
        HeapManager_enterScope();
        {
            ListHeader* header1 = BCPL_LIST_CREATE_EMPTY();
            for (int i = 0; i < objects_per_level; ++i) {
                BCPL_LIST_APPEND_OBJECT(header1, create_test_object3(iter * 1000 + i));
                total_objects++;
            }
            
            // Level 2
            HeapManager_enterScope();
            {
                ListHeader* header2 = BCPL_LIST_CREATE_EMPTY();
                for (int i = 0; i < objects_per_level; ++i) {
                    BCPL_LIST_APPEND_OBJECT(header2, create_test_object5(iter * 1000 + 100 + i));
                    total_objects++;
                }
                
                // Level 3
                HeapManager_enterScope();
                {
                    ListHeader* header3 = BCPL_LIST_CREATE_EMPTY();
                    for (int i = 0; i < objects_per_level; ++i) {
                        BCPL_LIST_APPEND_OBJECT(header3, create_test_object7(iter * 1000 + 200 + i));
                        total_objects++;
                    }
                    
                    // Level 4
                    HeapManager_enterScope();
                    {
                        ListHeader* header4 = BCPL_LIST_CREATE_EMPTY();
                        for (int i = 0; i < objects_per_level; ++i) {
                            BCPL_LIST_APPEND_OBJECT(header4, create_test_object9(iter * 1000 + 300 + i));
                            total_objects++;
                        }
                        
                        // Level 5
                        HeapManager_enterScope();
                        {
                            ListHeader* header5 = BCPL_LIST_CREATE_EMPTY();
                            for (int i = 0; i < objects_per_level; ++i) {
                                BCPL_LIST_APPEND_OBJECT(header5, create_test_object3(iter * 1000 + 400 + i));
                                total_objects++;
                            }
                        }
                        HeapManager_exitScope(); // Level 5
                    }
                    HeapManager_exitScope(); // Level 4
                }
                HeapManager_exitScope(); // Level 3
            }
            HeapManager_exitScope(); // Level 2
        }
        HeapManager_exitScope(); // Level 1
        
        if ((iter + 1) % 20 == 0) {
            std::cout << "Nested iteration " << (iter + 1) << " completed...\n";
        }
    }
    
    double elapsed = timer.stop();
    double rate = total_objects / (elapsed / 1000.0);
    
    std::cout << "Created " << total_objects << " objects in nested scopes in " << elapsed << " ms\n";
    std::cout << "Nested creation rate: " << rate << " objects/second\n";
    
    MemorySnapshot after;
    after.print_delta(before, "Nested Scope Stress");
    
    return true;
}

int main() {
    std::cout << "=== AGGRESSIVE OBJECT LIST STRESS TEST ===\n\n";
    
    // Enable SAMM
    HeapManager_setSAMMEnabled(1);
    std::cout << "SAMM enabled: " << (HeapManager_isSAMMEnabled() ? "YES" : "NO") << "\n";
    
    // Disable tracing for performance
    HeapManager::getInstance().setTraceEnabled(false);
    
    // Initial memory state
    MemorySnapshot initial;
    std::cout << "Initial memory state captured\n\n";
    
    Timer total_timer;
    total_timer.start();
    
    try {
        bool all_passed = true;
        
        all_passed &= test_high_volume_creation();
        all_passed &= test_memory_pressure();
        all_passed &= test_rapid_scope_cycling();
        all_passed &= test_mixed_size_stress();
        all_passed &= test_nested_scope_stress();
        
        double total_elapsed = total_timer.stop();
        
        // Wait for cleanup thread
        std::cout << "Waiting 10 seconds for final SAMM cleanup...\n";
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
        // Final memory analysis
        MemorySnapshot final;
        std::cout << "\n=== FINAL AGGRESSIVE STRESS TEST RESULTS ===\n";
        std::cout << "Total test time: " << total_elapsed << " ms\n";
        final.print_delta(initial, "OVERALL STRESS TEST");
        
        // HeapManager final metrics
        HeapManager::getInstance().printMetrics();
        
        std::cout << "All aggressive stress tests: " << (all_passed ? "PASSED" : "FAILED") << "\n";
        return all_passed ? 0 : 1;
        
    } catch (const std::exception& e) {
        std::cout << "EXCEPTION: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cout << "UNKNOWN EXCEPTION occurred\n";
        return 1;
    }
}
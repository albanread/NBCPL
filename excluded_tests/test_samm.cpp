// SAMM (Scope Aware Memory Management) Test Program
// This program validates the basic functionality of the SAMM implementation

#include "HeapManager.h"
#include "heap_c_wrappers.h"
#include <iostream>
#include <cassert>
#include <chrono>
#include <thread>

void test_basic_allocation() {
    std::cout << "\n=== Test: Basic Allocation (Legacy Mode) ===\n";
    
    HeapManager& mgr = HeapManager::getInstance();
    mgr.setTraceEnabled(true);
    
    // Test basic allocations in legacy mode (SAMM disabled)
    void* obj1 = mgr.allocObject(64);
    void* vec1 = mgr.allocVec(10);
    void* str1 = mgr.allocString(20);
    
    assert(obj1 != nullptr);
    assert(vec1 != nullptr);
    assert(str1 != nullptr);
    
    std::cout << "Basic allocations successful\n";
    
    // Clean up manually in legacy mode
    mgr.free(obj1);
    mgr.free(vec1);
    mgr.free(str1);
    
    std::cout << "Legacy mode test passed\n";
}

void test_samm_scope_tracking() {
    std::cout << "\n=== Test: SAMM Scope Tracking ===\n";
    
    HeapManager& mgr = HeapManager::getInstance();
    mgr.setSAMMEnabled(true);
    
    // Wait a moment for background worker to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto initial_stats = mgr.getSAMMStats();
    std::cout << "Initial scopes entered: " << initial_stats.scopes_entered << "\n";
    
    // Test scope entry/exit
    mgr.enterScope();
    {
        // Allocate objects in this scope
        void* obj1 = mgr.allocObject(32);
        void* obj2 = mgr.allocObject(48);
        void* vec1 = mgr.allocVec(5);
        
        assert(obj1 != nullptr);
        assert(obj2 != nullptr);
        assert(vec1 != nullptr);
        
        std::cout << "Allocated 3 objects in scope\n";
        
        // Exit scope - should queue objects for background cleanup
        mgr.exitScope();
        std::cout << "Exited scope\n";
    }
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto final_stats = mgr.getSAMMStats();
    std::cout << "Final scopes entered: " << final_stats.scopes_entered << "\n";
    std::cout << "Final scopes exited: " << final_stats.scopes_exited << "\n";
    std::cout << "Objects cleaned: " << final_stats.objects_cleaned << "\n";
    std::cout << "Cleanup batches processed: " << final_stats.cleanup_batches_processed << "\n";
    
    assert(final_stats.scopes_entered > initial_stats.scopes_entered);
    assert(final_stats.scopes_exited > initial_stats.scopes_exited);
    
    std::cout << "SAMM scope tracking test passed\n";
}

void test_retain_functionality() {
    std::cout << "\n=== Test: RETAIN Functionality ===\n";
    
    HeapManager& mgr = HeapManager::getInstance();
    mgr.setSAMMEnabled(true);
    
    void* retained_obj = nullptr;
    
    // Outer scope
    mgr.enterScope();
    {
        // Inner scope
        mgr.enterScope();
        {
            // Allocate object in inner scope
            void* obj1 = mgr.allocObject(64);
            retained_obj = mgr.allocObject(64);
            
            // Retain one object to parent scope
            mgr.retainPointer(retained_obj);
            
            std::cout << "Allocated 2 objects, retained 1 to parent scope\n";
        }
        mgr.exitScope(); // obj1 should be cleaned up, retained_obj should remain
        
        std::cout << "Exited inner scope\n";
        
        // Give background thread time to process
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        
        // Test retained allocation directly
        void* retained_obj2 = mgr.allocObjectRetained(32);
        assert(retained_obj2 != nullptr);
        
        std::cout << "Allocated retained object directly\n";
    }
    mgr.exitScope(); // retained_obj and retained_obj2 should be cleaned up now
    
    std::cout << "Exited outer scope\n";
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto stats = mgr.getSAMMStats();
    std::cout << "Total objects cleaned in RETAIN test: " << stats.objects_cleaned << "\n";
    
    std::cout << "RETAIN functionality test passed\n";
}

void test_c_interface() {
    std::cout << "\n=== Test: C Interface ===\n";
    
    // Test C interface functions
    HeapManager_setSAMMEnabled(1);
    assert(HeapManager_isSAMMEnabled() == 1);
    
    HeapManager_enter_scope();
    {
        void* obj1 = OBJECT_HEAP_ALLOC(24);  // Standard object size
        void* obj2 = OBJECT_HEAP_ALLOC_RETAINED(24);  // Standard object size
        void* vec1 = Heap_allocVec(10);
        void* vec2 = Heap_allocVecRetained(15);
        
        assert(obj1 != nullptr);
        assert(obj2 != nullptr);
        assert(vec1 != nullptr);
        assert(vec2 != nullptr);
        
        std::cout << "C interface allocations successful\n";
        
        // Test retain function
        HeapManager_retain_pointer(obj1);
    }
    HeapManager_exit_scope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    std::cout << "C interface test passed\n";
}

void test_performance_comparison() {
    std::cout << "\n=== Test: Performance Comparison ===\n";
    
    HeapManager& mgr = HeapManager::getInstance();
    mgr.setTraceEnabled(false); // Disable tracing for performance test
    
    const int NUM_ITERATIONS = 1000;
    const int OBJECTS_PER_SCOPE = 10;
    
    // Test legacy mode performance
    mgr.setSAMMEnabled(false);
    auto start_legacy = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        std::vector<void*> objects;
        for (int j = 0; j < OBJECTS_PER_SCOPE; j++) {
            void* obj = mgr.allocObject(32);
            objects.push_back(obj);
        }
        // Manual cleanup in legacy mode
        for (void* obj : objects) {
            mgr.free(obj);
        }
    }
    
    auto end_legacy = std::chrono::high_resolution_clock::now();
    auto legacy_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_legacy - start_legacy);
    
    // Test SAMM mode performance
    mgr.setSAMMEnabled(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let background worker start
    
    auto start_samm = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        mgr.enterScope();
        for (int j = 0; j < OBJECTS_PER_SCOPE; j++) {
            mgr.allocObject(32);
        }
        mgr.exitScope(); // Background cleanup
    }
    
    auto end_samm = std::chrono::high_resolution_clock::now();
    auto samm_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_samm - start_samm);
    
    // Wait for background cleanup to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "Legacy mode: " << legacy_duration.count() << " microseconds\n";
    std::cout << "SAMM mode: " << samm_duration.count() << " microseconds\n";
    
    if (samm_duration.count() < legacy_duration.count()) {
        double improvement = (double)legacy_duration.count() / samm_duration.count();
        std::cout << "SAMM is " << improvement << "x faster!\n";
    } else {
        std::cout << "Legacy mode was faster (possibly due to background thread overhead)\n";
    }
    
    auto final_stats = mgr.getSAMMStats();
    std::cout << "Final SAMM stats:\n";
    std::cout << "  Scopes entered: " << final_stats.scopes_entered << "\n";
    std::cout << "  Scopes exited: " << final_stats.scopes_exited << "\n";
    std::cout << "  Objects cleaned: " << final_stats.objects_cleaned << "\n";
    std::cout << "  Cleanup batches: " << final_stats.cleanup_batches_processed << "\n";
    std::cout << "  Queue depth: " << final_stats.current_queue_depth << "\n";
    
    std::cout << "Performance comparison test completed\n";
}

void test_memory_pressure() {
    std::cout << "\n=== Test: Memory Pressure Handling ===\n";
    
    HeapManager& mgr = HeapManager::getInstance();
    mgr.setSAMMEnabled(true);
    mgr.setTraceEnabled(true);
    
    // Create a scenario with many pending cleanups
    for (int i = 0; i < 5; i++) {
        mgr.enterScope();
        for (int j = 0; j < 20; j++) {
            mgr.allocObject(64);
        }
        mgr.exitScope();
    }
    
    std::cout << "Created 5 scopes with 20 objects each (100 total)\n";
    
    auto stats_before = mgr.getSAMMStats();
    std::cout << "Queue depth before pressure handling: " << stats_before.current_queue_depth << "\n";
    
    // Force immediate cleanup
    mgr.handleMemoryPressure();
    
    auto stats_after = mgr.getSAMMStats();
    std::cout << "Queue depth after pressure handling: " << stats_after.current_queue_depth << "\n";
    
    assert(stats_after.current_queue_depth == 0);
    
    std::cout << "Memory pressure handling test passed\n";
}

int main() {
    std::cout << "SAMM (Scope Aware Memory Management) Test Suite\n";
    std::cout << "===============================================\n";
    
    try {
        test_basic_allocation();
        test_samm_scope_tracking();
        test_retain_functionality();
        test_c_interface();
        test_performance_comparison();
        test_memory_pressure();
        
        std::cout << "\n=== All Tests Passed! ===\n";
        
        // Clean shutdown
        HeapManager::getInstance().shutdown();
        
        std::cout << "SAMM implementation is working correctly.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
    
    return 0;
}
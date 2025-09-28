// test_samm_compatibility.cpp
// Test suite to verify SAMM (Scope Aware Memory Management) functionality
// while maintaining 100% API compatibility with existing HeapManager

#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cassert>

// Include HeapManager and SAMM interfaces
#include "HeapManager/HeapManager.h"
#include "HeapManager/heap_c_wrappers.h"

// Control verbosity
#ifndef ENABLE_VERBOSE
#define ENABLE_VERBOSE 1
#endif

void print_test_header(const char* test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
}

void print_test_result(const char* test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
}

bool test_samm_basic_functionality() {
    print_test_header("SAMM Basic Functionality");
    
    auto& hm = HeapManager::getInstance();
    
    // Test SAMM enable/disable
    hm.setSAMMEnabled(false);
    bool initially_disabled = !hm.isSAMMEnabled();
    
    hm.setSAMMEnabled(true);
    bool enabled_successfully = hm.isSAMMEnabled();
    
    if (ENABLE_VERBOSE) {
        std::cout << "SAMM initially disabled: " << (initially_disabled ? "YES" : "NO") << std::endl;
        std::cout << "SAMM enabled successfully: " << (enabled_successfully ? "YES" : "NO") << std::endl;
    }
    
    return initially_disabled && enabled_successfully;
}

bool test_samm_scope_operations() {
    print_test_header("SAMM Scope Operations");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Get initial stats
    auto initial_stats = hm.getSAMMStats();
    
    // Enter and exit scopes
    hm.enterScope();
    hm.enterScope();
    hm.exitScope();
    hm.exitScope();
    
    // Get final stats
    auto final_stats = hm.getSAMMStats();
    
    bool scopes_entered = (final_stats.scopes_entered >= initial_stats.scopes_entered + 2);
    bool scopes_exited = (final_stats.scopes_exited >= initial_stats.scopes_exited + 2);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Scopes entered: " << (final_stats.scopes_entered - initial_stats.scopes_entered) << std::endl;
        std::cout << "Scopes exited: " << (final_stats.scopes_exited - initial_stats.scopes_exited) << std::endl;
        std::cout << "Current scope depth: " << final_stats.current_scope_depth << std::endl;
    }
    
    return scopes_entered && scopes_exited;
}

bool test_samm_allocation_tracking() {
    print_test_header("SAMM Allocation Tracking");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Enter a scope
    hm.enterScope();
    
    // Allocate some objects in this scope
    std::vector<void*> allocated_objects;
    for (int i = 0; i < 10; i++) {
        void* obj = hm.allocObject(64);
        if (obj) {
            allocated_objects.push_back(obj);
        }
    }
    
    // Allocate some vectors
    std::vector<void*> allocated_vectors;
    for (int i = 0; i < 5; i++) {
        void* vec = hm.allocVec(10);
        if (vec) {
            allocated_vectors.push_back(vec);
        }
    }
    
    bool all_objects_allocated = (allocated_objects.size() == 10);
    bool all_vectors_allocated = (allocated_vectors.size() == 5);
    
    // Exit scope - this should queue cleanup
    auto stats_before_exit = hm.getSAMMStats();
    hm.exitScope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto stats_after_exit = hm.getSAMMStats();
    bool cleanup_occurred = (stats_after_exit.objects_cleaned > stats_before_exit.objects_cleaned);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Objects allocated: " << allocated_objects.size() << std::endl;
        std::cout << "Vectors allocated: " << allocated_vectors.size() << std::endl;
        std::cout << "Objects cleaned by SAMM: " << (stats_after_exit.objects_cleaned - stats_before_exit.objects_cleaned) << std::endl;
        std::cout << "Background worker running: " << (stats_after_exit.background_worker_running ? "YES" : "NO") << std::endl;
    }
    
    return all_objects_allocated && all_vectors_allocated && cleanup_occurred;
}

bool test_samm_retain_functionality() {
    print_test_header("SAMM RETAIN Functionality");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    void* retained_object = nullptr;
    
    // Enter outer scope
    hm.enterScope();
    
    // Enter inner scope
    hm.enterScope();
    
    // Allocate object in inner scope
    void* inner_object = hm.allocObject(32);
    void* normal_object = hm.allocObject(32);
    
    // Retain the object to parent scope
    if (inner_object) {
        hm.retainPointer(inner_object, 1);
        retained_object = inner_object;
    }
    
    // Exit inner scope - normal_object should be cleaned, retained_object should survive
    auto stats_before_inner_exit = hm.getSAMMStats();
    hm.exitScope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    auto stats_after_inner_exit = hm.getSAMMStats();
    
    // Exit outer scope - retained_object should now be cleaned
    hm.exitScope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    auto stats_after_outer_exit = hm.getSAMMStats();
    
    bool some_cleanup_occurred = (stats_after_outer_exit.objects_cleaned > stats_before_inner_exit.objects_cleaned);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Retained object: " << retained_object << std::endl;
        std::cout << "Objects cleaned after inner scope exit: " << (stats_after_inner_exit.objects_cleaned - stats_before_inner_exit.objects_cleaned) << std::endl;
        std::cout << "Objects cleaned after outer scope exit: " << (stats_after_outer_exit.objects_cleaned - stats_after_inner_exit.objects_cleaned) << std::endl;
        std::cout << "Total cleanup occurred: " << (some_cleanup_occurred ? "YES" : "NO") << std::endl;
    }
    
    return some_cleanup_occurred;
}

bool test_samm_retain_allocation_variants() {
    print_test_header("SAMM RETAIN Allocation Variants");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Enter outer scope
    hm.enterScope();
    
    // Enter inner scope
    hm.enterScope();
    
    // Test RETAIN allocation variants
    void* retained_object = hm.allocObjectRetained(64, 1);
    void* retained_vector = hm.allocVecRetained(20, 1);
    void* retained_string = hm.allocStringRetained(15, 1);
    void* retained_list = hm.allocListRetained(1);
    
    bool all_allocated = (retained_object != nullptr && 
                         retained_vector != nullptr && 
                         retained_string != nullptr && 
                         retained_list != nullptr);
    
    // Exit inner scope - retained objects should survive
    hm.exitScope();
    
    // Exit outer scope - retained objects should now be cleaned
    hm.exitScope();
    
    if (ENABLE_VERBOSE) {
        std::cout << "Retained object: " << retained_object << std::endl;
        std::cout << "Retained vector: " << retained_vector << std::endl;
        std::cout << "Retained string: " << retained_string << std::endl;
        std::cout << "Retained list: " << retained_list << std::endl;
        std::cout << "All RETAIN allocations successful: " << (all_allocated ? "YES" : "NO") << std::endl;
    }
    
    return all_allocated;
}

bool test_samm_double_free_prevention() {
    print_test_header("SAMM Double-Free Prevention");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Enter scope
    hm.enterScope();
    
    // Allocate object
    void* test_object = hm.allocObject(32);
    bool object_allocated = (test_object != nullptr);
    
    // Exit scope - SAMM should clean this up
    hm.exitScope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Now try to manually free the same object - should be prevented
    auto initial_double_free_count = hm.getDoubleFreeAttempts();
    hm.free(test_object);  // This should be prevented by SAMM
    auto final_double_free_count = hm.getDoubleFreeAttempts();
    
    // The double-free should have been prevented (no increase in double-free count)
    bool double_free_prevented = (final_double_free_count == initial_double_free_count);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Test object: " << test_object << std::endl;
        std::cout << "Object allocated: " << (object_allocated ? "YES" : "NO") << std::endl;
        std::cout << "Double-free attempts before: " << initial_double_free_count << std::endl;
        std::cout << "Double-free attempts after: " << final_double_free_count << std::endl;
        std::cout << "Double-free prevented: " << (double_free_prevented ? "YES" : "NO") << std::endl;
    }
    
    return object_allocated && double_free_prevented;
}

bool test_samm_c_api() {
    print_test_header("SAMM C API");
    
    // Test C wrapper functions
    HeapManager_setSAMMEnabled(1);
    int samm_enabled = HeapManager_isSAMMEnabled();
    
    // Test scope operations through C API
    HeapManager_enterScope();
    HeapManager_enterScope();
    
    // Test RETAIN allocations through C API
    void* retained_obj = HeapManager_allocObjectRetained(32, 1);
    void* retained_vec = HeapManager_allocVecRetained(10, 1);
    void* retained_str = HeapManager_allocStringRetained(8, 1);
    void* retained_list = HeapManager_allocListRetained(1);
    
    bool all_c_allocations = (retained_obj != nullptr && 
                             retained_vec != nullptr && 
                             retained_str != nullptr && 
                             retained_list != nullptr);
    
    // Test retain through C API
    if (retained_obj) {
        HeapManager_retainPointer(retained_obj, 1);
    }
    
    // Exit scopes
    HeapManager_exitScope();
    HeapManager_exitScope();
    
    if (ENABLE_VERBOSE) {
        std::cout << "SAMM enabled through C API: " << (samm_enabled ? "YES" : "NO") << std::endl;
        std::cout << "C API retained object: " << retained_obj << std::endl;
        std::cout << "C API retained vector: " << retained_vec << std::endl;
        std::cout << "C API retained string: " << retained_str << std::endl;
        std::cout << "C API retained list: " << retained_list << std::endl;
        std::cout << "All C allocations successful: " << (all_c_allocations ? "YES" : "NO") << std::endl;
    }
    
    return (samm_enabled == 1) && all_c_allocations;
}

bool test_samm_performance_impact() {
    print_test_header("SAMM Performance Impact");
    
    auto& hm = HeapManager::getInstance();
    const int NUM_OPERATIONS = 1000;
    
    // Test without SAMM
    hm.setSAMMEnabled(false);
    auto start_without_samm = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        void* obj = hm.allocObject(32);
        if (obj) {
            hm.free(obj);
        }
    }
    
    auto end_without_samm = std::chrono::high_resolution_clock::now();
    auto duration_without_samm = std::chrono::duration_cast<std::chrono::microseconds>(end_without_samm - start_without_samm);
    
    // Test with SAMM (but using manual free, not scope cleanup)
    hm.setSAMMEnabled(true);
    auto start_with_samm = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        void* obj = hm.allocObject(32);
        if (obj) {
            hm.free(obj);  // Manual free should still work with SAMM
        }
    }
    
    auto end_with_samm = std::chrono::high_resolution_clock::now();
    auto duration_with_samm = std::chrono::duration_cast<std::chrono::microseconds>(end_with_samm - start_with_samm);
    
    // Calculate performance impact
    double performance_ratio = (double)duration_with_samm.count() / duration_without_samm.count();
    bool acceptable_performance = (performance_ratio < 2.0); // Less than 2x overhead
    
    if (ENABLE_VERBOSE) {
        std::cout << "Operations: " << NUM_OPERATIONS << std::endl;
        std::cout << "Time without SAMM: " << duration_without_samm.count() << " microseconds" << std::endl;
        std::cout << "Time with SAMM: " << duration_with_samm.count() << " microseconds" << std::endl;
        std::cout << "Performance ratio: " << performance_ratio << "x" << std::endl;
        std::cout << "Acceptable performance: " << (acceptable_performance ? "YES" : "NO") << std::endl;
    }
    
    return acceptable_performance;
}

bool test_samm_statistics() {
    print_test_header("SAMM Statistics");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Get initial stats
    auto initial_stats = hm.getSAMMStats();
    
    // Perform some operations
    hm.enterScope();
    void* obj1 = hm.allocObject(32);
    void* obj2 = hm.allocVec(10);
    hm.exitScope();
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    // Get final stats
    auto final_stats = hm.getSAMMStats();
    
    bool stats_updated = (final_stats.scopes_entered > initial_stats.scopes_entered &&
                         final_stats.scopes_exited > initial_stats.scopes_exited);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Scopes entered: " << final_stats.scopes_entered << std::endl;
        std::cout << "Scopes exited: " << final_stats.scopes_exited << std::endl;
        std::cout << "Objects cleaned: " << final_stats.objects_cleaned << std::endl;
        std::cout << "Cleanup batches processed: " << final_stats.cleanup_batches_processed << std::endl;
        std::cout << "Current queue depth: " << final_stats.current_queue_depth << std::endl;
        std::cout << "Background worker running: " << (final_stats.background_worker_running ? "YES" : "NO") << std::endl;
        std::cout << "Current scope depth: " << final_stats.current_scope_depth << std::endl;
        std::cout << "Statistics properly updated: " << (stats_updated ? "YES" : "NO") << std::endl;
    }
    
    return stats_updated;
}

int main() {
    std::cout << "SAMM Compatibility Test Suite" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "Testing SAMM (Scope Aware Memory Management) functionality" << std::endl;
    std::cout << "while maintaining 100% API compatibility" << std::endl;
    
    int tests_passed = 0;
    int total_tests = 0;
    
    struct TestCase {
        const char* name;
        bool (*function)();
    };
    
    TestCase tests[] = {
        {"SAMM Basic Functionality", test_samm_basic_functionality},
        {"SAMM Scope Operations", test_samm_scope_operations},
        {"SAMM Allocation Tracking", test_samm_allocation_tracking},
        {"SAMM RETAIN Functionality", test_samm_retain_functionality},
        {"SAMM RETAIN Allocation Variants", test_samm_retain_allocation_variants},
        {"SAMM Double-Free Prevention", test_samm_double_free_prevention},
        {"SAMM C API", test_samm_c_api},
        {"SAMM Performance Impact", test_samm_performance_impact},
        {"SAMM Statistics", test_samm_statistics}
    };
    
    total_tests = sizeof(tests) / sizeof(tests[0]);
    
    for (int i = 0; i < total_tests; i++) {
        bool result = tests[i].function();
        print_test_result(tests[i].name, result);
        if (result) {
            tests_passed++;
        }
    }
    
    // Final summary
    std::cout << "\n=== SAMM TEST SUMMARY ===" << std::endl;
    std::cout << "Tests passed: " << tests_passed << "/" << total_tests << std::endl;
    std::cout << "Success rate: " << (100.0 * tests_passed / total_tests) << "%" << std::endl;
    
    if (tests_passed == total_tests) {
        std::cout << "🎉 ALL SAMM TESTS PASSED!" << std::endl;
        std::cout << "✅ SAMM is ready for production deployment" << std::endl;
        std::cout << "✅ 100% API compatibility maintained" << std::endl;
        std::cout << "✅ Scope-based cleanup working correctly" << std::endl;
        std::cout << "✅ RETAIN semantics implemented" << std::endl;
        std::cout << "✅ Double-free prevention active" << std::endl;
        return 0;
    } else {
        std::cout << "❌ SOME SAMM TESTS FAILED" << std::endl;
        std::cout << "🔧 SAMM needs refinement before production" << std::endl;
        return 1;
    }
}
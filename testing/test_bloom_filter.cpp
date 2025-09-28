#include "HeapManager/BloomFilter.h"
#include <iostream>
#include <vector>
#include <cassert>

void test_basic_operations() {
    std::cout << "Testing basic Bloom filter operations..." << std::endl;
    
    TestDoubleFreeBloomFilter bloom;
    
    // Test adding and checking elements
    void* ptr1 = (void*)0x1000;
    void* ptr2 = (void*)0x2000;
    void* ptr3 = (void*)0x3000;
    
    // Initially, nothing should be in the filter
    assert(!bloom.check(ptr1));
    assert(!bloom.check(ptr2));
    assert(!bloom.check(ptr3));
    
    // Add ptr1 and ptr2
    bloom.add(ptr1);
    bloom.add(ptr2);
    
    // Now ptr1 and ptr2 should be detected (might be in set)
    assert(bloom.check(ptr1));
    assert(bloom.check(ptr2));
    
    // ptr3 should still not be detected (definitely not in set)
    assert(!bloom.check(ptr3));
    
    std::cout << "✅ Basic operations test passed!" << std::endl;
}

void test_false_positives() {
    std::cout << "Testing false positive behavior..." << std::endl;
    
    TestDoubleFreeBloomFilter bloom;
    
    // Add many elements to increase saturation
    std::vector<void*> added_ptrs;
    for (int i = 0; i < 1000; i++) {
        void* ptr = (void*)(0x10000 + i * 8);
        bloom.add(ptr);
        added_ptrs.push_back(ptr);
    }
    
    // All added elements should be detected
    for (void* ptr : added_ptrs) {
        assert(bloom.check(ptr));
    }
    
    // Check some elements that weren't added
    int false_positives = 0;
    int total_checks = 1000;
    
    for (int i = 0; i < total_checks; i++) {
        void* test_ptr = (void*)(0x50000 + i * 8); // Different range
        if (bloom.check(test_ptr)) {
            false_positives++;
        }
    }
    
    double fp_rate = (double)false_positives / total_checks;
    std::cout << "False positive rate: " << (fp_rate * 100) << "% (" 
              << false_positives << "/" << total_checks << ")" << std::endl;
    
    // Should be reasonable (less than 10% for this test configuration)
    assert(fp_rate < 0.1);
    
    std::cout << "✅ False positive test passed!" << std::endl;
}

void test_memory_usage() {
    std::cout << "Testing memory usage..." << std::endl;
    
    TestDoubleFreeBloomFilter bloom;
    
    size_t memory_usage = bloom.memory_usage();
    std::cout << "Bloom filter memory usage: " << memory_usage << " bytes" << std::endl;
    
    // Should be around 60KB for TestDoubleFreeBloomFilter (480000 bits / 8)
    assert(memory_usage == 60000);
    
    std::cout << "✅ Memory usage test passed!" << std::endl;
}

void test_clear_functionality() {
    std::cout << "Testing clear functionality..." << std::endl;
    
    TestDoubleFreeBloomFilter bloom;
    
    // Add some elements
    void* ptr1 = (void*)0x1000;
    void* ptr2 = (void*)0x2000;
    
    bloom.add(ptr1);
    bloom.add(ptr2);
    
    assert(bloom.check(ptr1));
    assert(bloom.check(ptr2));
    
    // Clear the filter
    bloom.clear();
    
    // Now nothing should be detected
    assert(!bloom.check(ptr1));
    assert(!bloom.check(ptr2));
    
    std::cout << "✅ Clear functionality test passed!" << std::endl;
}

void test_large_scale() {
    std::cout << "Testing large scale operations..." << std::endl;
    
    DoubleFreeBloomFilter bloom; // Use the full-size filter
    
    std::cout << "Full-size filter memory usage: " << bloom.memory_usage() << " bytes" << std::endl;
    
    // Add many elements
    const int num_elements = 100000;
    std::vector<void*> ptrs;
    
    for (int i = 0; i < num_elements; i++) {
        void* ptr = (void*)(0x100000 + i * 16);
        ptrs.push_back(ptr);
        bloom.add(ptr);
        
        if (i % 10000 == 0) {
            std::cout << "Added " << (i + 1) << " elements..." << std::endl;
        }
    }
    
    // Verify all added elements are detected
    for (void* ptr : ptrs) {
        assert(bloom.check(ptr));
    }
    
    // Estimate false positive rate
    double estimated_fp_rate = bloom.estimate_false_positive_rate(num_elements);
    std::cout << "Estimated false positive rate: " << (estimated_fp_rate * 100) << "%" << std::endl;
    
    std::cout << "✅ Large scale test passed!" << std::endl;
}

void test_double_free_simulation() {
    std::cout << "Testing double-free detection simulation..." << std::endl;
    
    TestDoubleFreeBloomFilter bloom;
    
    // Simulate memory allocations and frees
    std::vector<void*> allocated_ptrs;
    std::vector<void*> freed_ptrs;
    
    // Allocate some "memory"
    for (int i = 0; i < 50; i++) {
        void* ptr = (void*)(0x200000 + i * 32);
        allocated_ptrs.push_back(ptr);
    }
    
    // Free half of them
    for (int i = 0; i < 25; i++) {
        void* ptr = allocated_ptrs[i];
        bloom.add(ptr); // Add to "freed" set
        freed_ptrs.push_back(ptr);
    }
    
    // Test double-free detection
    for (void* ptr : freed_ptrs) {
        assert(bloom.check(ptr)); // Should detect as potentially freed
    }
    
    // Test that non-freed pointers are not detected
    for (int i = 25; i < 50; i++) {
        void* ptr = allocated_ptrs[i];
        assert(!bloom.check(ptr)); // Should not be detected as freed
    }
    
    std::cout << "✅ Double-free simulation test passed!" << std::endl;
}

int main() {
    std::cout << "=== Bloom Filter Test Suite ===" << std::endl;
    
    try {
        test_basic_operations();
        test_false_positives();
        test_memory_usage();
        test_clear_functionality();
        test_large_scale();
        test_double_free_simulation();
        
        std::cout << "\n🎉 All tests passed! Bloom filter implementation is working correctly." << std::endl;
        std::cout << "\nBloom Filter Summary:" << std::endl;
        std::cout << "- TestDoubleFreeBloomFilter: ~60KB memory, good for testing" << std::endl;
        std::cout << "- DoubleFreeBloomFilter: ~1.2MB memory, optimized for ~1M elements" << std::endl;
        std::cout << "- ConservativeDoubleFreeBloomFilter: ~900KB memory, lower false positive rate" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "❌ Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
#include "../HeapManager/BloomFilter.h"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Simple Bloom Filter Test" << std::endl;
    
    // Use the tiny test configuration (very small and fast)
    TinyTestBloomFilter bloom;
    
    // Test 1: Basic functionality
    void* ptr1 = (void*)0x1000;
    void* ptr2 = (void*)0x2000;
    
    // Should not be present initially
    assert(!bloom.check(ptr1));
    assert(!bloom.check(ptr2));
    
    // Add ptr1
    bloom.add(ptr1);
    
    // ptr1 should now be detected, ptr2 should not
    assert(bloom.check(ptr1));
    assert(!bloom.check(ptr2));
    
    // Add ptr2
    bloom.add(ptr2);
    
    // Both should now be detected
    assert(bloom.check(ptr1));
    assert(bloom.check(ptr2));
    
    std::cout << "Memory usage: " << bloom.memory_usage() << " bytes" << std::endl;
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}
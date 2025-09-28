// debug_samm_simple.cpp
// Simple SAMM debug test to isolate the cleanup issue

#include <iostream>
#include <thread>
#include <chrono>

// Include HeapManager with SAMM
#include "HeapManager/HeapManager.h"

int main() {
    std::cout << "=== SIMPLE SAMM DEBUG TEST ===" << std::endl;
    
    auto& hm = HeapManager::getInstance();
    
    std::cout << "1. Enabling SAMM..." << std::endl;
    hm.setSAMMEnabled(true);
    hm.setTraceEnabled(true);
    
    auto initial_stats = hm.getSAMMStats();
    std::cout << "Initial stats - worker running: " << (initial_stats.background_worker_running ? "YES" : "NO") << std::endl;
    
    std::cout << "2. Entering scope..." << std::endl;
    hm.enterScope();
    
    std::cout << "3. Allocating 3 objects..." << std::endl;
    void* obj1 = hm.allocObject(64);
    void* obj2 = hm.allocObject(32);
    void* obj3 = hm.allocObject(128);
    
    std::cout << "Allocated objects: " << obj1 << ", " << obj2 << ", " << obj3 << std::endl;
    
    auto mid_stats = hm.getSAMMStats();
    std::cout << "After allocation - scope depth: " << mid_stats.current_scope_depth << std::endl;
    std::cout << "After allocation - queue depth: " << mid_stats.current_queue_depth << std::endl;
    
    std::cout << "4. Exiting scope (should trigger cleanup)..." << std::endl;
    hm.exitScope();
    
    std::cout << "5. Waiting for cleanup..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    auto final_stats = hm.getSAMMStats();
    std::cout << "Final stats:" << std::endl;
    std::cout << "  Objects cleaned: " << final_stats.objects_cleaned << std::endl;
    std::cout << "  Cleanup batches: " << final_stats.cleanup_batches_processed << std::endl;
    std::cout << "  Queue depth: " << final_stats.current_queue_depth << std::endl;
    std::cout << "  Worker running: " << (final_stats.background_worker_running ? "YES" : "NO") << std::endl;
    
    if (final_stats.objects_cleaned > 0) {
        std::cout << "✅ SAMM cleanup WORKING!" << std::endl;
    } else {
        std::cout << "❌ SAMM cleanup NOT WORKING!" << std::endl;
    }
    
    return 0;
}
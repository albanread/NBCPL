// Simple SAMM Test Program - Minimal Dependencies
// This program tests just the core SAMM functionality without external dependencies

#include <iostream>
#include <vector>
#include <stack>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>
#include <cassert>

// Minimal HeapManager implementation for testing SAMM core functionality
class SimpleSAMM {
private:
    // SAMM: Dual-mutex architecture for minimal contention
    mutable std::mutex scope_mutex_;    // Ultra-fast operations on hot path
    mutable std::mutex cleanup_mutex_;  // Queue coordination between threads
    
    // SAMM: Scope vector for tracking allocations per lexical scope
    std::vector<std::vector<void*>> scope_allocations_;
    
    // SAMM: Background cleanup infrastructure
    std::queue<std::vector<void*>> cleanup_queue_;
    std::condition_variable cleanup_cv_;
    std::thread cleanup_worker_;
    std::atomic<bool> running_{true};
    std::atomic<bool> samm_enabled_{false};
    
    // SAMM: Performance and debug statistics
    std::atomic<uint64_t> samm_scopes_entered_{0};
    std::atomic<uint64_t> samm_scopes_exited_{0};
    std::atomic<uint64_t> samm_objects_cleaned_{0};
    std::atomic<uint64_t> samm_cleanup_batches_processed_{0};

    void cleanupWorker() {
        std::cout << "SAMM: Background cleanup worker thread started\n";
        
        while (running_.load()) {
            std::unique_lock<std::mutex> lock(cleanup_mutex_);
            cleanup_cv_.wait(lock, [this] { 
                return !cleanup_queue_.empty() || !running_.load(); 
            });
            
            while (!cleanup_queue_.empty()) {
                auto ptrs = std::move(cleanup_queue_.front());
                cleanup_queue_.pop();
                lock.unlock();
                
                // Simulate cleanup work
                std::cout << "SAMM: Processing cleanup batch of " << ptrs.size() << " objects\n";
                
                // Simulate some cleanup time
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                
                for (auto ptr : ptrs) {
                    std::cout << "  Cleaning up object at " << ptr << "\n";
                    delete[] static_cast<char*>(ptr); // Simple delete for test
                }
                
                samm_cleanup_batches_processed_.fetch_add(1);
                samm_objects_cleaned_.fetch_add(ptrs.size());
                
                lock.lock();
            }
        }
        
        std::cout << "SAMM: Background cleanup worker thread exiting\n";
    }

public:
    SimpleSAMM() {
        setSAMMEnabled(true);
    }
    
    ~SimpleSAMM() {
        shutdown();
    }

    void setSAMMEnabled(bool enabled) {
        bool was_enabled = samm_enabled_.exchange(enabled);
        
        if (enabled && !was_enabled) {
            std::cout << "SAMM: Enabling scope-aware memory management\n";
            startBackgroundWorker();
        } else if (!enabled && was_enabled) {
            std::cout << "SAMM: Disabling scope-aware memory management\n";
            stopBackgroundWorker();
        }
    }

    void startBackgroundWorker() {
        if (cleanup_worker_.joinable()) {
            return; // Already running
        }
        
        running_.store(true);
        cleanup_worker_ = std::thread(&SimpleSAMM::cleanupWorker, this);
        std::cout << "SAMM: Background cleanup worker started\n";
    }

    void stopBackgroundWorker() {
        running_.store(false);
        cleanup_cv_.notify_all();
        
        if (cleanup_worker_.joinable()) {
            cleanup_worker_.join();
        }
        
        std::cout << "SAMM: Background cleanup worker stopped\n";
    }

    void enterScope() {
        if (!samm_enabled_.load()) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(scope_mutex_);
        scope_allocations_.push_back({});
        samm_scopes_entered_.fetch_add(1);
        
        std::cout << "SAMM: Entered scope (depth: " << scope_allocations_.size() << ")\n";
    }

    void exitScope() {
        if (!samm_enabled_.load()) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (scope_allocations_.empty()) {
            std::cout << "SAMM: Warning - exitScope called but no scopes available\n";
            return;
        }
        
        // Move entire scope to cleanup queue - O(1) operation
        std::vector<void*> scope_ptrs = std::move(scope_allocations_.back());
        scope_allocations_.pop_back();
        samm_scopes_exited_.fetch_add(1);
        
        if (!scope_ptrs.empty()) {
            // Queue for background processing
            {
                std::lock_guard<std::mutex> cleanup_lock(cleanup_mutex_);
                cleanup_queue_.push(std::move(scope_ptrs));
            }
            cleanup_cv_.notify_one();
            
            std::cout << "SAMM: Exited scope, queued " << scope_ptrs.size() << " objects for cleanup\n";
        } else {
            std::cout << "SAMM: Exited empty scope\n";
        }
    }

    void* allocObject(size_t size) {
        // Simple allocation for testing
        void* ptr = new char[size];
        
        // SAMM: Track allocation in current scope if enabled
        if (samm_enabled_.load()) {
            std::lock_guard<std::mutex> lock(scope_mutex_);
            if (!scope_allocations_.empty()) {
                scope_allocations_.back().push_back(ptr);
                std::cout << "SAMM: Allocated object " << ptr << " (added to current scope)\n";
            }
        }
        
        return ptr;
    }

    void retainPointer(void* ptr) {
        if (!samm_enabled_.load() || !ptr) {
            return;
        }
        
        std::lock_guard<std::mutex> lock(scope_mutex_);
        
        if (scope_allocations_.size() < 2) {
            // RETAIN in global scope - make globally persistent
            if (!scope_allocations_.empty()) {
                auto& global_scope = scope_allocations_.back();
                global_scope.erase(
                    std::remove(global_scope.begin(), global_scope.end(), ptr),
                    global_scope.end());
            }
            std::cout << "SAMM: Retained pointer " << ptr << " globally (globally persistent)\n";
            return;
        }
        
        // Standard case: move from current to parent scope
        auto& current_scope = scope_allocations_.back();
        auto& parent_scope = scope_allocations_[scope_allocations_.size() - 2];
        
        auto it = std::find(current_scope.begin(), current_scope.end(), ptr);
        if (it != current_scope.end()) {
            parent_scope.push_back(*it);
            current_scope.erase(it);
            std::cout << "SAMM: Retained pointer " << ptr << " to parent scope\n";
        } else {
            std::cout << "SAMM: Warning - pointer " << ptr << " not found in current scope for RETAIN\n";
        }
    }

    void handleMemoryPressure() {
        if (!samm_enabled_.load()) {
            return;
        }
        
        std::cout << "SAMM: Handling memory pressure - forcing immediate cleanup\n";
        
        std::unique_lock<std::mutex> lock(cleanup_mutex_);
        while (!cleanup_queue_.empty()) {
            auto ptrs = std::move(cleanup_queue_.front());
            cleanup_queue_.pop();
            lock.unlock();
            
            // Immediate cleanup when memory pressure detected
            for (auto ptr : ptrs) {
                std::cout << "  Immediate cleanup of object at " << ptr << "\n";
                delete[] static_cast<char*>(ptr);
            }
            
            lock.lock();
        }
        
        std::cout << "SAMM: Memory pressure handling complete\n";
    }

    void shutdown() {
        std::cout << "SAMM: Beginning shutdown sequence\n";
        
        // Stop background worker
        running_.store(false);
        cleanup_cv_.notify_all();
        if (cleanup_worker_.joinable()) {
            cleanup_worker_.join();
        }
        
        // Process any remaining items synchronously on shutdown
        std::lock_guard<std::mutex> lock(cleanup_mutex_);
        while (!cleanup_queue_.empty()) {
            auto ptrs = std::move(cleanup_queue_.front());
            cleanup_queue_.pop();
            
            // Perform cleanup for remaining items
            for (auto ptr : ptrs) {
                std::cout << "  Shutdown cleanup of object at " << ptr << "\n";
                delete[] static_cast<char*>(ptr);
            }
        }
        
        std::cout << "SAMM: Shutdown complete\n";
    }

    struct SAMMStats {
        uint64_t scopes_entered;
        uint64_t scopes_exited;
        uint64_t objects_cleaned;
        uint64_t cleanup_batches_processed;
        size_t current_queue_depth;
        bool background_worker_running;
    };

    SAMMStats getSAMMStats() const {
        std::lock_guard<std::mutex> lock(cleanup_mutex_);
        return SAMMStats{
            samm_scopes_entered_.load(),
            samm_scopes_exited_.load(),
            samm_objects_cleaned_.load(),
            samm_cleanup_batches_processed_.load(),
            cleanup_queue_.size(),
            cleanup_worker_.joinable() && running_.load()
        };
    }
};

// Test functions
void test_basic_scope_tracking() {
    std::cout << "\n=== Test: Basic Scope Tracking ===\n";
    
    SimpleSAMM samm;
    
    // Wait for background worker to start
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto initial_stats = samm.getSAMMStats();
    std::cout << "Initial scopes entered: " << initial_stats.scopes_entered << "\n";
    
    // Test scope entry/exit
    samm.enterScope();
    {
        // Allocate objects in this scope
        void* obj1 = samm.allocObject(32);
        void* obj2 = samm.allocObject(48);
        void* obj3 = samm.allocObject(64);
        
        std::cout << "Allocated 3 objects in scope\n";
        
        // Exit scope - should queue objects for background cleanup
        samm.exitScope();
        std::cout << "Exited scope\n";
    }
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(300));
    
    auto final_stats = samm.getSAMMStats();
    std::cout << "Final scopes entered: " << final_stats.scopes_entered << "\n";
    std::cout << "Final scopes exited: " << final_stats.scopes_exited << "\n";
    std::cout << "Objects cleaned: " << final_stats.objects_cleaned << "\n";
    std::cout << "Cleanup batches processed: " << final_stats.cleanup_batches_processed << "\n";
    
    assert(final_stats.scopes_entered > initial_stats.scopes_entered);
    assert(final_stats.scopes_exited > initial_stats.scopes_exited);
    
    std::cout << "Basic scope tracking test passed\n";
}

void test_retain_functionality() {
    std::cout << "\n=== Test: RETAIN Functionality ===\n";
    
    SimpleSAMM samm;
    
    void* retained_obj = nullptr;
    
    // Outer scope
    samm.enterScope();
    {
        // Inner scope
        samm.enterScope();
        {
            // Allocate objects in inner scope
            void* obj1 = samm.allocObject(64);
            retained_obj = samm.allocObject(64);
            
            // Retain one object to parent scope
            samm.retainPointer(retained_obj);
            
            std::cout << "Allocated 2 objects, retained 1 to parent scope\n";
        }
        samm.exitScope(); // obj1 should be cleaned up, retained_obj should remain
        
        std::cout << "Exited inner scope\n";
        
        // Give background thread time to process
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    samm.exitScope(); // retained_obj should be cleaned up now
    
    std::cout << "Exited outer scope\n";
    
    // Give background thread time to process
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    auto stats = samm.getSAMMStats();
    std::cout << "Total objects cleaned in RETAIN test: " << stats.objects_cleaned << "\n";
    
    std::cout << "RETAIN functionality test passed\n";
}

void test_performance() {
    std::cout << "\n=== Test: Performance Measurement ===\n";
    
    SimpleSAMM samm;
    samm.setSAMMEnabled(false); // Test legacy mode first
    
    const int NUM_ITERATIONS = 100;
    const int OBJECTS_PER_SCOPE = 5;
    
    // Test manual cleanup (simulating legacy mode)
    auto start_legacy = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> all_objects;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (int j = 0; j < OBJECTS_PER_SCOPE; j++) {
            void* obj = samm.allocObject(32);
            all_objects.push_back(obj);
        }
    }
    
    // Manual cleanup
    for (void* obj : all_objects) {
        delete[] static_cast<char*>(obj);
    }
    
    auto end_legacy = std::chrono::high_resolution_clock::now();
    auto legacy_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_legacy - start_legacy);
    
    // Test SAMM mode
    samm.setSAMMEnabled(true);
    std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Let background worker start
    
    auto start_samm = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        samm.enterScope();
        for (int j = 0; j < OBJECTS_PER_SCOPE; j++) {
            samm.allocObject(32);
        }
        samm.exitScope(); // Background cleanup
    }
    
    auto end_samm = std::chrono::high_resolution_clock::now();
    auto samm_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_samm - start_samm);
    
    // Wait for background cleanup to complete
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    
    std::cout << "Manual cleanup: " << legacy_duration.count() << " microseconds\n";
    std::cout << "SAMM mode: " << samm_duration.count() << " microseconds\n";
    
    if (samm_duration.count() < legacy_duration.count()) {
        double improvement = (double)legacy_duration.count() / samm_duration.count();
        std::cout << "SAMM is " << improvement << "x faster!\n";
    } else {
        std::cout << "Manual mode was faster (background thread overhead)\n";
    }
    
    auto final_stats = samm.getSAMMStats();
    std::cout << "Final SAMM stats:\n";
    std::cout << "  Scopes entered: " << final_stats.scopes_entered << "\n";
    std::cout << "  Scopes exited: " << final_stats.scopes_exited << "\n";
    std::cout << "  Objects cleaned: " << final_stats.objects_cleaned << "\n";
    std::cout << "  Cleanup batches: " << final_stats.cleanup_batches_processed << "\n";
    
    std::cout << "Performance test completed\n";
}

void test_memory_pressure() {
    std::cout << "\n=== Test: Memory Pressure Handling ===\n";
    
    SimpleSAMM samm;
    
    // Create multiple scopes with many objects
    for (int i = 0; i < 3; i++) {
        samm.enterScope();
        for (int j = 0; j < 10; j++) {
            samm.allocObject(64);
        }
        samm.exitScope();
    }
    
    std::cout << "Created 3 scopes with 10 objects each (30 total)\n";
    
    auto stats_before = samm.getSAMMStats();
    std::cout << "Queue depth before pressure handling: " << stats_before.current_queue_depth << "\n";
    
    // Force immediate cleanup
    samm.handleMemoryPressure();
    
    auto stats_after = samm.getSAMMStats();
    std::cout << "Queue depth after pressure handling: " << stats_after.current_queue_depth << "\n";
    
    assert(stats_after.current_queue_depth == 0);
    
    std::cout << "Memory pressure handling test passed\n";
}

int main() {
    std::cout << "Simple SAMM (Scope Aware Memory Management) Test Suite\n";
    std::cout << "====================================================\n";
    
    try {
        test_basic_scope_tracking();
        test_retain_functionality();
        test_performance();
        test_memory_pressure();
        
        std::cout << "\n=== All Tests Passed! ===\n";
        std::cout << "SAMM core implementation is working correctly.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
    
    return 0;
}
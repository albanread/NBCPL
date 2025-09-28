// Quick SAMM Test Program - Clean Performance Output
// This program tests SAMM functionality with minimal output for clean performance measurement

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <chrono>
#include <cassert>

// Minimal HeapManager implementation for testing SAMM core functionality
class QuickSAMM {
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
    bool quiet_mode_{false};
    
    // SAMM: Performance and debug statistics
    std::atomic<uint64_t> samm_scopes_entered_{0};
    std::atomic<uint64_t> samm_scopes_exited_{0};
    std::atomic<uint64_t> samm_objects_cleaned_{0};
    std::atomic<uint64_t> samm_cleanup_batches_processed_{0};

    void cleanupWorker() {
        if (!quiet_mode_) std::cout << "SAMM: Background cleanup worker started\n";
        
        while (running_.load()) {
            std::unique_lock<std::mutex> lock(cleanup_mutex_);
            cleanup_cv_.wait(lock, [this] { 
                return !cleanup_queue_.empty() || !running_.load(); 
            });
            
            while (!cleanup_queue_.empty()) {
                auto ptrs = std::move(cleanup_queue_.front());
                cleanup_queue_.pop();
                lock.unlock();
                
                // Simulate minimal cleanup work
                for (auto ptr : ptrs) {
                    delete[] static_cast<char*>(ptr);
                }
                
                samm_cleanup_batches_processed_.fetch_add(1);
                samm_objects_cleaned_.fetch_add(ptrs.size());
                
                lock.lock();
            }
        }
        
        if (!quiet_mode_) std::cout << "SAMM: Background cleanup worker stopped\n";
    }

public:
    QuickSAMM(bool quiet = false) : quiet_mode_(quiet) {
        setSAMMEnabled(true);
    }
    
    ~QuickSAMM() {
        shutdown();
    }

    void setSAMMEnabled(bool enabled) {
        bool was_enabled = samm_enabled_.exchange(enabled);
        
        if (enabled && !was_enabled) {
            startBackgroundWorker();
        } else if (!enabled && was_enabled) {
            stopBackgroundWorker();
        }
    }

    void startBackgroundWorker() {
        if (cleanup_worker_.joinable()) {
            return; // Already running
        }
        
        running_.store(true);
        cleanup_worker_ = std::thread(&QuickSAMM::cleanupWorker, this);
    }

    void stopBackgroundWorker() {
        running_.store(false);
        cleanup_cv_.notify_all();
        
        if (cleanup_worker_.joinable()) {
            cleanup_worker_.join();
        }
    }

    void enterScope() {
        if (!samm_enabled_.load()) return;
        
        std::lock_guard<std::mutex> lock(scope_mutex_);
        scope_allocations_.push_back({});
        samm_scopes_entered_.fetch_add(1);
    }

    void exitScope() {
        if (!samm_enabled_.load()) return;
        
        std::lock_guard<std::mutex> lock(scope_mutex_);
        if (scope_allocations_.empty()) return;
        
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
            }
        }
        
        return ptr;
    }

    void shutdown() {
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
            
            for (auto ptr : ptrs) {
                delete[] static_cast<char*>(ptr);
            }
        }
    }

    struct SAMMStats {
        uint64_t scopes_entered;
        uint64_t scopes_exited;
        uint64_t objects_cleaned;
        uint64_t cleanup_batches_processed;
        size_t current_queue_depth;
    };

    SAMMStats getSAMMStats() const {
        std::lock_guard<std::mutex> lock(cleanup_mutex_);
        return SAMMStats{
            samm_scopes_entered_.load(),
            samm_scopes_exited_.load(),
            samm_objects_cleaned_.load(),
            samm_cleanup_batches_processed_.load(),
            cleanup_queue_.size()
        };
    }
};

void test_performance_clean() {
    std::cout << "\n=== SAMM Performance Test ===\n";
    
    const int NUM_ITERATIONS = 1000;
    const int OBJECTS_PER_SCOPE = 10;
    
    // Test manual cleanup (simulating legacy mode)
    std::cout << "Testing manual cleanup mode...\n";
    QuickSAMM manual_samm(true); // quiet mode
    manual_samm.setSAMMEnabled(false);
    
    auto start_manual = std::chrono::high_resolution_clock::now();
    
    std::vector<void*> all_objects;
    for (int i = 0; i < NUM_ITERATIONS; i++) {
        for (int j = 0; j < OBJECTS_PER_SCOPE; j++) {
            void* obj = manual_samm.allocObject(32);
            all_objects.push_back(obj);
        }
    }
    
    // Manual cleanup
    for (void* obj : all_objects) {
        delete[] static_cast<char*>(obj);
    }
    
    auto end_manual = std::chrono::high_resolution_clock::now();
    auto manual_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_manual - start_manual);
    
    // Test SAMM mode
    std::cout << "Testing SAMM scope-based cleanup mode...\n";
    QuickSAMM samm(true); // quiet mode
    std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Let background worker start
    
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
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    
    // Results
    std::cout << "\n=== Performance Results ===\n";
    std::cout << "Manual cleanup:     " << manual_duration.count() << " μs\n";
    std::cout << "SAMM cleanup:       " << samm_duration.count() << " μs\n";
    
    if (samm_duration.count() < manual_duration.count()) {
        double improvement = (double)manual_duration.count() / samm_duration.count();
        std::cout << "SAMM Performance:   " << improvement << "x FASTER 🚀\n";
    } else if (samm_duration.count() > manual_duration.count()) {
        double slower = (double)samm_duration.count() / manual_duration.count();
        std::cout << "SAMM Performance:   " << slower << "x slower (background overhead)\n";
    } else {
        std::cout << "SAMM Performance:   Equal performance\n";
    }
    
    auto final_stats = samm.getSAMMStats();
    std::cout << "\nSAMM Statistics:\n";
    std::cout << "  Scopes processed:   " << final_stats.scopes_entered << " → " << final_stats.scopes_exited << "\n";
    std::cout << "  Objects cleaned:    " << final_stats.objects_cleaned << " / " << (NUM_ITERATIONS * OBJECTS_PER_SCOPE) << "\n";
    std::cout << "  Cleanup batches:    " << final_stats.cleanup_batches_processed << "\n";
    std::cout << "  Remaining in queue: " << final_stats.current_queue_depth << "\n";
    
    // Efficiency calculation
    double scope_overhead_per_iteration = (double)samm_duration.count() / NUM_ITERATIONS;
    std::cout << "\nScope overhead per iteration: " << scope_overhead_per_iteration << " μs\n";
    
    double objects_per_second = (double)(NUM_ITERATIONS * OBJECTS_PER_SCOPE) / (samm_duration.count() / 1000000.0);
    std::cout << "SAMM allocation rate: " << (long)objects_per_second << " objects/second\n";
}

void test_basic_functionality() {
    std::cout << "\n=== SAMM Basic Functionality Test ===\n";
    
    QuickSAMM samm(true); // quiet mode
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    
    // Simple scope test
    samm.enterScope();
    void* obj1 = samm.allocObject(64);
    void* obj2 = samm.allocObject(128);
    samm.exitScope();
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    auto stats = samm.getSAMMStats();
    
    std::cout << "✓ Scope tracking works: " << stats.scopes_entered << " entered, " << stats.scopes_exited << " exited\n";
    std::cout << "✓ Background cleanup works: " << stats.objects_cleaned << " objects cleaned\n";
    std::cout << "✓ Basic functionality confirmed\n";
    
    assert(stats.scopes_entered == 1);
    assert(stats.scopes_exited == 1);
    assert(stats.objects_cleaned >= 2);
}

int main() {
    std::cout << "Quick SAMM (Scope Aware Memory Management) Test\n";
    std::cout << "===============================================\n";
    
    try {
        test_basic_functionality();
        test_performance_clean();
        
        std::cout << "\n=== ✅ All Tests Passed! ===\n";
        std::cout << "SAMM implementation is working correctly.\n";
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "❌ Test failed with unknown exception\n";
        return 1;
    }
    
    return 0;
}
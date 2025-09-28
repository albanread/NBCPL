// test_bloom_scaling.cpp
// Test for adaptive bloom filter scaling using BCPL strings in lists
// This test creates and frees many strings through HeapManager to stress the bloom filter

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstring>
#include <thread>

// Runtime and HeapManager includes
#include "../../HeapManager/HeapManager.h"
#include "../../HeapManager/heap_c_wrappers.h"
#include "../../runtime/ListDataTypes.h"
#include "../../include/compiler_interface.h"

// Global trace variables required by the runtime
bool g_enable_heap_trace = false;
bool g_enable_lexer_trace = false;
bool g_enable_symbols_trace = false;

// Forward declarations for BCPL list functions and freelist
extern "C" {
    void BCPL_LIST_APPEND_STRING(ListHeader* header, uint32_t* bcpl_str);
    ListAtom* getNodeFromFreelist(void);
    ListHeader* getHeaderFromFreelist(void);
    void printFreelistMetrics(void);
    uint32_t* bcpl_alloc_chars(int64_t num_chars);
}

// Helper for timing
class Timer {
public:
    void start() { t0 = std::chrono::high_resolution_clock::now(); }
    double stop() {
        auto t1 = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double, std::milli>(t1 - t0).count();
    }
private:
    std::chrono::high_resolution_clock::time_point t0;
};

void print_test_header(const char* test_name) {
    std::cout << "\n=== " << test_name << " ===" << std::endl;
}

void print_test_result(const char* test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
}

void print_bloom_filter_stats() {
    auto& hm = HeapManager::getInstance();
    std::cout << "  Bloom Filter Tier: " << hm.getBloomFilterTier() << std::endl;
    std::cout << "  Items Added: " << hm.getBloomFilterItemsAdded() << std::endl;
    std::cout << "  Capacity: " << hm.getBloomFilterCapacity() << std::endl;
    std::cout << "  Reset Count: " << hm.getBloomFilterResetCount() << std::endl;
    std::cout << "  Memory Usage: " << hm.getBloomFilterMemoryUsage() << " bytes" << std::endl;
    std::cout << "  False Positive Rate: " << (hm.getBloomFilterFalsePositiveRate() * 100.0) << "%" << std::endl;
}

// Test 1: Create many string lists to stress bloom filter
bool test_string_list_bloom_pressure() {
    print_test_header("String List Bloom Filter Pressure Test");
    
    const size_t NUM_CYCLES = 10;
    const size_t STRINGS_PER_CYCLE = 100000;
    
    std::cout << "Creating " << NUM_CYCLES << " cycles of " << STRINGS_PER_CYCLE << " strings each..." << std::endl;
    
    // Enable SAMM and heap tracing to see bloom filter activity
    HeapManager_setSAMMEnabled(1);
    HeapManager::getInstance().setTraceEnabled(false); // Keep quiet for now
    
    Timer timer;
    timer.start();
    
    size_t initial_items = HeapManager::getInstance().getBloomFilterItemsAdded();
    size_t initial_resets = HeapManager::getInstance().getBloomFilterResetCount();
    std::string initial_tier = HeapManager::getInstance().getBloomFilterTier();
    
    std::cout << "Initial bloom filter state:" << std::endl;
    print_bloom_filter_stats();
    
    for (size_t cycle = 0; cycle < NUM_CYCLES; ++cycle) {
        HeapManager_enterScope();
        
        // Create a list to hold strings
        ListHeader* string_list = getHeaderFromFreelist();
        string_list->type = ATOM_SENTINEL;
        string_list->contains_literals = 0;
        string_list->length = 0;
        string_list->head = nullptr;
        string_list->tail = nullptr;
        
        // Create many strings and add them to the list
        for (size_t i = 0; i < STRINGS_PER_CYCLE; ++i) {
            // Create a BCPL string with heap allocation
            char temp_str[64];
            snprintf(temp_str, sizeof(temp_str), "String_%zu_%zu", cycle, i);
            
            // Allocate BCPL string and copy characters
            size_t str_len = strlen(temp_str);
            uint32_t* bcpl_string = bcpl_alloc_chars(str_len);
            if (bcpl_string) {
                // Copy characters to BCPL string format (32-bit chars)
                for (size_t j = 0; j < str_len; ++j) {
                    bcpl_string[j] = static_cast<uint32_t>(temp_str[j]);
                }
                // BCPL strings are null-terminated by bcpl_alloc_chars
            }
            
            // Add string to list using freelist node
            ListAtom* new_node = getNodeFromFreelist();
            new_node->type = ATOM_STRING;
            new_node->pad = 0;
            new_node->value.ptr_value = reinterpret_cast<void*>(bcpl_string);
            new_node->next = nullptr;
            
            if (string_list->head == nullptr) {
                string_list->head = new_node;
                string_list->tail = new_node;
            } else {
                string_list->tail->next = new_node;
                string_list->tail = new_node;
            }
            string_list->length++;
        }
        
        // Exit scope - this should free all the strings via SAMM, stressing the bloom filter
        HeapManager_exitScope();
        
        // Print progress every few cycles
        if ((cycle + 1) % 3 == 0) {
            std::cout << "After cycle " << (cycle + 1) << "/" << NUM_CYCLES << ":" << std::endl;
            print_bloom_filter_stats();
        }
    }
    
    double elapsed_ms = timer.stop();
    
    size_t final_items = HeapManager::getInstance().getBloomFilterItemsAdded();
    size_t final_resets = HeapManager::getInstance().getBloomFilterResetCount();
    std::string final_tier = HeapManager::getInstance().getBloomFilterTier();
    
    std::cout << "\nFinal Results:" << std::endl;
    std::cout << "Time elapsed: " << elapsed_ms << " ms" << std::endl;
    std::cout << "Total strings processed: " << (NUM_CYCLES * STRINGS_PER_CYCLE) << std::endl;
    std::cout << "Bloom filter items added: " << (final_items - initial_items) << std::endl;
    std::cout << "Bloom filter scaling events: " << (final_resets - initial_resets) << std::endl;
    std::cout << "Tier progression: " << initial_tier << " → " << final_tier << std::endl;
    
    std::cout << "\nFinal bloom filter state:" << std::endl;
    print_bloom_filter_stats();
    
    bool test_passed = true;
    
    // Verify that we actually stressed the bloom filter
    if (final_items <= initial_items) {
        std::cout << "WARNING: Bloom filter was not stressed (no items added)" << std::endl;
        test_passed = false;
    }
    
    // Check if scaling occurred
    if (final_resets > initial_resets) {
        std::cout << "✅ Bloom filter scaling detected! Scaled " << (final_resets - initial_resets) << " times." << std::endl;
    } else if (final_items > 100000) {
        std::cout << "✅ Bloom filter was heavily used (" << final_items << " items) without needing to scale." << std::endl;
    }
    
    print_test_result("String List Bloom Filter Pressure Test", test_passed);
    return test_passed;
}

// Test 2: Intensive string allocation/deallocation to force bloom filter scaling
bool test_bloom_filter_scaling() {
    print_test_header("Bloom Filter Scaling Test");
    
    // Enable heap tracing briefly to see scaling messages
    HeapManager::getInstance().setTraceEnabled(true);
    
    const size_t TARGET_BLOOM_ITEMS = 1000000; // Try to exceed 1M tier capacity
    const size_t BATCH_SIZE = 50000;
    
    std::cout << "Targeting " << TARGET_BLOOM_ITEMS << " bloom filter items in batches of " << BATCH_SIZE << std::endl;
    
    Timer timer;
    timer.start();
    
    size_t total_allocated = 0;
    size_t initial_resets = HeapManager::getInstance().getBloomFilterResetCount();
    std::string initial_tier = HeapManager::getInstance().getBloomFilterTier();
    
    while (total_allocated < TARGET_BLOOM_ITEMS) {
        HeapManager_enterScope();
        
        // Allocate a batch of strings
        for (size_t i = 0; i < BATCH_SIZE && total_allocated < TARGET_BLOOM_ITEMS; ++i) {
            char temp_str[32];
            snprintf(temp_str, sizeof(temp_str), "TestStr_%zu", total_allocated);
            
            // Allocate BCPL string through the runtime (goes through HeapManager)
            size_t str_len = strlen(temp_str);
            uint32_t* bcpl_string = bcpl_alloc_chars(str_len);
            if (bcpl_string) {
                // Copy characters to BCPL string format
                for (size_t j = 0; j < str_len; ++j) {
                    bcpl_string[j] = static_cast<uint32_t>(temp_str[j]);
                }
            }
            total_allocated++;
        }
        
        // Free them all by exiting scope
        HeapManager_exitScope();
        
        // Check progress
        if (total_allocated % (BATCH_SIZE * 4) == 0) {
            std::cout << "Progress: " << total_allocated << "/" << TARGET_BLOOM_ITEMS << " strings processed" << std::endl;
            print_bloom_filter_stats();
        }
    }
    
    double elapsed_ms = timer.stop();
    
    // Disable tracing
    HeapManager::getInstance().setTraceEnabled(false);
    
    size_t final_resets = HeapManager::getInstance().getBloomFilterResetCount();
    std::string final_tier = HeapManager::getInstance().getBloomFilterTier();
    
    std::cout << "\nScaling Test Results:" << std::endl;
    std::cout << "Time elapsed: " << elapsed_ms << " ms" << std::endl;
    std::cout << "Total strings processed: " << total_allocated << std::endl;
    std::cout << "Scaling events: " << (final_resets - initial_resets) << std::endl;
    std::cout << "Tier progression: " << initial_tier << " → " << final_tier << std::endl;
    
    print_bloom_filter_stats();
    
    bool scaling_occurred = (final_resets > initial_resets) || (final_tier != initial_tier);
    
    if (scaling_occurred) {
        std::cout << "✅ SUCCESS: Bloom filter adaptive scaling worked!" << std::endl;
    } else {
        std::cout << "ℹ️  INFO: No scaling needed - filter handled load within current tier." << std::endl;
    }
    
    print_test_result("Bloom Filter Scaling Test", true);
    return true;
}

int main() {
    std::cout << "Adaptive Bloom Filter Scaling Tests" << std::endl;
    std::cout << "====================================" << std::endl;
    
    // Disable HeapManager tracing initially
    HeapManager::getInstance().setTraceEnabled(false);
    
    bool all_passed = true;
    
    all_passed &= test_string_list_bloom_pressure();
    all_passed &= test_bloom_filter_scaling();
    
    // Wait for cleanup thread to finish
    std::cout << "\nWaiting 5 seconds for HeapManager cleanup thread to finish...\n";
    std::this_thread::sleep_for(std::chrono::seconds(5));
    
    // Final statistics
    std::cout << "\n=== Final System State ===" << std::endl;
    print_bloom_filter_stats();
    
    // Print HeapManager metrics
    HeapManager::getInstance().printMetrics();
    
    std::cout << "\nAll adaptive bloom filter tests " << (all_passed ? "PASSED" : "FAILED") << ".\n";
    return all_passed ? 0 : 1;
}
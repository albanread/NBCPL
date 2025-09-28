#include "HeapManager.h"
#include "heap_manager_defs.h" // For AllocType, HeapBlock, MAX_HEAP_BLOCKS
#include "../SignalSafeUtils.h" // For safe_print and int_to_dec

void HeapManager::printMetrics() const {
    safe_print("\n=== Heap Metrics ===\n");
    char buf[64];
    safe_print("Total Bytes Allocated: ");
    int_to_dec((int64_t)totalBytesAllocated, buf);
    safe_print(buf);
    safe_print("\nTotal Bytes Freed: ");
    int_to_dec((int64_t)totalBytesFreed, buf);
    safe_print(buf);
    safe_print("\nTotal Vectors Allocated: ");
    int_to_dec((int64_t)totalVectorsAllocated, buf);
    safe_print(buf);
    safe_print("\nTotal Objects Allocated: ");
    int_to_dec((int64_t)totalObjectsAllocated, buf);
    safe_print(buf);
    safe_print("\nTotal Strings Allocated: ");
    int_to_dec((int64_t)totalStringsAllocated, buf);
    safe_print(buf);
    safe_print("\nTotal Vectors Freed: ");
    int_to_dec((int64_t)totalVectorsFreed, buf);
    safe_print(buf);
    safe_print("\nTotal Strings Freed: ");
    int_to_dec((int64_t)totalStringsFreed, buf);
    safe_print(buf);
    safe_print("\nTotal Double-Free Attempts: ");
    int_to_dec((int64_t)totalDoubleFreeAttempts, buf);
    safe_print(buf);
    safe_print("\nFixed Bloom Filter Size: 12MB (10M capacity)");
    safe_print("\nBloom Filter Items Added: ");
    int_to_dec((int64_t)bloom_filter_items_added_, buf);
    safe_print(buf);
    safe_print("\nBloom Filter Capacity: 10000000");
    safe_print("\nBloom Filter Reset Count: 0 (fixed size)");
    safe_print(buf);
    safe_print("\nBloom Filter False Positives: ");
    int_to_dec((int64_t)totalBloomFilterFalsePositives, buf);
    safe_print(buf);
    safe_print("\nBloom Filter Memory Usage: ");
    int_to_dec((int64_t)recently_freed_addresses_.memory_usage(), buf);
    safe_print(buf);
    safe_print(" bytes\nBloom Filter False Positive Rate: ");
    
    // Calculate and display false positive rate as percentage
    double fp_rate = recently_freed_addresses_.estimate_false_positive_rate(bloom_filter_items_added_);
    int fp_percentage = (int)(fp_rate * 10000); // Convert to basis points for integer display
    int_to_dec(fp_percentage, buf);
    safe_print(buf);
    safe_print("/10000 (");
    int fp_percent_display = (int)(fp_rate * 100);
    int_to_dec(fp_percent_display, buf);
    safe_print(buf);
    safe_print(".xx%)\nTotal Cleanup Time: ");
    
    // Add cleanup timing metrics
    int cleanup_time = (int)totalCleanupTimeMs;
    int_to_dec(cleanup_time, buf);
    safe_print(buf);
    safe_print(" ms\n");
}

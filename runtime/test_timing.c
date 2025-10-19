// test_timing.c
// Simple C test program to verify timing functionality works correctly
// This tests the timing API at the C level before integrating with BCPL

#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Helper function to create BCPL strings from C strings for testing
static uint32_t* create_test_bcpl_string(const char* c_str) {
    static uint32_t buffer[256];
    int i = 0;
    while (i < 255 && c_str[i] != '\0') {
        buffer[i] = (uint32_t)c_str[i];
        i++;
    }
    buffer[i] = 0;
    return buffer;
}

void test_basic_timing() {
    printf("=== Basic Timing Test ===\n");
    
    uint32_t* timer_name = create_test_bcpl_string("basic_test");
    
    TIMER_START(timer_name);
    
    // Do some work (busy loop)
    volatile long sum = 0;
    for (long i = 0; i < 1000000; i++) {
        sum += i;
    }
    
    TIMER_END(timer_name);
    
    printf("Basic timing test completed (sum = %ld)\n", sum);
}

void test_nested_timing() {
    printf("\n=== Nested Timing Test ===\n");
    
    uint32_t* outer_name = create_test_bcpl_string("outer_function");
    uint32_t* inner_name = create_test_bcpl_string("inner_function");
    
    TIMER_START(outer_name);
    
    // Some outer work
    volatile long outer_sum = 0;
    for (long i = 0; i < 500000; i++) {
        outer_sum += i;
    }
    
    TIMER_START(inner_name);
    
    // Some inner work
    volatile long inner_sum = 0;
    for (long i = 0; i < 300000; i++) {
        inner_sum += i * 2;
    }
    
    TIMER_END(inner_name);
    
    // More outer work
    for (long i = 0; i < 200000; i++) {
        outer_sum += i;
    }
    
    TIMER_END(outer_name);
    
    printf("Nested timing test completed (outer=%ld, inner=%ld)\n", 
           outer_sum, inner_sum);
}

void test_recursive_timing() {
    printf("\n=== Recursive Timing Test ===\n");
    
    uint32_t* recursive_name = create_test_bcpl_string("recursive_test");
    
    TIMER_START(recursive_name);
    printf("Outer recursive level\n");
    
    // Simulate nested calls with same timer name
    TIMER_START(recursive_name);
    printf("Inner recursive level\n");
    
    volatile long work = 0;
    for (long i = 0; i < 100000; i++) {
        work += i;
    }
    
    TIMER_END(recursive_name);
    printf("Back to outer recursive level\n");
    
    TIMER_END(recursive_name);
    
    printf("Recursive timing test completed (work = %ld)\n", work);
}

void test_repeated_calls() {
    printf("\n=== Repeated Calls Test ===\n");
    
    uint32_t* repeated_name = create_test_bcpl_string("repeated_function");
    
    for (int call = 1; call <= 5; call++) {
        TIMER_START(repeated_name);
        
        printf("Repeated call #%d\n", call);
        
        // Variable amount of work for each call
        volatile long sum = 0;
        for (long i = 0; i < call * 50000; i++) {
            sum += i;
        }
        
        TIMER_END(repeated_name);
    }
    
    printf("Repeated calls test completed\n");
}

void test_manual_display() {
    printf("\n=== Manual Display Test ===\n");
    
    // Do a quick timer for manual display
    uint32_t* manual_name = create_test_bcpl_string("manual_display_test");
    
    TIMER_START(manual_name);
    
    volatile long sum = 0;
    for (long i = 0; i < 75000; i++) {
        sum += i;
    }
    
    TIMER_END(manual_name);
    
    printf("Calling TIMER_DISPLAY() manually:\n");
    TIMER_DISPLAY();
}

void test_query_functions() {
    printf("\n=== Query Functions Test ===\n");
    
    uint32_t* query_name = create_test_bcpl_string("query_test");
    
    // Run timer multiple times
    for (int i = 0; i < 3; i++) {
        TIMER_START(query_name);
        
        volatile long sum = 0;
        for (long j = 0; j < 50000; j++) {
            sum += j;
        }
        
        TIMER_END(query_name);
    }
    
    // Query the results
    int64_t call_count = TIMER_GET_CALL_COUNT(query_name);
    int64_t total_ns = TIMER_GET_TOTAL_NS(query_name);
    
    printf("Query results for 'query_test':\n");
    printf("  Call count: %lld\n", (long long)call_count);
    printf("  Total time: %lld nanoseconds\n", (long long)total_ns);
    
    if (call_count > 0) {
        printf("  Average time: %lld nanoseconds per call\n", 
               (long long)(total_ns / call_count));
    }
}

void test_clear_function() {
    printf("\n=== Clear Function Test ===\n");
    
    uint32_t* clear_name = create_test_bcpl_string("clear_test");
    
    // Add some timing data
    TIMER_START(clear_name);
    volatile long sum = 0;
    for (long i = 0; i < 25000; i++) {
        sum += i;
    }
    TIMER_END(clear_name);
    
    printf("Before clear - call count: %lld\n", 
           (long long)TIMER_GET_CALL_COUNT(clear_name));
    
    // Clear all metrics
    printf("Calling TIMER_CLEAR()...\n");
    TIMER_CLEAR();
    
    printf("After clear - call count: %lld\n", 
           (long long)TIMER_GET_CALL_COUNT(clear_name));
    
    // Add new data after clear
    TIMER_START(clear_name);
    for (long i = 0; i < 15000; i++) {
        sum += i;
    }
    TIMER_END(clear_name);
    
    printf("After new timing - call count: %lld\n", 
           (long long)TIMER_GET_CALL_COUNT(clear_name));
}

int main(int argc, char* argv[]) {
    printf("BCPL Runtime Timing API Test\n");
    printf("============================\n\n");
    
    // Run all tests
    test_basic_timing();
    test_nested_timing();
    test_recursive_timing();
    test_repeated_calls();
    test_manual_display();
    test_query_functions();
    test_clear_function();
    
    printf("\n=== All Tests Completed ===\n");
    printf("Final metrics will be displayed automatically:\n");
    
    // The timing metrics will be displayed automatically when the program exits
    // via the atexit handler or finish() function
    
    return 0;
}
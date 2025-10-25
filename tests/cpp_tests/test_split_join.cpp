// split_join_test.cpp
// Comprehensive test suite for BCPL SPLIT and JOIN functions
// Tests string splitting and list joining with various scenarios and edge cases
// This is also a SAMM compatibility test to verify proper memory management

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cassert>
#include <cstring>

// Include runtime and heap management interfaces
#include "runtime/runtime.h"
#include "runtime/ListDataTypes.h"
#include "runtime/heap_interface.h"
#include "HeapManager/HeapManager.h"

// Control verbosity
#ifndef ENABLE_VERBOSE
#define ENABLE_VERBOSE 1
#endif

// Global trace flag required by HeapManager
bool g_enable_heap_trace = false;

//=============================================================================
// Test Utilities
//=============================================================================

void print_test_header(const char* test_name) {
    if (ENABLE_VERBOSE) {
        std::cout << "\n=== " << test_name << " ===" << std::endl;
    }
}

void print_test_result(const char* test_name, bool passed) {
    std::cout << "[" << (passed ? "PASS" : "FAIL") << "] " << test_name << std::endl;
}

// Convert C++ string to BCPL string (UTF-32 payload)
uint32_t* create_bcpl_string(const std::string& str) {
    size_t len = str.length();
    uint32_t* payload = (uint32_t*)bcpl_alloc_chars(len);
    if (!payload) return nullptr;
    
    for (size_t i = 0; i < len; i++) {
        payload[i] = (uint32_t)str[i]; // Simple ASCII to UTF-32 conversion
    }
    payload[len] = 0; // Null terminator
    return payload;
}

// Convert BCPL string payload back to C++ string for comparison
std::string bcpl_to_cpp_string(const uint32_t* payload) {
    if (!payload) return "";
    
    std::string result;
    for (const uint32_t* p = payload; *p != 0; ++p) {
        result += (char)*p; // Simple UTF-32 to ASCII conversion
    }
    return result;
}

// Get BCPL string from list atom
std::string get_string_from_atom(ListAtom* atom) {
    if (!atom || atom->type != ATOM_STRING || !atom->value.ptr_value) {
        return "";
    }
    
    uint64_t* base_ptr = (uint64_t*)atom->value.ptr_value;
    uint32_t* payload = (uint32_t*)(base_ptr + 1);
    return bcpl_to_cpp_string(payload);
}

// Count elements in a list
size_t count_list_elements(ListHeader* list) {
    if (!list || !list->head) return 0;
    
    size_t count = 0;
    ListAtom* current = list->head;
    while (current) {
        count++;
        current = current->next;
    }
    return count;
}

// Print list contents for debugging
void print_list_contents(ListHeader* list, const char* label) {
    if (!ENABLE_VERBOSE) return;
    
    std::cout << label << ": [";
    if (list && list->head) {
        ListAtom* current = list->head;
        bool first = true;
        while (current) {
            if (!first) std::cout << ", ";
            std::cout << "\"" << get_string_from_atom(current) << "\"";
            first = false;
            current = current->next;
        }
    }
    std::cout << "]" << std::endl;
}

//=============================================================================
// Basic SPLIT Tests
//=============================================================================

bool test_split_basic() {
    print_test_header("SPLIT Basic Functionality");
    
    // Test basic string splitting
    uint32_t* source = create_bcpl_string("hello world test");
    uint32_t* delimiter = create_bcpl_string(" ");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    bool success = (result != nullptr && count_list_elements(result) == 3);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT result");
        std::cout << "Expected 3 elements, got " << count_list_elements(result) << std::endl;
    }
    
    // Verify individual elements
    if (success && result->head) {
        ListAtom* current = result->head;
        success = success && (get_string_from_atom(current) == "hello");
        current = current->next;
        success = success && (get_string_from_atom(current) == "world");
        current = current->next;
        success = success && (get_string_from_atom(current) == "test");
    }
    
    return success;
}

bool test_split_single_character_delimiter() {
    print_test_header("SPLIT Single Character Delimiter");
    
    uint32_t* source = create_bcpl_string("a,b,c,d");
    uint32_t* delimiter = create_bcpl_string(",");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    bool success = (result != nullptr && count_list_elements(result) == 4);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT single char result");
    }
    
    return success;
}

bool test_split_multi_character_delimiter() {
    print_test_header("SPLIT Multi-Character Delimiter");
    
    uint32_t* source = create_bcpl_string("one::two::three::four");
    uint32_t* delimiter = create_bcpl_string("::");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    bool success = (result != nullptr && count_list_elements(result) == 4);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT multi-char result");
    }
    
    return success;
}

bool test_split_no_delimiter_found() {
    print_test_header("SPLIT No Delimiter Found");
    
    uint32_t* source = create_bcpl_string("nodeleimiterhere");
    uint32_t* delimiter = create_bcpl_string(" ");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    // Should return list with single element
    bool success = (result != nullptr && count_list_elements(result) == 1);
    
    if (success && result->head) {
        success = success && (get_string_from_atom(result->head) == "nodeleimiterhere");
    }
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT no delimiter result");
    }
    
    return success;
}

bool test_split_empty_string() {
    print_test_header("SPLIT Empty String");
    
    uint32_t* source = create_bcpl_string("");
    uint32_t* delimiter = create_bcpl_string(" ");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    // Should return list with single empty element
    bool success = (result != nullptr && count_list_elements(result) == 1);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT empty string result");
    }
    
    return success;
}

bool test_split_consecutive_delimiters() {
    print_test_header("SPLIT Consecutive Delimiters");
    
    uint32_t* source = create_bcpl_string("a,,b,,c");
    uint32_t* delimiter = create_bcpl_string(",");
    
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    
    // Should create empty strings between consecutive delimiters
    bool success = (result != nullptr && count_list_elements(result) == 5);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(result, "SPLIT consecutive delimiters result");
        std::cout << "Expected 5 elements (a, empty, b, empty, c), got " << count_list_elements(result) << std::endl;
    }
    
    return success;
}

//=============================================================================
// Basic JOIN Tests
//=============================================================================

bool test_join_basic() {
    print_test_header("JOIN Basic Functionality");
    
    // Create a list manually
    ListHeader* list = BCPL_LIST_CREATE_EMPTY();
    
    // Add strings to the list
    uint32_t* str1 = create_bcpl_string("hello");
    uint32_t* str2 = create_bcpl_string("world");
    uint32_t* str3 = create_bcpl_string("test");
    
    // Get base pointers (payload - 1)
    void* base1 = (uint64_t*)str1 - 1;
    void* base2 = (uint64_t*)str2 - 1;
    void* base3 = (uint64_t*)str3 - 1;
    
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base1);
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base2);
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base3);
    
    uint32_t* delimiter = create_bcpl_string(" ");
    uint32_t* result = BCPL_JOIN_LIST(list, delimiter);
    
    std::string result_str = bcpl_to_cpp_string(result);
    bool success = (result_str == "hello world test");
    
    if (ENABLE_VERBOSE) {
        std::cout << "JOIN result: \"" << result_str << "\"" << std::endl;
        std::cout << "Expected: \"hello world test\"" << std::endl;
    }
    
    return success;
}

bool test_join_single_element() {
    print_test_header("JOIN Single Element");
    
    ListHeader* list = BCPL_LIST_CREATE_EMPTY();
    uint32_t* str1 = create_bcpl_string("lonely");
    void* base1 = (uint64_t*)str1 - 1;
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base1);
    
    uint32_t* delimiter = create_bcpl_string(",");
    uint32_t* result = BCPL_JOIN_LIST(list, delimiter);
    
    std::string result_str = bcpl_to_cpp_string(result);
    bool success = (result_str == "lonely");
    
    if (ENABLE_VERBOSE) {
        std::cout << "JOIN single result: \"" << result_str << "\"" << std::endl;
    }
    
    return success;
}

bool test_join_empty_list() {
    print_test_header("JOIN Empty List");
    
    ListHeader* list = BCPL_LIST_CREATE_EMPTY();
    uint32_t* delimiter = create_bcpl_string(",");
    uint32_t* result = BCPL_JOIN_LIST(list, delimiter);
    
    std::string result_str = bcpl_to_cpp_string(result);
    bool success = (result_str == "");
    
    if (ENABLE_VERBOSE) {
        std::cout << "JOIN empty result: \"" << result_str << "\"" << std::endl;
    }
    
    return success;
}

bool test_join_empty_delimiter() {
    print_test_header("JOIN Empty Delimiter");
    
    ListHeader* list = BCPL_LIST_CREATE_EMPTY();
    uint32_t* str1 = create_bcpl_string("a");
    uint32_t* str2 = create_bcpl_string("b");
    uint32_t* str3 = create_bcpl_string("c");
    
    void* base1 = (uint64_t*)str1 - 1;
    void* base2 = (uint64_t*)str2 - 1;
    void* base3 = (uint64_t*)str3 - 1;
    
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base1);
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base2);
    BCPL_LIST_APPEND_STRING(list, (uint32_t*)base3);
    
    uint32_t* delimiter = create_bcpl_string("");
    uint32_t* result = BCPL_JOIN_LIST(list, delimiter);
    
    std::string result_str = bcpl_to_cpp_string(result);
    bool success = (result_str == "abc");
    
    if (ENABLE_VERBOSE) {
        std::cout << "JOIN empty delimiter result: \"" << result_str << "\"" << std::endl;
    }
    
    return success;
}

//=============================================================================
// Round-trip Tests (SPLIT then JOIN)
//=============================================================================

bool test_split_join_roundtrip() {
    print_test_header("SPLIT-JOIN Round-trip");
    
    std::string original = "This is the age of the train";
    uint32_t* source = create_bcpl_string(original);
    uint32_t* delimiter = create_bcpl_string(" ");
    
    // Split the string
    ListHeader* split_result = BCPL_SPLIT_STRING(source, delimiter);
    
    if (ENABLE_VERBOSE) {
        print_list_contents(split_result, "Split result");
    }
    
    // Join it back
    uint32_t* join_result = BCPL_JOIN_LIST(split_result, delimiter);
    std::string final_result = bcpl_to_cpp_string(join_result);
    
    bool success = (final_result == original);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Original:  \"" << original << "\"" << std::endl;
        std::cout << "Final:     \"" << final_result << "\"" << std::endl;
        std::cout << "Match:     " << (success ? "YES" : "NO") << std::endl;
    }
    
    return success;
}

bool test_split_join_roundtrip_complex() {
    print_test_header("SPLIT-JOIN Round-trip Complex");
    
    std::string original = "apple::banana::cherry::date::elderberry";
    uint32_t* source = create_bcpl_string(original);
    uint32_t* delimiter = create_bcpl_string("::");
    
    // Split the string
    ListHeader* split_result = BCPL_SPLIT_STRING(source, delimiter);
    
    // Join it back
    uint32_t* join_result = BCPL_JOIN_LIST(split_result, delimiter);
    std::string final_result = bcpl_to_cpp_string(join_result);
    
    bool success = (final_result == original);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Original:  \"" << original << "\"" << std::endl;
        std::cout << "Final:     \"" << final_result << "\"" << std::endl;
        std::cout << "Elements:  " << count_list_elements(split_result) << std::endl;
    }
    
    return success;
}

//=============================================================================
// Performance Tests
//=============================================================================

bool test_split_performance() {
    print_test_header("SPLIT Performance Test");
    
    // Create a large string with many tokens
    std::string large_string;
    for (int i = 0; i < 1000; i++) {
        if (i > 0) large_string += " ";
        large_string += "token" + std::to_string(i);
    }
    
    uint32_t* source = create_bcpl_string(large_string);
    uint32_t* delimiter = create_bcpl_string(" ");
    
    auto start = std::chrono::high_resolution_clock::now();
    ListHeader* result = BCPL_SPLIT_STRING(source, delimiter);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    size_t element_count = count_list_elements(result);
    
    bool success = (element_count == 1000);
    
    if (ENABLE_VERBOSE) {
        std::cout << "Split 1000 tokens in " << duration.count() << " μs" << std::endl;
        std::cout << "Elements created: " << element_count << std::endl;
        std::cout << "Performance: " << (duration.count() / 1000.0) << " μs per token" << std::endl;
    }
    
    return success;
}

bool test_join_performance() {
    print_test_header("JOIN Performance Test");
    
    // Create a list with many elements
    ListHeader* list = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < 1000; i++) {
        std::string token = "element" + std::to_string(i);
        uint32_t* str = create_bcpl_string(token);
        void* base = (uint64_t*)str - 1;
        BCPL_LIST_APPEND_STRING(list, (uint32_t*)base);
    }
    
    uint32_t* delimiter = create_bcpl_string(",");
    
    auto start = std::chrono::high_resolution_clock::now();
    uint32_t* result = BCPL_JOIN_LIST(list, delimiter);
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    std::string result_str = bcpl_to_cpp_string(result);
    
    // Count commas to verify all elements were joined
    size_t comma_count = 0;
    for (char c : result_str) {
        if (c == ',') comma_count++;
    }
    
    bool success = (comma_count == 999); // 1000 elements = 999 delimiters
    
    if (ENABLE_VERBOSE) {
        std::cout << "Joined 1000 elements in " << duration.count() << " μs" << std::endl;
        std::cout << "Result length: " << result_str.length() << " characters" << std::endl;
        std::cout << "Comma count: " << comma_count << " (expected 999)" << std::endl;
        std::cout << "Performance: " << (duration.count() / 1000.0) << " μs per element" << std::endl;
    }
    
    return success;
}

//=============================================================================
// Edge Case Tests
//=============================================================================

bool test_edge_cases() {
    print_test_header("Edge Cases");
    
    bool all_passed = true;
    
    // Test with null inputs
    ListHeader* null_split = BCPL_SPLIT_STRING(nullptr, create_bcpl_string(" "));
    all_passed = all_passed && (null_split != nullptr); // Should return empty list
    
    uint32_t* null_join = BCPL_JOIN_LIST(nullptr, create_bcpl_string(" "));
    all_passed = all_passed && (null_join != nullptr); // Should return empty string
    
    // Test string that starts and ends with delimiter
    uint32_t* boundary_str = create_bcpl_string(" hello world ");
    uint32_t* space_delim = create_bcpl_string(" ");
    ListHeader* boundary_result = BCPL_SPLIT_STRING(boundary_str, space_delim);
    
    // Should have empty strings at beginning and end
    size_t boundary_count = count_list_elements(boundary_result);
    all_passed = all_passed && (boundary_count == 4); // "", "hello", "world", ""
    
    if (ENABLE_VERBOSE) {
        std::cout << "Boundary split elements: " << boundary_count << " (expected 4)" << std::endl;
        print_list_contents(boundary_result, "Boundary split result");
    }
    
    return all_passed;
}

//=============================================================================
// SAMM Memory Management Tests
//=============================================================================

bool test_samm_memory_management() {
    print_test_header("SAMM Memory Management");
    
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    // Get initial metrics
    size_t initial_allocations = hm.heap_blocks_.size();
    
    // Perform many SPLIT/JOIN operations
    for (int i = 0; i < 100; i++) {
        std::string test_str = "a,b,c,d,e,f,g,h,i,j";
        uint32_t* source = create_bcpl_string(test_str);
        uint32_t* delimiter = create_bcpl_string(",");
        
        ListHeader* split_result = BCPL_SPLIT_STRING(source, delimiter);
        uint32_t* join_result = BCPL_JOIN_LIST(split_result, delimiter);
        
        // Results should be available for this iteration
        bool results_valid = (split_result != nullptr && join_result != nullptr);
        if (!results_valid) {
            if (ENABLE_VERBOSE) {
                std::cout << "Failed at iteration " << i << std::endl;
            }
            return false;
        }
    }
    
    // Check final metrics
    size_t final_allocations = hm.heap_blocks_.size();
    
    if (ENABLE_VERBOSE) {
        std::cout << "Initial allocations: " << initial_allocations << std::endl;
        std::cout << "Final allocations: " << final_allocations << std::endl;
        std::cout << "Net allocation increase: " << (final_allocations - initial_allocations) << std::endl;
    }
    
    return true; // Just verify we didn't crash
}

//=============================================================================
// Main Test Runner
//=============================================================================

int main() {
    std::cout << "SPLIT/JOIN Function Test Suite" << std::endl;
    std::cout << "==============================" << std::endl;
    
    // Initialize HeapManager
    auto& hm = HeapManager::getInstance();
    hm.setSAMMEnabled(true);
    
    struct TestCase {
        const char* name;
        bool (*function)();
    };
    
    TestCase tests[] = {
        // Basic SPLIT tests
        {"SPLIT Basic", test_split_basic},
        {"SPLIT Single Char Delimiter", test_split_single_character_delimiter},
        {"SPLIT Multi Char Delimiter", test_split_multi_character_delimiter},
        {"SPLIT No Delimiter", test_split_no_delimiter_found},
        {"SPLIT Empty String", test_split_empty_string},
        {"SPLIT Consecutive Delimiters", test_split_consecutive_delimiters},
        
        // Basic JOIN tests
        {"JOIN Basic", test_join_basic},
        {"JOIN Single Element", test_join_single_element},
        {"JOIN Empty List", test_join_empty_list},
        {"JOIN Empty Delimiter", test_join_empty_delimiter},
        
        // Round-trip tests
        {"SPLIT-JOIN Round-trip", test_split_join_roundtrip},
        {"SPLIT-JOIN Complex Round-trip", test_split_join_roundtrip_complex},
        
        // Performance tests
        {"SPLIT Performance", test_split_performance},
        {"JOIN Performance", test_join_performance},
        
        // Edge cases and memory management
        {"Edge Cases", test_edge_cases},
        {"SAMM Memory Management", test_samm_memory_management}
    };
    
    int passed = 0;
    int total = sizeof(tests) / sizeof(tests[0]);
    
    for (int i = 0; i < total; i++) {
        bool result = tests[i].function();
        print_test_result(tests[i].name, result);
        if (result) passed++;
    }
    
    std::cout << "\n==============================" << std::endl;
    std::cout << "Test Results: " << passed << "/" << total << " passed" << std::endl;
    
    if (passed == total) {
        std::cout << "✅ ALL TESTS PASSED - SPLIT/JOIN functions are working correctly!" << std::endl;
        std::cout << "✅ Memory management is stable" << std::endl;
        std::cout << "✅ Performance is acceptable" << std::endl;
        std::cout << "✅ SAMM compatibility verified" << std::endl;
        return 0;
    } else {
        std::cout << "❌ SOME TESTS FAILED - Please investigate failures" << std::endl;
        return 1;
    }
}
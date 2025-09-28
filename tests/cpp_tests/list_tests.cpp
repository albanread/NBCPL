// list_tests.cpp
// Tests for BCPL runtime list operations: creation, appending, scope-based freeing, and performance

#include <iostream>
#include <cassert>
#include <vector>
#include <chrono>
#include <cstring>
#include <thread>
#include <cstdlib>
#include <string>

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
    void BCPL_LIST_APPEND_INT(ListHeader* header, int64_t value);
    void BCPL_LIST_APPEND_STRING(ListHeader* header, uint32_t* value);
    void BCPL_LIST_APPEND_OBJECT(ListHeader* header, void* object_ptr);
    void* bcpl_alloc_chars(int64_t num_chars);
    void* Heap_allocObject(size_t size);
    void* OBJECT_HEAP_ALLOC(void* class_ptr);
    ListHeader* BCPL_LIST_CREATE_EMPTY(void);

    // Embedded optimization metrics
    void embedded_string_print_metrics(void);

    ListAtom* getNodeFromFreelist(void);
    ListHeader* getHeaderFromFreelist(void);
    void returnNodeToFreelist(ListAtom* node);
    void returnHeaderToFreelist(ListHeader* header);
    void printFreelistMetrics(void);
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

// Global counters for list element tracking
static size_t g_list_headers_created = 0;
static size_t g_list_atoms_created = 0;
static size_t g_list_headers_destroyed = 0;
static size_t g_list_atoms_destroyed = 0;

// Utility: Create a list of N integers using direct pointer manipulation like BCPL
ListHeader* create_int_list(size_t n) {
    // Direct freelist allocation like BCPL compiler generates
    ListHeader* header = getHeaderFromFreelist();
    assert(header && "Failed to allocate list header");
    header->type = ATOM_SENTINEL;
    header->contains_literals = 0;
    header->length = 0;
    header->head = nullptr;
    header->tail = nullptr;
    
    g_list_headers_created++;  // Track list header creation

    for (size_t i = 0; i < n; ++i) {
        // Direct pointer manipulation like BCPL compiler does
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = ATOM_INT;
        new_node->pad = 0;
        new_node->value.int_value = static_cast<int64_t>(i);
        new_node->next = nullptr;

        if (header->head == nullptr) {
            header->head = new_node;
            header->tail = new_node;
        } else {
            header->tail->next = new_node;
            header->tail = new_node;
        }
        header->length++;
        g_list_atoms_created++;  // Track each list atom creation
    }
    return header;
}

// Utility: Count list elements (for tracking only - SAMM handles freeing)
void count_list_for_destroy(ListHeader* header) {
    if (!header) return;
    
    // Count the atoms for tracking purposes
    ListAtom* atom = header->head;
    while (atom) {
        g_list_atoms_destroyed++;
        atom = atom->next;
    }
    g_list_headers_destroyed++;  // Count header destruction
}

// Test 1: Simple list creation and destruction
bool test_list_create_destroy() {
    print_test_header("List Creation/Destruction");
    const size_t N = 1000;
    
    HeapManager_enterScope();
    ListHeader* header = create_int_list(N);
    bool ok = (header && header->length == N);

    // Check values
    ListAtom* atom = header->head;
    size_t idx = 0;
    while (atom) {
        if (atom->type != ATOM_INT || atom->value.int_value != static_cast<int64_t>(idx)) {
            ok = false;
            break;
        }
        atom = atom->next;
        idx++;
    }
    ok = ok && (idx == N);

    count_list_for_destroy(header);  // Count for tracking
    HeapManager_exitScope();  // SAMM will free everything
    print_test_result("List Creation/Destruction", ok);
    return ok;
}

// Test 2: Appending to a list
bool test_list_append() {
    print_test_header("List Append");
    
    HeapManager_enterScope();
    ListHeader* header = create_int_list(10);
    size_t orig_len = header->length;

    // Append 10 more using direct pointer manipulation
    for (size_t i = 0; i < 10; ++i) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = ATOM_INT;
        new_node->pad = 0;
        new_node->value.int_value = static_cast<int64_t>(orig_len + i);
        new_node->next = nullptr;
        header->tail->next = new_node;
        header->tail = new_node;
        header->length++;
        g_list_atoms_created++;  // Track appended atoms
    }

    // Verify
    bool ok = (header->length == 20);
    ListAtom* atom = header->head;
    size_t idx = 0;
    while (atom) {
        if (atom->type != ATOM_INT || atom->value.int_value != static_cast<int64_t>(idx)) {
            ok = false;
            break;
        }
        atom = atom->next;
        idx++;
    }
    ok = ok && (idx == 20);

    count_list_for_destroy(header);  // Count for tracking
    HeapManager_exitScope();  // SAMM will free everything
    print_test_result("List Append", ok);
    return ok;
}

// Test 3: Scope-based freeing (SAMM)
bool test_scope_freeing() {
    print_test_header("Scope-based Freeing (SAMM)");
    HeapManager_setSAMMEnabled(1);

    HeapManager_enterScope();
    ListHeader* header = getHeaderFromFreelist();
    assert(header);
    header->type = ATOM_SENTINEL;
    header->contains_literals = 0;
    header->length = 0;
    header->head = nullptr;
    header->tail = nullptr;
    g_list_headers_created++;  // Track list header creation
    // Add a few atoms using direct pointer manipulation
    for (int i = 0; i < 5; ++i) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = ATOM_INT;
        new_node->pad = 0;
        new_node->value.int_value = i;
        new_node->next = nullptr;
        if (header->head == nullptr) {
            header->head = new_node;
            header->tail = new_node;
        } else {
            header->tail->next = new_node;
            header->tail = new_node;
        }
        header->length++;
        g_list_atoms_created++;  // Track each atom creation
    }
    HeapManager_exitScope(); // Should free header and atoms
    // We can't check header directly, but this should not crash or leak header.
    print_test_result("Scope-based Freeing (SAMM)", true);
    return true;
}

// Test 4: Performance of list creation/destruction
bool test_list_performance() {
    print_test_header("List Performance (Creation/Destruction)");
    const size_t N = 100000;
    Timer timer;
    
    HeapManager_enterScope();
    timer.start();
    ListHeader* header = create_int_list(N);
    double create_ms = timer.stop();

    timer.start();
    count_list_for_destroy(header);  // Count for tracking
    HeapManager_exitScope();  // SAMM cleanup
    double destroy_ms = timer.stop();

    std::cout << "Created " << N << " element list in " << create_ms << " ms\n";
    std::cout << "SAMM cleanup took " << destroy_ms << " ms\n";
    print_test_result("List Performance", true);
    return true;
}

// Test 5: Small lists test (5000 lists with 6 items each)
bool test_small_lists() {
    print_test_header("Small Lists Creation (5000 lists × 6 items)");
    const size_t NUM_LISTS = 5000;
    const size_t ITEMS_PER_LIST = 6;
    
    size_t headers_before = g_list_headers_created;
    size_t atoms_before = g_list_atoms_created;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_LISTS; ++i) {
        ListHeader* header = create_int_list(ITEMS_PER_LIST);
        // Lists will be cleaned up by SAMM when scope exits
    }
    HeapManager_exitScope();
    
    double create_ms = timer.stop();
    
    size_t headers_created = g_list_headers_created - headers_before;
    size_t atoms_created = g_list_atoms_created - atoms_before;
    
    std::cout << "Created " << NUM_LISTS << " lists with " << ITEMS_PER_LIST << " items each in " << create_ms << " ms\n";
    std::cout << "Total list headers created: " << headers_created << std::endl;
    std::cout << "Total list atoms created: " << atoms_created << std::endl;
    std::cout << "Expected: " << NUM_LISTS << " headers, " << (NUM_LISTS * ITEMS_PER_LIST) << " atoms\n";
    
    bool headers_correct = (headers_created == NUM_LISTS);
    bool atoms_correct = (atoms_created == (NUM_LISTS * ITEMS_PER_LIST));
    bool passed = headers_correct && atoms_correct;
    
    print_test_result("Small Lists Creation", passed);
    return passed;
}

// Test 6: Append speed test for small lists (create small list, append 6 items, repeat 5000 times)
bool test_small_list_append_speed() {
    print_test_header("Small List Append Speed (5000 × append 6 items to new lists)");
    const size_t NUM_ITERATIONS = 5000;
    const size_t ITEMS_TO_APPEND = 6;
    
    size_t atoms_before = g_list_atoms_created;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        // Create a small initial list (1 item)
        ListHeader* header = create_int_list(1);
        
        // Append 6 more items using direct pointer manipulation
        for (size_t j = 0; j < ITEMS_TO_APPEND; ++j) {
            ListAtom* new_node = getNodeFromFreelist();
            new_node->type = ATOM_INT;
            new_node->pad = 0;
            new_node->value.int_value = static_cast<int64_t>(j + 1000);
            new_node->next = nullptr;
            header->tail->next = new_node;
            header->tail = new_node;
            header->length++;
            g_list_atoms_created++;  // Track appended atoms
        }
    }
    HeapManager_exitScope();
    
    double append_ms = timer.stop();
    
    size_t total_atoms_appended = g_list_atoms_created - atoms_before - NUM_ITERATIONS; // Subtract initial atoms
    
    std::cout << "Appended " << ITEMS_TO_APPEND << " items to " << NUM_ITERATIONS << " small lists in " << append_ms << " ms\n";
    std::cout << "Total atoms appended: " << total_atoms_appended << std::endl;
    std::cout << "Expected appends: " << (NUM_ITERATIONS * ITEMS_TO_APPEND) << std::endl;
    std::cout << "Append rate: " << (total_atoms_appended / append_ms * 1000) << " appends/second\n";
    
    bool passed = (total_atoms_appended == NUM_ITERATIONS * ITEMS_TO_APPEND);
    print_test_result("Small List Append Speed", passed);
    return passed;
}

// Test 7: Append speed test for large lists (create large list, append 6 items, repeat 5000 times)
bool test_large_list_append_speed() {
    print_test_header("Large List Append Speed (5000 × append 6 items to 1000-item lists)");
    const size_t NUM_ITERATIONS = 5000;
    const size_t ITEMS_TO_APPEND = 6;
    const size_t LARGE_LIST_SIZE = 1000;
    
    size_t atoms_before = g_list_atoms_created;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        // Create a large initial list
        ListHeader* header = create_int_list(LARGE_LIST_SIZE);
        
        // Append 6 more items using direct pointer manipulation
        for (size_t j = 0; j < ITEMS_TO_APPEND; ++j) {
            ListAtom* new_node = getNodeFromFreelist();
            new_node->type = ATOM_INT;
            new_node->pad = 0;
            new_node->value.int_value = static_cast<int64_t>(j + 2000);
            new_node->next = nullptr;
            header->tail->next = new_node;
            header->tail = new_node;
            header->length++;
            g_list_atoms_created++;  // Track appended atoms
        }
    }
    HeapManager_exitScope();
    
    double append_ms = timer.stop();
    
    size_t total_atoms_appended = g_list_atoms_created - atoms_before - (NUM_ITERATIONS * LARGE_LIST_SIZE); // Subtract initial atoms
    
    std::cout << "Appended " << ITEMS_TO_APPEND << " items to " << NUM_ITERATIONS << " large lists (" << LARGE_LIST_SIZE << " items each) in " << append_ms << " ms\n";
    std::cout << "Total atoms appended: " << total_atoms_appended << std::endl;
    std::cout << "Expected appends: " << (NUM_ITERATIONS * ITEMS_TO_APPEND) << std::endl;
    std::cout << "Append rate: " << (total_atoms_appended / append_ms * 1000) << " appends/second\n";
    
    bool passed = (total_atoms_appended == NUM_ITERATIONS * ITEMS_TO_APPEND);
    print_test_result("Large List Append Speed", passed);
    return passed;
}

// Test 8: Optimized small list append speed using BCPL_LIST_APPEND_INT
bool test_small_list_append_speed_optimized() {
    print_test_header("Optimized Small List Append Speed (5000 × append 6 items using BCPL_LIST_APPEND_INT)");
    const size_t NUM_ITERATIONS = 5000;
    const size_t ITEMS_TO_APPEND = 6;
    
    size_t atoms_before = g_list_atoms_created;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        // Create a small initial list (1 item)
        ListHeader* header = create_int_list(1);
        
        // Append 6 more items using optimized BCPL function
        for (size_t j = 0; j < ITEMS_TO_APPEND; ++j) {
            BCPL_LIST_APPEND_INT(header, static_cast<int64_t>(j + 1000));
            g_list_atoms_created++;  // Track appended atoms
        }
    }
    HeapManager_exitScope();
    
    double append_ms = timer.stop();
    
    size_t total_atoms_appended = g_list_atoms_created - atoms_before - NUM_ITERATIONS; // Subtract initial atoms
    
    std::cout << "Appended " << ITEMS_TO_APPEND << " items to " << NUM_ITERATIONS << " small lists in " << append_ms << " ms\n";
    std::cout << "Total atoms appended: " << total_atoms_appended << std::endl;
    std::cout << "Expected appends: " << (NUM_ITERATIONS * ITEMS_TO_APPEND) << std::endl;
    std::cout << "Append rate: " << (total_atoms_appended / append_ms * 1000) << " appends/second\n";
    
    bool passed = (total_atoms_appended == NUM_ITERATIONS * ITEMS_TO_APPEND);
    print_test_result("Optimized Small List Append Speed", passed);
    return passed;
}

// Test 9: Optimized large list append speed using BCPL_LIST_APPEND_INT
bool test_large_list_append_speed_optimized() {
    print_test_header("Optimized Large List Append Speed (5000 × append 6 items using BCPL_LIST_APPEND_INT)");
    const size_t NUM_ITERATIONS = 5000;
    const size_t ITEMS_TO_APPEND = 6;
    const size_t LARGE_LIST_SIZE = 1000;
    
    size_t atoms_before = g_list_atoms_created;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_ITERATIONS; ++i) {
        // Create a large initial list
        ListHeader* header = create_int_list(LARGE_LIST_SIZE);
        
        // Append 6 more items using optimized BCPL function
        for (size_t j = 0; j < ITEMS_TO_APPEND; ++j) {
            BCPL_LIST_APPEND_INT(header, static_cast<int64_t>(j + 2000));
            g_list_atoms_created++;  // Track appended atoms
        }
    }
    HeapManager_exitScope();
    
    double append_ms = timer.stop();
    
    size_t total_atoms_appended = g_list_atoms_created - atoms_before - (NUM_ITERATIONS * LARGE_LIST_SIZE); // Subtract initial atoms
    
    std::cout << "Appended " << ITEMS_TO_APPEND << " items to " << NUM_ITERATIONS << " large lists (" << LARGE_LIST_SIZE << " items each) in " << append_ms << " ms\n";
    std::cout << "Total atoms appended: " << total_atoms_appended << std::endl;
    std::cout << "Expected appends: " << (NUM_ITERATIONS * ITEMS_TO_APPEND) << std::endl;
    std::cout << "Append rate: " << (total_atoms_appended / append_ms * 1000) << " appends/second\n";
    
    bool passed = (total_atoms_appended == NUM_ITERATIONS * ITEMS_TO_APPEND);
    print_test_result("Optimized Large List Append Speed", passed);
    return passed;
}

// Test 10: Direct comparison - manual vs BCPL function append
bool test_append_comparison() {
    print_test_header("Append Comparison: Manual vs BCPL_LIST_APPEND_INT (10000 appends each)");
    const size_t NUM_APPENDS = 10000;
    
    // Test 1: Manual append
    Timer timer1;
    timer1.start();
    
    HeapManager_enterScope();
    ListHeader* header1 = create_int_list(1);
    for (size_t i = 0; i < NUM_APPENDS; ++i) {
        ListAtom* new_node = getNodeFromFreelist();
        new_node->type = ATOM_INT;
        new_node->pad = 0;
        new_node->value.int_value = static_cast<int64_t>(i);
        new_node->next = nullptr;
        header1->tail->next = new_node;
        header1->tail = new_node;
        header1->length++;
    }
    HeapManager_exitScope();
    
    double manual_ms = timer1.stop();
    
    // Test 2: BCPL function append
    Timer timer2;
    timer2.start();
    
    HeapManager_enterScope();
    ListHeader* header2 = create_int_list(1);
    for (size_t i = 0; i < NUM_APPENDS; ++i) {
        BCPL_LIST_APPEND_INT(header2, static_cast<int64_t>(i));
    }
    HeapManager_exitScope();
    
    double bcpl_ms = timer2.stop();
    
    std::cout << "Manual append: " << NUM_APPENDS << " items in " << manual_ms << " ms\n";
    std::cout << "Manual rate: " << (NUM_APPENDS / manual_ms * 1000) << " appends/second\n";
    std::cout << "BCPL append: " << NUM_APPENDS << " items in " << bcpl_ms << " ms\n";
    std::cout << "BCPL rate: " << (NUM_APPENDS / bcpl_ms * 1000) << " appends/second\n";
    
    double speedup = manual_ms / bcpl_ms;
    std::cout << "BCPL speedup: " << speedup << "x ";
    if (speedup > 1.0) {
        std::cout << "(BCPL is faster)" << std::endl;
    } else {
        std::cout << "(Manual is faster)" << std::endl;
    }
    
    bool passed = (header1->length == NUM_APPENDS + 1) && (header2->length == NUM_APPENDS + 1);
    print_test_result("Append Comparison", passed);
    return passed;
}

// Test 11: O(1) vs O(n) append behavior test
bool test_append_scaling_behavior() {
    print_test_header("Append Scaling Behavior (O(1) vs O(n) detection)");
    
    const size_t NUM_TESTS = 5;
    const size_t BASE_SIZE = 1000;
    const size_t NUM_APPENDS = 1000;
    
    std::cout << "Testing append performance as list size increases:\n";
    std::cout << "List Size\tAppend Time (ms)\tRate (appends/sec)\tScaling Factor\n";
    
    double prev_time = 0.0;
    
    HeapManager_enterScope();
    
    for (size_t test = 0; test < NUM_TESTS; ++test) {
        size_t list_size = BASE_SIZE * (test + 1);
        
        // Create list of specified size
        ListHeader* header = create_int_list(list_size);
        
        // Time the append operations
        Timer timer;
        timer.start();
        
        for (size_t i = 0; i < NUM_APPENDS; ++i) {
            BCPL_LIST_APPEND_INT(header, static_cast<int64_t>(i));
        }
        
        double append_time = timer.stop();
        double append_rate = NUM_APPENDS / append_time * 1000;
        
        double scaling_factor = (prev_time > 0.0) ? (append_time / prev_time) : 1.0;
        
        std::cout << list_size << "\t\t" << append_time << "\t\t" 
                  << static_cast<size_t>(append_rate) << "\t\t" << scaling_factor << std::endl;
        
        prev_time = append_time;
    }
    
    HeapManager_exitScope();
    
    std::cout << "\nIf append is O(1), scaling factor should be ~1.0\n";
    std::cout << "If append is O(n), scaling factor will increase with list size\n";
    
    print_test_result("Append Scaling Behavior", true);
    return true;
}

// Test 12: Pure append benchmark - shows freelist efficiency
bool test_pure_append_benchmark() {
    print_test_header("Pure Append Benchmark (100,000 appends using BCPL_LIST_APPEND_INT)");
    const size_t NUM_APPENDS = 100000;
    
    HeapManager_enterScope();
    
    // Create one list and keep appending to it
    ListHeader* header = create_int_list(0);  // Start with empty list
    
    Timer timer;
    timer.start();
    
    // Pure append loop - this is where BCPL freelist shines
    for (size_t i = 0; i < NUM_APPENDS; ++i) {
        BCPL_LIST_APPEND_INT(header, static_cast<int64_t>(i));
    }
    
    double append_ms = timer.stop();
    
    HeapManager_exitScope();
    
    std::cout << "Pure append: " << NUM_APPENDS << " items in " << append_ms << " ms\n";
    std::cout << "Append rate: " << (NUM_APPENDS / append_ms * 1000) << " appends/second\n";
    std::cout << "Final list length: " << header->length << std::endl;
    
    // Print freelist metrics to show efficiency
    std::cout << "Freelist metrics after benchmark:\n";
    printFreelistMetrics();
    
    bool passed = (header->length == NUM_APPENDS);
    print_test_result("Pure Append Benchmark", passed);
    return passed;
}

// Helper function to create BCPL string from C++ string (baseline)
uint32_t* create_bcpl_string(const std::string& cpp_string) {
    uint32_t* result = static_cast<uint32_t*>(bcpl_alloc_chars(cpp_string.length()));
    if (!result) return nullptr;
    
    for (size_t i = 0; i < cpp_string.length(); i++) {
        result[i] = static_cast<uint32_t>(static_cast<unsigned char>(cpp_string[i]));
    }
    // Null terminator is handled by bcpl_alloc_chars
    return result;
}

// All optimizations are now embedded in bcpl_alloc_chars and create_bcpl_string
// So this function is identical to the baseline version
uint32_t* create_bcpl_string_optimized(const std::string& cpp_string) {
    return create_bcpl_string(cpp_string);
}

// Helper to generate random words for testing
std::string generate_random_word(size_t min_len = 3, size_t max_len = 12) {
    static const char* words[] = {
        "the", "quick", "brown", "fox", "jumps", "over", "lazy", "dog",
        "hello", "world", "computer", "science", "programming", "language",
        "memory", "allocation", "freelist", "performance", "benchmark", "test",
        "random", "string", "data", "structure", "algorithm", "efficiency",
        "paragraph", "sentence", "word", "character", "unicode", "text",
        "append", "insert", "delete", "search", "sort", "merge", "split",
        "join", "concatenate", "compare", "length", "size", "capacity"
    };
    static const size_t word_count = sizeof(words) / sizeof(words[0]);
    
    if (rand() % 3 == 0) {
        // Generate compound word occasionally
        std::string word1 = words[rand() % word_count];
        std::string word2 = words[rand() % word_count];
        return word1 + "_" + word2;
    } else {
        return words[rand() % word_count];
    }
}

// Helper to generate paragraph-like text
std::string generate_paragraph_text(size_t word_count = 20) {
    std::string paragraph;
    for (size_t i = 0; i < word_count; ++i) {
        if (i > 0) paragraph += " ";
        paragraph += generate_random_word();
        
        // Add punctuation occasionally
        if (i > 0 && (i + 1) % 7 == 0 && i < word_count - 1) {
            if (rand() % 2 == 0) paragraph += ",";
        }
    }
    paragraph += ".";
    return paragraph;
}

// Test 12: BCPL String List Creation and Append Speed
bool test_string_list_creation() {
    print_test_header("BCPL String List Creation (1000 lists × 10 strings each)");
    const size_t NUM_LISTS = 1000;
    const size_t STRINGS_PER_LIST = 10;
    
    size_t strings_created = 0;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    
    for (size_t i = 0; i < NUM_LISTS; ++i) {
        ListHeader* header = getHeaderFromFreelist();
        header->type = ATOM_SENTINEL;
        header->head = nullptr;
        header->tail = nullptr;
        header->length = 0;
        
        for (size_t j = 0; j < STRINGS_PER_LIST; ++j) {
            std::string test_word = generate_random_word();
            uint32_t* bcpl_string = create_bcpl_string(test_word);
            BCPL_LIST_APPEND_STRING(header, bcpl_string);
            strings_created++;
        }
    }
    
    HeapManager_exitScope();
    
    double creation_ms = timer.stop();
    
    std::cout << "Created " << NUM_LISTS << " string lists with " << STRINGS_PER_LIST 
              << " strings each in " << creation_ms << " ms\n";
    std::cout << "Total strings created: " << strings_created << std::endl;
    std::cout << "String creation rate: " << (strings_created / creation_ms * 1000) << " strings/second\n";
    
    bool passed = (strings_created == NUM_LISTS * STRINGS_PER_LIST);
    print_test_result("BCPL String List Creation", passed);
    return passed;
}

// Test 13: Large String List Performance (paragraph-like content)
bool test_large_string_list_performance() {
    print_test_header("Large String List Performance (100 lists × 50 paragraph strings)");
    const size_t NUM_LISTS = 100;
    const size_t STRINGS_PER_LIST = 50;
    
    size_t total_string_chars = 0;
    size_t strings_created = 0;
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    
    for (size_t i = 0; i < NUM_LISTS; ++i) {
        ListHeader* header = getHeaderFromFreelist();
        header->type = ATOM_SENTINEL;
        header->head = nullptr;
        header->tail = nullptr;
        header->length = 0;
        
        for (size_t j = 0; j < STRINGS_PER_LIST; ++j) {
            // Generate paragraph-like text
            std::string paragraph = generate_paragraph_text(8 + (rand() % 15));
            uint32_t* bcpl_string = create_bcpl_string(paragraph);
            BCPL_LIST_APPEND_STRING(header, bcpl_string);
            
            total_string_chars += paragraph.length();
            strings_created++;
        }
    }
    
    HeapManager_exitScope();
    
    double creation_ms = timer.stop();
    
    std::cout << "Created " << NUM_LISTS << " large string lists with " << STRINGS_PER_LIST 
              << " strings each in " << creation_ms << " ms\n";
    std::cout << "Total strings: " << strings_created << ", Total characters: " << total_string_chars << std::endl;
    std::cout << "Average string length: " << (total_string_chars / strings_created) << " characters\n";
    std::cout << "String creation rate: " << (strings_created / creation_ms * 1000) << " strings/second\n";
    std::cout << "Character processing rate: " << (total_string_chars / creation_ms * 1000) << " chars/second\n";
    
    bool passed = (strings_created == NUM_LISTS * STRINGS_PER_LIST);
    print_test_result("Large String List Performance", passed);
    return passed;
}

// Test 14: String vs Int Append Comparison
bool test_string_vs_int_append_comparison() {
    print_test_header("String vs Int Append Comparison (10,000 appends each)");
    const size_t NUM_APPENDS = 10000;
    
    // Test 1: Int appends
    Timer timer1;
    timer1.start();
    
    HeapManager_enterScope();
    ListHeader* int_header = getHeaderFromFreelist();
    int_header->type = ATOM_SENTINEL;
    int_header->head = nullptr;
    int_header->tail = nullptr;
    int_header->length = 0;
    
    for (size_t i = 0; i < NUM_APPENDS; ++i) {
        BCPL_LIST_APPEND_INT(int_header, static_cast<int64_t>(i));
    }
    HeapManager_exitScope();
    
    double int_ms = timer1.stop();
    
    // Test 2: String appends
    Timer timer2;
    timer2.start();
    
    HeapManager_enterScope();
    ListHeader* string_header = getHeaderFromFreelist();
    string_header->type = ATOM_SENTINEL;
    string_header->head = nullptr;
    string_header->tail = nullptr;
    string_header->length = 0;
    
    for (size_t i = 0; i < NUM_APPENDS; ++i) {
        std::string word = generate_random_word();
        uint32_t* bcpl_string = create_bcpl_string(word);
        BCPL_LIST_APPEND_STRING(string_header, bcpl_string);
    }
    HeapManager_exitScope();
    
    double string_ms = timer2.stop();
    
    std::cout << "Int append: " << NUM_APPENDS << " items in " << int_ms << " ms\n";
    std::cout << "Int rate: " << (NUM_APPENDS / int_ms * 1000) << " appends/second\n";
    std::cout << "String append: " << NUM_APPENDS << " items in " << string_ms << " ms\n";
    std::cout << "String rate: " << (NUM_APPENDS / string_ms * 1000) << " appends/second\n";
    
    double ratio = string_ms / int_ms;
    std::cout << "String overhead: " << ratio << "x ";
    if (ratio > 2.0) {
        std::cout << "(significant overhead due to string allocation)" << std::endl;
    } else if (ratio > 1.5) {
        std::cout << "(moderate overhead)" << std::endl;
    } else {
        std::cout << "(minimal overhead)" << std::endl;
    }
    
    bool passed = (int_header->length == NUM_APPENDS) && (string_header->length == NUM_APPENDS);
    print_test_result("String vs Int Append Comparison", passed);
    return passed;
}

// Test 15: Mixed Content String Lists (simulating real-world usage)
bool test_mixed_content_string_lists() {
    print_test_header("Mixed Content String Lists (500 scopes × varying list sizes)");
    const size_t NUM_SCOPES = 500;
    
    size_t total_lists = 0;
    size_t total_strings = 0;
    size_t total_chars = 0;
    
    Timer timer;
    timer.start();
    
    srand(42); // Reproducible randomness
    
    for (size_t scope = 0; scope < NUM_SCOPES; ++scope) {
        HeapManager_enterScope();
        
        // Variable number of lists per scope (1-5)
        size_t lists_in_scope = 1 + (rand() % 5);
        
        for (size_t list_idx = 0; list_idx < lists_in_scope; ++list_idx) {
            ListHeader* header = getHeaderFromFreelist();
            header->type = ATOM_SENTINEL;
            header->head = nullptr;
            header->tail = nullptr;
            header->length = 0;
            
            // Variable strings per list (5-25)
            size_t strings_in_list = 5 + (rand() % 21);
            
            for (size_t str_idx = 0; str_idx < strings_in_list; ++str_idx) {
                std::string content;
                
                // Mix different types of content
                int content_type = rand() % 4;
                switch (content_type) {
                    case 0: // Single word
                        content = generate_random_word();
                        break;
                    case 1: // Short phrase (2-4 words)
                        content = generate_paragraph_text(2 + (rand() % 3));
                        break;
                    case 2: // Medium sentence (5-10 words)
                        content = generate_paragraph_text(5 + (rand() % 6));
                        break;
                    case 3: // Long sentence (10-20 words)
                        content = generate_paragraph_text(10 + (rand() % 11));
                        break;
                }
                
                uint32_t* bcpl_string = create_bcpl_string(content);
                BCPL_LIST_APPEND_STRING(header, bcpl_string);
                
                total_strings++;
                total_chars += content.length();
            }
            total_lists++;
        }
        
        HeapManager_exitScope(); // Clean up this scope
    }
    
    double total_ms = timer.stop();
    
    std::cout << "Created " << total_lists << " string lists across " << NUM_SCOPES << " scopes in " << total_ms << " ms\n";
    std::cout << "Total strings: " << total_strings << ", Total characters: " << total_chars << std::endl;
    std::cout << "Average strings per list: " << (total_strings / total_lists) << std::endl;
    std::cout << "Average characters per string: " << (total_chars / total_strings) << std::endl;
    std::cout << "String creation rate: " << (total_strings / total_ms * 1000) << " strings/second\n";
    std::cout << "Character processing rate: " << (total_chars / total_ms * 1000) << " chars/second\n";
    
    print_test_result("Mixed Content String Lists", true);
    return true;
}

// Test 16: String Performance with Embedded Optimizations
bool test_optimized_string_performance() {
    print_test_header("String Performance with Embedded Optimizations (100,000 strings)");
    const size_t NUM_STRINGS = 100000;
    
    std::cout << "Testing string allocation with embedded optimizations...\n";
    std::cout << "Note: All bcpl_alloc_chars() calls now use optimized string pool + SIMD copying\n";
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    std::vector<uint32_t*> test_strings;
    for (size_t i = 0; i < NUM_STRINGS; ++i) {
        std::string word = generate_random_word();
        uint32_t* bcpl_string = create_bcpl_string(word);
        test_strings.push_back(bcpl_string);
    }
    HeapManager_exitScope();
    
    double total_ms = timer.stop();
    
    std::cout << "OPTIMIZED RUNTIME: " << NUM_STRINGS << " strings in " << total_ms << " ms\n";
    std::cout << "String creation rate: " << (NUM_STRINGS / total_ms * 1000) << " strings/second\n";
    
    // Print optimization metrics
    std::cout << "\nEmbedded optimization metrics:\n";
    embedded_string_print_metrics();
    
    bool passed = (test_strings.size() == NUM_STRINGS);
    print_test_result("String Performance with Embedded Optimizations", passed);
    return passed;
}

// Test 17: String Allocation Bottleneck Analysis
bool test_string_allocation_bottleneck() {
    print_test_header("String Allocation Bottleneck Analysis (profiling current implementation)");
    const size_t NUM_STRINGS = 50000;
    
    std::cout << "Analyzing where time is spent in string allocation...\n";
    
    // Test 1: Pure C++ string creation (baseline)
    Timer timer1;
    timer1.start();
    std::vector<std::string> cpp_strings;
    for (size_t i = 0; i < NUM_STRINGS; ++i) {
        std::string word = generate_random_word();
        cpp_strings.push_back(word);
    }
    double cpp_creation_ms = timer1.stop();
    
    // Test 2: BCPL string allocation only (no copying)
    Timer timer2;
    timer2.start();
    HeapManager_enterScope();
    std::vector<uint32_t*> bcpl_strings_empty;
    for (size_t i = 0; i < NUM_STRINGS; ++i) {
        size_t len = 5 + (i % 20); // Variable lengths 5-24
        uint32_t* bcpl_string = static_cast<uint32_t*>(bcpl_alloc_chars(len));
        bcpl_strings_empty.push_back(bcpl_string);
    }
    HeapManager_exitScope();
    double allocation_only_ms = timer2.stop();
    
    // Test 3: Full BCPL string creation (allocation + copying)
    Timer timer3;
    timer3.start();
    HeapManager_enterScope();
    std::vector<uint32_t*> bcpl_strings_full;
    for (size_t i = 0; i < NUM_STRINGS; ++i) {
        std::string word = generate_random_word();
        uint32_t* bcpl_string = create_bcpl_string(word);
        bcpl_strings_full.push_back(bcpl_string);
    }
    HeapManager_exitScope();
    double full_creation_ms = timer3.stop();
    
    // Analysis
    double copying_overhead = full_creation_ms - allocation_only_ms;
    double allocation_overhead = allocation_only_ms - cpp_creation_ms;
    
    std::cout << "Performance breakdown for " << NUM_STRINGS << " strings:\n";
    std::cout << "C++ string creation: " << cpp_creation_ms << " ms (" 
              << (NUM_STRINGS / cpp_creation_ms * 1000) << " strings/sec)\n";
    std::cout << "BCPL allocation only: " << allocation_only_ms << " ms (" 
              << (NUM_STRINGS / allocation_only_ms * 1000) << " strings/sec)\n";
    std::cout << "Full BCPL creation: " << full_creation_ms << " ms (" 
              << (NUM_STRINGS / full_creation_ms * 1000) << " strings/sec)\n";
    
    std::cout << "\nBottleneck analysis:\n";
    std::cout << "Allocation overhead: " << allocation_overhead << " ms (" 
              << (allocation_overhead / full_creation_ms * 100) << "%)\n";
    std::cout << "Character copying overhead: " << copying_overhead << " ms (" 
              << (copying_overhead / full_creation_ms * 100) << "%)\n";
    
    std::cout << "\nOptimization opportunities:\n";
    if (allocation_overhead > copying_overhead) {
        std::cout << "• PRIMARY: Optimize bcpl_alloc_chars() - use string pool/arena allocator\n";
        std::cout << "• SECONDARY: Optimize character copying - use SIMD/bulk copy\n";
    } else {
        std::cout << "• PRIMARY: Optimize character copying - use SIMD/bulk copy\n";
        std::cout << "• SECONDARY: Optimize bcpl_alloc_chars() - use string pool\n";
    }
    
    print_test_result("String Allocation Bottleneck Analysis", true);
    return true;
}

// Test 18: String List Integration with Embedded Optimizations
bool test_optimized_string_list_integration() {
    print_test_header("String List Integration with Embedded Optimizations (1000 lists × 20 strings)");
    const size_t NUM_LISTS = 1000;
    const size_t STRINGS_PER_LIST = 20;
    
    std::cout << "Testing string list creation with optimized runtime...\n";
    
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    for (size_t i = 0; i < NUM_LISTS; ++i) {
        ListHeader* header = getHeaderFromFreelist();
        header->type = ATOM_SENTINEL;
        header->head = nullptr;
        header->tail = nullptr;
        header->length = 0;
        
        for (size_t j = 0; j < STRINGS_PER_LIST; ++j) {
            std::string word = generate_random_word();
            uint32_t* bcpl_string = create_bcpl_string(word);
            BCPL_LIST_APPEND_STRING(header, bcpl_string);
        }
    }
    HeapManager_exitScope();
    
    double total_ms = timer.stop();
    
    size_t total_strings = NUM_LISTS * STRINGS_PER_LIST;
    
    std::cout << "OPTIMIZED RUNTIME: " << total_strings << " strings in " << total_ms << " ms\n";
    std::cout << "String creation rate: " << (total_strings / total_ms * 1000) << " strings/second\n";
    
    std::cout << "Real-world performance benefits:\n";
    std::cout << "• Document parsing: High-speed text processing\n";
    std::cout << "• Log analysis: Efficient string handling\n";
    std::cout << "• Data processing: Fast string list operations\n";
    
    print_test_result("String List Integration with Embedded Optimizations", true);
    return true;
}

// ============================================================================
// Object Testing Infrastructure
// ============================================================================

// Mock RELEASE methods - no-ops since HeapManager frees object memory
extern "C" void mock_release_noop() {
    // No-op: HeapManager handles freeing the object's own memory
    // Only needed if object owned external resources (files, other objects, etc.)
}

// Mock CREATE methods
extern "C" void mock_create_function() {
    // Safe no-op constructor
}

// Vtable structures for different object types
struct VTable3 {
    void (*create_func)();
    void (*release_func)();
};

struct VTable5 {
    void (*create_func)();
    void (*release_func)();
};

struct VTable7 {
    void (*create_func)();
    void (*release_func)();
};

struct VTable9 {
    void (*create_func)();
    void (*release_func)();
};

// Global vtable instances - all use same no-op RELEASE
static VTable3 vtable_3slots = { mock_create_function, mock_release_noop };
static VTable5 vtable_5slots = { mock_create_function, mock_release_noop };
static VTable7 vtable_7slots = { mock_create_function, mock_release_noop };
static VTable9 vtable_9slots = { mock_create_function, mock_release_noop };

// Variable-sized object structures (3-9 integer slots as requested)
struct TestObject3 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    // Total: 32 bytes
};

struct TestObject5 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    int64_t slot4;     // 8 bytes
    int64_t slot5;     // 8 bytes
    // Total: 48 bytes
};

struct TestObject7 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    int64_t slot4;     // 8 bytes
    int64_t slot5;     // 8 bytes
    int64_t slot6;     // 8 bytes
    int64_t slot7;     // 8 bytes
    // Total: 64 bytes
};

struct TestObject9 {
    void* vtable;      // 8 bytes - vtable pointer
    int64_t slot1;     // 8 bytes
    int64_t slot2;     // 8 bytes
    int64_t slot3;     // 8 bytes
    int64_t slot4;     // 8 bytes
    int64_t slot5;     // 8 bytes
    int64_t slot6;     // 8 bytes
    int64_t slot7;     // 8 bytes
    int64_t slot8;     // 8 bytes
    int64_t slot9;     // 8 bytes
    // Total: 80 bytes
};

// Object creation functions with proper size calculation
TestObject3* create_test_object3(int64_t base_value) {
    TestObject3* obj = (TestObject3*)Heap_allocObject(sizeof(TestObject3));
    if (obj) {
        obj->vtable = &vtable_3slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
    }
    return obj;
}

TestObject5* create_test_object5(int64_t base_value) {
    TestObject5* obj = (TestObject5*)Heap_allocObject(sizeof(TestObject5));
    if (obj) {
        obj->vtable = &vtable_5slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
        obj->slot4 = base_value + 3;
        obj->slot5 = base_value + 4;
    }
    return obj;
}

TestObject7* create_test_object7(int64_t base_value) {
    TestObject7* obj = (TestObject7*)Heap_allocObject(sizeof(TestObject7));
    if (obj) {
        obj->vtable = &vtable_7slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
        obj->slot4 = base_value + 3;
        obj->slot5 = base_value + 4;
        obj->slot6 = base_value + 5;
        obj->slot7 = base_value + 6;
    }
    return obj;
}

TestObject9* create_test_object9(int64_t base_value) {
    TestObject9* obj = (TestObject9*)Heap_allocObject(sizeof(TestObject9));
    if (obj) {
        obj->vtable = &vtable_9slots;
        obj->slot1 = base_value;
        obj->slot2 = base_value + 1;
        obj->slot3 = base_value + 2;
        obj->slot4 = base_value + 3;
        obj->slot5 = base_value + 4;
        obj->slot6 = base_value + 5;
        obj->slot7 = base_value + 6;
        obj->slot8 = base_value + 7;
        obj->slot9 = base_value + 8;
    }
    return obj;
}

// Test 20: Object list creation and basic operations
bool test_object_list_creation() {
    print_test_header("Object List Creation (1000 lists × 10 objects each)");
    
    size_t objects_created = 0;
    Timer timer;
    timer.start();
    
    for (int i = 0; i < 1000; ++i) {
        HeapManager_enterScope();
        ListHeader* header = BCPL_LIST_CREATE_EMPTY();
        
        for (int j = 0; j < 10; ++j) {
            // Create objects with varying slot counts (3-9 slots)
            switch (j % 4) {
                case 0: {
                    TestObject3* obj = create_test_object3(i * 10 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    break;
                }
                case 1: {
                    TestObject5* obj = create_test_object5(i * 10 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    break;
                }
                case 2: {
                    TestObject7* obj = create_test_object7(i * 10 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    break;
                }
                case 3: {
                    TestObject9* obj = create_test_object9(i * 10 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    break;
                }
            }
            objects_created++;
        }
        
        count_list_for_destroy(header);
        HeapManager_exitScope();
    }
    
    double elapsed = timer.stop();
    std::cout << "Created 1000 object lists with 10 objects each in " << elapsed << " ms\n";
    std::cout << "Total objects created: " << objects_created << "\n";
    std::cout << "Object creation rate: " << (objects_created / (elapsed / 1000.0)) << " objects/second\n";
    
    print_test_result("Object List Creation", true);
    return true;
}

// Test 21: Object list append performance comparison
bool test_object_append_performance() {
    print_test_header("Object Append Performance (10,000 appends each object type)");
    
    const int num_appends = 10000;
    
    // Test Object3 appends
    Timer timer;
    timer.start();
    HeapManager_enterScope();
    ListHeader* header3 = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < num_appends; ++i) {
        TestObject3* obj = create_test_object3(i);
        BCPL_LIST_APPEND_OBJECT(header3, obj);
    }
    double time3 = timer.stop();
    count_list_for_destroy(header3);
    HeapManager_exitScope();
    
    // Test Object9 appends
    timer.start();
    HeapManager_enterScope();
    ListHeader* header9 = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < num_appends; ++i) {
        TestObject9* obj = create_test_object9(i);
        BCPL_LIST_APPEND_OBJECT(header9, obj);
    }
    double time9 = timer.stop();
    count_list_for_destroy(header9);
    HeapManager_exitScope();
    
    double rate3 = num_appends / (time3 / 1000.0);
    double rate9 = num_appends / (time9 / 1000.0);
    
    std::cout << "Object3 (32 bytes): " << num_appends << " items in " << time3 << " ms\n";
    std::cout << "Object3 rate: " << rate3 << " appends/second\n";
    std::cout << "Object9 (80 bytes): " << num_appends << " items in " << time9 << " ms\n";
    std::cout << "Object9 rate: " << rate9 << " appends/second\n";
    
    double overhead_factor = time9 / time3;
    std::cout << "Size overhead factor: " << overhead_factor << "x\n";
    
    print_test_result("Object Append Performance", true);
    return true;
}

// Test 22: Mixed object lists with different sizes
bool test_mixed_object_lists() {
    print_test_header("Mixed Object Lists (500 lists × varying sizes)");
    
    size_t total_objects = 0;
    size_t total_bytes = 0;
    Timer timer;
    timer.start();
    
    for (int i = 0; i < 500; ++i) {
        HeapManager_enterScope();
        ListHeader* header = BCPL_LIST_CREATE_EMPTY();
        
        int list_size = 5 + (i % 20); // 5-24 objects per list
        for (int j = 0; j < list_size; ++j) {
            int type = (i + j) % 4;
            switch (type) {
                case 0: {
                    TestObject3* obj = create_test_object3(i * 100 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    total_bytes += sizeof(TestObject3);
                    break;
                }
                case 1: {
                    TestObject5* obj = create_test_object5(i * 100 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    total_bytes += sizeof(TestObject5);
                    break;
                }
                case 2: {
                    TestObject7* obj = create_test_object7(i * 100 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    total_bytes += sizeof(TestObject7);
                    break;
                }
                case 3: {
                    TestObject9* obj = create_test_object9(i * 100 + j);
                    BCPL_LIST_APPEND_OBJECT(header, obj);
                    total_bytes += sizeof(TestObject9);
                    break;
                }
            }
            total_objects++;
        }
        
        count_list_for_destroy(header);
        HeapManager_exitScope();
    }
    
    double elapsed = timer.stop();
    std::cout << "Created 500 mixed object lists in " << elapsed << " ms\n";
    std::cout << "Total objects: " << total_objects << ", Total bytes: " << total_bytes << "\n";
    std::cout << "Average object size: " << (total_bytes / total_objects) << " bytes\n";
    std::cout << "Object creation rate: " << (total_objects / (elapsed / 1000.0)) << " objects/second\n";
    std::cout << "Memory allocation rate: " << (total_bytes / (elapsed / 1000.0) / 1024.0 / 1024.0) << " MB/second\n";
    
    print_test_result("Mixed Object Lists", true);
    return true;
}

// Test 23: Object vs Int vs String append comparison
bool test_object_vs_primitive_comparison() {
    print_test_header("Object vs Primitive Append Comparison (10,000 appends each)");
    
    const int num_appends = 10000;
    
    // Test int appends
    Timer timer;
    timer.start();
    HeapManager_enterScope();
    ListHeader* int_header = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < num_appends; ++i) {
        BCPL_LIST_APPEND_INT(int_header, i);
    }
    double int_time = timer.stop();
    count_list_for_destroy(int_header);
    HeapManager_exitScope();
    
    // Test object appends
    timer.start();
    HeapManager_enterScope();
    ListHeader* obj_header = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < num_appends; ++i) {
        TestObject5* obj = create_test_object5(i);
        BCPL_LIST_APPEND_OBJECT(obj_header, obj);
    }
    double obj_time = timer.stop();
    count_list_for_destroy(obj_header);
    HeapManager_exitScope();
    
    // Test string appends
    timer.start();
    HeapManager_enterScope();
    ListHeader* str_header = BCPL_LIST_CREATE_EMPTY();
    for (int i = 0; i < num_appends; ++i) {
        uint32_t* str = create_bcpl_string("test");
        BCPL_LIST_APPEND_STRING(str_header, str);
    }
    double str_time = timer.stop();
    count_list_for_destroy(str_header);
    HeapManager_exitScope();
    
    double int_rate = num_appends / (int_time / 1000.0);
    double obj_rate = num_appends / (obj_time / 1000.0);
    double str_rate = num_appends / (str_time / 1000.0);
    
    std::cout << "Int append: " << num_appends << " items in " << int_time << " ms\n";
    std::cout << "Int rate: " << int_rate << " appends/second\n";
    std::cout << "Object append: " << num_appends << " items in " << obj_time << " ms\n";
    std::cout << "Object rate: " << obj_rate << " appends/second\n";
    std::cout << "String append: " << num_appends << " items in " << str_time << " ms\n";
    std::cout << "String rate: " << str_rate << " appends/second\n";
    
    std::cout << "Object overhead vs Int: " << (obj_time / int_time) << "x\n";
    std::cout << "String overhead vs Int: " << (str_time / int_time) << "x\n";
    std::cout << "Object vs String: " << (obj_time / str_time) << "x\n";
    
    print_test_result("Object vs Primitive Comparison", true);
    return true;
}

// Test 24: Large object list stress test
bool test_large_object_list_stress() {
    print_test_header("Large Object List Stress Test (100,000 objects)");
    
    const int num_objects = 100000;
    Timer timer;
    timer.start();
    
    HeapManager_enterScope();
    ListHeader* header = BCPL_LIST_CREATE_EMPTY();
    
    for (int i = 0; i < num_objects; ++i) {
        // Rotate through different object sizes to stress the allocator
        switch (i % 4) {
            case 0: {
                TestObject3* obj = create_test_object3(i);
                BCPL_LIST_APPEND_OBJECT(header, obj);
                break;
            }
            case 1: {
                TestObject5* obj = create_test_object5(i);
                BCPL_LIST_APPEND_OBJECT(header, obj);
                break;
            }
            case 2: {
                TestObject7* obj = create_test_object7(i);
                BCPL_LIST_APPEND_OBJECT(header, obj);
                break;
            }
            case 3: {
                TestObject9* obj = create_test_object9(i);
                BCPL_LIST_APPEND_OBJECT(header, obj);
                break;
            }
        }
    }
    
    double elapsed = timer.stop();
    std::cout << "Created list with " << num_objects << " objects in " << elapsed << " ms\n";
    std::cout << "Final list length: " << header->length << "\n";
    std::cout << "Object creation rate: " << (num_objects / (elapsed / 1000.0)) << " objects/second\n";
    
    count_list_for_destroy(header);
    HeapManager_exitScope();
    
    print_test_result("Large Object List Stress Test", true);
    return true;
}

// Test 19: Memory leak check (manual, for valgrind)
bool test_memory_leak() {
    print_test_header("Memory Leak Check (manual/valgrind)");
    for (int i = 0; i < 1000; ++i) {
        HeapManager_enterScope();
        ListHeader* header = create_int_list(1000);
        count_list_for_destroy(header);  // Count for tracking
        HeapManager_exitScope();  // SAMM cleanup
    }
    std::cout << "Run this test under valgrind or similar to check for leaks.\n";
    print_test_result("Memory Leak Check", true);
    return true;
}

int main() {
    // Disable HeapManager tracing to suppress verbose output
    HeapManager::getInstance().setTraceEnabled(false);
    
    // Capture HeapManager stats before tests
    auto stats_before = HeapManager::getInstance().getSAMMStats();
    size_t double_frees_before = HeapManager::getInstance().getDoubleFreeAttempts();

    bool ok = true;
    ok &= test_list_create_destroy();
    ok &= test_list_append();
    ok &= test_scope_freeing();
    ok &= test_list_performance();
    ok &= test_small_lists();
    ok &= test_small_list_append_speed();
    ok &= test_large_list_append_speed();
    ok &= test_small_list_append_speed_optimized();
    ok &= test_large_list_append_speed_optimized();
    ok &= test_append_comparison();
    ok &= test_append_scaling_behavior();
    ok &= test_pure_append_benchmark();
    ok &= test_string_list_creation();
    ok &= test_large_string_list_performance();
    ok &= test_string_vs_int_append_comparison();
    ok &= test_mixed_content_string_lists();
    ok &= test_string_allocation_bottleneck();
    ok &= test_optimized_string_performance();
    ok &= test_optimized_string_list_integration();
    ok &= test_object_list_creation();
    ok &= test_object_append_performance();
    ok &= test_mixed_object_lists();
    ok &= test_object_vs_primitive_comparison();
    ok &= test_large_object_list_stress();
    ok &= test_memory_leak();

    // Wait for cleanup thread to finish (10 seconds)
    std::cout << "\nWaiting 10 seconds for HeapManager cleanup thread to finish...\n";
    std::this_thread::sleep_for(std::chrono::seconds(10));

    // Capture HeapManager stats after tests
    auto stats_after = HeapManager::getInstance().getSAMMStats();
    size_t double_frees_after = HeapManager::getInstance().getDoubleFreeAttempts();

    std::cout << "\nHeapManager stats before: "
              << "objects_cleaned=" << stats_before.objects_cleaned
              << ", double_frees=" << double_frees_before << std::endl;

    std::cout << "HeapManager stats after: "
              << "objects_cleaned=" << stats_after.objects_cleaned
              << ", double_frees=" << double_frees_after << std::endl;

    size_t total_alloc = HeapManager::getInstance().getTotalBytesAllocated();
    size_t total_freed = HeapManager::getInstance().getTotalBytesFreed();
    size_t bytes_leaked = total_alloc > total_freed ? (total_alloc - total_freed) : 0;
    std::cout << "\n[HeapManager] Bytes leaked (according to HeapManager): " << bytes_leaked << std::endl;

    if (double_frees_after > double_frees_before) {
        std::cout << "[WARNING] Double-free attempts detected during test!\n";
    }
    if (stats_after.current_queue_depth != 0) {
        std::cout << "[WARNING] Objects remain in cleanup queue after test (possible leak)!\n";
    }

    // Print list element tracking results
    std::cout << "\n=== List Element Tracking (Test Level) ===" << std::endl;
    std::cout << "List headers created: " << g_list_headers_created << std::endl;
    std::cout << "List atoms created: " << g_list_atoms_created << std::endl;
    std::cout << "List headers destroyed: " << g_list_headers_destroyed << std::endl;
    std::cout << "List atoms destroyed: " << g_list_atoms_destroyed << std::endl;
    
    size_t headers_leaked = g_list_headers_created > g_list_headers_destroyed ? 
                           (g_list_headers_created - g_list_headers_destroyed) : 0;
    size_t atoms_leaked = g_list_atoms_created > g_list_atoms_destroyed ? 
                         (g_list_atoms_created - g_list_atoms_destroyed) : 0;
    
    std::cout << "List headers leaked: " << headers_leaked << std::endl;
    std::cout << "List atoms leaked: " << atoms_leaked << std::endl;

    // Print adaptive bloom filter scaling metrics
    std::cout << "\n=== Adaptive Bloom Filter Metrics ===" << std::endl;
    std::cout << "Current tier: " << HeapManager::getInstance().getBloomFilterTier() << std::endl;
    std::cout << "Reset count: " << HeapManager::getInstance().getBloomFilterResetCount() << std::endl;
    std::cout << "Current capacity: " << HeapManager::getInstance().getBloomFilterCapacity() << std::endl;
    std::cout << "Items added: " << HeapManager::getInstance().getBloomFilterItemsAdded() << std::endl;
    std::cout << "Memory usage: " << HeapManager::getInstance().getBloomFilterMemoryUsage() << " bytes" << std::endl;
    std::cout << "False positive rate: " << (HeapManager::getInstance().getBloomFilterFalsePositiveRate() * 100.0) << "%" << std::endl;

    // Print freelist adaptive scaling metrics
    printFreelistMetrics();

    // Optionally print full metrics
    HeapManager::getInstance().printMetrics();

    std::cout << "\nAll list tests " << (ok ? "PASSED" : "FAILED") << ".\n";
    return ok ? 0 : 1;
}
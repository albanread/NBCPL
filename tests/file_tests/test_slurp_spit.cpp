// Test for SLURP and SPIT runtime functions (write/read file roundtrip)
#include <iostream>
#include <string>
#include <cassert>
#include <cstring>

// Define missing global for HeapManager linkage
extern "C" int g_enable_heap_trace = 0;

extern "C" {
    #include "../../runtime/runtime.h"
    uint32_t* SLURP(uint32_t* filename_str);
    void SPIT(uint32_t* bcpl_string, uint32_t* filename_str);
    void* bcpl_alloc_chars(int64_t num_chars);
    void bcpl_free(void* ptr);
}

// Helper to create BCPL string from C string
uint32_t* create_bcpl_string(const std::string& str) {
    uint32_t* bcpl_str = (uint32_t*)bcpl_alloc_chars(str.length());
    if (!bcpl_str) return nullptr;
    for (size_t i = 0; i < str.length(); i++) {
        bcpl_str[i] = (uint32_t)str[i];
    }
    return bcpl_str;
}

// Helper to convert BCPL string to C++ string
std::string bcpl_to_string(uint32_t* bcpl_str) {
    if (!bcpl_str) return "";
    std::string result;
    size_t i = 0;
    while (bcpl_str[i] != 0) {
        result += (char)bcpl_str[i];
        i++;
    }
    return result;
}

int main() {
    std::cout << "==== SLURP/SPIT RUNTIME TEST ====" << std::endl;
    std::string filename = "test_slurp_spit.txt";
    std::string content = "Hello, SLURP and SPIT!\nThis is a test.";

    uint32_t* bcpl_filename = create_bcpl_string(filename);
    uint32_t* bcpl_content = create_bcpl_string(content);

    // Write file using SPIT
    SPIT(bcpl_content, bcpl_filename);
    std::cout << "SPIT: wrote content to file." << std::endl;

    // Read file using SLURP
    uint32_t* bcpl_read = SLURP(bcpl_filename);
    if (!bcpl_read) {
        std::cout << "FAIL: SLURP returned nullptr" << std::endl;
        bcpl_free(bcpl_filename);
        bcpl_free(bcpl_content);
        return 1;
    }
    std::string read_back = bcpl_to_string(bcpl_read);

    if (read_back == content) {
        std::cout << "PASS: SLURP content matches SPIT content" << std::endl;
    } else {
        std::cout << "FAIL: SLURP content does not match SPIT content" << std::endl;
        std::cout << "Expected: " << content << std::endl;
        std::cout << "Got:      " << read_back << std::endl;
    }

    bcpl_free(bcpl_filename);
    bcpl_free(bcpl_content);
    bcpl_free(bcpl_read);

    std::cout << "==== END SLURP/SPIT TEST ====" << std::endl;
    return 0;
}
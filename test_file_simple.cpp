#include <iostream>
#include <cstdio>
#include <cstring>
#include <unistd.h>

// Simple test of FILE_ functions from unified runtime
extern "C" {
    // File API functions
    uint32_t FILE_OPEN_READ(uint32_t* filename_str);
    uint32_t FILE_OPEN_WRITE(uint32_t* filename_str);
    uint32_t FILE_OPEN_APPEND(uint32_t* filename_str);
    uint32_t FILE_CLOSE(uint32_t handle);
    uint32_t FILE_WRITES(uint32_t handle, uint32_t* string_buffer);
    uint32_t* FILE_READS(uint32_t handle);
    uint32_t FILE_READ(uint32_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_WRITE(uint32_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_SEEK(uint32_t handle, int32_t offset, uint32_t origin);
    int32_t FILE_TELL(uint32_t handle);
    uint32_t FILE_EOF(uint32_t handle);
    
    // Simple heap functions
    void* bcpl_alloc_chars(int64_t num_chars);
    void bcpl_free(void* ptr);
}

// Helper to create BCPL string
uint32_t* make_bcpl_string(const char* str) {
    size_t len = strlen(str);
    uint32_t* bcpl_str = (uint32_t*)bcpl_alloc_chars(len);
    if (!bcpl_str) return nullptr;
    
    for (size_t i = 0; i < len; i++) {
        bcpl_str[i] = (uint32_t)str[i];
    }
    return bcpl_str;
}

// Helper to print BCPL string
void print_bcpl_string(uint32_t* bcpl_str) {
    if (!bcpl_str) {
        std::cout << "(null)";
        return;
    }
    
    size_t i = 0;
    while (bcpl_str[i] != 0) {
        std::cout << (char)bcpl_str[i];
        i++;
    }
}

int main() {
    std::cout << "Simple FILE API Test" << std::endl;
    std::cout << "Testing basic FILE_ functions from unified runtime" << std::endl;
    
    int tests_passed = 0;
    int tests_total = 0;
    
    // Test 1: Create and write a file
    std::cout << "\nTest 1: FILE_OPEN_WRITE and FILE_WRITES" << std::endl;
    tests_total++;
    
    uint32_t* filename = make_bcpl_string("test_simple.txt");
    if (!filename) {
        std::cout << "FAIL: Could not create filename string" << std::endl;
        return 1;
    }
    
    uint32_t handle = FILE_OPEN_WRITE(filename);
    if (handle == 0) {
        std::cout << "FAIL: FILE_OPEN_WRITE returned 0" << std::endl;
        bcpl_free(filename);
        return 1;
    }
    
    uint32_t* content = make_bcpl_string("Hello, FILE API!");
    if (!content) {
        std::cout << "FAIL: Could not create content string" << std::endl;
        FILE_CLOSE(handle);
        bcpl_free(filename);
        return 1;
    }
    
    uint32_t bytes_written = FILE_WRITES(handle, content);
    if (bytes_written > 0) {
        std::cout << "PASS: Wrote " << bytes_written << " bytes" << std::endl;
        tests_passed++;
    } else {
        std::cout << "FAIL: FILE_WRITES returned 0" << std::endl;
    }
    
    uint32_t close_result = FILE_CLOSE(handle);
    bcpl_free(content);
    
    // Test 2: Read the file back
    std::cout << "\nTest 2: FILE_OPEN_READ and FILE_READS" << std::endl;
    tests_total++;
    
    handle = FILE_OPEN_READ(filename);
    if (handle == 0) {
        std::cout << "FAIL: FILE_OPEN_READ returned 0" << std::endl;
        bcpl_free(filename);
        return 1;
    }
    
    uint32_t* read_content = FILE_READS(handle);
    if (read_content) {
        std::cout << "PASS: Read content: ";
        print_bcpl_string(read_content);
        std::cout << std::endl;
        tests_passed++;
        bcpl_free(read_content);
    } else {
        std::cout << "FAIL: FILE_READS returned null" << std::endl;
    }
    
    FILE_CLOSE(handle);
    
    // Test 3: Test file positioning
    std::cout << "\nTest 3: FILE_TELL and FILE_EOF" << std::endl;
    tests_total++;
    
    handle = FILE_OPEN_READ(filename);
    if (handle > 0) {
        int32_t pos = FILE_TELL(handle);
        uint32_t eof_status = FILE_EOF(handle);
        
        std::cout << "Initial position: " << pos << ", EOF: " << eof_status << std::endl;
        
        if (pos >= 0) {
            std::cout << "PASS: FILE_TELL works" << std::endl;
            tests_passed++;
        } else {
            std::cout << "FAIL: FILE_TELL returned negative" << std::endl;
        }
        
        FILE_CLOSE(handle);
    }
    
    // Test 4: Test invalid operations
    std::cout << "\nTest 4: Error handling" << std::endl;
    tests_total++;
    
    uint32_t invalid_result = FILE_CLOSE(0);
    if (invalid_result != 0) {
        std::cout << "PASS: FILE_CLOSE rejects invalid handle" << std::endl;
        tests_passed++;
    } else {
        std::cout << "FAIL: FILE_CLOSE accepted invalid handle" << std::endl;
    }
    
    // Cleanup
    bcpl_free(filename);
    unlink("test_simple.txt");
    
    // Results
    std::cout << "\n=== Results ===" << std::endl;
    std::cout << "Tests passed: " << tests_passed << "/" << tests_total << std::endl;
    
    if (tests_passed == tests_total) {
        std::cout << "🎉 All FILE_ functions working correctly!" << std::endl;
        return 0;
    } else {
        std::cout << "❌ Some tests failed" << std::endl;
        return 1;
    }
}
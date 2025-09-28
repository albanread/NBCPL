#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cassert>
#include <fstream>

// Define missing global for HeapManager linkage
extern "C" int g_enable_heap_trace = 0;

// Include runtime headers
extern "C" {
    #include "../../runtime/runtime.h"
    // FILE_ API
    uintptr_t FILE_OPEN_READ(uint32_t* filename_str);
    uintptr_t FILE_OPEN_WRITE(uint32_t* filename_str);
    uintptr_t FILE_OPEN_APPEND(uint32_t* filename_str);
    uint32_t FILE_CLOSE(uintptr_t handle);
    uint32_t FILE_WRITES(uintptr_t handle, uint32_t* string_buffer);
    uint32_t* FILE_READS(uintptr_t handle);
    uint32_t FILE_READ(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_WRITE(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_SEEK(uintptr_t handle, int32_t offset, uint32_t origin);
    int32_t FILE_TELL(uintptr_t handle);
    uint32_t FILE_EOF(uintptr_t handle);

    // Heap management
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
    // Null terminator is handled by bcpl_alloc_chars
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

void test_file_write_and_read() {
    std::cout << "=== FILE_ API: Write and Read Test ===" << std::endl;
    std::string filename = "test_file_runtime.txt";
    std::string content = "Hello, FILE_ API!\nSecond line.";

    // --- Write ---
    uint32_t* bcpl_filename = create_bcpl_string(filename);
    uint32_t* bcpl_content = create_bcpl_string(content);

    uintptr_t handle = FILE_OPEN_WRITE(bcpl_filename);
    if (handle == 0) {
        std::cout << "FAIL: FILE_OPEN_WRITE returned 0 (could not open file for writing)" << std::endl;
        bcpl_free(bcpl_filename);
        bcpl_free(bcpl_content);
        return;
    }
    uint32_t bytes_written = FILE_WRITES(handle, bcpl_content);
    if (bytes_written == 0) {
        std::cout << "FAIL: FILE_WRITES wrote 0 bytes" << std::endl;
    } else {
        std::cout << "PASS: FILE_WRITES wrote " << bytes_written << " bytes" << std::endl;
    }
    FILE_CLOSE(handle);

    // --- Read ---
    handle = FILE_OPEN_READ(bcpl_filename);
    if (handle == 0) {
        std::cout << "FAIL: FILE_OPEN_READ returned 0 (could not open file for reading)" << std::endl;
        bcpl_free(bcpl_filename);
        bcpl_free(bcpl_content);
        return;
    }
    uint32_t* bcpl_read = FILE_READS(handle);
    if (!bcpl_read) {
        std::cout << "FAIL: FILE_READS returned nullptr" << std::endl;
    } else {
        std::string read_back = bcpl_to_string(bcpl_read);
        if (read_back == content) {
            std::cout << "PASS: FILE_READS content matches written content" << std::endl;
        } else {
            std::cout << "FAIL: FILE_READS content does not match written content" << std::endl;
            std::cout << "Expected: " << content << std::endl;
            std::cout << "Got:      " << read_back << std::endl;
        }
        bcpl_free(bcpl_read);
    }
    FILE_CLOSE(handle);

    bcpl_free(bcpl_filename);
    bcpl_free(bcpl_content);
}

void test_file_seek_and_tell() {
    std::cout << "=== FILE_ API: Seek and Tell Test ===" << std::endl;
    std::string filename = "test_file_seek.txt";
    std::string content = "0123456789ABCDEF";

    // Write file
    uint32_t* bcpl_filename = create_bcpl_string(filename);
    uint32_t* bcpl_content = create_bcpl_string(content);
    uintptr_t handle = FILE_OPEN_WRITE(bcpl_filename);
    FILE_WRITES(handle, bcpl_content);
    FILE_CLOSE(handle);

    // Open for read
    handle = FILE_OPEN_READ(bcpl_filename);
    if (handle == 0) {
        std::cout << "FAIL: FILE_OPEN_READ for seek test" << std::endl;
        bcpl_free(bcpl_filename);
        bcpl_free(bcpl_content);
        return;
    }

    int32_t pos = FILE_TELL(handle);
    if (pos != 0) {
        std::cout << "FAIL: FILE_TELL at start should be 0, got " << pos << std::endl;
    } else {
        std::cout << "PASS: FILE_TELL at start is 0" << std::endl;
    }

    uint32_t seek_result = FILE_SEEK(handle, 5, 0); // SEEK_SET
    pos = FILE_TELL(handle);
    if (pos != 5) {
        std::cout << "FAIL: FILE_TELL after seek to 5 should be 5, got " << pos << std::endl;
    } else {
        std::cout << "PASS: FILE_TELL after seek to 5 is 5" << std::endl;
    }

    FILE_CLOSE(handle);
    bcpl_free(bcpl_filename);
    bcpl_free(bcpl_content);
}

void test_file_error_cases() {
    std::cout << "=== FILE_ API: Error Cases Test ===" << std::endl;
    uint32_t* bcpl_filename = create_bcpl_string("nonexistent_file.txt");
    uintptr_t handle = FILE_OPEN_READ(bcpl_filename);
    if (handle == 0) {
        std::cout << "PASS: FILE_OPEN_READ returns 0 for non-existent file" << std::endl;
    } else {
        std::cout << "FAIL: FILE_OPEN_READ should return 0 for non-existent file" << std::endl;
        FILE_CLOSE(handle);
    }
    bcpl_free(bcpl_filename);

    // Try to write to invalid handle
    uint32_t* bcpl_content = create_bcpl_string("dummy");
    uint32_t bytes = FILE_WRITES(0, bcpl_content);
    if (bytes == 0) {
        std::cout << "PASS: FILE_WRITES returns 0 for invalid handle" << std::endl;
    } else {
        std::cout << "FAIL: FILE_WRITES should return 0 for invalid handle" << std::endl;
    }
    bcpl_free(bcpl_content);
}

int main() {
    std::cout << "==== FILE_ API RUNTIME TEST SUITE ====" << std::endl;
    test_file_write_and_read();
    test_file_seek_and_tell();
    test_file_error_cases();
    std::cout << "==== END OF FILE_ API TESTS ====" << std::endl;
    return 0;
}
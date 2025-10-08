#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <unistd.h>

// Include runtime headers
#include "runtime/runtime.h"
#include "HeapManager/heap_manager_defs.h"

// External functions from the runtime
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
    
    // Heap management functions
    void* bcpl_alloc_chars(int64_t num_chars);
    void bcpl_free(void* ptr);
    
    // Metrics functions
    void update_io_metrics_file_opened(void);
    void update_io_metrics_file_closed(void);
    void update_io_metrics_read(size_t bytes);
    void update_io_metrics_write(size_t bytes);
}

class FileRuntimeTester {
private:
    int tests_run = 0;
    int tests_passed = 0;
    std::vector<std::string> test_files;
    
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
    
    // Helper to create test file with known content
    void create_test_file(const std::string& filename, const std::string& content) {
        std::ofstream file(filename);
        file << content;
        file.close();
        test_files.push_back(filename);
    }
    
    // Test assertion helper
    void assert_test(bool condition, const std::string& test_name, const std::string& message = "") {
        tests_run++;
        if (condition) {
            tests_passed++;
            std::cout << "✓ " << test_name << std::endl;
        } else {
            std::cout << "✗ " << test_name << " - " << message << std::endl;
        }
    }
    
    // Cleanup test files
    void cleanup_test_files() {
        for (const auto& filename : test_files) {
            unlink(filename.c_str());
        }
        test_files.clear();
    }
    
public:
    void run_all_tests() {
        std::cout << "=== FILE Runtime API Test Suite ===" << std::endl;
        
        test_file_open_close();
        test_file_writes_reads();
        test_file_seek_tell_eof();
        test_file_low_level_io();
        test_file_append_mode();
        test_error_handling();
        test_unicode_support();
        test_large_files();
        
        cleanup_test_files();
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        std::cout << "Success rate: " << (100.0 * tests_passed / tests_run) << "%" << std::endl;
    }
    
private:
    void test_file_open_close() {
        std::cout << "\n--- Testing FILE_OPEN_* and FILE_CLOSE ---" << std::endl;
        
        // Test FILE_OPEN_WRITE and FILE_CLOSE
        uint32_t* filename = create_bcpl_string("test_write.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "FILE_OPEN_WRITE creates valid handle");
        
        uint32_t close_result = FILE_CLOSE(handle);
        assert_test(close_result == 0, "FILE_CLOSE returns success");
        
        bcpl_free(filename);
        test_files.push_back("test_write.txt");
        
        // Test FILE_OPEN_READ on existing file
        create_test_file("test_read.txt", "test content");
        filename = create_bcpl_string("test_read.txt");
        handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "FILE_OPEN_READ opens existing file");
        FILE_CLOSE(handle);
        bcpl_free(filename);
        
        // Test FILE_OPEN_APPEND
        filename = create_bcpl_string("test_append.txt");
        handle = FILE_OPEN_APPEND(filename);
        assert_test(handle != 0, "FILE_OPEN_APPEND creates/opens file");
        FILE_CLOSE(handle);
        bcpl_free(filename);
        test_files.push_back("test_append.txt");
        
        // Test opening non-existent file for reading
        filename = create_bcpl_string("nonexistent_file.txt");
        handle = FILE_OPEN_READ(filename);
        assert_test(handle == 0, "FILE_OPEN_READ returns 0 for non-existent file");
        bcpl_free(filename);
    }
    
    void test_file_writes_reads() {
        std::cout << "\n--- Testing FILE_WRITES and FILE_READS ---" << std::endl;
        
        // Create file and write content
        uint32_t* filename = create_bcpl_string("test_string_io.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for string I/O");
        
        uint32_t* test_content = create_bcpl_string("Hello, World!\nLine 2\nLine 3");
        uint32_t bytes_written = FILE_WRITES(handle, test_content);
        assert_test(bytes_written > 0, "FILE_WRITES writes content");
        
        FILE_CLOSE(handle);
        
        // Read content back
        handle = FILE_OPEN_READ(filename);
        uint32_t* read_content = FILE_READS(handle);
        assert_test(read_content != nullptr, "FILE_READS returns content");
        
        std::string original = bcpl_to_string(test_content);
        std::string read_back = bcpl_to_string(read_content);
        assert_test(original == read_back, "FILE_READS content matches written content");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
        bcpl_free(test_content);
        bcpl_free(read_content);
        test_files.push_back("test_string_io.txt");
    }
    
    void test_file_seek_tell_eof() {
        std::cout << "\n--- Testing FILE_SEEK, FILE_TELL, FILE_EOF ---" << std::endl;
        
        // Create file with known content
        create_test_file("test_seek.txt", "0123456789ABCDEF");
        
        uint32_t* filename = create_bcpl_string("test_seek.txt");
        uint32_t handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "Open file for seek operations");
        
        // Test initial position
        int32_t pos = FILE_TELL(handle);
        assert_test(pos == 0, "Initial file position is 0");
        
        // Test EOF at start (should be false)
        uint32_t eof_status = FILE_EOF(handle);
        assert_test(eof_status == 0, "EOF is false at start of file");
        
        // Seek to middle of file
        uint32_t seek_result = FILE_SEEK(handle, 5, 0); // SEEK_SET
        assert_test(seek_result == 0, "FILE_SEEK to position 5 succeeds");
        
        pos = FILE_TELL(handle);
        assert_test(pos == 5, "FILE_TELL reports correct position after seek");
        
        // Seek relative
        seek_result = FILE_SEEK(handle, 3, 1); // SEEK_CUR
        assert_test(seek_result == 0, "FILE_SEEK relative succeeds");
        
        pos = FILE_TELL(handle);
        assert_test(pos == 8, "FILE_TELL reports correct position after relative seek");
        
        // Seek to end
        seek_result = FILE_SEEK(handle, 0, 2); // SEEK_END
        assert_test(seek_result == 0, "FILE_SEEK to end succeeds");
        
        eof_status = FILE_EOF(handle);
        assert_test(eof_status != 0, "EOF is true at end of file");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
    }
    
    void test_file_low_level_io() {
        std::cout << "\n--- Testing FILE_READ and FILE_WRITE ---" << std::endl;
        
        // Create file with binary data
        uint32_t* filename = create_bcpl_string("test_binary.dat");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        
        // Write binary data
        uint32_t write_buffer[10];
        for (int i = 0; i < 10; i++) {
            write_buffer[i] = i * 2;
        }
        
        uint32_t bytes_written = FILE_WRITE(handle, write_buffer, 10);
        assert_test(bytes_written == 10, "FILE_WRITE writes correct number of bytes");
        
        FILE_CLOSE(handle);
        
        // Read binary data back
        handle = FILE_OPEN_READ(filename);
        uint32_t read_buffer[10];
        memset(read_buffer, 0, sizeof(read_buffer));
        
        uint32_t bytes_read = FILE_READ(handle, read_buffer, 10);
        assert_test(bytes_read == 10, "FILE_READ reads correct number of bytes");
        
        bool data_matches = true;
        for (int i = 0; i < 10; i++) {
            if (read_buffer[i] != (uint32_t)(i * 2)) {
                data_matches = false;
                break;
            }
        }
        assert_test(data_matches, "FILE_READ data matches written data");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
        test_files.push_back("test_binary.dat");
    }
    
    void test_file_append_mode() {
        std::cout << "\n--- Testing FILE_OPEN_APPEND behavior ---" << std::endl;
        
        // Create initial file
        create_test_file("test_append_mode.txt", "Initial content\n");
        
        // Open in append mode and add content
        uint32_t* filename = create_bcpl_string("test_append_mode.txt");
        uint32_t handle = FILE_OPEN_APPEND(filename);
        assert_test(handle != 0, "FILE_OPEN_APPEND opens existing file");
        
        uint32_t* append_content = create_bcpl_string("Appended content\n");
        uint32_t bytes_written = FILE_WRITES(handle, append_content);
        assert_test(bytes_written > 0, "FILE_WRITES appends content");
        
        FILE_CLOSE(handle);
        
        // Read entire file to verify append
        handle = FILE_OPEN_READ(filename);
        uint32_t* full_content = FILE_READS(handle);
        std::string content_str = bcpl_to_string(full_content);
        
        bool contains_both = (content_str.find("Initial content") != std::string::npos) &&
                           (content_str.find("Appended content") != std::string::npos);
        assert_test(contains_both, "File contains both initial and appended content");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
        bcpl_free(append_content);
        bcpl_free(full_content);
    }
    
    void test_error_handling() {
        std::cout << "\n--- Testing Error Handling ---" << std::endl;
        
        // Test invalid handle operations
        uint32_t result = FILE_CLOSE(0);
        assert_test(result != 0, "FILE_CLOSE returns error for invalid handle");
        
        uint32_t* dummy_buffer = create_bcpl_string("dummy");
        uint32_t bytes = FILE_WRITES(0, dummy_buffer);
        assert_test(bytes == 0, "FILE_WRITES returns 0 for invalid handle");
        
        uint32_t* read_result = FILE_READS(0);
        assert_test(read_result == nullptr, "FILE_READS returns NULL for invalid handle");
        
        int32_t pos = FILE_TELL(0);
        assert_test(pos == -1, "FILE_TELL returns -1 for invalid handle");
        
        uint32_t eof_result = FILE_EOF(0);
        assert_test(eof_result != 0, "FILE_EOF returns error for invalid handle");
        
        // Test NULL pointer handling
        uint32_t handle = FILE_OPEN_READ(nullptr);
        assert_test(handle == 0, "FILE_OPEN_READ handles NULL filename");
        
        bcpl_free(dummy_buffer);
    }
    
    void test_unicode_support() {
        std::cout << "\n--- Testing Unicode Support ---" << std::endl;
        
        // Note: This is a basic test. Full Unicode requires proper UTF-8 encoding
        uint32_t* filename = create_bcpl_string("test_unicode.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        
        // Test with extended ASCII characters
        uint32_t* unicode_content = create_bcpl_string("Hello áéíóú 世界");
        uint32_t bytes_written = FILE_WRITES(handle, unicode_content);
        assert_test(bytes_written > 0, "FILE_WRITES handles extended characters");
        
        FILE_CLOSE(handle);
        
        // Read back and verify
        handle = FILE_OPEN_READ(filename);
        uint32_t* read_unicode = FILE_READS(handle);
        assert_test(read_unicode != nullptr, "FILE_READS handles Unicode content");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
        bcpl_free(unicode_content);
        bcpl_free(read_unicode);
        test_files.push_back("test_unicode.txt");
    }
    
    void test_large_files() {
        std::cout << "\n--- Testing Large File Operations ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_large.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        
        // Write a moderately large amount of data
        uint32_t* chunk = create_bcpl_string("This is a test chunk of data that will be repeated many times.\n");
        uint32_t total_written = 0;
        
        for (int i = 0; i < 100; i++) {
            uint32_t bytes = FILE_WRITES(handle, chunk);
            total_written += bytes;
        }
        
        assert_test(total_written > 1000, "Large file write operations succeed");
        FILE_CLOSE(handle);
        
        // Test seeking in large file
        handle = FILE_OPEN_READ(filename);
        uint32_t seek_result = FILE_SEEK(handle, 500, 0); // SEEK_SET to position 500
        assert_test(seek_result == 0, "Seek works in large files");
        
        int32_t pos = FILE_TELL(handle);
        assert_test(pos == 500, "FILE_TELL works correctly in large files");
        
        FILE_CLOSE(handle);
        bcpl_free(filename);
        bcpl_free(chunk);
        test_files.push_back("test_large.txt");
    }
};

int main() {
    std::cout << "FILE Runtime API Test Suite" << std::endl;
    std::cout << "Testing all FILE_ commands against the BCPL runtime" << std::endl;
    
    FileRuntimeTester tester;
    tester.run_all_tests();
    
    return 0;
}
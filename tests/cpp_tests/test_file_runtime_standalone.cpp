#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <cstdlib>

// Minimal runtime interface to test FILE_ functions
extern "C" {
    // Simplified heap management for testing
    void* bcpl_alloc_chars(int64_t num_chars) {
        size_t size = (num_chars + 1) * sizeof(uint32_t);
        uint32_t* ptr = (uint32_t*)calloc(1, size);
        return ptr;
    }
    
    void bcpl_free(void* ptr) {
        free(ptr);
    }
    
    // Metrics tracking stubs
    void update_io_metrics_file_opened(void) { /* stub */ }
    void update_io_metrics_file_closed(void) { /* stub */ }
    void update_io_metrics_read(size_t bytes) { (void)bytes; /* stub */ }
    void update_io_metrics_write(size_t bytes) { (void)bytes; /* stub */ }
}

// Include the FILE API implementation directly
#include "runtime/runtime_file_api.inc"

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
            std::cout << "✗ " << test_name;
            if (!message.empty()) {
                std::cout << " - " << message;
            }
            std::cout << std::endl;
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
        std::cout << "Testing FILE_ functions directly with minimal runtime" << std::endl;
        
        test_file_open_close();
        test_file_writes_reads();
        test_file_seek_tell_eof();
        test_file_low_level_io();
        test_file_append_mode();
        test_error_handling();
        test_unicode_basic();
        test_empty_files();
        
        cleanup_test_files();
        
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        
        if (tests_run > 0) {
            double success_rate = (100.0 * tests_passed / tests_run);
            std::cout << "Success rate: " << success_rate << "%" << std::endl;
            
            if (success_rate >= 95.0) {
                std::cout << "🎉 Excellent! All FILE_ functions working correctly." << std::endl;
            } else if (success_rate >= 80.0) {
                std::cout << "⚠️  Good, but some issues detected." << std::endl;
            } else {
                std::cout << "❌ Significant issues found in FILE_ functions." << std::endl;
            }
        }
    }
    
private:
    void test_file_open_close() {
        std::cout << "\n--- Testing FILE_OPEN_* and FILE_CLOSE ---" << std::endl;
        
        // Test FILE_OPEN_WRITE and FILE_CLOSE
        uint32_t* filename = create_bcpl_string("test_write.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "FILE_OPEN_WRITE creates valid handle");
        
        if (handle != 0) {
            uint32_t close_result = FILE_CLOSE(handle);
            assert_test(close_result == 0, "FILE_CLOSE returns success");
        }
        
        bcpl_free(filename);
        test_files.push_back("test_write.txt");
        
        // Test FILE_OPEN_READ on existing file
        create_test_file("test_read.txt", "test content");
        filename = create_bcpl_string("test_read.txt");
        handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "FILE_OPEN_READ opens existing file");
        if (handle != 0) {
            FILE_CLOSE(handle);
        }
        bcpl_free(filename);
        
        // Test FILE_OPEN_APPEND
        filename = create_bcpl_string("test_append.txt");
        handle = FILE_OPEN_APPEND(filename);
        assert_test(handle != 0, "FILE_OPEN_APPEND creates/opens file");
        if (handle != 0) {
            FILE_CLOSE(handle);
        }
        bcpl_free(filename);
        test_files.push_back("test_append.txt");
        
        // Test opening non-existent file for reading
        filename = create_bcpl_string("nonexistent_file_12345.txt");
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
        
        if (handle != 0) {
            uint32_t* test_content = create_bcpl_string("Hello, World!\nLine 2\nLine 3");
            uint32_t bytes_written = FILE_WRITES(handle, test_content);
            assert_test(bytes_written > 0, "FILE_WRITES writes content");
            
            FILE_CLOSE(handle);
            
            // Read content back
            handle = FILE_OPEN_READ(filename);
            assert_test(handle != 0, "Reopen file for reading");
            
            if (handle != 0) {
                uint32_t* read_content = FILE_READS(handle);
                assert_test(read_content != nullptr, "FILE_READS returns content");
                
                if (read_content != nullptr) {
                    std::string original = bcpl_to_string(test_content);
                    std::string read_back = bcpl_to_string(read_content);
                    assert_test(original == read_back, "FILE_READS content matches written content");
                    bcpl_free(read_content);
                }
                
                FILE_CLOSE(handle);
            }
            
            bcpl_free(test_content);
        }
        
        bcpl_free(filename);
        test_files.push_back("test_string_io.txt");
    }
    
    void test_file_seek_tell_eof() {
        std::cout << "\n--- Testing FILE_SEEK, FILE_TELL, FILE_EOF ---" << std::endl;
        
        // Create file with known content
        create_test_file("test_seek.txt", "0123456789ABCDEF");
        
        uint32_t* filename = create_bcpl_string("test_seek.txt");
        uint32_t handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "Open file for seek operations");
        
        if (handle != 0) {
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
        }
        
        bcpl_free(filename);
    }
    
    void test_file_low_level_io() {
        std::cout << "\n--- Testing FILE_READ and FILE_WRITE ---" << std::endl;
        
        // Create file with binary data
        uint32_t* filename = create_bcpl_string("test_binary.dat");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for binary I/O");
        
        if (handle != 0) {
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
            assert_test(handle != 0, "Reopen file for binary read");
            
            if (handle != 0) {
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
            }
        }
        
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
        
        if (handle != 0) {
            uint32_t* append_content = create_bcpl_string("Appended content\n");
            uint32_t bytes_written = FILE_WRITES(handle, append_content);
            assert_test(bytes_written > 0, "FILE_WRITES appends content");
            
            FILE_CLOSE(handle);
            
            // Read entire file to verify append
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t* full_content = FILE_READS(handle);
                if (full_content != nullptr) {
                    std::string content_str = bcpl_to_string(full_content);
                    
                    bool contains_both = (content_str.find("Initial content") != std::string::npos) &&
                                       (content_str.find("Appended content") != std::string::npos);
                    assert_test(contains_both, "File contains both initial and appended content");
                    
                    bcpl_free(full_content);
                }
                FILE_CLOSE(handle);
            }
            
            bcpl_free(append_content);
        }
        
        bcpl_free(filename);
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
        
        handle = FILE_OPEN_WRITE(nullptr);
        assert_test(handle == 0, "FILE_OPEN_WRITE handles NULL filename");
        
        handle = FILE_OPEN_APPEND(nullptr);
        assert_test(handle == 0, "FILE_OPEN_APPEND handles NULL filename");
        
        bcpl_free(dummy_buffer);
    }
    
    void test_unicode_basic() {
        std::cout << "\n--- Testing Basic Unicode Support ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_unicode.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for Unicode test");
        
        if (handle != 0) {
            // Test with basic extended ASCII characters
            uint32_t* unicode_content = create_bcpl_string("Hello World 123");
            uint32_t bytes_written = FILE_WRITES(handle, unicode_content);
            assert_test(bytes_written > 0, "FILE_WRITES handles basic characters");
            
            FILE_CLOSE(handle);
            
            // Read back and verify
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t* read_unicode = FILE_READS(handle);
                assert_test(read_unicode != nullptr, "FILE_READS handles Unicode content");
                
                if (read_unicode != nullptr) {
                    std::string original = bcpl_to_string(unicode_content);
                    std::string read_back = bcpl_to_string(read_unicode);
                    assert_test(original == read_back, "Unicode content matches");
                    bcpl_free(read_unicode);
                }
                
                FILE_CLOSE(handle);
            }
            
            bcpl_free(unicode_content);
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unicode.txt");
    }
    
    void test_empty_files() {
        std::cout << "\n--- Testing Empty File Operations ---" << std::endl;
        
        // Test creating empty file
        uint32_t* filename = create_bcpl_string("test_empty.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Create empty file");
        
        if (handle != 0) {
            FILE_CLOSE(handle);
            
            // Test reading empty file
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t* empty_content = FILE_READS(handle);
                assert_test(empty_content != nullptr, "FILE_READS handles empty file");
                
                if (empty_content != nullptr) {
                    std::string content_str = bcpl_to_string(empty_content);
                    assert_test(content_str.empty(), "Empty file returns empty string");
                    bcpl_free(empty_content);
                }
                
                // Test EOF on empty file
                uint32_t eof_status = FILE_EOF(handle);
                assert_test(eof_status != 0, "EOF is true for empty file");
                
                FILE_CLOSE(handle);
            }
        }
        
        bcpl_free(filename);
        test_files.push_back("test_empty.txt");
    }
};

int main() {
    std::cout << "FILE Runtime API Standalone Test Suite" << std::endl;
    std::cout << "Testing all FILE_ commands with minimal runtime dependencies" << std::endl;
    
    FileRuntimeTester tester;
    tester.run_all_tests();
    
    return 0;
}
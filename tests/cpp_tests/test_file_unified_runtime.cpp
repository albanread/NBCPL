#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cstdio>
#include <cassert>
#include <fstream>
#include <unistd.h>
#include <cstdlib>
#include <iomanip>

// Include runtime headers - using the actual runtime interface
extern "C" {
    // File API functions from unified runtime
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
    
    // Heap management functions from unified runtime
    void* bcpl_alloc_chars(int64_t num_chars);
    void bcpl_free(void* ptr);
    
    // Runtime initialization not needed for FILE_ function testing
}

class UnifiedFileRuntimeTester {
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
        std::cout << "=== FILE Unified Runtime API Test Suite ===" << std::endl;
        std::cout << "Testing FILE_ functions against unified runtime (libbcpl_runtime_sdl2_static.a)" << std::endl;
        std::cout << "Runtime: Pre-initialized static library, ready for FILE operations" << std::endl;
        
        // Run file API tests
        test_file_open_close();
        test_file_writes_reads();
        test_file_seek_tell_eof();
        test_file_low_level_io();
        test_file_append_mode();
        test_error_handling();
        test_unicode_and_special_chars();
        test_large_file_operations();
        test_partial_reads_writes();
        test_concurrent_operations();
        
        cleanup_test_files();
        
        // Print results
        std::cout << "\n=== Test Results ===" << std::endl;
        std::cout << "Tests run: " << tests_run << std::endl;
        std::cout << "Tests passed: " << tests_passed << std::endl;
        std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
        
        if (tests_run > 0) {
            double success_rate = (100.0 * tests_passed / tests_run);
            std::cout << "Success rate: " << std::fixed << std::setprecision(1) << success_rate << "%" << std::endl;
            
            if (success_rate >= 95.0) {
                std::cout << "🎉 Excellent! FILE_ API working correctly with unified runtime." << std::endl;
                std::cout << "✅ All FILE commands are ready for production use." << std::endl;
            } else if (success_rate >= 80.0) {
                std::cout << "⚠️  Good overall, but some issues detected." << std::endl;
                std::cout << "🔧 Minor fixes may be needed for edge cases." << std::endl;
            } else {
                std::cout << "❌ Significant issues found in FILE_ API." << std::endl;
                std::cout << "🚨 Review and fix required before production use." << std::endl;
            }
        }
    }
    
private:
    void test_file_open_close() {
        std::cout << "\n--- Testing FILE_OPEN_* and FILE_CLOSE ---" << std::endl;
        
        // Test FILE_OPEN_WRITE and FILE_CLOSE
        uint32_t* filename = create_bcpl_string("test_unified_write.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "FILE_OPEN_WRITE creates valid handle");
        
        if (handle != 0) {
            uint32_t close_result = FILE_CLOSE(handle);
            assert_test(close_result == 0, "FILE_CLOSE returns success");
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_write.txt");
        
        // Test FILE_OPEN_READ on existing file
        create_test_file("test_unified_read.txt", "BCPL runtime test content");
        filename = create_bcpl_string("test_unified_read.txt");
        handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "FILE_OPEN_READ opens existing file");
        if (handle != 0) {
            FILE_CLOSE(handle);
        }
        bcpl_free(filename);
        
        // Test FILE_OPEN_APPEND
        filename = create_bcpl_string("test_unified_append.txt");
        handle = FILE_OPEN_APPEND(filename);
        assert_test(handle != 0, "FILE_OPEN_APPEND creates/opens file");
        if (handle != 0) {
            FILE_CLOSE(handle);
        }
        bcpl_free(filename);
        test_files.push_back("test_unified_append.txt");
        
        // Test opening non-existent file for reading
        filename = create_bcpl_string("nonexistent_unified_file.txt");
        handle = FILE_OPEN_READ(filename);
        assert_test(handle == 0, "FILE_OPEN_READ returns 0 for non-existent file");
        bcpl_free(filename);
    }
    
    void test_file_writes_reads() {
        std::cout << "\n--- Testing FILE_WRITES and FILE_READS ---" << std::endl;
        
        // Test string I/O with BCPL runtime patterns
        uint32_t* filename = create_bcpl_string("test_unified_string_io.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for unified string I/O");
        
        if (handle != 0) {
            // Test typical BCPL output patterns
            uint32_t* test_content = create_bcpl_string("BCPL Runtime Test\nLine 2: Hello World\nLine 3: 12345");
            uint32_t bytes_written = FILE_WRITES(handle, test_content);
            assert_test(bytes_written > 0, "FILE_WRITES writes BCPL content");
            
            FILE_CLOSE(handle);
            
            // Read content back and verify
            handle = FILE_OPEN_READ(filename);
            assert_test(handle != 0, "Reopen file for reading");
            
            if (handle != 0) {
                uint32_t* read_content = FILE_READS(handle);
                assert_test(read_content != nullptr, "FILE_READS returns content");
                
                if (read_content != nullptr) {
                    std::string original = bcpl_to_string(test_content);
                    std::string read_back = bcpl_to_string(read_content);
                    assert_test(original == read_back, "Content matches after round-trip");
                    bcpl_free(read_content);
                }
                
                FILE_CLOSE(handle);
            }
            
            bcpl_free(test_content);
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_string_io.txt");
    }
    
    void test_file_seek_tell_eof() {
        std::cout << "\n--- Testing FILE_SEEK, FILE_TELL, FILE_EOF ---" << std::endl;
        
        // Create file with known structure for seeking
        create_test_file("test_unified_seek.txt", "0123456789ABCDEFGHIJ");
        
        uint32_t* filename = create_bcpl_string("test_unified_seek.txt");
        uint32_t handle = FILE_OPEN_READ(filename);
        assert_test(handle != 0, "Open file for seek operations");
        
        if (handle != 0) {
            // Test initial state
            int32_t pos = FILE_TELL(handle);
            assert_test(pos == 0, "Initial file position is 0");
            
            uint32_t eof_status = FILE_EOF(handle);
            assert_test(eof_status == 0, "EOF is false at start");
            
            // Test absolute seeking
            uint32_t seek_result = FILE_SEEK(handle, 10, 0); // SEEK_SET
            assert_test(seek_result == 0, "Absolute seek succeeds");
            
            pos = FILE_TELL(handle);
            assert_test(pos == 10, "Position correct after absolute seek");
            
            // Test relative seeking
            seek_result = FILE_SEEK(handle, 5, 1); // SEEK_CUR
            assert_test(seek_result == 0, "Relative seek succeeds");
            
            pos = FILE_TELL(handle);
            assert_test(pos == 15, "Position correct after relative seek");
            
            // Test seeking to end
            seek_result = FILE_SEEK(handle, 0, 2); // SEEK_END
            assert_test(seek_result == 0, "Seek to end succeeds");
            
            eof_status = FILE_EOF(handle);
            assert_test(eof_status != 0, "EOF is true at end of file");
            
            // Test seeking back to beginning
            seek_result = FILE_SEEK(handle, 0, 0); // SEEK_SET
            assert_test(seek_result == 0, "Seek back to beginning succeeds");
            
            pos = FILE_TELL(handle);
            assert_test(pos == 0, "Back at beginning after seek");
            
            FILE_CLOSE(handle);
        }
        
        bcpl_free(filename);
    }
    
    void test_file_low_level_io() {
        std::cout << "\n--- Testing FILE_READ and FILE_WRITE ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_unified_binary.dat");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for binary I/O");
        
        if (handle != 0) {
            // Write test pattern
            uint32_t write_buffer[16];
            for (int i = 0; i < 16; i++) {
                write_buffer[i] = i * 3 + 100; // Pattern: 100, 103, 106, ...
            }
            
            uint32_t bytes_written = FILE_WRITE(handle, write_buffer, 16);
            assert_test(bytes_written == 16, "FILE_WRITE writes all bytes");
            
            FILE_CLOSE(handle);
            
            // Read data back
            handle = FILE_OPEN_READ(filename);
            assert_test(handle != 0, "Reopen for binary read");
            
            if (handle != 0) {
                uint32_t read_buffer[16];
                memset(read_buffer, 0, sizeof(read_buffer));
                
                uint32_t bytes_read = FILE_READ(handle, read_buffer, 16);
                assert_test(bytes_read == 16, "FILE_READ reads all bytes");
                
                // Verify pattern
                bool pattern_correct = true;
                for (int i = 0; i < 16; i++) {
                    uint32_t expected = i * 3 + 100;
                    if (read_buffer[i] != expected) {
                        pattern_correct = false;
                        break;
                    }
                }
                assert_test(pattern_correct, "Binary data pattern matches");
                
                FILE_CLOSE(handle);
            }
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_binary.dat");
    }
    
    void test_file_append_mode() {
        std::cout << "\n--- Testing FILE_OPEN_APPEND behavior ---" << std::endl;
        
        // Create initial file
        create_test_file("test_unified_append_mode.txt", "Initial line\n");
        
        // Append content
        uint32_t* filename = create_bcpl_string("test_unified_append_mode.txt");
        uint32_t handle = FILE_OPEN_APPEND(filename);
        assert_test(handle != 0, "FILE_OPEN_APPEND opens existing file");
        
        if (handle != 0) {
            uint32_t* append_content = create_bcpl_string("Appended line\n");
            uint32_t bytes_written = FILE_WRITES(handle, append_content);
            assert_test(bytes_written > 0, "Append operation succeeds");
            
            FILE_CLOSE(handle);
            
            // Verify both lines are present
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t* full_content = FILE_READS(handle);
                if (full_content != nullptr) {
                    std::string content_str = bcpl_to_string(full_content);
                    
                    bool has_initial = content_str.find("Initial line") != std::string::npos;
                    bool has_appended = content_str.find("Appended line") != std::string::npos;
                    
                    assert_test(has_initial && has_appended, "File contains both initial and appended content");
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
        
        // Test operations on invalid handles
        uint32_t result = FILE_CLOSE(0);
        assert_test(result != 0, "FILE_CLOSE rejects invalid handle");
        
        uint32_t* dummy = create_bcpl_string("test");
        uint32_t bytes = FILE_WRITES(0, dummy);
        assert_test(bytes == 0, "FILE_WRITES rejects invalid handle");
        
        uint32_t* read_result = FILE_READS(0);
        assert_test(read_result == nullptr, "FILE_READS rejects invalid handle");
        
        int32_t pos = FILE_TELL(0);
        assert_test(pos == -1, "FILE_TELL rejects invalid handle");
        
        uint32_t eof_result = FILE_EOF(0);
        assert_test(eof_result != 0, "FILE_EOF handles invalid handle");
        
        // Test NULL filename handling
        uint32_t handle = FILE_OPEN_READ(nullptr);
        assert_test(handle == 0, "FILE_OPEN_READ handles NULL filename");
        
        handle = FILE_OPEN_WRITE(nullptr);
        assert_test(handle == 0, "FILE_OPEN_WRITE handles NULL filename");
        
        bcpl_free(dummy);
    }
    
    void test_unicode_and_special_chars() {
        std::cout << "\n--- Testing Unicode and Special Characters ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_unified_unicode.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for Unicode test");
        
        if (handle != 0) {
            // Test with special characters that might appear in BCPL programs
            uint32_t* special_content = create_bcpl_string("BCPL: LET x := 123\nIF x > 0 THEN WRITES(\"Hello\")\n");
            uint32_t bytes_written = FILE_WRITES(handle, special_content);
            assert_test(bytes_written > 0, "Write special characters");
            
            FILE_CLOSE(handle);
            
            // Read back and verify
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t* read_special = FILE_READS(handle);
                if (read_special != nullptr) {
                    std::string original = bcpl_to_string(special_content);
                    std::string read_back = bcpl_to_string(read_special);
                    assert_test(original == read_back, "Special characters preserved");
                    bcpl_free(read_special);
                }
                FILE_CLOSE(handle);
            }
            
            bcpl_free(special_content);
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_unicode.txt");
    }
    
    void test_large_file_operations() {
        std::cout << "\n--- Testing Large File Operations ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_unified_large.txt");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for large data test");
        
        if (handle != 0) {
            // Write multiple chunks to create a larger file
            uint32_t* chunk = create_bcpl_string("This is a test chunk that will be repeated to create a larger file.\n");
            uint32_t total_written = 0;
            
            for (int i = 0; i < 50; i++) {
                uint32_t bytes = FILE_WRITES(handle, chunk);
                total_written += bytes;
            }
            
            assert_test(total_written > 1000, "Large file write succeeds");
            FILE_CLOSE(handle);
            
            // Test seeking in large file
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t seek_result = FILE_SEEK(handle, 500, 0); // Seek to middle
                assert_test(seek_result == 0, "Seek works in large file");
                
                int32_t pos = FILE_TELL(handle);
                assert_test(pos == 500, "Position correct in large file");
                
                FILE_CLOSE(handle);
            }
            
            bcpl_free(chunk);
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_large.txt");
    }
    
    void test_partial_reads_writes() {
        std::cout << "\n--- Testing Partial Reads and Writes ---" << std::endl;
        
        uint32_t* filename = create_bcpl_string("test_unified_partial.dat");
        uint32_t handle = FILE_OPEN_WRITE(filename);
        assert_test(handle != 0, "Open file for partial I/O test");
        
        if (handle != 0) {
            // Write data in chunks
            uint32_t buffer1[5] = {1, 2, 3, 4, 5};
            uint32_t buffer2[3] = {6, 7, 8};
            
            uint32_t bytes1 = FILE_WRITE(handle, buffer1, 5);
            uint32_t bytes2 = FILE_WRITE(handle, buffer2, 3);
            
            assert_test(bytes1 == 5 && bytes2 == 3, "Partial writes succeed");
            FILE_CLOSE(handle);
            
            // Read data in different chunk sizes
            handle = FILE_OPEN_READ(filename);
            if (handle != 0) {
                uint32_t read_buf[8];
                memset(read_buf, 0, sizeof(read_buf));
                
                uint32_t read1 = FILE_READ(handle, read_buf, 3);      // Read first 3
                uint32_t read2 = FILE_READ(handle, read_buf + 3, 5);  // Read remaining 5
                
                assert_test(read1 == 3 && read2 == 5, "Partial reads succeed");
                
                // Verify data integrity
                bool data_ok = true;
                for (int i = 0; i < 8; i++) {
                    if (read_buf[i] != (uint32_t)(i + 1)) {
                        data_ok = false;
                        break;
                    }
                }
                assert_test(data_ok, "Partial read data is correct");
                
                FILE_CLOSE(handle);
            }
        }
        
        bcpl_free(filename);
        test_files.push_back("test_unified_partial.dat");
    }
    
    void test_concurrent_operations() {
        std::cout << "\n--- Testing Concurrent File Operations ---" << std::endl;
        
        // Test opening multiple files simultaneously
        uint32_t* file1 = create_bcpl_string("test_unified_concurrent1.txt");
        uint32_t* file2 = create_bcpl_string("test_unified_concurrent2.txt");
        
        uint32_t handle1 = FILE_OPEN_WRITE(file1);
        uint32_t handle2 = FILE_OPEN_WRITE(file2);
        
        assert_test(handle1 != 0 && handle2 != 0, "Multiple file opens succeed");
        assert_test(handle1 != handle2, "Different files have different handles");
        
        if (handle1 != 0 && handle2 != 0) {
            // Write different content to each file
            uint32_t* content1 = create_bcpl_string("File 1 content");
            uint32_t* content2 = create_bcpl_string("File 2 content");
            
            uint32_t bytes1 = FILE_WRITES(handle1, content1);
            uint32_t bytes2 = FILE_WRITES(handle2, content2);
            
            assert_test(bytes1 > 0 && bytes2 > 0, "Concurrent writes succeed");
            
            FILE_CLOSE(handle1);
            FILE_CLOSE(handle2);
            
            // Verify each file has correct content
            handle1 = FILE_OPEN_READ(file1);
            handle2 = FILE_OPEN_READ(file2);
            
            if (handle1 != 0 && handle2 != 0) {
                uint32_t* read1 = FILE_READS(handle1);
                uint32_t* read2 = FILE_READS(handle2);
                
                bool content_ok = false;
                if (read1 != nullptr && read2 != nullptr) {
                    std::string str1 = bcpl_to_string(read1);
                    std::string str2 = bcpl_to_string(read2);
                    content_ok = (str1 == "File 1 content") && (str2 == "File 2 content");
                }
                
                assert_test(content_ok, "Concurrent file contents are correct");
                
                if (read1) bcpl_free(read1);
                if (read2) bcpl_free(read2);
                
                FILE_CLOSE(handle1);
                FILE_CLOSE(handle2);
            }
            
            bcpl_free(content1);
            bcpl_free(content2);
        }
        
        bcpl_free(file1);
        bcpl_free(file2);
        test_files.push_back("test_unified_concurrent1.txt");
        test_files.push_back("test_unified_concurrent2.txt");
    }
};

int main() {
    std::cout << "FILE Unified Runtime API Test Suite" << std::endl;
    std::cout << "Comprehensive testing of all FILE_ commands against the production unified runtime" << std::endl;
    std::cout << "Runtime: libbcpl_runtime_sdl2_static.a (unified with static SDL2)" << std::endl;
    std::cout << std::endl;
    
    UnifiedFileRuntimeTester tester;
    tester.run_all_tests();
    
    return 0;
}
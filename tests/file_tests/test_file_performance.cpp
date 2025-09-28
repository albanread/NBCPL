
// Performance test for FILE_ API: measures speed of small file operations
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <cstring>
#include <cassert>

// Define missing global for HeapManager linkage
extern "C" int g_enable_heap_trace = 0;

extern "C" {
    #include "../../runtime/runtime.h"
    uintptr_t FILE_OPEN_WRITE(uint32_t* filename_str);
    uintptr_t FILE_OPEN_READ(uint32_t* filename_str);
    uintptr_t FILE_OPEN_APPEND(uint32_t* filename_str);
    uint32_t FILE_CLOSE(uintptr_t handle);
    uint32_t FILE_WRITES(uintptr_t handle, uint32_t* string_buffer);
    uint32_t* FILE_READS(uintptr_t handle);
    uint32_t FILE_READ(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_WRITE(uintptr_t handle, uint32_t* buffer, uint32_t size);
    uint32_t FILE_SEEK(uintptr_t handle, int32_t offset, uint32_t origin);
    int32_t FILE_TELL(uintptr_t handle);
    uint32_t FILE_EOF(uintptr_t handle);

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

struct PerfStats {
    double write_time_ms = 0;
    double read_time_ms = 0;
    double append_time_ms = 0;
    double seek_time_ms = 0;
    double open_close_time_ms = 0;
    size_t bytes_written = 0;
    size_t bytes_read = 0;
    size_t iterations = 0;
};

void performance_test(size_t iterations, size_t file_size) {
    PerfStats stats;
    stats.iterations = iterations;
    std::string filename = "perf_test_file.txt";
    std::string content(file_size, 'X');

    uint32_t* bcpl_filename = create_bcpl_string(filename);
    uint32_t* bcpl_content = create_bcpl_string(content);

    // --- Write test ---
    auto t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        uintptr_t handle = FILE_OPEN_WRITE(bcpl_filename);
        assert(handle != 0);
        uint32_t written = FILE_WRITES(handle, bcpl_content);
        stats.bytes_written += written;
        FILE_CLOSE(handle);
    }
    auto t2 = std::chrono::high_resolution_clock::now();
    stats.write_time_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Read test ---
    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        uintptr_t handle = FILE_OPEN_READ(bcpl_filename);
        assert(handle != 0);
        uint32_t* read = FILE_READS(handle);
        if (read) {
            stats.bytes_read += bcpl_to_string(read).size();
            bcpl_free(read);
        }
        FILE_CLOSE(handle);
    }
    t2 = std::chrono::high_resolution_clock::now();
    stats.read_time_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Append test ---
    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        uintptr_t handle = FILE_OPEN_APPEND(bcpl_filename);
        assert(handle != 0);
        uint32_t written = FILE_WRITES(handle, bcpl_content);
        stats.bytes_written += written;
        FILE_CLOSE(handle);
    }
    t2 = std::chrono::high_resolution_clock::now();
    stats.append_time_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Seek test ---
    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        uintptr_t handle = FILE_OPEN_READ(bcpl_filename);
        assert(handle != 0);
        for (int j = 0; j < 10; ++j) {
            FILE_SEEK(handle, j, 0); // SEEK_SET
            FILE_TELL(handle);
        }
        FILE_CLOSE(handle);
    }
    t2 = std::chrono::high_resolution_clock::now();
    stats.seek_time_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    // --- Open/Close test ---
    t1 = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations * 10; ++i) {
        uintptr_t handle = FILE_OPEN_READ(bcpl_filename);
        assert(handle != 0);
        FILE_CLOSE(handle);
    }
    t2 = std::chrono::high_resolution_clock::now();
    stats.open_close_time_ms = std::chrono::duration<double, std::milli>(t2 - t1).count();

    bcpl_free(bcpl_filename);
    bcpl_free(bcpl_content);

    // --- Report ---
    std::cout << "==== FILE_ API PERFORMANCE TEST ====" << std::endl;
    std::cout << "Iterations: " << stats.iterations << std::endl;
    std::cout << "File size per op: " << file_size << " bytes" << std::endl;
    std::cout << "Total bytes written: " << stats.bytes_written << std::endl;
    std::cout << "Total bytes read:    " << stats.bytes_read << std::endl;
    std::cout << "Write time:   " << stats.write_time_ms << " ms ("
              << (stats.write_time_ms / iterations) << " ms/op)" << std::endl;
    std::cout << "Read time:    " << stats.read_time_ms << " ms ("
              << (stats.read_time_ms / iterations) << " ms/op)" << std::endl;
    std::cout << "Append time:  " << stats.append_time_ms << " ms ("
              << (stats.append_time_ms / iterations) << " ms/op)" << std::endl;
    std::cout << "Seek time:    " << stats.seek_time_ms << " ms ("
              << (stats.seek_time_ms / iterations) << " ms/op)" << std::endl;
    std::cout << "Open/Close:   " << stats.open_close_time_ms << " ms ("
              << (stats.open_close_time_ms / (iterations * 10)) << " ms/op)" << std::endl;
    std::cout << "==== END PERFORMANCE TEST ====" << std::endl;
}

int main() {
    size_t iterations = 1000;
    size_t file_size = 256; // bytes per file operation
    performance_test(iterations, file_size);
    return 0;
}
/*
 * fast_char_copy.h
 * High-performance character copying for BCPL string operations
 * 
 * This module provides optimized implementations for converting C++ std::string
 * (UTF-8) to BCPL string format (UTF-32). The current character-by-character
 * loop accounts for 53.5% of string creation time, making this a critical
 * optimization target.
 * 
 * Optimizations included:
 * - SIMD vectorized copying for long strings
 * - Unrolled loops for medium strings
 * - Branch-optimized paths for different string lengths
 * - Platform-specific optimizations (ARM64 NEON, x86 SSE/AVX)
 */

#ifndef FAST_CHAR_COPY_H
#define FAST_CHAR_COPY_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Configuration Constants ---
#define FAST_COPY_SIMD_THRESHOLD 16    // Use SIMD for strings >= 16 chars
#define FAST_COPY_UNROLL_THRESHOLD 8   // Use loop unrolling for >= 8 chars
#define FAST_COPY_INLINE_THRESHOLD 4   // Inline small copies for <= 4 chars

// Performance statistics
typedef struct FastCopyStats {
    size_t total_copies;
    size_t total_chars_copied;
    size_t simd_copies;          // Copies that used SIMD
    size_t unrolled_copies;      // Copies that used unrolled loops
    size_t inline_copies;        // Copies that used inline paths
    size_t fallback_copies;      // Copies that used original loop
    double avg_chars_per_copy;
    double simd_usage_rate;      // Percentage of copies using SIMD
} FastCopyStats;

// --- Public API ---

/**
 * Optimized replacement for the character copying loop in create_bcpl_string()
 * 
 * This function replaces:
 *   for (size_t i = 0; i < cpp_string.length(); i++) {
 *       result[i] = static_cast<uint32_t>(static_cast<unsigned char>(cpp_string[i]));
 *   }
 * 
 * @param dest Destination BCPL string buffer (uint32_t array)
 * @param src Source C++ string data (const char*)
 * @param length Number of characters to copy
 */
void fast_copy_chars_to_bcpl(uint32_t* dest, const char* src, size_t length);

/**
 * Optimized bulk copy for multiple strings
 * Useful when processing arrays of strings
 * 
 * @param dest_strings Array of destination BCPL string buffers
 * @param src_strings Array of source C strings
 * @param lengths Array of string lengths
 * @param count Number of strings to process
 * @return Number of strings successfully copied
 */
size_t fast_copy_bulk_strings(uint32_t** dest_strings, const char** src_strings, 
                              const size_t* lengths, size_t count);

/**
 * Get performance statistics for character copying
 */
FastCopyStats fast_copy_get_stats(void);

/**
 * Print detailed performance metrics
 */
void fast_copy_print_metrics(void);

/**
 * Reset performance statistics
 */
void fast_copy_reset_stats(void);

/**
 * Initialize the fast copy system (auto-called on first use)
 * @return true on success, false on failure
 */
bool fast_copy_init(void);

/**
 * Check what optimizations are available on this platform
 */
typedef struct FastCopyCapabilities {
    bool has_simd;               // SIMD instructions available
    bool has_neon;               // ARM NEON available
    bool has_sse2;               // x86 SSE2 available
    bool has_avx2;               // x86 AVX2 available
    const char* platform_name;   // Platform description
    size_t optimal_simd_width;   // Optimal SIMD vector width in chars
} FastCopyCapabilities;

FastCopyCapabilities fast_copy_get_capabilities(void);

// --- Platform-Specific Optimized Functions ---

/**
 * SIMD-optimized copy for long strings
 * Uses ARM NEON or x86 SSE/AVX depending on platform
 * 
 * @param dest Destination buffer (must be 16-byte aligned)
 * @param src Source buffer  
 * @param length Number of characters (must be >= FAST_COPY_SIMD_THRESHOLD)
 */
void fast_copy_simd_chars(uint32_t* dest, const char* src, size_t length);

/**
 * Unrolled loop copy for medium strings
 * Processes 8 characters at a time with loop unrolling
 * 
 * @param dest Destination buffer
 * @param src Source buffer
 * @param length Number of characters (should be >= FAST_COPY_UNROLL_THRESHOLD)
 */
void fast_copy_unrolled_chars(uint32_t* dest, const char* src, size_t length);

/**
 * Inline copy for very short strings
 * Hand-optimized for 1-4 character strings
 * 
 * @param dest Destination buffer
 * @param src Source buffer
 * @param length Number of characters (should be <= FAST_COPY_INLINE_THRESHOLD)
 */
void fast_copy_inline_chars(uint32_t* dest, const char* src, size_t length);

/**
 * Original fallback implementation
 * Used for compatibility and as baseline for benchmarking
 * 
 * @param dest Destination buffer
 * @param src Source buffer
 * @param length Number of characters
 */
void fast_copy_fallback_chars(uint32_t* dest, const char* src, size_t length);

// --- Utility Functions ---

/**
 * Validate that a character copy was performed correctly
 * Useful for testing and debugging optimized implementations
 * 
 * @param dest Destination buffer to check
 * @param src Source buffer to compare against
 * @param length Number of characters to validate
 * @return true if copy is correct, false otherwise
 */
bool fast_copy_validate(const uint32_t* dest, const char* src, size_t length);

/**
 * Benchmark different copy implementations
 * Measures performance of SIMD vs unrolled vs inline vs fallback
 * 
 * @param test_string_length Length of strings to benchmark
 * @param iterations Number of iterations to run
 */
void fast_copy_benchmark(size_t test_string_length, size_t iterations);

/**
 * Memory alignment helper
 * Check if a pointer is properly aligned for SIMD operations
 * 
 * @param ptr Pointer to check
 * @param alignment Required alignment (typically 16 or 32 bytes)
 * @return true if properly aligned
 */
bool fast_copy_is_aligned(const void* ptr, size_t alignment);

#ifdef __cplusplus
}
#endif

#endif // FAST_CHAR_COPY_H
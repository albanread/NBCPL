/*
 * fast_char_copy.c
 * High-performance character copying implementation for BCPL string operations
 * 
 * This module provides optimized implementations for converting C++ std::string
 * (UTF-8) to BCPL string format (UTF-32). This addresses the 53.5% bottleneck
 * identified in string creation performance.
 */

#include "fast_char_copy.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

// Platform detection
#ifdef __ARM_NEON
#include <arm_neon.h>
#define HAS_NEON 1
#else
#define HAS_NEON 0
#endif

#ifdef __SSE2__
#include <emmintrin.h>
#define HAS_SSE2 1
#else
#define HAS_SSE2 0
#endif

#ifdef __AVX2__
#include <immintrin.h>
#define HAS_AVX2 1
#else
#define HAS_AVX2 0
#endif

// --- Global Statistics ---
static FastCopyStats g_copy_stats = {0};
static bool g_copy_initialized = false;

// --- Platform Detection ---
static FastCopyCapabilities g_capabilities = {0};

static void detect_platform_capabilities(void) {
    g_capabilities.has_neon = HAS_NEON;
    g_capabilities.has_sse2 = HAS_SSE2;
    g_capabilities.has_avx2 = HAS_AVX2;
    g_capabilities.has_simd = HAS_NEON || HAS_SSE2 || HAS_AVX2;
    
    if (HAS_AVX2) {
        g_capabilities.platform_name = "x86_64 with AVX2";
        g_capabilities.optimal_simd_width = 32; // 256-bit vectors
    } else if (HAS_SSE2) {
        g_capabilities.platform_name = "x86_64 with SSE2";
        g_capabilities.optimal_simd_width = 16; // 128-bit vectors
    } else if (HAS_NEON) {
        g_capabilities.platform_name = "ARM64 with NEON";
        g_capabilities.optimal_simd_width = 16; // 128-bit vectors
    } else {
        g_capabilities.platform_name = "Generic (no SIMD)";
        g_capabilities.optimal_simd_width = 4; // 32-bit scalar
    }
}

// --- Implementation Functions ---

void fast_copy_fallback_chars(uint32_t* dest, const char* src, size_t length) {
    // Original implementation - character by character
    for (size_t i = 0; i < length; i++) {
        dest[i] = static_cast<uint32_t>(static_cast<unsigned char>(src[i]));
    }
}

void fast_copy_inline_chars(uint32_t* dest, const char* src, size_t length) {
    // Hand-optimized for very short strings (1-4 chars)
    switch (length) {
        case 4:
            dest[3] = (uint32_t)(unsigned char)src[3];
            // fallthrough
        case 3:
            dest[2] = (uint32_t)(unsigned char)src[2];
            // fallthrough
        case 2:
            dest[1] = (uint32_t)(unsigned char)src[1];
            // fallthrough
        case 1:
            dest[0] = (uint32_t)(unsigned char)src[0];
            break;
        case 0:
            break;
        default:
            // Fallback for unexpected lengths
            fast_copy_fallback_chars(dest, src, length);
            break;
    }
}

void fast_copy_unrolled_chars(uint32_t* dest, const char* src, size_t length) {
    // Process 8 characters at a time with loop unrolling
    size_t i = 0;
    size_t unroll_limit = length - (length % 8);
    
    for (i = 0; i < unroll_limit; i += 8) {
        dest[i + 0] = (uint32_t)(unsigned char)src[i + 0];
        dest[i + 1] = (uint32_t)(unsigned char)src[i + 1];
        dest[i + 2] = (uint32_t)(unsigned char)src[i + 2];
        dest[i + 3] = (uint32_t)(unsigned char)src[i + 3];
        dest[i + 4] = (uint32_t)(unsigned char)src[i + 4];
        dest[i + 5] = (uint32_t)(unsigned char)src[i + 5];
        dest[i + 6] = (uint32_t)(unsigned char)src[i + 6];
        dest[i + 7] = (uint32_t)(unsigned char)src[i + 7];
    }
    
    // Handle remaining characters
    for (; i < length; i++) {
        dest[i] = (uint32_t)(unsigned char)src[i];
    }
}

#if HAS_NEON
void fast_copy_simd_chars(uint32_t* dest, const char* src, size_t length) {
    // ARM NEON implementation
    size_t i = 0;
    size_t simd_limit = length - (length % 16);
    
    for (i = 0; i < simd_limit; i += 16) {
        // Load 16 bytes from source
        uint8x16_t src_bytes = vld1q_u8((const uint8_t*)(src + i));
        
        // Convert to 4 sets of 4 uint32_t values
        uint8x8_t low8 = vget_low_u8(src_bytes);
        uint8x8_t high8 = vget_high_u8(src_bytes);
        
        // Widen to 16-bit
        uint16x8_t low16 = vmovl_u8(low8);
        uint16x8_t high16 = vmovl_u8(high8);
        
        // Widen to 32-bit and store
        uint32x4_t part0 = vmovl_u16(vget_low_u16(low16));
        uint32x4_t part1 = vmovl_u16(vget_high_u16(low16));
        uint32x4_t part2 = vmovl_u16(vget_low_u16(high16));
        uint32x4_t part3 = vmovl_u16(vget_high_u16(high16));
        
        vst1q_u32(dest + i + 0, part0);
        vst1q_u32(dest + i + 4, part1);
        vst1q_u32(dest + i + 8, part2);
        vst1q_u32(dest + i + 12, part3);
    }
    
    // Handle remaining characters
    for (; i < length; i++) {
        dest[i] = (uint32_t)(unsigned char)src[i];
    }
}
#elif HAS_SSE2
void fast_copy_simd_chars(uint32_t* dest, const char* src, size_t length) {
    // x86 SSE2 implementation
    size_t i = 0;
    size_t simd_limit = length - (length % 16);
    
    for (i = 0; i < simd_limit; i += 16) {
        // Load 16 bytes
        __m128i src_bytes = _mm_loadu_si128((const __m128i*)(src + i));
        
        // Unpack to 16-bit (zero extend)
        __m128i low8 = _mm_unpacklo_epi8(src_bytes, _mm_setzero_si128());
        __m128i high8 = _mm_unpackhi_epi8(src_bytes, _mm_setzero_si128());
        
        // Unpack to 32-bit (zero extend)
        __m128i part0 = _mm_unpacklo_epi16(low8, _mm_setzero_si128());
        __m128i part1 = _mm_unpackhi_epi16(low8, _mm_setzero_si128());
        __m128i part2 = _mm_unpacklo_epi16(high8, _mm_setzero_si128());
        __m128i part3 = _mm_unpackhi_epi16(high8, _mm_setzero_si128());
        
        // Store results
        _mm_storeu_si128((__m128i*)(dest + i + 0), part0);
        _mm_storeu_si128((__m128i*)(dest + i + 4), part1);
        _mm_storeu_si128((__m128i*)(dest + i + 8), part2);
        _mm_storeu_si128((__m128i*)(dest + i + 12), part3);
    }
    
    // Handle remaining characters
    for (; i < length; i++) {
        dest[i] = (uint32_t)(unsigned char)src[i];
    }
}
#else
void fast_copy_simd_chars(uint32_t* dest, const char* src, size_t length) {
    // No SIMD available - fall back to unrolled loop
    fast_copy_unrolled_chars(dest, src, length);
}
#endif

// --- Public API Implementation ---

bool fast_copy_init(void) {
    if (g_copy_initialized) {
        return true;
    }
    
    detect_platform_capabilities();
    
    // Initialize statistics
    memset(&g_copy_stats, 0, sizeof(g_copy_stats));
    
    g_copy_initialized = true;
    
    printf("FastCharCopy: Initialized for %s\n", g_capabilities.platform_name);
    printf("SIMD support: %s (optimal width: %zu bytes)\n", 
           g_capabilities.has_simd ? "YES" : "NO", 
           g_capabilities.optimal_simd_width);
    
    return true;
}

void fast_copy_chars_to_bcpl(uint32_t* dest, const char* src, size_t length) {
    if (!g_copy_initialized) {
        fast_copy_init();
    }
    
    // Update statistics
    g_copy_stats.total_copies++;
    g_copy_stats.total_chars_copied += length;
    
    // Choose optimal copy strategy based on length
    if (length == 0) {
        return;
    } else if (length <= FAST_COPY_INLINE_THRESHOLD) {
        fast_copy_inline_chars(dest, src, length);
        g_copy_stats.inline_copies++;
    } else if (g_capabilities.has_simd && length >= FAST_COPY_SIMD_THRESHOLD) {
        fast_copy_simd_chars(dest, src, length);
        g_copy_stats.simd_copies++;
    } else if (length >= FAST_COPY_UNROLL_THRESHOLD) {
        fast_copy_unrolled_chars(dest, src, length);
        g_copy_stats.unrolled_copies++;
    } else {
        fast_copy_fallback_chars(dest, src, length);
        g_copy_stats.fallback_copies++;
    }
}

size_t fast_copy_bulk_strings(uint32_t** dest_strings, const char** src_strings, 
                              const size_t* lengths, size_t count) {
    size_t copied = 0;
    
    for (size_t i = 0; i < count; i++) {
        if (dest_strings[i] && src_strings[i]) {
            fast_copy_chars_to_bcpl(dest_strings[i], src_strings[i], lengths[i]);
            copied++;
        }
    }
    
    return copied;
}

FastCopyStats fast_copy_get_stats(void) {
    if (g_copy_stats.total_copies > 0) {
        g_copy_stats.avg_chars_per_copy = (double)g_copy_stats.total_chars_copied / g_copy_stats.total_copies;
        g_copy_stats.simd_usage_rate = (double)g_copy_stats.simd_copies / g_copy_stats.total_copies * 100.0;
    }
    
    return g_copy_stats;
}

void fast_copy_print_metrics(void) {
    FastCopyStats stats = fast_copy_get_stats();
    
    printf("\n=== Fast Character Copy Metrics ===\n");
    printf("Platform: %s\n", g_capabilities.platform_name);
    printf("Total copies performed: %zu\n", stats.total_copies);
    printf("Total characters copied: %zu\n", stats.total_chars_copied);
    printf("Average chars per copy: %.1f\n", stats.avg_chars_per_copy);
    printf("\nOptimization usage:\n");
    printf("  SIMD copies: %zu (%.1f%%)\n", stats.simd_copies, stats.simd_usage_rate);
    printf("  Unrolled copies: %zu (%.1f%%)\n", stats.unrolled_copies, 
           stats.total_copies > 0 ? (double)stats.unrolled_copies / stats.total_copies * 100.0 : 0.0);
    printf("  Inline copies: %zu (%.1f%%)\n", stats.inline_copies,
           stats.total_copies > 0 ? (double)stats.inline_copies / stats.total_copies * 100.0 : 0.0);
    printf("  Fallback copies: %zu (%.1f%%)\n", stats.fallback_copies,
           stats.total_copies > 0 ? (double)stats.fallback_copies / stats.total_copies * 100.0 : 0.0);
    printf("=======================================\n");
}

void fast_copy_reset_stats(void) {
    memset(&g_copy_stats, 0, sizeof(g_copy_stats));
    printf("FastCharCopy: Statistics reset\n");
}

FastCopyCapabilities fast_copy_get_capabilities(void) {
    if (!g_copy_initialized) {
        fast_copy_init();
    }
    return g_capabilities;
}

bool fast_copy_validate(const uint32_t* dest, const char* src, size_t length) {
    for (size_t i = 0; i < length; i++) {
        uint32_t expected = (uint32_t)(unsigned char)src[i];
        if (dest[i] != expected) {
            printf("FastCharCopy validation failed at position %zu: got %u, expected %u\n", 
                   i, dest[i], expected);
            return false;
        }
    }
    return true;
}

void fast_copy_benchmark(size_t test_string_length, size_t iterations) {
    printf("FastCharCopy benchmark: %zu chars × %zu iterations\n", test_string_length, iterations);
    
    // Prepare test data
    char* test_src = (char*)malloc(test_string_length);
    uint32_t* test_dest = (uint32_t*)malloc(test_string_length * sizeof(uint32_t));
    
    if (!test_src || !test_dest) {
        printf("Benchmark failed: memory allocation error\n");
        free(test_src);
        free(test_dest);
        return;
    }
    
    // Initialize test string
    for (size_t i = 0; i < test_string_length; i++) {
        test_src[i] = (char)(32 + (i % 95)); // Printable ASCII
    }
    
    // Benchmark different implementations
    struct timespec start, end;
    double ms;
    
    // Fallback implementation
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < iterations; i++) {
        fast_copy_fallback_chars(test_dest, test_src, test_string_length);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    printf("  Fallback: %.2f ms (%.1f M chars/sec)\n", ms, 
           (iterations * test_string_length) / (ms * 1000.0));
    
    // Unrolled implementation
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < iterations; i++) {
        fast_copy_unrolled_chars(test_dest, test_src, test_string_length);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    printf("  Unrolled: %.2f ms (%.1f M chars/sec)\n", ms, 
           (iterations * test_string_length) / (ms * 1000.0));
    
    // SIMD implementation (if available)
    if (g_capabilities.has_simd && test_string_length >= FAST_COPY_SIMD_THRESHOLD) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        for (size_t i = 0; i < iterations; i++) {
            fast_copy_simd_chars(test_dest, test_src, test_string_length);
        }
        clock_gettime(CLOCK_MONOTONIC, &end);
        ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
        printf("  SIMD: %.2f ms (%.1f M chars/sec)\n", ms, 
               (iterations * test_string_length) / (ms * 1000.0));
    }
    
    // Optimized dispatch
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (size_t i = 0; i < iterations; i++) {
        fast_copy_chars_to_bcpl(test_dest, test_src, test_string_length);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    ms = (end.tv_sec - start.tv_sec) * 1000.0 + (end.tv_nsec - start.tv_nsec) / 1000000.0;
    printf("  Optimized: %.2f ms (%.1f M chars/sec)\n", ms, 
           (iterations * test_string_length) / (ms * 1000.0));
    
    free(test_src);
    free(test_dest);
}

bool fast_copy_is_aligned(const void* ptr, size_t alignment) {
    return ((uintptr_t)ptr % alignment) == 0;
}
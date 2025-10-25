/**
 * @file cairo_bridge_utils.cpp
 * @brief Utility functions for bridging BCPL strings and vectors to C++ types for Cairo integration
 * 
 * This file provides the missing utility functions that the Cairo runtime needs
 * to convert between BCPL data types and C/C++ data types.
 */

#include <cstring>
#include <cstdlib>
#include <string>

// BCPL type definitions (should match runtime_api.h)
typedef uint32_t* bcpl_string_t;
typedef uint32_t* bcpl_vector_t;

extern "C" {

/**
 * Convert a BCPL string to a C string
 * BCPL strings are stored as arrays of 32-bit words where each character is a UTF-32 codepoint
 * This simplified version handles ASCII characters properly like WRITES function
 */
char* bcpl_string_to_c_string(bcpl_string_t bcpl_str) {
    if (!bcpl_str) {
        return nullptr;
    }
    
    // Find the length of the BCPL string (null-terminated)
    size_t len = 0;
    while (bcpl_str[len] != 0 && len < 1000) { // Safety limit
        len++;
    }
    
    // Allocate buffer for C string using calloc for zero-initialization
    char* c_str = (char*)calloc(len + 1, sizeof(char));
    if (!c_str) {
        return nullptr;
    }
    
    // Convert each 32-bit character to ASCII (simplified)
    for (size_t i = 0; i < len; i++) {
        uint32_t codepoint = bcpl_str[i];
        // Handle ASCII range safely
        if (codepoint > 0 && codepoint <= 0x7F) {
            c_str[i] = (char)codepoint;
        } else {
            c_str[i] = '?'; // Replace non-ASCII or null with '?'
        }
    }
    
    // Ensure null termination (calloc already does this, but be explicit)
    c_str[len] = '\0';
    return c_str;
}

/**
 * Convert a C string to a BCPL string
 * This allocates a new BCPL string (simplified ASCII version)
 */
bcpl_string_t c_string_to_bcpl_string(const char* c_str) {
    if (!c_str) {
        return nullptr;
    }
    
    size_t c_len = strlen(c_str);
    
    // Allocate BCPL string (length + null terminator)
    bcpl_string_t bcpl_str = (bcpl_string_t)malloc((c_len + 1) * sizeof(uint32_t));
    if (!bcpl_str) {
        return nullptr;
    }
    
    // Simple ASCII conversion
    for (size_t i = 0; i < c_len; i++) {
        bcpl_str[i] = (uint32_t)(unsigned char)c_str[i];
    }
    
    bcpl_str[c_len] = 0; // Null terminator
    return bcpl_str;
}

/**
 * Convert a BCPL vector to a double array
 * BCPL vectors are assumed to be arrays of doubles stored as pairs of 32-bit words
 */
double* bcpl_vector_to_double_array(bcpl_vector_t bcpl_vec, size_t* count) {
    if (!bcpl_vec || !count) {
        if (count) *count = 0;
        return nullptr;
    }
    
    // The first word of a BCPL vector typically contains its length
    // For simplicity, we'll assume the vector is null-terminated or has a length prefix
    // This implementation assumes the vector contains doubles stored as 64-bit values
    // split across two 32-bit words (little-endian)
    
    size_t vec_len = 0;
    
    // Count non-zero pairs (assuming doubles are stored as two consecutive 32-bit words)
    while (bcpl_vec[vec_len * 2] != 0 || bcpl_vec[vec_len * 2 + 1] != 0) {
        vec_len++;
        // Safety check to prevent infinite loops
        if (vec_len > 10000) break;
    }
    
    *count = vec_len;
    if (vec_len == 0) {
        return nullptr;
    }
    
    double* result = (double*)malloc(vec_len * sizeof(double));
    if (!result) {
        *count = 0;
        return nullptr;
    }
    
    // Convert pairs of 32-bit words to doubles
    for (size_t i = 0; i < vec_len; i++) {
        // Reconstruct 64-bit double from two 32-bit words
        uint64_t bits = ((uint64_t)bcpl_vec[i * 2 + 1] << 32) | bcpl_vec[i * 2];
        result[i] = *(double*)&bits;
    }
    
    return result;
}

/**
 * Convert a BCPL vector to a uint32_t array
 */
uint32_t* bcpl_vector_to_uint32_array(bcpl_vector_t bcpl_vec, size_t* count) {
    if (!bcpl_vec || !count) {
        if (count) *count = 0;
        return nullptr;
    }
    
    // Count non-zero elements
    size_t vec_len = 0;
    while (bcpl_vec[vec_len] != 0) {
        vec_len++;
        // Safety check
        if (vec_len > 10000) break;
    }
    
    *count = vec_len;
    if (vec_len == 0) {
        return nullptr;
    }
    
    uint32_t* result = (uint32_t*)malloc(vec_len * sizeof(uint32_t));
    if (!result) {
        *count = 0;
        return nullptr;
    }
    
    // Copy the data
    for (size_t i = 0; i < vec_len; i++) {
        result[i] = bcpl_vec[i];
    }
    
    return result;
}

} // extern "C"
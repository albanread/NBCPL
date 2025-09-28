// runtime_io.inc
// File I/O implementation for BCPL runtime
// This file is included by runtime.c (for standalone builds) and runtime_bridge.cpp (for JIT)
// Do not include this file directly in other source files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper to convert a BCPL string to a temporary C string for file operations
static char* bcpl_to_c_string(const uint32_t* bcpl_str) {
    if (!bcpl_str) return NULL;

    // Calculate length of BCPL string
    size_t len = 0;
    while (bcpl_str[len] != 0) {
        len++;
    }

    // Allocate a zero-initialized buffer using calloc for safety.
    // This guarantees null termination even if the loop fails.
    char* c_str = (char*)calloc(len * 4 + 1, sizeof(char));
    if (!c_str) return NULL;

    // Convert the string
    size_t pos = 0;
    for (size_t i = 0; i < len; i++) {
        uint32_t codepoint = bcpl_str[i];

        if (codepoint < 0x80) {
            // ASCII
            c_str[pos++] = (char)codepoint;
        } else if (codepoint < 0x800) {
            // 2-byte sequence
            c_str[pos++] = (char)(0xC0 | ((codepoint >> 6) & 0x1F));
            c_str[pos++] = (char)(0x80 | (codepoint & 0x3F));
        } else if (codepoint < 0x10000) {
            // 3-byte sequence
            c_str[pos++] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
            c_str[pos++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            c_str[pos++] = (char)(0x80 | (codepoint & 0x3F));
        } else {
            // 4-byte sequence
            c_str[pos++] = (char)(0xF0 | ((codepoint >> 18) & 0x07));
            c_str[pos++] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
            c_str[pos++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            c_str[pos++] = (char)(0x80 | (codepoint & 0x3F));
        }
    }
    // Final null terminator is already set by calloc, but we set it again for clarity.
    c_str[pos] = '\0';

    return c_str;
}

#include "BCPLError.h"
extern void _BCPL_SET_ERROR(BCPLErrorCode, const char*, const char*);

uint32_t* SLURP(uint32_t* filename_str) {
    if (!filename_str) return NULL;

    char* c_filename = bcpl_to_c_string(filename_str);
    if (!c_filename) return NULL;

    FILE* file = fopen(c_filename, "rb");
    if (!file) {
        _BCPL_SET_ERROR(ERROR_FILE_NOT_FOUND, "SLURP", "fopen failed for the given filename");
        free(c_filename);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    if (file_size < 0) {
        fclose(file);
        free(c_filename);
        return NULL;
    }

    // Allocate buffer for file contents
    unsigned char* temp_buffer = (unsigned char*)malloc(file_size);
    if (!temp_buffer) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "SLURP", "malloc failed for file buffer");
        fclose(file);
        free(c_filename);
        return NULL;
    }

    // Read file
    size_t bytes_read = fread(temp_buffer, 1, file_size, file);
    fclose(file);
    free(c_filename);

    if (bytes_read != (size_t)file_size) {
        _BCPL_SET_ERROR(ERROR_FILE_IO, "SLURP", "fread did not read expected number of bytes");
        free(temp_buffer);
        return NULL;
    }

    // First pass: count Unicode codepoints
    size_t codepoint_count = 0;
    const unsigned char* p_count = temp_buffer;
    const unsigned char* end = temp_buffer + bytes_read;
    while (p_count < end) {
        decode_utf8_char(&p_count, end); // advances pointer
        codepoint_count++;
    }

    // Single allocation for result
    uint32_t* result_payload = (uint32_t*)bcpl_alloc_chars(codepoint_count);
    if (!result_payload) {
        _BCPL_SET_ERROR(ERROR_OUT_OF_MEMORY, "SLURP", "bcpl_alloc_chars failed for result string");
        free(temp_buffer);
        return NULL;
    }

    // Second pass: decode and populate
    const unsigned char* p_decode = temp_buffer;
    for (size_t i = 0; i < codepoint_count; ++i) {
        result_payload[i] = decode_utf8_char(&p_decode, end);
    }
    // Null terminator is already handled by bcpl_alloc_chars

    // Cleanup and return
    free(temp_buffer);
    return result_payload;
}


void SPIT(uint32_t* bcpl_string, uint32_t* filename_str) {
    if (!bcpl_string || !filename_str) return;

    // Convert filename to C string
    char* c_filename = bcpl_to_c_string(filename_str);
    if (!c_filename) return;

    FILE* file = fopen(c_filename, "wb");
    free(c_filename);
    if (!file) return;

    // Write the BCPL string as UTF-8
    size_t len = 0;
    while (bcpl_string[len] != 0) len++;

    // Convert to UTF-8 and write
    for (size_t i = 0; i < len; ++i) {
        uint32_t codepoint = bcpl_string[i];
        if (codepoint < 0x80) {
            fputc((char)codepoint, file);
        } else if (codepoint < 0x800) {
            fputc(0xC0 | ((codepoint >> 6) & 0x1F), file);
            fputc(0x80 | (codepoint & 0x3F), file);
        } else if (codepoint < 0x10000) {
            fputc(0xE0 | ((codepoint >> 12) & 0x0F), file);
            fputc(0x80 | ((codepoint >> 6) & 0x3F), file);
            fputc(0x80 | (codepoint & 0x3F), file);
        } else {
            fputc(0xF0 | ((codepoint >> 18) & 0x07), file);
            fputc(0x80 | ((codepoint >> 12) & 0x3F), file);
            fputc(0x80 | ((codepoint >> 6) & 0x3F), file);
            fputc(0x80 | (codepoint & 0x3F), file);
        }
    }

    fclose(file);
}

//------------------------------------------------------------
// Prints a newline character to the standard output.
// BCPL Signature: NEWLINE()
void NEWLINE(void) {
    printf("\n");
    fflush(stdout);
}

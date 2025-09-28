// runtime_file_api.inc
// Comprehensive File API implementation for BCPL runtime
// This file is included by runtime.c (for standalone builds) and runtime_bridge.cpp (for JIT)
// Do not include this file directly in other source files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

// Include heap manager for metrics tracking
#include "../HeapManager/heap_manager_defs.h"

// Local UTF-8 decoding function to avoid include conflicts
#ifndef DECODE_UTF8_CHAR_DEFINED
#define DECODE_UTF8_CHAR_DEFINED
static uint32_t decode_utf8_char(const unsigned char** ptr, const unsigned char* end) {
    const unsigned char* p = *ptr;

    if (p >= end) {
        return 0; // End of buffer
    }

    uint32_t c = *p++;

    // Single byte (ASCII)
    if ((c & 0x80) == 0) {
        *ptr = p;
        return c;
    }

    // Multi-byte sequence
    int extra_bytes = 0;
    uint32_t min_code = 0;

    if ((c & 0xE0) == 0xC0) {
        // 2-byte sequence
        extra_bytes = 1;
        min_code = 0x80;
        c &= 0x1F;
    } else if ((c & 0xF0) == 0xE0) {
        // 3-byte sequence
        extra_bytes = 2;
        min_code = 0x800;
        c &= 0x0F;
    } else if ((c & 0xF8) == 0xF0) {
        // 4-byte sequence
        extra_bytes = 3;
        min_code = 0x10000;
        c &= 0x07;
    } else {
        // Invalid UTF-8 sequence
        *ptr = p;
        return 0xFFFD; // Unicode replacement character
    }

    // Read continuation bytes
    for (int i = 0; i < extra_bytes; i++) {
        if (p >= end || (*p & 0xC0) != 0x80) {
            // Invalid continuation byte or premature end of buffer
            *ptr = p;
            return 0xFFFD; // Unicode replacement character
        }
        c = (c << 6) | (*p++ & 0x3F);
    }

    // Check for overlong encoding
    if (c < min_code) {
        *ptr = p;
        return 0xFFFD; // Unicode replacement character
    }

    // Check for valid Unicode range
    if (c > 0x10FFFF || (c >= 0xD800 && c <= 0xDFFF)) {
        *ptr = p;
        return 0xFFFD; // Unicode replacement character
    }

    *ptr = p;
    return c;
}
#endif // DECODE_UTF8_CHAR_DEFINED

// Helper to convert a BCPL string to a temporary C string for file operations
static char* bcpl_to_c_string_for_file(const uint32_t* bcpl_str) {
    if (!bcpl_str) return NULL;

    // Calculate length of BCPL string with safety bounds to prevent hanging
    size_t len = 0;
    const size_t MAX_STRING_LEN = 4096; // Safety limit

    // Safe string length calculation with early termination
    while (len < MAX_STRING_LEN) {
        uint32_t ch = bcpl_str[len];

        // Found null terminator - string ends here
        if (ch == 0) {
            break;
        }

        // Basic validation: reject obviously corrupted values
        if (ch > 0x10FFFF) {
            // Invalid Unicode - likely corrupted pointer
            return NULL;
        }

        len++;
    }

    // If we hit the limit without finding null terminator, reject the string
    if (len >= MAX_STRING_LEN) {
        return NULL;
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
    // Final null terminator is already set by calloc
    c_str[pos] = '\0';

    return c_str;
}

//==============================================================================
// Opening and Closing Files
//==============================================================================

// FILE_OPEN_READ(filename): Opens a file for reading.
// Returns: file handle (FILE*) cast to uintptr_t, or 0 on failure
uintptr_t FILE_OPEN_READ(uint32_t* filename_str) {
    if (!filename_str) return 0;

    char* c_filename = bcpl_to_c_string_for_file(filename_str);
    if (!c_filename) return 0;

    FILE* file = fopen(c_filename, "rb");
    free(c_filename);

    if (file) {
        update_io_metrics_file_opened();
    }

    return (uintptr_t)file;
}

// FILE_OPEN_WRITE(filename): Creates/truncates a file for writing.
// Returns: file handle (FILE*) cast to uintptr_t, or 0 on failure
uintptr_t FILE_OPEN_WRITE(uint32_t* filename_str) {
    if (!filename_str) return 0;

    char* c_filename = bcpl_to_c_string_for_file(filename_str);
    if (!c_filename) return 0;

    FILE* file = fopen(c_filename, "wb");
    free(c_filename);

    if (file) {
        update_io_metrics_file_opened();
    }

    return (uintptr_t)file;
}

// FILE_OPEN_APPEND(filename): Opens or creates a file to append to.
// Returns: file handle (FILE*) cast to uintptr_t, or 0 on failure
uintptr_t FILE_OPEN_APPEND(uint32_t* filename_str) {
    if (!filename_str) return 0;

    char* c_filename = bcpl_to_c_string_for_file(filename_str);
    if (!c_filename) return 0;

    FILE* file = fopen(c_filename, "ab");
    free(c_filename);

    if (file) {
        update_io_metrics_file_opened();
    }

    return (uintptr_t)file;
}

// FILE_CLOSE(handle): Closes the file.
// Returns: 0 on success, non-zero on failure
uint32_t FILE_CLOSE(uintptr_t handle) {
    if (handle == 0) return 1; // Invalid handle

    FILE* file = (FILE*)(uintptr_t)handle;
    int result = fclose(file);

    if (result == 0) {
        update_io_metrics_file_closed();
    }

    return (uint32_t)result;
}

//==============================================================================
// High-Level String I/O
//==============================================================================

// FILE_WRITES(handle, string_buffer): Writes a null-terminated string to the file.
// The runtime will automatically calculate the string's length.
// Returns: number of bytes written, or 0 on failure
uint32_t FILE_WRITES(uintptr_t handle, uint32_t* string_buffer) {
    if (!string_buffer || handle == 0) return 0;

    FILE* file = (FILE*)(uintptr_t)handle;

    // Calculate length of BCPL string
    size_t len = 0;
    while (string_buffer[len] != 0) {
        len++;
    }

    // Convert to UTF-8 bytes
    char* utf8_buffer = (char*)malloc(len * 4 + 1);
    if (!utf8_buffer) return 0;

    size_t pos = 0;
    for (size_t i = 0; i < len; i++) {
        uint32_t codepoint = string_buffer[i];

        if (codepoint < 0x80) {
            // ASCII
            utf8_buffer[pos++] = (char)codepoint;
        } else if (codepoint < 0x800) {
            // 2-byte sequence
            utf8_buffer[pos++] = (char)(0xC0 | ((codepoint >> 6) & 0x1F));
            utf8_buffer[pos++] = (char)(0x80 | (codepoint & 0x3F));
        } else if (codepoint < 0x10000) {
            // 3-byte sequence
            utf8_buffer[pos++] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
            utf8_buffer[pos++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            utf8_buffer[pos++] = (char)(0x80 | (codepoint & 0x3F));
        } else {
            // 4-byte sequence
            utf8_buffer[pos++] = (char)(0xF0 | ((codepoint >> 18) & 0x07));
            utf8_buffer[pos++] = (char)(0x80 | ((codepoint >> 12) & 0x3F));
            utf8_buffer[pos++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
            utf8_buffer[pos++] = (char)(0x80 | (codepoint & 0x3F));
        }
    }

    size_t bytes_written = fwrite(utf8_buffer, 1, pos, file);
    free(utf8_buffer);

    if (bytes_written > 0) {
        update_io_metrics_write(bytes_written);
    }

    return (uint32_t)bytes_written;
}

// FILE_READS(handle): Reads all content from the file's current position to the end
// and returns it as a new string.
// Returns: A new BCPL string containing the file's content, or NULL on failure
uint32_t* FILE_READS(uintptr_t handle) {
    if (handle == 0) return NULL;

    FILE* file = (FILE*)(uintptr_t)handle;

    // Get current file position
    long current_pos = ftell(file);
    if (current_pos < 0) return NULL;

    // Seek to end to get size of remaining content
    if (fseek(file, 0, SEEK_END) != 0) return NULL;
    long end_pos = ftell(file);
    if (end_pos < 0) return NULL;

    // Calculate size of content to read
    long content_size = end_pos - current_pos;
    if (content_size <= 0) {
        // Seek back to original position
        fseek(file, current_pos, SEEK_SET);
        // Return empty string
        uint32_t* empty_str = (uint32_t*)bcpl_alloc_chars(0);
        return empty_str;
    }

    // Seek back to original position
    if (fseek(file, current_pos, SEEK_SET) != 0) return NULL;

    // Allocate buffer for file content
    unsigned char* temp_buffer = (unsigned char*)malloc(content_size);
    if (!temp_buffer) return NULL;

    // Read file content
    size_t bytes_read = fread(temp_buffer, 1, content_size, file);
    if (bytes_read != (size_t)content_size) {
        free(temp_buffer);
        return NULL;
    }

    // Update metrics
    update_io_metrics_read(bytes_read);

    // First pass: count Unicode codepoints
    size_t codepoint_count = 0;
    const unsigned char* p_count = temp_buffer;
    const unsigned char* end = temp_buffer + bytes_read;
    while (p_count < end) {
        decode_utf8_char(&p_count, end); // advances pointer
        codepoint_count++;
    }

    // Allocate BCPL string
    uint32_t* result_payload = (uint32_t*)bcpl_alloc_chars(codepoint_count);
    if (!result_payload) {
        free(temp_buffer);
        return NULL;
    }

    // Second pass: decode and populate
    const unsigned char* p_decode = temp_buffer;
    for (size_t i = 0; i < codepoint_count; ++i) {
        result_payload[i] = decode_utf8_char(&p_decode, end);
    }
    // Null terminator is already handled by bcpl_alloc_chars

    free(temp_buffer);
    return result_payload;
}

//==============================================================================
// Low-Level Byte I/O
//==============================================================================

// FILE_READ(handle, buffer, size): Reads size bytes into a buffer.
// Returns: number of bytes actually read
uint32_t FILE_READ(uintptr_t handle, uint32_t* buffer, uint32_t size) {
    if (!buffer || size == 0 || handle == 0) return 0;

    FILE* file = (FILE*)(uintptr_t)handle;

    // Allocate temporary byte buffer
    unsigned char* temp_buffer = (unsigned char*)malloc(size);
    if (!temp_buffer) return 0;

    size_t bytes_read = fread(temp_buffer, 1, size, file);

    // Copy bytes to BCPL buffer (each uint32_t holds one byte)
    for (size_t i = 0; i < bytes_read; i++) {
        buffer[i] = (uint32_t)temp_buffer[i];
    }

    free(temp_buffer);

    if (bytes_read > 0) {
        update_io_metrics_read(bytes_read);
    }

    return (uint32_t)bytes_read;
}

// FILE_WRITE(handle, buffer, size): Writes size bytes from a buffer.
// Returns: number of bytes actually written
uint32_t FILE_WRITE(uintptr_t handle, uint32_t* buffer, uint32_t size) {
    if (!buffer || size == 0 || handle == 0) return 0;

    FILE* file = (FILE*)(uintptr_t)handle;

    // Allocate temporary byte buffer
    unsigned char* temp_buffer = (unsigned char*)malloc(size);
    if (!temp_buffer) return 0;

    // Copy from BCPL buffer to bytes
    for (uint32_t i = 0; i < size; i++) {
        temp_buffer[i] = (unsigned char)(buffer[i] & 0xFF);
    }

    size_t bytes_written = fwrite(temp_buffer, 1, size, file);
    free(temp_buffer);

    if (bytes_written > 0) {
        update_io_metrics_write(bytes_written);
    }

    return (uint32_t)bytes_written;
}

//==============================================================================
// File Navigation & Status
//==============================================================================

// FILE_SEEK(handle, offset, origin): Moves the file pointer.
// origin: 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END
// Returns: 0 on success, non-zero on failure
uint32_t FILE_SEEK(uintptr_t handle, int32_t offset, uint32_t origin) {
    if (handle == 0) return 1;

    FILE* file = (FILE*)(uintptr_t)handle;

    int whence;
    switch (origin) {
        case 0: whence = SEEK_SET; break;
        case 1: whence = SEEK_CUR; break;
        case 2: whence = SEEK_END; break;
        default: return 1; // Invalid origin
    }

    int result = fseek(file, (long)offset, whence);
    return (uint32_t)result;
}

// FILE_TELL(handle): Gets the current file pointer position.
// Returns: current position, or -1 on error
int32_t FILE_TELL(uintptr_t handle) {
    if (handle == 0) return -1;

    FILE* file = (FILE*)(uintptr_t)handle;
    long pos = ftell(file);
    return (int32_t)pos;
}

// FILE_EOF(handle): Checks if the end-of-file is reached.
// Returns: non-zero if EOF, 0 otherwise
uint32_t FILE_EOF(uintptr_t handle) {
    if (handle == 0) return 1;

    FILE* file = (FILE*)(uintptr_t)handle;
    return (uint32_t)feof(file);
}

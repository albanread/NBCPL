# NewBCPL File Runtime Module

This document provides a comprehensive overview and reference for the file I/O runtime module in NewBCPL. It covers the purpose, available commands, usage patterns, integration details, and best practices for using file operations from BCPL code.

---

## Overview

The NewBCPL file runtime module enables BCPL programs to perform file input and output using a modern, robust API. All file operations are accessed through the runtime, ensuring portability, safety, and consistent API design.

- **Access**: All file features are available via routines prefixed with `FILE_`.
- **Integration**: The module is always included in the runtime and available in JIT, static, and standalone builds.
- **Design**: The interface is designed for clarity, with explicit error handling, UTF-8 support, and integration with runtime metrics.

---

## Core Design Principles

1. **Consistent Parameter Ordering**: File handle is always the first parameter.
2. **Explicit Error Handling**: Functions return 0 or NULL on failure.
3. **UTF-8 Support**: All string operations handle Unicode properly.
4. **Metrics Integration**: All operations update runtime I/O metrics.
5. **Memory Safety**: Proper bounds checking and resource management.

---

## File API Reference

### File Opening and Closing

#### `FILE_OPEN_READ(filename)`
Opens a file for reading.
- **Parameters**: `filename` (BCPL string)
- **Returns**: File handle (non-zero) on success, 0 on failure

#### `FILE_OPEN_WRITE(filename)`
Creates or truncates a file for writing.
- **Parameters**: `filename` (BCPL string)
- **Returns**: File handle (non-zero) on success, 0 on failure

#### `FILE_OPEN_APPEND(filename)`
Opens or creates a file for appending to the end.
- **Parameters**: `filename` (BCPL string)
- **Returns**: File handle (non-zero) on success, 0 on failure

#### `FILE_CLOSE(handle)`
Closes an open file.
- **Parameters**: `handle` (file handle)
- **Returns**: 0 on success, non-zero on failure

---

### High-Level String I/O

#### `FILE_WRITES(handle, string_buffer)`
Writes a null-terminated BCPL string to the file. Handles UTF-8 encoding.
- **Parameters**: 
  - `handle` (file handle)
  - `string_buffer` (BCPL string)
- **Returns**: Number of bytes written, or 0 on failure

#### `FILE_READS(handle)`
Reads all content from the file's current position to the end and returns it as a new BCPL string.
- **Parameters**: `handle` (file handle)
- **Returns**: New BCPL string containing file content, or NULL on failure

---

### Low-Level Byte I/O

#### `FILE_READ(handle, buffer, size)`
Reads a specified number of bytes into a buffer.
- **Parameters**:
  - `handle` (file handle)
  - `buffer` (buffer to read into; each uint32_t holds one byte)
  - `size` (number of bytes to read)
- **Returns**: Number of bytes actually read

#### `FILE_WRITE(handle, buffer, size)`
Writes a specified number of bytes from a buffer.
- **Parameters**:
  - `handle` (file handle)
  - `buffer` (buffer to write from; each uint32_t contains one byte)
  - `size` (number of bytes to write)
- **Returns**: Number of bytes actually written

---

### File Navigation & Status

#### `FILE_SEEK(handle, offset, origin)`
Moves the file pointer to a new position.
- **Parameters**:
  - `handle` (file handle)
  - `offset` (offset in bytes, can be negative)
  - `origin` (reference point: 0=SEEK_SET, 1=SEEK_CUR, 2=SEEK_END)
- **Returns**: 0 on success, non-zero on failure

#### `FILE_TELL(handle)`
Gets the current file pointer position.
- **Parameters**: `handle` (file handle)
- **Returns**: Current position in bytes, or -1 on error

#### `FILE_EOF(handle)`
Checks if the end-of-file has been reached.
- **Parameters**: `handle` (file handle)
- **Returns**: Non-zero if EOF, 0 otherwise

---

## Runtime Metrics Integration

The file API automatically updates the following runtime metrics:

- **Files Opened**: Incremented by FILE_OPEN_READ, FILE_OPEN_WRITE, FILE_OPEN_APPEND
- **Files Closed**: Incremented by FILE_CLOSE
- **Bytes Written**: Incremented by FILE_WRITES and FILE_WRITE
- **Bytes Read**: Incremented by FILE_READS and FILE_READ

You can view metrics with the runtime's built-in metrics reporting functions.

---

## Complete Usage Example

```bcpl
LET START() BE {
    // Write to a file
    LET write_handle = FILE_OPEN_WRITE("example.txt")
    TEST write_handle = 0 THEN {
        WRITES("ERROR: Cannot create file*N")
        RETURN
    } ELSE {
        FILE_WRITES(write_handle, "Hello, File API!*N")
        FILE_WRITES(write_handle, "This is line 2*N")
        FILE_CLOSE(write_handle)
        WRITES("File written successfully*N")
    }

    // Read the file back
    LET read_handle = FILE_OPEN_READ("example.txt")
    TEST read_handle = 0 THEN {
        WRITES("ERROR: Cannot open file*N")
    } ELSE {
        LET content = FILE_READS(read_handle)
        TEST content = 0 THEN {
            WRITES("ERROR: Cannot read file*N")
        } ELSE {
            WRITES("File content:*N")
            WRITES(content)
        }
        FILE_CLOSE(read_handle)
    }

    // Append to the file
    LET append_handle = FILE_OPEN_APPEND("example.txt")
    TEST append_handle = 0 THEN {
        WRITES("ERROR: Cannot open file for append*N")
    } ELSE {
        FILE_WRITES(append_handle, "Appended line*N")
        FILE_CLOSE(append_handle)
        WRITES("Text appended successfully*N")
    }
}
```

---

## Error Handling Best Practices

1. **Always check return values**: File handles of 0 indicate failure.
2. **Use TEST statements**: Proper BCPL conditional syntax for error handling.
3. **Close files**: Always call FILE_CLOSE when done with a file.
4. **Check before operations**: Verify handles are valid before FILE_WRITES, FILE_READS, etc.

---

## Performance Notes

- **FILE_READS** allocates memory for the entire file content.
- **FILE_WRITES** automatically handles UTF-8 encoding conversion.
- **Low-level functions** (`FILE_READ`/`FILE_WRITE`) provide maximum performance for binary data.
- **String functions** are optimized for text processing with Unicode support.

---

## Thread Safety

The file API functions are not inherently thread-safe. If using multiple threads, coordinate file access through proper synchronization mechanisms.

---

## Compatibility

This API is available in:
- ✅ JIT mode
- ✅ Static compilation mode
- ✅ Standalone runtime builds
- ✅ SDL2-enabled builds

All functions are registered at runtime initialization and available immediately.

---

## Implementation and Integration Notes

- **Location**: Implementation is in `runtime/runtime_file_api.inc`.
- **Registration**: Functions are registered in the runtime bridge and available to BCPL code.
- **Heap Management**: Uses the runtime's heap manager for BCPL string allocations.
- **UTF-8 Support**: All string I/O is UTF-8 safe and supports Unicode.
- **Error Codes**: All functions return 0 or NULL on failure, and non-zero or valid handles on success.

---

## Known Issues

- All known HeapManager-related crashes affecting the FILE API have been fixed. The file runtime module is now stable in all supported runtime builds.

---

## Further Reading

- See `FILE_API_REFERENCE.md` for a detailed function-by-function reference and more examples.
- For implementation details, see `runtime/runtime_file_api.inc`.
- For test results and troubleshooting, see `FILE_RUNTIME_TEST_SUMMARY.md`.

---
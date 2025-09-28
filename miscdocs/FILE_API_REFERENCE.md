# NewBCPL File API Reference

## Overview

The NewBCPL File API provides comprehensive file I/O operations with explicit file open modes, byte-level read/write capabilities, and convenient string-level read/write operations. All functions use consistent parameter ordering with the file handle as the first argument when applicable.

## Core Design Principles

1. **Consistent Parameter Ordering**: File handle is always the first parameter
2. **Explicit Error Handling**: Functions return 0 or NULL on failure
3. **UTF-8 Support**: All string operations handle Unicode properly
4. **Metrics Integration**: All operations update runtime I/O metrics
5. **Memory Safety**: Proper bounds checking and resource management

## Function Reference

### File Opening and Closing

#### FILE_OPEN_READ(filename)
Opens a file for reading.
- **Parameters**: `filename` - BCPL string containing the file path
- **Returns**: File handle (non-zero) on success, 0 on failure
- **Example**: `LET handle = FILE_OPEN_READ("data.txt")`

#### FILE_OPEN_WRITE(filename)
Creates or truncates a file for writing.
- **Parameters**: `filename` - BCPL string containing the file path
- **Returns**: File handle (non-zero) on success, 0 on failure
- **Example**: `LET handle = FILE_OPEN_WRITE("output.txt")`

#### FILE_OPEN_APPEND(filename)
Opens or creates a file for appending to the end.
- **Parameters**: `filename` - BCPL string containing the file path
- **Returns**: File handle (non-zero) on success, 0 on failure
- **Example**: `LET handle = FILE_OPEN_APPEND("log.txt")`

#### FILE_CLOSE(handle)
Closes an open file.
- **Parameters**: `handle` - File handle returned from FILE_OPEN_*
- **Returns**: 0 on success, non-zero on failure
- **Example**: `LET result = FILE_CLOSE(handle)`

### High-Level String I/O

#### FILE_WRITES(handle, string_buffer)
Writes a null-terminated string to the file. The runtime automatically calculates the string's length and handles UTF-8 encoding.
- **Parameters**: 
  - `handle` - File handle
  - `string_buffer` - BCPL string to write
- **Returns**: Number of bytes written, or 0 on failure
- **Example**: `LET bytes = FILE_WRITES(handle, "Hello World!*N")`

#### FILE_READS(handle)
Reads all content from the file's current position to the end and returns it as a new BCPL string.
- **Parameters**: `handle` - File handle
- **Returns**: New BCPL string containing file content, or NULL on failure
- **Example**: `LET content = FILE_READS(handle)`

### Low-Level Byte I/O

#### FILE_READ(handle, buffer, size)
Reads a specified number of bytes into a buffer.
- **Parameters**:
  - `handle` - File handle
  - `buffer` - Buffer to read into (each uint32_t holds one byte)
  - `size` - Number of bytes to read
- **Returns**: Number of bytes actually read
- **Example**: `LET bytes_read = FILE_READ(handle, buffer, 1024)`

#### FILE_WRITE(handle, buffer, size)
Writes a specified number of bytes from a buffer.
- **Parameters**:
  - `handle` - File handle
  - `buffer` - Buffer to write from (each uint32_t contains one byte)
  - `size` - Number of bytes to write
- **Returns**: Number of bytes actually written
- **Example**: `LET bytes_written = FILE_WRITE(handle, buffer, 1024)`

### File Navigation & Status

#### FILE_SEEK(handle, offset, origin)
Moves the file pointer to a new position.
- **Parameters**:
  - `handle` - File handle
  - `offset` - Offset in bytes (can be negative)
  - `origin` - Reference point: 0=SEEK_SET (beginning), 1=SEEK_CUR (current), 2=SEEK_END (end)
- **Returns**: 0 on success, non-zero on failure
- **Example**: `LET result = FILE_SEEK(handle, 0, 0)` // Seek to beginning

#### FILE_TELL(handle)
Gets the current file pointer position.
- **Parameters**: `handle` - File handle
- **Returns**: Current position in bytes, or -1 on error
- **Example**: `LET position = FILE_TELL(handle)`

#### FILE_EOF(handle)
Checks if the end-of-file has been reached.
- **Parameters**: `handle` - File handle
- **Returns**: Non-zero if EOF, 0 otherwise
- **Example**: `LET is_eof = FILE_EOF(handle)`

## Runtime Metrics Integration

The file API automatically updates the following runtime metrics:

- **Files Opened**: Incremented by FILE_OPEN_READ, FILE_OPEN_WRITE, FILE_OPEN_APPEND
- **Files Closed**: Incremented by FILE_CLOSE
- **Bytes Written**: Incremented by FILE_WRITES and FILE_WRITE
- **Bytes Read**: Incremented by FILE_READS and FILE_READ

View metrics with the runtime's built-in metrics reporting functions.

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

## Error Handling Best Practices

1. **Always check return values**: File handles of 0 indicate failure
2. **Use TEST statements**: Proper BCPL conditional syntax for error handling
3. **Close files**: Always call FILE_CLOSE when done with a file
4. **Check before operations**: Verify handles are valid before FILE_WRITES, FILE_READS, etc.

## Performance Notes

- **FILE_READS** allocates memory for the entire file content
- **FILE_WRITES** automatically handles UTF-8 encoding conversion
- **Low-level functions** (FILE_READ/FILE_WRITE) provide maximum performance for binary data
- **String functions** are optimized for text processing with Unicode support

## Thread Safety

The file API functions are not inherently thread-safe. If using multiple threads, coordinate file access through proper synchronization mechanisms.

## Compatibility

This API is available in:
- ✅ JIT mode
- ✅ Static compilation mode
- ✅ Standalone runtime builds
- ✅ SDL2-enabled builds

All functions are registered at runtime initialization and available immediately.
# SPLIT/JOIN Function Testing Process

This document describes how to test the BCPL SPLIT and JOIN functions using the comprehensive C++ test suite.

## Overview

The SPLIT and JOIN functions are critical string/list operations in NewBCPL:
- **SPLIT**: Takes a string and delimiter, returns a list of strings
- **JOIN**: Takes a list of strings and delimiter, returns a single string

This test suite verifies their correctness, performance, and memory management behavior.

## Prerequisites

Before running the tests, you need to build the runtime and compiler:

### 1. Build the Runtime Libraries
```bash
cd NewBCPL
./buildruntime
```

This creates the static runtime library (`libbcpl_runtime_sdl2_static.a`) that contains the SPLIT and JOIN implementations.

### 2. Build the Main Compiler
```bash
./build.sh
```

This builds the NewBCPL compiler and associated tools.

### 3. Verify Prerequisites
Check that these files exist:
```bash
ls -la libbcpl_runtime_sdl2_static.a  # Runtime library
ls -la NewBCPL                        # Main compiler executable
```

## Running the Tests

### Basic Test Execution
```bash
# Build and run all tests
make -f Makefile.split_join_test test

# Build and run with verbose output
make -f Makefile.split_join_test test-verbose

# Build and run quietly (minimal output)
make -f Makefile.split_join_test test-quiet
```

### Specific Test Categories
```bash
# Test only SPLIT functionality
make -f Makefile.split_join_test test-split

# Test only JOIN functionality
make -f Makefile.split_join_test test-join

# Test round-trip operations (SPLIT then JOIN)
make -f Makefile.split_join_test test-roundtrip

# Run performance benchmarks
make -f Makefile.split_join_test test-performance

# Test memory management (SAMM compatibility)
make -f Makefile.split_join_test test-memory
```

### Production Readiness Check
```bash
# Quick check if functions are ready for production use
make -f Makefile.split_join_test check-production
```

## Expected Test Results

### Successful Run
```
SPLIT/JOIN Function Test Suite
==============================

[PASS] SPLIT Basic
[PASS] SPLIT Single Char Delimiter
[PASS] SPLIT Multi Char Delimiter
[PASS] SPLIT No Delimiter
[PASS] SPLIT Empty String
[PASS] SPLIT Consecutive Delimiters
[PASS] JOIN Basic
[PASS] JOIN Single Element
[PASS] JOIN Empty List
[PASS] JOIN Empty Delimiter
[PASS] SPLIT-JOIN Round-trip
[PASS] SPLIT-JOIN Complex Round-trip
[PASS] SPLIT Performance
[PASS] JOIN Performance
[PASS] Edge Cases
[PASS] SAMM Memory Management

==============================
Test Results: 16/16 passed
✅ ALL TESTS PASSED - SPLIT/JOIN functions are working correctly!
```

### Performance Expectations
- SPLIT: < 1 μs per token for typical strings
- JOIN: < 1 μs per element for typical lists
- Memory usage should be stable (no major leaks)

## Understanding the Test Coverage

### SPLIT Tests
1. **Basic functionality**: Simple space-delimited strings
2. **Single character delimiters**: Comma, semicolon, etc.
3. **Multi-character delimiters**: "::", " -> ", etc.
4. **Edge cases**: Empty strings, no delimiters found
5. **Consecutive delimiters**: Handling of empty tokens
6. **Performance**: Large strings with many tokens

### JOIN Tests
1. **Basic functionality**: Simple list joining
2. **Single element lists**: No delimiter should appear
3. **Empty lists**: Should return empty string
4. **Empty delimiters**: Concatenation without separator
5. **Performance**: Large lists with many elements

### Round-trip Tests
1. **Identity preservation**: SPLIT(JOIN(list)) == list
2. **Complex delimiters**: Multi-character separators
3. **Edge case preservation**: Empty tokens, boundaries

### Memory Management Tests
1. **SAMM compatibility**: Proper scope-aware cleanup
2. **No memory leaks**: Stable allocation patterns
3. **Large operation stability**: Bulk processing

## Locating and Modifying the Source Code

### SPLIT and JOIN Implementations

The functions are implemented in two versions:

#### 1. C++ Version (for JIT/dynamic builds)
**File**: `runtime/runtime_string_ops.cpp`
```cpp
extern "C" ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload)
extern "C" uint32_t* BCPL_JOIN_LIST(ListHeader* list_header, uint32_t* delimiter_payload)
```

#### 2. Pure C Version (for static builds)
**File**: `runtime/runtime_string_ops.c`
```c
struct ListHeader* BCPL_SPLIT_STRING(uint32_t* source_payload, uint32_t* delimiter_payload)
uint32_t* BCPL_JOIN_LIST(struct ListHeader* list_header, uint32_t* delimiter_payload)
```

### Key Implementation Details

1. **String Format**: BCPL strings use UTF-32 encoding (uint32_t arrays)
2. **Memory Layout**: Strings have a length prefix followed by payload
3. **List Structure**: Defined in `runtime/ListDataTypes.h`
4. **Memory Allocation**: Uses `bcpl_alloc_chars()` from `runtime/heap_interface.h`

### Making Changes

#### Modifying the Implementation
1. Edit the appropriate file (`runtime_string_ops.cpp` or `runtime_string_ops.c`)
2. Rebuild the runtime: `./buildruntime`
3. Rebuild the test: `make -f Makefile.split_join_test rebuild`
4. Run tests: `make -f Makefile.split_join_test test`

#### Adding New Test Cases
1. Edit `split_join_test.cpp`
2. Add new test functions following the pattern:
   ```cpp
   bool test_my_new_case() {
       print_test_header("My New Test");
       // ... test implementation
       return success;
   }
   ```
3. Add to the test array in `main()`
4. Rebuild and test

### Common Issues and Debugging

#### Build Failures
- **Missing runtime library**: Run `./buildruntime`
- **Missing compiler**: Run `./build.sh`
- **Include path issues**: Check that headers exist in `runtime/` and `HeapManager/`

#### Test Failures
- **SPLIT returning wrong count**: Check delimiter matching logic
- **JOIN producing wrong result**: Check list traversal and memory layout
- **Memory issues**: Check allocation/deallocation in heap interface
- **Performance problems**: Profile with `make -f Makefile.split_join_test profile`

#### Debugging Steps
1. Run with verbose output: `make -f Makefile.split_join_test test-verbose`
2. Run specific test category to isolate issues
3. Add debug prints to the implementation
4. Use debugger: `gdb ./split_join_test`

## Integration with BCPL Code

### Using SPLIT in BCPL
```bcpl
LET START() BE $(
   LET S = "hello,world,test"
   LET L = SPLIT(S, ",")
   FOREACH ITEM IN L DO
   $(
       WRITES(ITEM)
       WRITES("*N")
   $)
$)
```

### Using JOIN in BCPL
```bcpl
LET START() BE $(
   LET L = LIST("apple", "banana", "cherry")
   LET RESULT = JOIN(L, " | ")
   WRITES(RESULT)  // Output: "apple | banana | cherry"
$)
```

### Testing BCPL Integration
After verifying the C++ tests pass, test with actual BCPL code:
```bash
# Create a test BCPL file
echo 'LET START() BE $( LET S = "a,b,c"; LET L = SPLIT(S, ","); WRITES(JOIN(L, "-")) $)' > test_split_join.bcl

# Compile and run
./NewBCPL test_split_join.bcl
```

## Performance Optimization

### Monitoring Performance
- Use `make test-performance` for benchmarks
- Profile builds: `make profile`
- Monitor memory usage with SAMM metrics

### Optimization Areas
1. **String copying**: Minimize unnecessary allocations
2. **List traversal**: Optimize for large lists
3. **Memory alignment**: Ensure proper 16-byte alignment
4. **Cache efficiency**: Consider memory access patterns

## File Locations Quick Reference

### Implementation Files
- `runtime/runtime_string_ops.cpp` - C++ implementation (JIT builds)
- `runtime/runtime_string_ops.c` - Pure C implementation (static builds)
- `runtime/ListDataTypes.h` - List structure definitions
- `runtime/heap_interface.h` - Memory allocation functions

### Test Files
- `split_join_test.cpp` - Main test suite
- `Makefile.split_join_test` - Build configuration
- `my_split_test.md` - This documentation

### Supporting Files
- `runtime/runtime.h` - Function declarations
- `HeapManager/HeapManager.h` - Memory management
- `libbcpl_runtime_sdl2_static.a` - Compiled runtime library

## Step-by-Step Testing Workflow

### Initial Setup (once)
```bash
cd NewBCPL
./buildruntime          # Build runtime library
./build.sh              # Build compiler
```

### Regular Testing (each change)
```bash
# After modifying SPLIT/JOIN implementation
./buildruntime          # Rebuild runtime
make -f Makefile.split_join_test rebuild  # Rebuild test
make -f Makefile.split_join_test test      # Run tests
```

### Verification Workflow
```bash
# 1. Quick verification
make -f Makefile.split_join_test check-production

# 2. Detailed analysis
make -f Makefile.split_join_test test-verbose

# 3. Performance check
make -f Makefile.split_join_test test-performance

# 4. Memory stability
make -f Makefile.split_join_test test-memory
```

## Troubleshooting Common Issues

### Compilation Errors
```bash
# Check runtime library exists
ls -la libbcpl_runtime_sdl2_static.a

# Check include paths
ls -la runtime/runtime.h
ls -la HeapManager/HeapManager.h
```

### Runtime Errors
```bash
# Run with debugging symbols
make -f Makefile.split_join_test debug
gdb ./split_join_test
```

### Performance Issues
```bash
# Profile the code
make -f Makefile.split_join_test profile
./split_join_test
gprof split_join_test gmon.out > profile.txt
```

## Conclusion

This test suite provides comprehensive coverage of SPLIT and JOIN functionality, ensuring:
- ✅ Correctness across all use cases
- ✅ Performance within acceptable bounds
- ✅ Memory management stability
- ✅ SAMM compatibility
- ✅ Production readiness verification

Regular testing helps maintain code quality and catch regressions early in the development process. The combination of C++ unit tests and BCPL integration tests provides confidence that these critical string processing functions work correctly in all scenarios.
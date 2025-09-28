# BCPL Runtime System

This directory contains the BCPL runtime system, which provides core functionality for both JIT-compiled and standalone-compiled BCPL programs.

## Overview

The BCPL runtime is designed with a unique architecture that allows the same code to be used in two different compilation modes:

1. **JIT Mode**: When the BCPL compiler is used with the `-jit` flag, the runtime is linked directly into the compiler and used to execute code immediately.

2. **Standalone Mode**: When compiling BCPL to standalone executables, the runtime is compiled as a separate C library that is linked with the generated assembly.

## File Structure

- **runtime.h**: C-compatible header file defining the public API
- **runtime_core.inc**: Core runtime functions (WRITES, WRITEN, etc.)
- **runtime_string_utils.inc**: String handling with UTF-8 support
- **runtime_io.inc**: File I/O operations
- **runtime.c**: C implementation for standalone builds (includes main())
- **heap_c_bridge.cpp**: C wrappers for the C++ HeapManager
- **runtime_bridge.cpp**: C++ implementation for JIT mode
- **runtime_test.c**: Test program for the standalone runtime

## Design Approach

The key design principle is the use of `.inc` files as shared implementation that can be included by both C and C++ compilation units. This provides a single source of truth for runtime functions while allowing them to be compiled in different contexts.

### Memory Management

- In JIT mode: Uses the C++ `HeapManager` class
- In standalone mode: Uses a simple malloc-based allocator

### String Handling

The runtime provides full UTF-8 support for BCPL strings, enabling:

- Unicode character handling
- Safe string conversions
- File I/O with proper encoding

## How to Build

### For JIT Mode

The runtime is automatically built as part of the main compiler build:

```sh
./build.sh
```

### For Standalone Mode

The standalone C runtime is built when you compile a BCPL program to a standalone executable:

```sh
./compile_standalone.sh your_program.b
```

This creates an executable that includes the C runtime linked with the compiled BCPL code.

## Extending the Runtime

When adding new runtime functions, **follow this checklist to ensure your function is available to both JIT and standalone BCPL programs, and to avoid linker errors**:

### Runtime Function Checklist

1. **Implement the Function**
   - Write your function in C or C++.
   - Use C-compatible types (`int64_t`, `double`, `void*`, etc).
   - Use `extern "C"` for C++ code to ensure C linkage.
   - Place the implementation in a relevant file (e.g., `runtime/runtime_core.inc`, `runtime/runtime_string_utils.inc`, or a new `.inc`/`.cpp` file).

2. **Declare the Function in a Header**
   - Add a forward declaration in a C-compatible header (e.g., `runtime.h`, `heap_interface.h`, or a new header).
   - Example:
     ```c
     #ifdef __cplusplus
     extern "C" {
     #endif

     int64_t ADD_NUMBERS(int64_t a, int64_t b);

     #ifdef __cplusplus
     }
     #endif
     ```

3. **(For C++/JIT Heap Functions) Add a C ABI Wrapper**
   - If your function is a C++ method (e.g., a `HeapManager` method), you **must** add an `extern "C"` wrapper in `HeapManager/heap_c_wrappers.cpp` that forwards to the singleton.
   - Example:
     ```cpp
     extern "C" void* Heap_allocVec(size_t numElements) {
         return HeapManager::getInstance().allocVec(numElements);
     }
     ```
   - **MAINTAINER NOTE:** If you add new HeapManager methods that need to be called from C or assembly (the runtime), you MUST add a corresponding wrapper in `heap_c_wrappers.cpp`. Otherwise, you will get linker errors!
   - See also: `runtime/README.md` (this file) for more information on runtime ABI requirements.

4. **Register the Function with the Runtime Manager**
   - Edit the `register_runtime_functions` function in `runtime/RuntimeBridge.cpp` to register your function.
   - Example:
     ```cpp
     register_runtime_function("ADD_NUMBERS", 2, reinterpret_cast<void*>(ADD_NUMBERS));
     ```
   - The first argument is the uppercase name for BCPL code, the second is the number of parameters, and the third is the function pointer.
   - For functions returning floating-point, add `FunctionType::FLOAT` as a fourth argument.

5. **(Optional but Recommended) Add to the Symbol Table**
   - For better type checking, add your function to `RuntimeSymbols` in `RuntimeSymbols.cpp`.
   - Example:
     ```cpp
     registerRuntimeFunction(symbol_table, "ADD_NUMBERS", {
         {VarType::INTEGER, false}, // Parameter 'a'
         {VarType::INTEGER, false}  // Parameter 'b'
     });
     ```

6. **Rebuild the Runtime**
   - After making changes, rebuild the runtime libraries:
     ```sh
     ./buildruntime --jit
     ./buildruntime --standalone
     ```
   - This ensures your function is available in both JIT and standalone builds.

---

### Example: Adding a New Runtime Function

Suppose you want to add a function `ADD_NUMBERS`:

1. **Implement in a `.inc` or `.cpp` file:**
   ```c
   #ifdef __cplusplus
   extern "C" {
   #endif

   int64_t ADD_NUMBERS(int64_t a, int64_t b) {
       return a + b;
   }

   #ifdef __cplusplus
   }
   #endif
   ```

2. **Declare in a header:**
   ```c
   #ifdef __cplusplus
   extern "C" {
   #endif

   int64_t ADD_NUMBERS(int64_t a, int64_t b);

   #ifdef __cplusplus
   }
   #endif
   ```

3. **Register in `RuntimeBridge.cpp`:**
   ```cpp
   register_runtime_function("ADD_NUMBERS", 2, reinterpret_cast<void*>(ADD_NUMBERS));
   ```

4. **(Optional) Add to `RuntimeSymbols.cpp`:**
   ```cpp
   registerRuntimeFunction(symbol_table, "ADD_NUMBERS", {
       {VarType::INTEGER, false}, // Parameter 'a'
       {VarType::INTEGER, false}  // Parameter 'b'
   });
   ```

5. **Rebuild the runtime.**

---

### Additional Notes

- For heap manager or object/class support, always ensure C ABI wrappers exist for any C++ methods that must be called from C or assembly. See `HeapManager/heap_c_wrappers.cpp` for examples and add new wrappers there as needed.
- If you encounter linker errors about missing symbols (e.g., `Heap_allocVec`), check that you have provided a C ABI wrapper and that the file is included in your build.


## Testing

You can test the standalone runtime independently using:

```sh
clang -o runtime_test runtime_test.c runtime.c
./runtime_test
```

This will verify that basic runtime functions work as expected.
# BCPL Runtime System Documentation

This document describes the architecture and operation of the BCPL runtime system, including how the compiler connects to the runtime.

## Overview

The BCPL runtime system is designed to support two compilation modes:
1. **JIT Mode** - For immediate execution with the compiler
2. **Standalone Mode** - For generating standalone executables

This dual approach is achieved through a carefully designed bridge system that allows the same code to be compiled and used in different contexts.

## Architecture

### Core Components

1. **RuntimeManager** - Singleton that manages all registered runtime functions
2. **HeapManager** - C++ heap management system for memory allocation
3. **Runtime Bridge** - C/C++ interfaces between the compiler and runtime components
4. **List Management System** - Efficient handling of BCPL lists with freelists

## Memory Management

### HeapManager (C++ Component)
Located in `/HeapManager/` directory, provides core heap management functionality through a singleton pattern. It handles allocation and deallocation of:
- Vectors (64-bit word arrays)
- Objects (for object-oriented features)
- Strings
- Includes metrics tracking and debugging capabilities

### Runtime Bridge (C/C++ Interface)
Located in `/runtime/` directory, provides C-compatible interfaces to the HeapManager functionality:
- `heap_interface.cpp` - Bridges C++ HeapManager to C runtime functions
- `jit_heap_bridge.cpp` - Implements freelist management for JIT mode
- `runtime.c` - C implementation for standalone builds

## How Memory Allocation Works

### JIT Mode
1. The runtime uses the C++ HeapManager directly
2. Memory allocations go through `bcpl_alloc_words()` and `bcpl_alloc_chars()`
3. These functions delegate to HeapManager's `allocVec()` and `allocString()`
4. Freelist management is handled in `jit_heap_bridge.cpp` for efficiency

### Standalone Mode
1. Uses simple malloc/free-based allocator
2. `bcpl_alloc_words()` and `bcpl_alloc_chars()` use standard C malloc
3. Memory is tracked with a header containing the size

## List Handling System

The runtime includes a sophisticated list management system:
- **ListHeader** and **ListAtom** structures for list representation
- **Freelists** for efficient node allocation/deallocation (both in JIT and standalone modes)
- Functions for:
  - Creating empty lists
  - Appending elements (int, float, string, list)
  - Copying lists (shallow and deep)
  - Concatenating lists
  - Searching within lists
  - Reversing lists

## Compiler Integration

### RuntimeManager Connection

The compiler connects to the runtime system through the **RuntimeManager** singleton:

1. **Function Registration**: During compiler startup, `register_runtime_functions()` in `/runtime/RuntimeBridge.cpp` registers all standard runtime functions with the RuntimeManager.

2. **Function Lookup**: Throughout compilation, the compiler checks `RuntimeManager::is_function_registered()` to identify runtime function calls.

3. **Metadata Access**: The compiler accesses function metadata (address, argument count, function type) through RuntimeManager during code generation.

### Compilation Integration Points

#### During AST Analysis:
- Checks if function calls refer to registered runtime functions
- Determines function types (integer vs floating-point)

#### During Code Generation:
- Uses RuntimeManager to get function metadata
- Generates appropriate calls to runtime functions

#### During Assembly Generation:
- The `AssemblyWriter` consults RuntimeManager to identify external symbols
- Ensures runtime functions are properly declared in assembly output

#### During Linking:
- The `Linker` uses RuntimeManager for relocations
- Ensures all registered runtime functions are properly resolved

## Runtime Function Registration Process

When adding a new runtime function, follow these steps:

1. **Implement the Function** - Write in C or C++ with C-compatible types
2. **Add C ABI Wrapper** (for C++/JIT Heap Functions) - Add wrapper in `HeapManager/heap_c_wrappers.cpp`
3. **Declare in Header** - Add forward declaration in a C-compatible header
4. **Register Function** - Add to `register_runtime_functions()` in `runtime/RuntimeBridge.cpp`
5. **Optional: Add to Symbol Table** - Register function type information in `RuntimeSymbols.cpp`
6. **Rebuild** - Compile with updated runtime libraries

## Key Files and Their Roles

### Runtime Directory (`/runtime/`)
- `runtime.h` - Public API header
- `runtime.c` - Standalone C implementation  
- `jit_runtime.cpp` - JIT compilation entry point
- `heap_interface.cpp` - C++ implementation bridging to HeapManager
- `jit_heap_bridge.cpp` - JIT freelist management
- `RuntimeBridge.cpp` - Function registration and runtime initialization

### HeapManager Directory (`/HeapManager/`)
- `HeapManager.h/cpp` - Core heap management class
- `heap_c_wrappers.cpp` - C ABI wrappers for HeapManager methods

## Design Principles

1. **Single Source of Truth**: `.inc` files are included by both C and C++ compilation units
2. **ABI Compatibility**: All runtime functions use C linkage for compatibility
3. **Extensibility**: Clear process for adding new functions (as documented in README.md)
4. **Debugging Support**: Trace logging and signal-safe debugging utilities
5. **Performance**: Freelist management to avoid frequent malloc/free calls

## Runtime Function Metadata

Each registered runtime function has metadata stored in the RuntimeManager:
- **Name**: Function name for lookup (case-insensitive)
- **Argument Count**: Number of parameters expected
- **Address**: Actual memory address of the implementation
- **Function Type**: Whether it's standard or floating-point
- **Table Offset**: Position in the global function pointer table for JIT execution

## Usage Modes

### JIT Mode
- Runtime functions are linked directly to the HeapManager
- Immediate execution capability during compilation

### Standalone Mode
- Runtime functions are linked to C implementations
- Generates standalone executables that include the runtime

This architecture allows the same BCPL code to be compiled and executed in either JIT mode (for development/testing) or as standalone executables (for deployment), with consistent memory management and runtime behavior.
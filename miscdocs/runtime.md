BCPLRuntime/runtime.md
```
# BCPLRuntime – Runtime Documentation

## Overview

**BCPLRuntime** is a C++ runtime library designed to support execution of BCPL programs. It provides robust memory management, runtime function registration, symbol table integration, signal-safe utilities, and diagnostics. The runtime is modular, signal-safe, and suitable for integration with interpreters or JIT compilers.

---

## Features

- **Heap Management**: Allocation, deallocation, resizing, and tracking of BCPL objects, vectors, strings, and lists.
- **Runtime Function Table**: Registration and lookup of runtime functions, with support for JIT and dynamic linking.
- **Symbol Table Integration**: Centralized registration of runtime functions/routines for BCPL code.
- **Signal Handling**: Robust fatal signal handler for debugging and crash diagnostics.
- **Signal-Safe Utilities**: Printing and formatting functions that are safe to use in signal contexts.

---

## Directory Structure

```
BCPLRuntime/
├── DataTypes.h                # Core BCPL type definitions and utilities
├── HeapManager/               # Heap management implementation
│   ├── HeapManager.h/cpp      # HeapManager class and singleton
│   ├── Heap_alloc*.cpp        # Allocation helpers (object, vector, string, list)
│   ├── Heap_free.cpp          # Deallocation logic
│   ├── Heap_resize*.cpp       # Resizing logic for vectors/strings
│   ├── Heap_dumpHeap*.cpp     # Heap dump and signal-safe dump
│   ├── Heap_printMetrics.cpp  # Heap metrics reporting
│   ├── heap_c_wrappers.*      # C-callable wrappers for heap functions
│   ├── heap_manager_defs.h    # Allocation types and heap block tracking
│   ├── heap_manager_globals.cpp # Global heap tracking array and metrics
├── RuntimeManager.h/cpp       # Runtime function registration and pointer table
├── RuntimeSymbols.h/cpp       # Symbol table population for runtime functions
├── SignalHandler.cpp          # Fatal signal handler and diagnostics
├── SignalSafeUtils.h/cpp      # Signal-safe printing and formatting utilities
```

---

## Core Components

### 1. HeapManager

- **Singleton class** for all heap operations.
- Tracks allocations in a circular buffer (`g_heap_blocks_array`).
- Supports allocation of objects, vectors, strings, and lists.
- Provides deallocation and resizing.
- Metrics and heap dump functions for debugging.
- C-callable wrappers for integration with C/assembly.

**Example Usage:**
```cpp
#include "HeapManager.h"
void* obj = HeapManager::getInstance().allocObject(64);
HeapManager::getInstance().free(obj);
```

### 2. RuntimeManager

- Registers runtime functions (name, argument count, address, type).
- Maintains a global pointer table for JIT and dynamic linking.
- Provides lookup, offset calculation, and address setting.
- Singleton pattern for global access.

**Example Usage:**
```cpp
RuntimeManager::instance().register_function("READN", 0, (void*)readn_impl);
auto& func = RuntimeManager::instance().get_function("READN");
```

### 3. RuntimeSymbols

- Static class for registering all runtime functions/routines in a symbol table.
- Supports integer-returning, float-returning, and void routines.
- Used to populate the BCPL symbol table for runtime linkage.

**Example Usage:**
```cpp
SymbolTable table;
RuntimeSymbols::registerAll(table);
```

### 4. Signal Handling & Utilities

- **SignalHandler**: Installs a fatal signal handler (SIGSEGV, SIGBUS, etc.) for crash diagnostics.
- **SignalSafeUtils**: Provides `safe_print`, `u64_to_hex`, `int_to_dec`, and UTF-8 encoding, all safe for use in signal contexts.

---

## Building

The project is designed for CMake. Example build steps:

```bash
mkdir build && cd build
cmake ..
make
```

---

## API Highlights

### HeapManager API

- `void* allocObject(size_t size)`
- `void* allocVec(size_t numElements)`
- `void* allocString(size_t numChars)`
- `void* allocList()`
- `void free(void* payload)`
- `void printMetrics()`
- `void dumpHeap()`
- `void dumpHeapSignalSafe()`

### RuntimeManager API

- `void register_function(const std::string& name, int num_args, void* address, FunctionType type)`
- `const RuntimeFunction& get_function(const std::string& name) const`
- `size_t get_function_offset(const std::string& name) const`
- `void set_function_address(const std::string& name, void* address)`
- `void print_registered_functions() const`
- `void populate_function_pointer_table(void* data_segment_base) const`

### SignalSafeUtils API

- `void safe_print(const char* str)`
- `void u64_to_hex(uint64_t val, char* buf)`
- `void int_to_dec(int64_t val, char* buf)`
- `size_t safeEncode_utf8_char(uint32_t char_code, char* buffer)`

---

## Example: Allocating and Freeing a Vector

```cpp
#include "HeapManager.h"
void* vec = HeapManager::getInstance().allocVec(10);
// ... use the vector ...
HeapManager::getInstance().free(vec);
```

---

## Contributing

- Fork and clone the repository.
- Create a feature branch.
- Add tests for new features.
- Submit a pull request.

---

## License

MIT License. See `LICENSE` for details.

---

**For further details on any subsystem, see the corresponding header and implementation files.**

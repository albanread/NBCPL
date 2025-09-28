NewBCPL/book/runtime_modules.md
# Runtime Modules: Public and Private Interfaces

This document describes how the NewBCPL runtime exposes its features to BCPL programs, the distinction between public and private runtime APIs, and the design philosophy that integrates the compiler and runtime within a UNIX environment.

---

## 1. Public vs. Private Runtime Features

The NewBCPL runtime is architected to provide a clear separation between **public** features—those directly accessible from BCPL code—and **private** features, which are used internally by the compiler and runtime for efficient execution and resource management.

### Public Runtime Features

Public features are exposed as routines and functions that BCPL programs can invoke directly. These include:

- **Standard Library Routines**: Functions for string manipulation, arithmetic, list processing, etc.
- **File and I/O Operations**: Routines for opening, reading, writing, and closing files.
- **Platform Abstractions**: Interfaces for interacting with the underlying operating system in a portable way.
- **Specialized Modules**: Optional modules such as SDL2 for graphics and audio, which are exposed as callable routines when enabled.

These routines are documented and form the stable API surface for BCPL developers.

### Private Runtime Features

Private features are not directly accessible from BCPL code. Instead, they are used by the compiler-generated code or by the runtime itself to implement language semantics and manage resources. Examples include:

- **Memory Management**: Allocation, deallocation, and garbage collection routines invoked by the compiler when new objects, lists, or strings are created.
- **Error Handling**: Internal mechanisms for reporting and propagating runtime errors.
- **Low-Level Data Structures**: Functions for managing internal representations of lists, vectors, and objects.
- **Optimization Hooks**: Specialized routines used by the compiler to implement optimizations (e.g., fast string copying, heap management).

These private APIs are subject to change and are not intended for direct use by BCPL programmers.

---

## 2. How BCPL Accesses Runtime Features

### Calling Public Routines

BCPL programs access runtime features by calling routines and functions defined in the runtime. For example:

```bcpl
LET f = fopen("myfile.txt", "r")
LET line = freadline(f)
fclose(f)
```

Here, `fopen`, `freadline`, and `fclose` are public routines provided by the runtime's file module.

### Compiler-Generated Calls to Private Routines

When the BCPL compiler translates source code, it may emit calls to private runtime functions that are not visible at the language level. For example:

- **Memory Allocation**: When a new list or object is created, the compiler emits a call to a private allocation routine.
- **Bounds Checking**: The compiler may insert calls to internal routines that check array or vector bounds at runtime.
- **Internal Bookkeeping**: For features like garbage collection or reference counting, the compiler-generated code interacts with private runtime APIs.

This dual-layer approach allows the runtime to provide a clean, stable interface to users while retaining the flexibility to evolve and optimize its internal implementation.

---

## 3. Example: Public and Private Runtime Usage

### Public (User-Visible) Example

```bcpl
LET img = sdl_load_image("logo.png")
sdl_draw_image(img, 100, 100)
```

Here, `sdl_load_image` and `sdl_draw_image` are public routines exposed by the SDL2 module.

### Private (Compiler-Only) Example

When compiling a list operation, the compiler might emit:

```c
void* new_list = BCPL_LIST_ALLOC(num_elements);
```

This call is not visible in BCPL source code but is essential for correct execution.

---

## 4. Modular Runtime Design

The runtime is organized into modules, each responsible for a specific domain:

- **Core**: Memory management, error handling, and basic data types.
- **Files**: File I/O and stream operations.
- **Strings**: String manipulation and encoding.
- **SDL2**: Graphics, audio, and input (optional).
- **Platform**: Abstractions for OS-specific features.

Each module exposes a set of public routines and may define private functions for internal use.

---

## 5. UNIX Environment Integration

The NewBCPL compiler and runtime are designed to operate seamlessly within the UNIX environment:

- **All I/O is Mediated by the Runtime**: BCPL programs do not perform direct system calls. Instead, all file and device I/O is routed through the runtime, which provides a consistent API and handles platform differences.
- **Standard Streams**: Input and output routines are mapped to UNIX standard streams (stdin, stdout, stderr) by default.
- **Process Integration**: The runtime can interact with UNIX processes, environment variables, and signals, abstracting these details for BCPL code.
- **Portability**: By isolating all system interactions within the runtime, BCPL programs remain portable across UNIX-like systems.

---

## 6. Extending the Runtime

New modules can be added to the runtime to support additional features (e.g., networking, advanced graphics, or platform-specific APIs). The modular design ensures that:

- Public APIs remain stable for BCPL programs.
- Private APIs can evolve to support new optimizations or platform requirements.
- The compiler can be updated to take advantage of new runtime capabilities without breaking existing code.

---

## Summary

- The NewBCPL runtime exposes both public (user-facing) and private (compiler/internal) APIs.
- BCPL programs access runtime features via routines and functions; the compiler also emits calls to private runtime functions for memory management and other low-level tasks.
- All I/O and system interaction is handled by the runtime, ensuring portability and consistency within UNIX environments.
- The modular runtime design supports extensibility and robust separation of concerns.

For more details on specific modules, see the corresponding documentation in the `runtime/` directory and related design notes.
NewBCPL/book/runtime_linking.md
# Runtime Linking in NewBCPL

This document describes how the NewBCPL system links generated code to the runtime, supporting both JIT (Just-In-Time) execution and standalone compilation. It references the mechanisms in `main.cpp` and the runtime bridge (`RuntimeBridge.h`/`.cpp`), and explains the flow for both modes.

---

## Overview

The NewBCPL runtime is designed to support two primary modes of execution:

- **JIT Mode**: Source code is compiled in-memory and executed immediately (`--run`).
- **Standalone Mode**: Source code is compiled to assembly, assembled with Clang, and linked with the runtime to produce a native executable (`--exec`).

Both modes rely on a well-defined interface between the generated code and the runtime, but the linking and invocation mechanisms differ.

---

## JIT Mode: In-Memory Compilation and Execution

In JIT mode, the workflow is:

1. **Source Parsing and Analysis**: The compiler parses the BCPL source, builds the AST, and performs semantic analysis and optimizations.
2. **Code Generation**: The compiler generates machine code directly into an in-memory buffer.
3. **Runtime Initialization**: The runtime is initialized in-process using the runtime bridge (`runtime::initialize_runtime()` and `runtime::register_runtime_functions()`).
4. **Function Table Setup**: The runtime bridge registers all runtime functions with the `RuntimeManager`, making them available for JIT-compiled code.
5. **Execution**: The JIT-compiled code is executed by jumping directly to the entry point in the code buffer.

### Diagram: JIT Mode Linking

```mermaid
flowchart TD
    Source[BCPL Source]
    Compiler[NewBCPL Compiler (JIT)]
    CodeBuffer[In-Memory Code Buffer]
    RuntimeBridge[RuntimeBridge]
    Runtime[Runtime Libraries]
    Execution[Immediate Execution]

    Source --> Compiler
    Compiler --> CodeBuffer
    Compiler -->|Registers Functions| RuntimeBridge
    RuntimeBridge --> Runtime
    CodeBuffer -->|Calls| Runtime
    CodeBuffer --> Execution
```

#### Key Points

- **No external linking step**: All code and runtime are in the same process.
- **Function registration**: The runtime bridge exposes C/C++ runtime functions to the JIT code via a function pointer table.
- **Dynamic**: Useful for rapid development, debugging, and interactive environments.

---

## Standalone Mode: Static Compilation and Linking

In standalone mode, the workflow is:

1. **Source Parsing and Analysis**: As in JIT mode.
2. **Assembly Generation**: The compiler emits an assembly file (`.s`).
3. **External Compilation**: The assembly is compiled and linked with the runtime using Clang.
4. **Runtime Library**: The runtime is provided as a static or dynamic library (`libbcpl_runtime.a`, `libbcpl_runtime_sdl2.a`, etc.).
5. **Execution**: The resulting executable is run as a native process, with the runtime linked in.

### Diagram: Standalone Mode Linking

```mermaid
flowchart TD
    Source[BCPL Source]
    Compiler[NewBCPL Compiler]
    Assembly[Assembly (.s)]
    Clang[Clang/Linker]
    RuntimeLib[Runtime Library]
    Executable[Native Executable]
    OS[Operating System]

    Source --> Compiler
    Compiler --> Assembly
    Assembly --> Clang
    RuntimeLib --> Clang
    Clang --> Executable
    Executable --> OS
```

#### Key Points

- **External linking**: The runtime is linked as a library at build time.
- **Multiple runtime variants**: The user can select different runtime libraries (e.g., with SDL2 support).
- **Native execution**: The resulting binary is standalone and can be distributed.

---

## The Role of RuntimeBridge

The `RuntimeBridge` module is central to both modes:

- **In JIT mode**: It registers runtime functions with the `RuntimeManager`, making them available to JIT-compiled code via a function pointer table.
- **In standalone mode**: It provides the C/C++ API that the generated code expects, ensuring that calls from the compiled assembly resolve to the correct runtime implementations.

### Example: Registering Runtime Functions

In `main.cpp`:

```cpp
runtime::initialize_runtime();
runtime::register_runtime_functions();
```

In `RuntimeBridge.h`:

```cpp
namespace runtime {
    void register_runtime_functions();
    void initialize_runtime();
    // ...
}
```

These calls ensure that all runtime services (memory management, I/O, error handling, etc.) are available to the generated code, regardless of execution mode.

---

## Mode Selection and Build Flow

The mode is selected via command-line flags:

- `--run` or `-r`: JIT mode (in-memory execution)
- `--exec` or `-e`: Standalone mode (assembly + external linking)
- `--runtime=MODE`: Explicitly select runtime variant (`jit`, `standalone`, `unified`)

In standalone mode, the build flow in `main.cpp` (see `handle_static_compilation`) selects the appropriate runtime library and invokes Clang to produce the final executable.

---

## Summary Table

| Mode         | Compilation Target | Linking           | Runtime Integration      | Use Case                |
|--------------|-------------------|-------------------|-------------------------|-------------------------|
| JIT          | In-memory code    | In-process        | Function pointer table  | Development, debugging  |
| Standalone   | Assembly (.s)     | External (Clang)  | Static/Dynamic library  | Distribution, shipping  |

---

## Conclusion

The NewBCPL runtime is engineered for flexibility, supporting both rapid JIT execution and robust standalone binaries. The runtime bridge ensures a consistent interface, allowing the same generated code to run in either mode with minimal changes. This architecture enables both fast iteration and reliable deployment.

For more technical details, see:
- `main.cpp` (mode selection, build flow)
- `runtime/RuntimeBridge.h` and `.cpp` (function registration and API)
- `runtime_overview.md` (general runtime architecture)
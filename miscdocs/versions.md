# BCPLRuntime Versions

This document details which source files and components are included in the **standalone runtime** versus the **JIT runtime** builds of BCPLRuntime.

---

## Overview

- **Standalone Runtime**: Provides a minimal, portable runtime for BCPL programs. Intended for static linking or embedding in interpreters.
- **JIT Runtime**: Extends the standalone runtime with additional support for Just-In-Time compilation, dynamic symbol resolution, and runtime bridging for JIT execution environments.

---

## Standalone Runtime

### Core Files

These files are included in the standalone runtime build (`libbcpl_runtime_c.a`):

- `DataTypes.h`
- `HeapManager.h`
- `HeapManager/HeapManager.h`
- `HeapManager/HeapManager.cpp`
- `HeapManager/Heap_allocList.cpp`
- `HeapManager/Heap_allocObject.cpp`
- `HeapManager/Heap_allocString.cpp`
- `HeapManager/Heap_allocVec.cpp`
- `HeapManager/Heap_free.cpp`
- `HeapManager/Heap_resizeString.cpp`
- `HeapManager/Heap_resizeVec.cpp`
- `HeapManager/Heap_dumpHeap.cpp`
- `HeapManager/Heap_dumpHeapSignalSafe.cpp`
- `HeapManager/Heap_printMetrics.cpp`
- `HeapManager/heap_c_wrappers.cpp`
- `HeapManager/heap_c_wrappers.h`
- `HeapManager/heap_manager_defs.h`
- `HeapManager/heap_manager_globals.cpp`
- `RuntimeManager.h`
- `RuntimeManager.cpp`
- `RuntimeSymbols.h`
- `RuntimeSymbols.cpp`
- `SignalHandler.cpp`
- `SignalSafeUtils.h`
- `SignalSafeUtils.cpp`
- `runtime/standalone_heap.c`
- `runtime/runtime.c`
- `runtime/runtime.h`
- `runtime/ListDataTypes.h`
- `runtime/main.c`
- `runtime/starter.c`
- `runtime/heap_interface.c`
- `runtime/heap_interface.h`
- `runtime/runtime_core.inc`
- `runtime/runtime_io.inc`
- `runtime/runtime_string_utils.inc`
- `runtime/runtime_string_ops.cpp`
- `runtime/runtime_test.c`
- `runtime/runtimetester.cpp`

### Build Artifacts

- `build/static/libbcpl_runtime_c.a`
- `libbcpl_runtime_c.a`

---

## JIT Runtime

### Core Files

The JIT runtime build (`libbcpl_runtime_jit.a`) includes **all standalone runtime files**, plus the following JIT-specific files:

- `runtime/jit_heap_bridge.cpp`
- `runtime/jit_runtime.cpp`
- `runtime/RuntimeBridge.cpp`
- `runtime/RuntimeBridge.h`
- `runtime/runtime_bridge.cpp`
- Any additional JIT-specific headers or source files required for bridging and dynamic code generation.

### Build Artifacts

- `build/jit/libbcpl_runtime_jit.a`
- `libbcpl_runtime_jit.a`
- `runtime/build/libbcpl_runtime_jit.a`

---

## Build Scripts

- `buildjit.sh` — Builds the JIT runtime.
- `buildstatic.sh` — Builds the standalone runtime.
- `buildruntime` — General build script (may invoke either or both builds).

---

## Notes

- The **standalone runtime** is suitable for static analysis, interpreters, and environments where JIT is not required.
- The **JIT runtime** is required for environments that perform dynamic code generation or require runtime bridging with external compiled code.
- Both versions share the majority of the codebase, with the JIT runtime adding only the files necessary for dynamic execution and bridging.

---

## Quick Reference Table

| File/Component                | Standalone | JIT |
|-------------------------------|:----------:|:---:|
| Core heap & runtime files      |     ✔️     | ✔️  |
| `jit_heap_bridge.cpp`         |            | ✔️  |
| `jit_runtime.cpp`             |            | ✔️  |
| `RuntimeBridge.cpp`/`.h`      |            | ✔️  |
| `runtime_bridge.cpp`          |            | ✔️  |
| JIT build artifacts           |            | ✔️  |
| Standalone build artifacts    |     ✔️     |     |

---

For further details, see the respective `CMakeLists.txt` and build scripts.
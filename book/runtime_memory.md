# Runtime Memory Management: SAMM Overview

## What is SAMM?

**SAMM** stands for **Scope Aware Memory Management**. It is the memory management subsystem used in the NewBCPL runtime to provide safe, efficient, and automatic cleanup of dynamically allocated resources.

## Key Features

- **Scope Awareness**: SAMM tracks allocations in relation to program scopes (such as function calls or blocks). When a scope exits, all memory associated with that scope can be automatically reclaimed.
- **Automatic Cleanup**: Reduces the risk of memory leaks by ensuring that temporary allocations are freed as soon as they are no longer needed.
- **Integration with Runtime**: SAMM is tightly integrated with the BCPL runtime, supporting both JIT and standalone execution modes.
- **Performance**: Designed to minimize overhead and fragmentation, making it suitable for both small scripts and large, long-running programs.

## How It Works

- When a BCPL program enters a new scope (e.g., a function call), SAMM creates a new allocation context.
- All dynamic allocations (lists, strings, objects, etc.) within that scope are tracked.
- Upon exiting the scope, SAMM automatically frees all tracked allocations, unless they have been explicitly promoted to a higher scope or returned.
- This model is similar to stack allocation, but works for heap-allocated objects and supports complex control flow.

## Benefits

- **Safety**: Eliminates many classes of memory leaks and dangling pointer bugs.
- **Simplicity**: Programmers do not need to manually free most temporary objects.
- **Portability**: Works consistently across all supported runtime modes and platforms.

## Usage Notes

- SAMM is enabled by default in the NewBCPL runtime.
- Advanced users can control or disable SAMM for specific allocations if manual memory management is required.
- SAMM works transparently with the rest of the runtime, including file I/O, SDL2, and other modules.

## Further Reading

For more technical details, see the runtime source code and design notes related to memory management and the HeapManager subsystem.

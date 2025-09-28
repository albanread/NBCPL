# SAMM Overview: Simple, Predictable Scope-Based Memory Management

## What is SAMM?

**SAMM** stands for **Scope Aware Memory Management**. It is the memory management system used in the NewBCPL runtime, designed to provide safe, efficient, and highly predictable handling of heap-allocated objects. SAMM is fundamentally different from traditional garbage collectors or reference counting systems: it is simpler, more deterministic, and tailored for the most common programming patterns in BCPL. SAMM is mainly designed for managing lists of objects, which are a central data structure in BCPL programs.

---

## Key Principles

### 1. Scope-Based Recycling

- **Scope Mirrors Locals:** The lifetime of heap objects (lists, strings, objects) is tied directly to the scope in which they are created—just like local variables.
- **Automatic Cleanup:** When a scope (such as a function or block) exits, all heap objects allocated within it are automatically recycled, unless they have been explicitly promoted or retained.
- **No Surprises:** Memory is reclaimed immediately and deterministically at scope exit, making program behavior easy to reason about.

### 2. Not a Garbage Collector or Reference Counter

- **No GC Pauses:** SAMM does not scan the heap or pause execution for collection. However, memory is reclaimed by a separate background reclamation thread, which operates outside the main program flow to ensure responsiveness.
- **No Ref Counting:** SAMM does not increment or decrement reference counts on every assignment or pointer copy. Instead, it uses explicit hints (`RETAIN`, `REMANAGE`) for advanced cases where objects need to outlive their original scope.
- **Simple and Fast:** The system is lightweight and incurs minimal runtime overhead.

### 3. Special Support for Lists, Strings, and Objects

- **Optimized Allocation and Reuse:** The heap manager provides fast, specialized allocation routines for lists, strings, and objects. SAMM aggressively reuses memory from previous allocations and avoids unnecessary system allocations whenever possible. For example, lists use freelists for nodes and headers, reducing the number of slow system allocations.
- **String and Vector Pools:** Strings and vectors are allocated with length prefixes and null terminators, allowing for efficient copying and resizing.
- **Object Support:** Class instances are heap-allocated and managed with the same scope-based rules, but can also be retained or remanaged for advanced patterns.

### 4. Predictable Behavior and Limitations

- **Predictability:** Because memory is always reclaimed at scope exit, programs using SAMM are highly predictable—there are no mysterious leaks or pauses.
- **Limitations:** SAMM is not designed for complex, cyclic, or highly-shared data structures. If you need to share objects across many scopes or build graphs with cycles, you must use `RETAIN` and `REMANAGE` explicitly, or manage memory manually.
- **Design Philosophy:** SAMM is designed for the most common use cases in BCPL: local data, temporary objects, and especially lists of objects. It is optimized for modern systems with ample memory, where fast allocation and reclamation are more important than minimizing memory footprint. The "S" in SAMM could stand for **Simple** or **Scope-based**.

### 5. Safety Features

- **Automatic Cleanup:** Heap objects are never leaked if used according to scope rules.
- **No Dangling Pointers:** Objects are not freed while still in use within their scope.
- **Blocks Double Frees:** SAMM tracks the state of each heap object and prevents double frees, ensuring that attempts to free the same object twice are safely blocked and reported as errors.
- **Explicit Ownership Transfer:** `SEND` and `ACCEPT` make ownership transfer explicit and safe.
- **Safer by Design:** The "S" in SAMM could also stand for **Safer**—the system is engineered to prevent common memory bugs by default.

---

## Summary

SAMM is a modern, scope-based memory manager that brings the predictability and safety of stack allocation to heap objects. It is not a garbage collector or reference counter, but a simpler, more deterministic system with special support for lists, strings, and objects. SAMM is designed to be both **simple** and **safer**, making it ideal for most BCPL programs while providing advanced features for expert users.

For more details, see the documentation on SAMM scopes, string management, and advanced memory management features.
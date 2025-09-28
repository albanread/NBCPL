# Runtime String Management and SAMM

## Overview

String management in the NewBCPL runtime is designed for safety, efficiency, and seamless integration with the language's memory model. The runtime provides robust support for dynamic strings, Unicode (UTF-8) handling, and automatic memory management through SAMM (Scope Aware Memory Management).

---

## Key Features

### 1. Dynamic String Allocation

- All BCPL strings are dynamically allocated in the runtime heap.
- Strings can be created, copied, concatenated, and manipulated without manual memory management.
- The runtime provides optimized routines for common string operations (copy, compare, length, substring, etc.).

### 2. UTF-8 and Unicode Support

- All string operations are UTF-8 aware, allowing full Unicode text processing.
- Conversion routines ensure that BCPL strings can safely represent and manipulate international text.
- File I/O and external interfaces automatically handle encoding and decoding.

### 3. SAMM Integration

- Every string allocation is tracked by SAMM, associating it with the current scope (function, block, etc.).
- When a scope exits, all temporary strings allocated within that scope are automatically freed.
- This eliminates most memory leaks and dangling pointer issues related to string usage.

### 4. Efficient String Pooling and Reuse

- The runtime may pool or reuse string buffers for performance, reducing allocation overhead.
- Short-lived strings (such as those created during expression evaluation) are efficiently recycled.

### 5. Safe Interoperability

- Strings passed between BCPL and runtime modules (e.g., file API, SDL2, etc.) are always managed safely.
- The runtime ensures that string memory is valid for the duration of its use, even across module boundaries.

---

## How SAMM Handles Strings

- **Automatic Cleanup**: When a string is created in a function or block, it is registered with SAMM. If the string is not returned or promoted to a higher scope, it is automatically freed when the scope exits.
- **Promotion**: If a string needs to outlive its scope (e.g., returned from a function), SAMM can promote its allocation context, ensuring it remains valid.
- **No Manual Free Required**: In most cases, BCPL programmers do not need to explicitly free strings. SAMM handles cleanup transparently.

---

## Example: String Lifetime

```bcpl
LET make_greeting(name) = VALOF
$(
  LET greeting = CONCAT("Hello, ", name)
  RESULTIS greeting  // SAMM promotes 'greeting' so it survives after function returns
$)

// Temporary strings created inside expressions are cleaned up automatically
LET temp = CONCAT("foo", "bar")  // 'temp' is managed by SAMM
```

---

## Performance and Safety

- **No Buffer Overflows**: All string operations are bounds-checked.
- **No Dangling Pointers**: Strings are never freed while still in use.
- **Low Fragmentation**: SAMM and the string allocator work together to minimize heap fragmentation.

---

## Advanced Notes

- The runtime provides specialized routines for fast string copying and allocation, used internally for performance-critical paths.
- String memory is compatible with other runtime modules (file I/O, SDL2, etc.), ensuring consistent behavior across the system.
- SAMM's string management is designed to be extensible for future optimizations (e.g., string interning, deduplication).

---

## Summary

- String management in NewBCPL is automatic, safe, and efficient thanks to SAMM.
- Programmers can focus on logic and algorithms, not manual memory management.
- The runtime's string features are robust enough for modern Unicode applications and high-performance systems programming.

For more details, see the runtime source code and design notes on string operations and memory management.
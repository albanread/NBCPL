NewBCPL/book/runtime_samm_lists.md
# SAMM and List Management in NewBCPL

## Overview

Lists are a fundamental data structure in BCPL and are heavily used for everything from collections of objects to representing strings and vectors. SAMM (Scope Aware Memory Management) is specifically optimized to manage lists efficiently, safely, and predictably. This document describes how SAMM handles list allocation, recycling, and safety, and how this design benefits typical BCPL programs.

---

## List Allocation

- **Freelist-Based Allocation:**
  SAMM uses freelists for both list headers and list nodes. When a new list or list element is needed, the runtime first checks the freelist for available memory blocks before requesting new memory from the system. This minimizes the number of slow system allocations and improves performance, especially for programs that create and destroy many lists.

- **Scope Association:**
  Every list allocated by the runtime is associated with the current scope (such as a function or block). This means that the lifetime of the list is automatically tied to the scope in which it was created.

- **Efficient Construction:**
  List construction routines are optimized for both small and large lists. Bulk allocation and append operations are supported, and the runtime can efficiently handle both shallow and deep copies of lists.

---

## Recycling and Reuse

- **Automatic Recycling at Scope Exit:**
  When a scope exits, all lists (and their nodes) allocated within that scope are automatically reclaimed and returned to the freelist. This ensures that memory is recycled quickly and predictably, without the need for explicit deallocation by the programmer.

- **Aggressive Reuse:**
  The freelist mechanism allows the runtime to aggressively reuse memory for new lists, reducing heap fragmentation and allocation overhead. This is especially beneficial for programs that frequently create and discard temporary lists.

- **No Garbage Collection Pauses:**
  Because memory is reclaimed (eventually) after scope exit and not by a complex garbage collector, there are no unpredictable pauses or delays in program execution.

---

## Safety Features

- **No Dangling Pointers:**
  Lists are never freed while still in use within their scope. SAMM ensures that any list returned from a function (using `SEND` and `ACCEPT`) is promoted to the caller's scope and remains valid.

- **Blocks Double Frees:**
  SAMM tracks the state of each list and prevents double frees. Attempts to free the same list twice are safely blocked and reported as errors.

- **Automatic Cleanup:**
  Programmers do not need to manually free lists in most cases. SAMM handles cleanup transparently, reducing the risk of memory leaks.

- **Explicit Ownership Transfer:**
  When lists need to be shared or returned from a function, ownership transfer is explicit via `SEND` and `ACCEPT`, making memory management predictable and safe.

---

## Limitations and Advanced Use

- **Not for Cyclic or Highly-Shared Structures:**
  SAMM's scope-based model is not designed for complex cyclic graphs or highly-shared lists. For these cases, advanced features like `RETAIN` and `REMANAGE` must be used, or manual memory management may be required.

- **Designed for Common Patterns:**
  The list management strategy in SAMM is optimized for the most common BCPL patterns: local lists, temporary collections, and simple sharing between functions.


---

## Example: List Lifetime

```bcpl
LET make_numbers(n) = VALOF
$(
  LET lst = NEW_LIST(n)
  FOR i = 1 TO n DO
    APPEND(lst, i)
  SEND lst  // Ownership transferred to caller
$)

LET START() BE $(
  LET numbers = make_numbers(10)
  ACCEPT numbers
  // Use 'numbers' safely here
  // No need to manually free; SAMM will clean up at scope exit
$)
```

---

## Summary

SAMM provides a fast, safe, and predictable memory management model for lists in NewBCPL. By tying list lifetimes to scope, using freelists for recycling, and providing explicit ownership transfer, SAMM eliminates many common memory management bugs and overheads. While not suitable for every advanced use case, it is ideal for the majority of NewBCPL programs that rely on lists as their primary data structure.

For more details on advanced memory management, see the documentation on `RETAIN`, `REMANAGE`, and SAMM's handling of complex structures.

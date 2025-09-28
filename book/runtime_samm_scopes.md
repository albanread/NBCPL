NewBCPL/book/runtime_samm_scopes.md
# SAMM and Scope-Based Memory Management

## What is a Scope?

In the context of the NewBCPL runtime and SAMM (Scope Aware Memory Management), a **scope** is a region of program execution with a well-defined entry and exit point. Scopes are typically:

- Function calls (the most common scope boundary)
- Blocks within functions (such as loops or conditional branches)
- Temporary evaluation contexts (such as expression evaluation or temporary variable lifetimes)

Each scope has its own allocation context. When a scope is entered, SAMM begins tracking all heap allocations (lists, strings, objects, etc.) made within that scope. When the scope exits, SAMM automatically reclaims all memory associated with it, unless the memory has been explicitly promoted to a higher scope.

**In this compiler, the scope of heap objects mirrors the scope of local variables:** both are tied to the lifetime of the scope in which they are created. When a function or block ends, all local variables and all heap objects allocated within that scope are recycled together. This makes heap object management predictable and safe, just like stack-based local variables.

---

## Automatic Cleanup of Heap Objects

SAMM's primary job is to ensure that heap-allocated objects do not leak memory and are cleaned up as soon as they are no longer needed. This is achieved by:

- **Tracking allocations**: Every heap allocation is tagged with the current scope context.
- **Scope exit cleanup**: When a scope exits (e.g., a function returns), all allocations made within that scope are automatically freed, unless they have been promoted.
- **Promotion**: If an object needs to survive beyond the current scope (for example, if it is returned from a function), it is promoted to the parent scope.

This model is similar to stack allocation for local variables, but applies to heap-allocated objects, making it both efficient and safe.

---

## Returning Values and Ownership: `RESULTIS`, `SEND`, and `ACCEPT`

### `RESULTIS`

The standard way to return a value from a BCPL function is with `RESULTIS`. This is ideal for returning primitive values such as integers and floats, which are copied by value and do not require any special memory management. When you use `RESULTIS` with a primitive, SAMM does not need to track or promote anything.

**Important:** `RESULTIS` should NOT be used to return heap-allocated objects (such as lists, strings, or class instances). If you use `RESULTIS` with a heap object, the object may be cleaned up when the callee's scope exits, leading to dangling pointers or use-after-free errors. For heap objects, use `SEND` instead.

```bcpl
LET get_sum(a, b) = VALOF
$(
  RESULTIS a + b  // Safe for ints/floats
$)
```

### `SEND` and Ownership Transfer

`SEND` is required for returning heap-allocated objects (such as lists, strings, or class instances). It explicitly transfers both the value and ownership of the heap object from the callee to the caller. `SEND` signals to the runtime and compiler that the callee is relinquishing responsibility for managing the memory of that object. In effect, `SEND x` is equivalent to `RETAIN x; RESULTIS x`—the object is retained and promoted to the caller's scope.

**If you use `RESULTIS` with a heap object, you risk a memory management bug:** the object may be cleaned up when the callee's scope exits, leading to dangling pointers or use-after-free errors. `SEND` makes the transfer of both value and ownership explicit and safe.

```bcpl
LET makePoint() = VALOF $(
  LET p = NEW Point
  p.CREATE(50, 75)
  SEND p    // Required for heap objects: transfers ownership to caller
$)
```

### `ACCEPT` and Caller Management

The caller must use `ACCEPT` to explicitly bring a value received via `SEND` under the caller's scope management. `ACCEPT x` is similar to `REMANAGE x`, telling SAMM that the caller now manages the lifetime of the object. If `ACCEPT` is called multiple times on the same variable, a warning may be issued, but it is allowed.

```bcpl
LET p1 = makePoint()
ACCEPT p1  // Now p1 is managed by the caller's scope
```

#### Example: SEND/ACCEPT in Practice

```bcpl
LET makeColorPoint() = VALOF $(
  LET cp = NEW ColorPoint
  cp.CREATE(100, 150, 255)
  SEND cp   // Transfer ownership to caller
$)

LET START() BE $(
  LET cp1 = makeColorPoint()
  ACCEPT cp1
  WRITES("ColorPoint accepted: x=")
  WRITEN(cp1.getX())
  WRITES(", y=")
  WRITEN(cp1.getY())
  WRITES(", color=")
  WRITEN(cp1.getColor())
  WRITES("*N")
$)
```

- **SEND**: Transfers both the value and ownership of the heap object to the caller.
- **ACCEPT**: Brings the value into assisted management by the caller's scope. If omitted, the object may not be properly managed and could leak or be prematurely freed.


---

## Advanced Use Cases: Globals, Shared Pointers, and Manual Management

### Globals

Global variables exist outside of any function scope and are managed separately. SAMM does not automatically clean up global allocations; they persist for the lifetime of the program unless explicitly freed.

### Shared Pointers and RETAIN/REMANAGE

For advanced scenarios where objects need to be shared across multiple scopes or have lifetimes that do not fit the standard scope model, SAMM provides **RETAIN** and **REMANAGE** features. These are hints to the compiler and runtime to adjust the management context or reference count for heap-allocated objects.

- **RETAIN x, y, z**: Increases the reference count of one or more objects, preventing them from being freed when their original scope exits. This is useful for shared data structures or objects referenced from multiple places.
- **REMANAGE x**: Changes the management context of an object, allowing it to be adopted by a different scope or to be managed manually.

#### Example: RETAIN

```bcpl
LET shared = NEW_LIST(10)
RETAIN shared  // Prevents automatic cleanup; shared can now be passed to other functions or stored globally
```

#### Example: REMANAGE

```bcpl
LET temp = NEW_STRING("temporary")
REMANAGE temp  // Moves temp's management to the current scope or a specified new scope
```

These features give expert programmers fine-grained control over memory management, enabling advanced patterns such as object pools, caches, or cross-scope data sharing.

- A variable for a heap-allocated item is a pointer to a heap item; SAMM manages the actual heap items, not the pointers themselves.

---

## Summary

- **Scope** in SAMM refers to any region of code with a clear entry and exit, most commonly function calls.
- SAMM automatically cleans up all heap-allocated objects as their scope exits, unless they are promoted or retained.
- Use `RESULTIS` for standard returns (automatic promotion), and `SEND` for yielding or returning multiple values (caller must accept and manage).
- For globals and shared objects, use `retain` and `remanage` to control lifetimes beyond standard scope rules.
- This model provides both safety and flexibility, allowing most code to be memory-leak free by default, while supporting advanced use cases for expert users.

For more technical details, see the runtime source code and design notes on SAMM and heap management.

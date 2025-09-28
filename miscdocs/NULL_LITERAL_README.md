# The Null Literal (`?`) in NewBCPL

## Overview

NewBCPL introduces the `?` null literal - a type-safe, minimalist way to represent null pointer values.
This feature adds some null support while maintaining BCPL's elegant simplicity.

## Syntax

The null literal is represented by a single question mark:

```bcpl
LET ptr = ?
```

## What is the Null Literal?

The `?` symbol represents a null pointer value. It compiles to zero (0) at the machine level but provides semantic clarity and type safety at the language level. Unlike using raw zero values, `?` explicitly communicates the intent to work with null pointers.

## Basic Usage

### Assignment
```bcpl
LET START() BE $(
    LET pointer = ?           // Assign null to pointer
    LET another_ptr = ?       // Multiple null assignments

    pointer := ?              // Reassign to null
$)
```

### Comparison
```bcpl
LET START() BE $(
    LET ptr = ?

    // Check if pointer is null
    IF ptr = ? THEN
        WRITES("Pointer is null*N")

    // Check if pointer is not null
    IF ptr ~= ? THEN
        WRITES("Pointer is not null*N")
$)
```

### Multiple Null Variables
```bcpl
LET START() BE $(
    LET ptr1 = ?
    LET ptr2 = ?
    LET ptr3 = ?

    // All null pointers are equal
    IF ptr1 = ptr2 AND ptr2 = ptr3 THEN
        WRITES("All pointers are null*N")
$)
```

## Advanced Usage

### Arithmetic Context
When used in arithmetic expressions, `?` behaves as zero:

```bcpl
LET START() BE $(
    LET result = ? + 42       // result = 42
    LET sum = ? * 100         // sum = 0
    LET value = 50 - ?        // value = 50

    WRITES("? + 42 = ")
    WRITEN(result)            // Prints: 42
    WRITES("*N")
$)
```

### Conditional Expressions
```bcpl
LET START() BE $(
    LET ptr = ?
    LET result = ptr = ? -> 100, 200    // result = 100

    WRITES("Result: ")
    WRITEN(result)
    WRITES("*N")
$)
```

### Function Parameters and Returns
```bcpl
FUNCTION get_optional_value(has_value) = VALOF $(
    TEST has_value THEN
        RESULTIS 42
    ELSE
        RESULTIS ?            // Return null
$)

LET START() BE $(
    LET value = get_optional_value(FALSE)

    IF value = ? THEN
        WRITES("No value returned*N")
    ELSE $(
        WRITES("Value: ")
        WRITEN(value)
        WRITES("*N")
    $)
$)
```

## Type Safety and Semantics

### Pointer Semantics
The null literal is designed for pointer types and null-checking:

```bcpl
CLASS MyClass $(
    DECL value

    ROUTINE CREATE(initial_val) BE $(
        value := initial_val
    $)
$)

LET START() BE $(
    LET obj = ?               // Start with null object

    IF obj = ? THEN
        obj := NEW MyClass(42) // Create object if null

    IF obj ~= ? THEN
        WRITES("Object created successfully*N")
$)
```

### Memory Management Integration
The null literal works seamlessly with NewBCPL's automatic memory management:

```bcpl
LET START() BE $(
    LET obj = NEW MyClass(100)  // Create object

    // Use object...

    obj := ?                    // Reset to null

    // Automatic cleanup will handle the original object
    // when it goes out of scope
$)
```

## Design Philosophy

### Why `?` Instead of `NULL` or `nil`?

1. **Minimalism**: BCPL values conciseness - `?` is shorter than `NULL`
2. **Visual Clarity**: The question mark suggests "unknown" or "empty"
3. **Semantic Intent**: Clearly indicates optional/nullable values
4. **Type Safety**: Distinct from numeric zero while compiling to zero

### Consistency with BCPL

The null literal maintains BCPL's core principles:
- **Simple syntax**: Single character representation
- **Efficient compilation**: Compiles directly to zero
- **Clear semantics**: Obvious meaning in context
- **Minimal overhead**: No runtime penalty

## Best Practices

### 1. Use for Pointer Types
```bcpl
// Good: Clear pointer semantics
LET object_ptr = ?
LET string_ptr = ?

// Avoid: Confusing with regular integers
LET count = ?     // Better: LET count = 0
```

### 2. Always Check Before Use
```bcpl
LET START() BE $(
    LET obj = get_object()

    IF obj ~= ? THEN $(
        // Safe to use obj
        obj.some_method()
    $)
$)
```

### 3. Initialize Pointers Explicitly
```bcpl
// Good: Clear intent
LET ptr = ?

// Less clear: Uninitialized
LET ptr
```

### 4. Use in Optional Returns
```bcpl
FUNCTION find_item(list, target) = VALOF $(
    // Search logic...

    TEST found THEN
        RESULTIS item
    ELSE
        RESULTIS ?    // Clear "not found" signal
$)
```

## Examples

### Simple Null Checking
```bcpl
LET START() BE $(
    WRITES("=== Null Literal Demo ===*N")

    LET ptr = ?
    WRITES("Created null pointer*N")

    IF ptr = ? THEN
        WRITES("Pointer is null (correct!)*N")

    ptr := 42
    IF ptr ~= ? THEN
        WRITES("Pointer now has value*N")

    ptr := ?
    WRITES("Reset to null*N")
$)
```

### Optional Object Pattern
```bcpl
CLASS Optional $(
    DECL has_value
    DECL stored_value

    ROUTINE CREATE() BE $(
        has_value := FALSE
        stored_value := ?
    $)

    ROUTINE set(value) BE $(
        stored_value := value
        has_value := TRUE
    $)

    FUNCTION get() = VALOF $(
        TEST has_value THEN
            RESULTIS stored_value
        ELSE
            RESULTIS ?
    $)
$)
```

## Implementation Details

### Compilation
- **Lexer**: `?` tokenized as `TokenType::NullLiteral`
- **Parser**: Creates `NullLiteral` AST nodes
- **Code Generation**: Compiles to `MOVZ reg, #0`
- **Runtime**: Zero overhead - identical to using 0

### Memory Layout
The null literal compiles to machine zero, making it compatible with:
- C NULL pointers
- System null handles
- Memory-mapped zero addresses
- Standard pointer arithmetic

## Migration from Zero

If you have existing code using `0` for null pointers, you can gradually migrate:

```bcpl
// Old style
LET ptr = 0
IF ptr = 0 THEN ...

// New style
LET ptr = ?
IF ptr = ? THEN ...
```

Both styles work identically at runtime, but `?` provides better semantic clarity.

## Compatibility

The null literal is:
- ✅ **Backward compatible**: Doesn't break existing code
- ✅ **Type safe**: Clear pointer semantics
- ✅ **Zero overhead**: Compiles to optimal machine code
- ✅ **Platform agnostic**: Works on all supported architectures

## Conclusion

The `?` null literal brings modern null safety to BCPL while preserving the language's elegance and efficiency. It provides a clear, type-safe way to work with nullable values that's both expressive for developers and efficient at runtime.

This feature demonstrates how classic languages can evolve to incorporate modern programming concepts without sacrificing their fundamental character and performance advantages.

---

**NewBCPL Compiler Version**: 1.0.599+
**Feature Status**: Stable, Production Ready
**Performance Impact**: Zero runtime overhead

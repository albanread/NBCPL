NewBCPL/book/NewBCPL_Types.md
```

# NewBCPL Types

## Historical Background: Types in Classic BCPL

The original BCPL language was designed for portability and simplicity, targeting a wide range of hardware. Its type system was minimal and pragmatic:

- **Machine Word as Universal Type:**
  Every variable in BCPL was a machine word (typically 16, 32, or 64 bits depending on the platform).
  This word could be interpreted as:
  - An integer (signed or unsigned)
  - A pointer (to code or data)
  - A sequence of bits (for bitwise operations)
  - A character or part of a string (using indirection)

- **Strings:**
  Strings were represented as sequences of characters in memory, typically with a length prefix or a terminating character.
- **Vectors:**
  BCPL supported heap-allocated vectors (arrays) of words, which could be used for any purpose.
- **Floats:**
  Floating-point support was added later, and not all BCPL implementations included it.

This flexible, low-level approach allowed BCPL to be used for systems programming.

---

## The NewBCPL Type System

NewBCPL extends the BCPL type system, introducing both value types and heap-allocated types, while retaining the language's pragmatic spirit.

### Value Types

NewBCPL value types are stored in CPU registers, but the register usage and even which registers are used can vary depending on the type. For example, integer and pointer values are typically passed and stored in general-purpose registers, while floating-point values use floating-point registers, and pairs may use dedicated or paired registers depending on the architecture. These differences reflect literal architectural distinctions in the underlying machine.

The simplest way to tell the compiler what you intend is to use the `LET` declaration for integer, pointer, or general-purpose values, and the `FLET` declaration for floating-point values. This helps the compiler select the correct register class and calling convention for your variables and function arguments.

Similarly, when writing functions, you can specify the intended return type using `VALOF` for integer (or general-purpose) return values, and `FVALOF` for floating-point return values. These very simple declarations make your intention clear to the compiler and guide its type inference choices for the function's result.

**Examples:**
```bcpl
LET x = 42           // Inferred as INTEGER
FLET y = 3.14        // Inferred as FLOAT

LET f = VALOF $( ... $)      // Function returns an integer value
LET g = FVALOF $( ... $)     // Function returns a floating-point value
```

By using `LET`, `FLET`, `VALOF`, and `FVALOF`, you provide the compiler with explicit information about your intent, which can be especially helpful in complex code or when interfacing with low-level routines.

- **INTEGER**
  Standard signed integer values, stored in general-purpose registers.
- **FLOAT**
  IEEE-754 double-precision floating-point values, stored in floating-point registers.
- **PAIR**
  A value type representing two 32-bit integers packed into a 64-bit value, may use special handling or paired registers.
- **FPAIR**
  A value type representing two 32-bit floats packed into a 64-bit value, may use special handling or paired registers.

### Heap-Allocated Types

- **VECTOR**
  A heap-allocated array of values (can be of any type).
- **STRING**
  A heap-allocated sequence of characters, with special runtime and language support.
- **LIST**
  A heap-allocated linked list structure.
- **OBJECT**
  A heap-allocated structure supporting fields and (optionally) methods, enabling object-oriented programming.

### Type Modifiers

- **POINTER**
  Any type can be used as a pointer type, using the `^` (caret) syntax or the `POINTER` keyword.
- **CONST**
  (If supported) Indicates a value that should not be modified after initialization.

---

## Type Inference and Type Hints

NewBCPL is designed to infer types automatically in most cases. The compiler analyzes how a variable or expression is used and assigns the most appropriate type. This means that, for most code, you do **not** need to provide explicit type annotations.



**Example:**
```bcpl
LET x = 42         // Inferred as INTEGER
LET y = 3.14       // Inferred as FLOAT
LET s = "hello"    // Inferred as STRING
LET p = PAIR(1,2)  // Inferred as PAIR
```

However, there are situations where you may want to provide a type hint, such as for documentation, optimization, or to resolve ambiguity. This is done using the `AS` syntax.

---

## The `AS` Syntax for Type Hints

You can provide an explicit type hint in variable declarations and certain expressions using the `AS` keyword.

**Syntax:**
```bcpl
LET <name> AS <type> = <expression>
```

**Examples:**
```bcpl
LET x AS INTEGER = 42
LET y AS FLOAT = 3.14
LET s AS STRING = "hello"
LET p AS PAIR = PAIR(100, 200)
LET q AS FPAIR = FPAIR(1.0, 2.0)
```

### Pointer Types

To declare a pointer type, use the caret (`^`) or the `POINTER` keyword:

```bcpl
LET ptr AS ^INTEGER = ...
LET vec AS POINTER TO VECTOR OF INTEGER = ...
```

### Container Types

You can specify container types using `LIST OF`, `VECTOR OF`, etc.:

```bcpl
LET nums AS VECTOR OF INTEGER = ...
LET names AS LIST OF STRING = ...
```

### Combined Types

Pointer to a vector of floats:
```bcpl
LET pfvec AS ^VECTOR OF FLOAT = ...
```

---

## Supported Type Keywords

- `INTEGER`
- `FLOAT`
- `PAIR`
- `FPAIR`
- `STRING`
- `ANY`
- `LIST`
- `VECTOR`
- `OBJECT`
- (and combinations with `POINTER`, `CONST`, etc.)

**Warning:**
If you do provide a type hint using `AS`, the compiler will **trust your selection over its own inference rules**. If you specify an incorrect or incompatible type hint, this can cause subtle or serious problems at compile time or runtime. **Be careful and ensure your type hints are correct and match your intent.**


---

## Summary

- **Classic BCPL** treated all data as machine words, with flexible but unsafe type usage.
- **NewBCPL** introduces a richer type system with additional value and heap-allocated types, supporting safe and simple programming patterns.
- **Type inference** means you rarely need to specify types, but you can use `AS` for explicit type hints.
- **Pointer and container types** are supported with clear, readable syntax.

This approach balances the power and flexibility of traditional BCPL with some additional safety and convenience.

---

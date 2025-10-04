# How LIST() Works in NewBCPL

This document provides a comprehensive overview of how the `LIST()` functionality works in NewBCPL, covering parsing, compilation, runtime implementation, and performance characteristics.

## Table of Contents

1. [Overview](#overview)
2. [Syntax and Parsing](#syntax-and-parsing)
3. [Compilation Strategies](#compilation-strategies)
4. [Runtime Implementation](#runtime-implementation)
5. [Type System Integration](#type-system-integration)
6. [Performance Characteristics](#performance-characteristics)
7. [Memory Management](#memory-management)
8. [Examples](#examples)

## Overview

NewBCPL's `LIST()` functionality provides a powerful and efficient way to create and manipulate heterogeneous collections. The system is designed with two distinct paths:

- **Static Path**: For compile-time constant lists, stored in `.data` section
- **Dynamic Path**: For runtime-constructed lists, built dynamically in heap

This dual approach provides optimal performance for constant data while maintaining full flexibility for dynamic construction.

## Syntax and Parsing

### Basic Syntax

```bcpl
LIST()                          // Empty list
LIST(1, 2, 3)                  // Integer list
LIST("hello", "world")         // String list
LIST(1, "mixed", 3.14)         // Mixed type list
MANIFESTLIST(1, 2, 3)          // Read-only constant list
```

### Parsing Implementation

Lists are parsed in `pz_parse_expressions.cpp`:

```cpp
ExprPtr Parser::parse_list_expression(bool is_manifest) {
    consume(TokenType::LParen, "Expect '(' after LIST or MANIFESTLIST.");
    
    std::vector<ExprPtr> initializers;
    
    // Support LIST() as an empty list literal
    if (check(TokenType::RParen)) {
        consume(TokenType::RParen, "Expect ')' to close empty list.");
        auto node = std::make_unique<ListExpression>(std::vector<ExprPtr>{});
        node->is_manifest = is_manifest;
        return node;
    }
    
    // Parse comma-separated expressions
    do {
        initializers.push_back(parse_expression());
    } while (match(TokenType::Comma));
    
    consume(TokenType::RParen, "Expect ')' to close LIST expression.");
    
    auto node = std::make_unique<ListExpression>(std::move(initializers));
    node->is_manifest = is_manifest;
    return node;
}
```

## Compilation Strategies

The compiler uses two different strategies based on the nature of the list elements:

### Static Path (Compile-time Constants)

**When Used**: All elements are compile-time constants (literals)

**Process**:
1. `DataGenerator::add_list_literal()` creates static data layout
2. List structure stored in `.data` section
3. At runtime, either:
   - Direct pointer load (for `MANIFESTLIST`)
   - Deep copy operation (for mutable `LIST`)

**Generated Code**:
```assembly
// For MANIFESTLIST (direct pointer)
ADRP X0, list_label
ADD  X0, X0, #:lo12:list_label

// For LIST (deep copy)
ADRP X0, list_label  
ADD  X0, X0, #:lo12:list_label
BL   DEEPCOPYLITERALLIST
```

### Dynamic Path (Runtime Construction)

**When Used**: Any element is a variable or expression

**Process**:
1. Call `BCPL_LIST_CREATE_EMPTY()` to create header
2. Evaluate each expression
3. Call appropriate `BCPL_LIST_APPEND_*` function
4. Result is pointer to constructed list

**Generated Code**:
```assembly
// Create empty list
BL   BCPL_LIST_CREATE_EMPTY
MOV  X19, X0  // Save list header

// For each element:
// ... evaluate expression to get value in register ...
MOV  X0, X19  // List header
MOV  X1, <value_reg>  // Element value
BL   BCPL_LIST_APPEND_INT  // Or _FLOAT, _STRING, etc.
```

## Runtime Implementation

### Data Structures

#### ListHeader Structure
```c
typedef struct ListHeader {
    int32_t  type;              // Always ATOM_SENTINEL (0)
    int32_t  contains_literals; // 1 = static data, 0 = heap allocated
    int64_t  length;            // Number of elements
    ListAtom* head;             // Pointer to first element
    ListAtom* tail;             // Pointer to last element (for O(1) appends)
} ListHeader;
```

#### ListAtom Structure
```c
typedef struct ListAtom {
    int32_t type;        // Type tag (ATOM_INT, ATOM_FLOAT, etc.)
    int32_t pad;         // Padding for alignment
    union {
        int64_t int_value;      // For integers
        double float_value;     // For floats
        void* ptr_value;        // For strings, lists, objects
    } value;
    struct ListAtom* next;      // Pointer to next element
} ListAtom;
```

#### Type Tags
```c
#define ATOM_SENTINEL     0  // Header marker
#define ATOM_INT          1  // Integer value
#define ATOM_FLOAT        2  // Float value  
#define ATOM_STRING       3  // String pointer
#define ATOM_LIST_POINTER 4  // Nested list pointer
#define ATOM_OBJECT       5  // Object pointer
#define ATOM_PAIR         6  // Pair value
```

### Runtime Functions

#### List Creation
- `BCPL_LIST_CREATE_EMPTY()` → Creates empty `ListHeader*`

#### List Appending
- `BCPL_LIST_APPEND_INT(header, value)` → Append integer
- `BCPL_LIST_APPEND_FLOAT(header, value)` → Append float
- `BCPL_LIST_APPEND_STRING(header, string_ptr)` → Append string
- `BCPL_LIST_APPEND_LIST(header, list_ptr)` → Append nested list

#### List Operations
- `HD(list)` → Get first element (head)
- `TL(list)` → Get tail (rest of list)
- `LENGTH(list)` → Get number of elements
- `SPLIT(string, delimiter)` → Split string into list
- `JOIN(list, separator)` → Join list elements into string

#### Memory Management
- `BCPL_FREE_LIST(header)` → Free dynamically allocated list
- `DEEPCOPYLITERALLIST(static_data)` → Copy static list to heap

## Type System Integration

### Return Types

Lists integrate with NewBCPL's comprehensive type system:

```cpp
// Basic list types
VarType::POINTER_TO_LIST_OF_ANY        // Generic list
VarType::POINTER_TO_LIST_OF_INTEGER    // List of integers
VarType::POINTER_TO_LIST_OF_FLOAT      // List of floats
VarType::POINTER_TO_LIST_OF_STRING     // List of strings
VarType::POINTER_TO_LIST_OF_LIST       // Nested lists
VarType::POINTER_TO_LIST_OF_OBJECT     // List of objects

// Const variants (for MANIFESTLIST)
VarType::CONST_POINTER_TO_LIST_OF_ANY
VarType::CONST_POINTER_TO_LIST_OF_INTEGER
// ... etc
```

### Type Inference

The compiler can infer specific list types:

```bcpl
LET int_list = LIST(1, 2, 3);         // POINTER_TO_LIST_OF_INTEGER
LET float_list = LIST(1.0, 2.0, 3.0); // POINTER_TO_LIST_OF_FLOAT
LET str_list = LIST("a", "b", "c");    // POINTER_TO_LIST_OF_STRING
LET mixed_list = LIST(1, "b", 3.0);    // POINTER_TO_LIST_OF_ANY
```

## Performance Characteristics

### Static Lists
- **Creation**: Very fast (pre-allocated in `.data`)
- **Access**: Direct pointer arithmetic
- **Memory**: Efficient (no allocation overhead)
- **Limitation**: Requires deep copy for mutability

### Dynamic Lists  
- **Creation**: Moderate cost (heap allocation + function calls)
- **Appending**: O(1) due to tail pointer optimization
- **Access**: Standard linked list traversal
- **Flexibility**: Full runtime construction capability

### Manifest Lists
- **Creation**: Fastest (direct pointer)
- **Access**: Direct pointer arithmetic
- **Memory**: Most efficient (no copying)
- **Limitation**: Read-only

## Memory Management

### Static Data Layout (.data section)
```
ListLiteralHeader: [type|pad|tail|head|length]
                         ↓
ListAtom 1:        [type|pad|value|next] ──┐
                                          ↓
ListAtom 2:        [type|pad|value|next] ──┐
                                          ↓
ListAtom 3:        [type|pad|value|NULL]
```

### Dynamic Heap Layout
```
ListHeader: [type|literals_flag|length|head|tail]
                                        ↓     ↑
ListAtom 1: [type|pad|value|next] ──────┘     │
                           ↓                  │
ListAtom 2: [type|pad|value|next] ────────────┘
                           ↓
ListAtom 3: [type|pad|value|NULL]
```

### Cleanup Rules
- **Dynamic lists**: Must call `BCPL_FREE_LIST()` or `FREEVEC()`
- **Static lists (after deep copy)**: Must call `BCPL_FREE_LIST()`
- **Manifest lists**: No cleanup needed (static data)

## Examples

### Basic Usage
```bcpl
LET START() BE {
    // Empty lists
    LET empty = LIST();
    LET empty_manifest = MANIFESTLIST();
    
    // Static constant lists
    LET numbers = LIST(1, 2, 3, 4, 5);
    LET words = LIST("apple", "banana", "cherry");
    
    // Dynamic lists (contains variables)
    LET x = 10;
    LET dynamic = LIST(x, x*2, x*3);
    
    // Mixed type lists
    LET mixed = LIST(42, "answer", 3.14, "pi");
    
    // Nested lists
    LET nested = LIST(LIST(1, 2), LIST(3, 4), LIST(5, 6));
    
    // List operations
    WRITEF("First element: %N*N", HD(numbers));
    WRITEF("List length: %N*N", LENGTH(numbers));
    WRITEF("Second element: %N*N", HD(TL(numbers)));
    
    // Runtime appending
    LET growing = LIST(1, 2);
    growing := APND(growing, 3);  // Add integer
    growing := SPND(growing, "end");  // Add string
    
    // Cleanup (don't free manifest lists!)
    BCPL_FREE_LIST(empty);
    BCPL_FREE_LIST(numbers);
    BCPL_FREE_LIST(words);
    BCPL_FREE_LIST(dynamic);
    BCPL_FREE_LIST(mixed);
    BCPL_FREE_LIST(nested);
    BCPL_FREE_LIST(growing);
}
```

### Advanced Patterns
```bcpl
// List comprehension style
LET squares = LIST();
FOR i = 1 TO 10 DO {
    squares := APND(squares, i * i);
}

// Functional style operations
LET filtered = FILTER(numbers, is_even);
LET reversed = REVERSE(numbers);
LET concatenated = CONCAT(list1, list2);

// String operations
LET parts = SPLIT("hello,world,bcpl", ",");
LET rejoined = JOIN(parts, " | ");
```

## Implementation Files

- **Parsing**: `pz_parse_expressions.cpp` (lines 408-538)
- **Code Generation**: `generators/gen_ListExpression.cpp`
- **Data Generation**: `DataGenerator.cpp` (`add_list_literal`)
- **Runtime Types**: `runtime/ListDataTypes.h`
- **Runtime Bridge**: `runtime/RuntimeBridge.cpp`
- **Type System**: `DataTypes.h` (list type definitions)

This comprehensive system provides both high performance for constant data and full flexibility for dynamic list construction, making it suitable for a wide range of programming patterns in NewBCPL.
# Logical AND/OR Operators Fix

**Date**: December 2024  
**Status**: ✅ COMPLETE  
**Impact**: Critical Language Feature  

## Overview

Fixed logical AND and OR operators (`AND`, `OR`) in the NewBCPL compiler. These fundamental language constructs had **never been properly implemented** and would fail during compilation with linker errors.

## The Problem

Logical expressions like `IF x > 0 AND y < 10 THEN ...` or `LET result = a OR b` would fail with:
```
NewBCPL Compiler Error: Error: Undefined label '.L1' encountered during linking.
```

This affected any code using:
- `1 AND 0`  
- `flag OR condition`
- Complex conditionals with short-circuiting logic

## Root Causes Discovered

### 1. **Label Definition Bug (Both AND & OR)**
- **Issue**: `generate_short_circuit_and()` and `generate_short_circuit_or()` used `emit(Encoder::create_directive(...))` to define labels
- **Problem**: This created directive instructions but didn't register labels with the instruction stream for the linker
- **Location**: `NewCodeGenerator.cpp` lines 1072, 1075, 1103, 1111

### 2. **Lexer Token Mapping Bug (OR only)**  
- **Issue**: `"OR"` keyword mapped to `TokenType::Or` instead of `TokenType::LogicalOr`
- **Problem**: Parser expected `TokenType::LogicalOr` for binary operations, causing parse failures
- **Location**: `Lexer.cpp` keywords map

### 3. **ARM64 Instruction Bug (OR only)**
- **Issue**: MOVK instructions used incorrect shift values (1, 2, 3)
- **Problem**: ARM64 MOVK requires shifts as multiples of 16 bits (0, 16, 32, 48)
- **Location**: `NewCodeGenerator.cpp` MOVK instruction generation

## The Fix

### Phase 1: Label Definition (Both Operators)
```cpp
// BEFORE (broken):
emit(Encoder::create_directive(false_label + ":"));
emit(Encoder::create_directive(end_label + ":"));

// AFTER (working):
instruction_stream_.define_label(false_label);
instruction_stream_.define_label(end_label);
```

### Phase 2: Lexer Token Mapping (OR)
```cpp
// BEFORE (broken):
{"OR", TokenType::Or}

// AFTER (working):  
{"OR", TokenType::LogicalOr}
```

### Phase 3: ARM64 Instruction Fix (OR)
```cpp
// BEFORE (invalid ARM64):
emit(Encoder::create_movk_imm(reg, 0xFFFF, 1));  // Invalid shift
emit(Encoder::create_movk_imm(reg, 0xFFFF, 2));  // Invalid shift  
emit(Encoder::create_movk_imm(reg, 0xFFFF, 3));  // Invalid shift

// AFTER (valid ARM64):
emit(Encoder::create_movk_imm(reg, 0xFFFF, 16)); // Valid shift
emit(Encoder::create_movk_imm(reg, 0xFFFF, 32)); // Valid shift
emit(Encoder::create_movk_imm(reg, 0xFFFF, 48)); // Valid shift
```

## Debugging Process

1. **Initial symptom**: Undefined label errors during linking
2. **Trace analysis**: Parser successfully created BinaryOp nodes  
3. **Code generation**: Labels were being generated but not properly defined
4. **Linker investigation**: Branch targets couldn't find corresponding label definitions
5. **Root cause**: Wrong label definition method used throughout short-circuit logic

## Test Cases

### Logical AND ✅
```bcpl
LET START() BE {
    LET X = 1 AND 0    // Result: 0 (false)
    LET Z = 1 AND 1    // Result: -1 (true in BCPL)  
    LET Y = 0 AND 0    // Result: 0 (false)
    WRITEF(" TEST X %d Z %d Y %d*N", X, Z, Y)
}
```

### Logical OR ✅  
```bcpl
LET START() BE {
    LET X = 1 OR 0     // Result: -1 (true)
    LET Z = 1 OR 1     // Result: -1 (true)
    LET Y = 0 OR 0     // Result: 0 (false)
    WRITEF(" TEST X %d Z %d Y %d*N", X, Z, Y)  
}
```

## Impact

- **Enables fundamental language feature**: Logical operators now work for the first time
- **Unlocks complex conditionals**: `IF a AND b OR c THEN ...`  
- **Short-circuit evaluation**: Proper BCPL semantics for logical operations
- **Broader language support**: Many BCPL programs now compilable

## Files Modified

- `NewBCPL/NewCodeGenerator.cpp` - Fixed label definition in both AND/OR functions
- `NewBCPL/Lexer.cpp` - Fixed OR token mapping  
- `NewBCPL/encoders/enc_create_directive.cpp` - Removed debug output

## Lessons Learned

- **Label management**: Critical distinction between `emit(directive)` vs `instruction_stream_.define_label()`
- **Token type mapping**: Lexer-parser contract must be consistent
- **ARM64 specifics**: MOVK shift values have hardware constraints  
- **Testing gaps**: Fundamental operators had never been properly tested

## Note

It's remarkable that these core language operators had never been properly implemented or tested. This fix enables a significant portion of BCPL language functionality that was previously broken.
# Parameter Comments Enhancement

This document demonstrates the improved parameter comment system that adds meaningful comments to MOV instructions when setting up function arguments in registers x0-x7.

## Overview

When calling functions, the compiler now generates assembly with informative comments that show which parameter is being moved to which register, using the actual parameter names from the function signature.

## Before (Old System)

```assembly
MOV X0, X27   ; No comment (this pointer)
MOV X1, X20   ; No comment
MOV X2, X21   ; No comment
BL Point::set ; Direct call without parameter comments
```

## After (New System)

```assembly
MOV X0, X27     // this pointer
MOV X1, X20     // Parameter 0: x
MOV X2, X21     // Parameter 1: y
BL Point::set   // Direct call with parameter comments
```

## Technical Implementation

### 1. Enhanced Symbol Table

The `Symbol::ParameterInfo` structure now includes parameter names:

```cpp
struct ParameterInfo {
    std::string name;    // NEW: Actual parameter name
    VarType type;
    bool is_optional;
};
```

### 2. Updated Encoders

New comment variants of MOV encoders:
- `create_mov_reg_comment(dest, src, comment)` - For integer parameters
- `create_fmov_reg_comment(dest, src, comment)` - For float parameters  
- `create_scvtf_reg_comment(dest, src, comment)` - For int-to-float conversions

### 3. Enhanced Argument Coercion

The `coerce_arguments_to_abi()` function now accepts parameter names and generates meaningful comments:

```cpp
void coerce_arguments_to_abi(
    const std::vector<std::string>& arg_regs,
    const std::vector<VarType>& arg_types,
    const std::vector<VarType>& expected_types,
    const std::vector<std::string>& param_names  // NEW
);
```

## Example Function Calls

### User-Defined Functions

For a BCPL function declared as:
```bcpl
LET draw_rectangle(width, height, color) BE
    // implementation
```

A call like `draw_rectangle(100, 50, RED)` generates:
```assembly
MOV X0, X5      // Parameter 0: width
MOV X1, X6      // Parameter 1: height  
MOV X2, X7      // Parameter 2: color
BL draw_rectangle
```

### Runtime Functions

For runtime functions like `PUTWORD(ptr, offset, value)`:
```assembly
MOV X0, X3      // Parameter 0: ptr
MOV X1, X4      // Parameter 1: offset
MOV X2, X5      // Parameter 2: value
BL PUTWORD_veneer
```

### Method Calls

**Virtual method calls** (via vtable dispatch):
```assembly
LDR X9, [X27, #0]    ; Load vtable pointer
LDR X10, [X9, #0]    ; Load method address  
MOV X0, X27          // this pointer
MOV X1, X20          // Parameter 0: x
MOV X2, X21          // Parameter 1: y
BLR X10              // Call Point::CREATE
```

**Direct method calls** (non-virtual/final methods):
```assembly
MOV X0, X27          // this pointer
MOV X1, X20          // Parameter 0: x
MOV X2, X21          // Parameter 1: y
BL Point::set        // Direct call to method
```

**Routine method calls** with parameters:
```assembly
LDR X9, [X15, #0]    ; Load vtable pointer
LDR X10, [X9, #16]   ; Load method address
MOV X0, X15          // this pointer
MOV X1, X16          // Parameter 0: width
MOV X2, X17          // Parameter 1: height
MOV X3, X18          // Parameter 2: color
BLR X10              // Call SomeClass::draw method
```

For SUPER method calls like `SUPER.CREATE(x, y)`:
```assembly
LDR X9, [X0, #0]     ; Load vtable pointer
LDR X10, [X9, #0]    ; Load parent method address
MOV X0, X27          // this pointer
MOV X1, X20          // Parameter 0: x
MOV X2, X21          // Parameter 1: y
BLR X10              // Call CREATE (SUPER)
```

For object cleanup (RELEASE method):
```assembly
LDR X9, [X0, #0]     ; Load vtable for RELEASE
LDR X10, [X9, #8]    ; Load RELEASE address
BLR X10              // Call RELEASE method
```

### Type Conversions

When type conversion is needed:
```assembly
MOV X0, X1      // Parameter 0: count
SCVTF D0, X2    // Parameter 1: scale (int->float)
BL some_function
```

## Benefits

1. **Improved Debugging**: Easy to see which parameters are being passed
2. **Better Code Review**: Clear understanding of function call setup
3. **Reduced Errors**: Helps identify parameter ordering issues
4. **Documentation**: Assembly code is self-documenting

## Implementation Files

- `Symbol.h` - Enhanced ParameterInfo structure
- `ClassTable.h` - Enhanced ClassMethodInfo::ParameterInfo structure
- `analysis/SymbolDiscoveryPass.cpp` - Parameter name capture during parsing
- `analysis/sym_ClassDiscovery.cpp` - Class method parameter name capture
- `RuntimeSymbols.cpp` - Runtime function parameter names
- `encoders/enc_create_scvtf_reg.cpp` - New SCVTF comment encoder
- `encoders/enc_create_branch_with_link_register.cpp` - New BLR comment encoder
- `generators/helpers/gen_coerce_to_arg.cpp` - Enhanced argument coercion
- `generators/gen_FunctionCall.cpp` - Function and method call parameter handling
- `generators/gen_RoutineCallStatement.cpp` - Routine call parameter handling

## Fallback Behavior

When parameter names are not available, the system falls back to generic names:
- `Parameter 0: arg0`
- `Parameter 1: arg1`
- etc.

This ensures the system works even for functions without proper parameter name information.
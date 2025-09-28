# ARM64 Calling Convention Bug Analysis and Prevention

## Executive Summary

This document analyzes a critical class of bugs in ARM64 code generation that occurs when calling convention rules are violated during function calls. The specific bug pattern involves improper use of caller-saved registers that get overwritten during nested function calls, leading to data corruption.

## The Bug Pattern

### Root Cause
The ARM64 calling convention defines two categories of registers:
- **Caller-saved registers (X0-X18, D0-D7, D16-D31)**: The calling function must save these if it needs their values after a function call
- **Callee-saved registers (X19-X30, D8-D15)**: The called function must preserve these and restore them before returning

### Classic Failure Scenario
```assembly
; Function receives parameter in X3
MOV X15, X3           ; Store parameter in caller-saved register X15
; ... other code ...
BL some_function      ; Call another function
; X15 may now contain garbage because it's caller-saved!
STR X15, [X27, #24]   ; Store corrupted value to memory
```

## Real-World Example: ColorPoint Constructor Bug

### The Scenario
1. **Setup**: `ColorPoint::CREATE` receives color value `255` in argument register `X3`
2. **Storage**: Code moves `255` from `X3` to `X15` (caller-saved register)
3. **Nested Call**: Constructor calls parent `Point::CREATE`
4. **Corruption**: During parent call, `X15` gets overwritten with `0`
5. **Bug**: Constructor stores `0` instead of `255` for color value

### Assembly Analysis
```assembly
; ColorPoint::CREATE function
102f28200: MOV X15, X3       ; Move initialColor (255) into X15 - BUG!
; ... setup other parameters ...
102f28228: BL Point::CREATE  ; Call parent constructor
; X15 is now corrupted because it's caller-saved
102f2822c: STR X15, [X27, #24] ; Store corrupted value (0) instead of 255
```

## BCPL Compiler Vulnerability Analysis

### Vulnerable Code Patterns

#### 1. SUPER Call Implementation (CRITICAL)
The original `handle_super_call` implementation was particularly vulnerable:

```cpp
// VULNERABLE CODE - DO NOT USE
void handle_super_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    // ... setup code ...
    emit(Encoder::create_mov_reg("X0", this_ptr_reg));
    
    // DANGEROUS: Manual register assignment without ABI compliance
    for (size_t i = 0; i < arg_result_regs.size(); ++i) {
        std::string target_reg = "X" + std::to_string(i + 1);
        emit(Encoder::create_mov_reg(target_reg, arg_result_regs[i])); // BUG!
        register_manager_.release_register(arg_result_regs[i]);
    }
    
    emit(Encoder::create_branch_with_link(method_info->qualified_name));
    // Values in caller-saved registers may be corrupted!
}
```

#### 2. Other At-Risk Patterns
- Direct register assignment without type consideration
- Storing intermediate values in caller-saved registers before calls
- Manual argument marshaling without proper ABI compliance

### The Fix: ARM64 ABI-Compliant Argument Coercion

#### Correct Implementation
```cpp
// SECURE CODE - ARM64 ABI Compliant
void handle_super_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    // ... setup code ...
    
    // Collect type information for proper register allocation
    std::vector<VarType> arg_types;
    std::vector<VarType> expected_types;
    std::vector<std::string> param_names;
    
    for (size_t i = 0; i < arg_result_regs.size(); ++i) {
        VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
        arg_types.push_back(arg_expr_type);
        
        VarType expected_type = VarType::UNKNOWN;
        std::string param_name = "arg" + std::to_string(i);
        if (has_param_info && i < method_symbol.parameters.size()) {
            expected_type = method_symbol.parameters[i].type;
            if (!method_symbol.parameters[i].name.empty()) {
                param_name = method_symbol.parameters[i].name;
            }
        }
        expected_types.push_back(expected_type);
        param_names.push_back(param_name);
    }
    
    // Set up 'this' pointer
    emit(Encoder::create_mov_reg_comment("X0", this_ptr_reg, "this pointer"));
    
    // CRITICAL: Use ARM64 ABI compliant argument coercion
    coerce_arguments_to_abi(arg_result_regs, arg_types, expected_types, param_names);
    
    // Make the call
    emit(Encoder::create_branch_with_link(method_info->qualified_name));
    register_manager_.invalidate_caller_saved_registers();
}
```

#### Key Components of the Fix

##### 1. ARM64 ABI-Compliant Register Assignment
The `coerce_arguments_to_abi` function implements proper ARM64 calling convention:

```cpp
void coerce_arguments_to_abi(
    const std::vector<std::string>& arg_regs,
    const std::vector<VarType>& arg_types,
    const std::vector<VarType>& expected_types,
    const std::vector<std::string>& param_names
) {
    // Maintain separate counters as required by ARM64 ABI
    int ngrn = 0; // Next General-purpose Register Number (X0-X7)
    int nsrn = 0; // Next SIMD and Floating-point Register Number (D0-D7)
    
    // Batch operations to prevent register conflicts
    std::vector<MoveOperation> mov_operations;
    
    for (size_t i = 0; i < arg_regs.size(); ++i) {
        VarType target_type = (expected_types[i] != VarType::UNKNOWN) ? 
                             expected_types[i] : arg_types[i];
        
        if (target_type == VarType::FLOAT) {
            std::string dest_d_reg = "D" + std::to_string(nsrn);
            // Queue appropriate move/conversion operation
            nsrn++;
        } else {
            std::string dest_x_reg = "X" + std::to_string(ngrn);
            // Queue appropriate move/conversion operation
            ngrn++;
        }
    }
    
    // Execute all operations atomically
    for (const auto& op : mov_operations) {
        // Emit with proper comments for debugging
    }
}
```

##### 2. Register Conflict Prevention
- **Batch Operations**: Collect all register moves before executing them
- **Atomic Execution**: Prevent register manager from reusing destination registers
- **Type-Aware Assignment**: Separate integer and floating-point register spaces

##### 3. Proper Register Lifecycle Management
- **Early Release Prevention**: Don't release source registers until after moves
- **Caller-Saved Invalidation**: Properly invalidate caller-saved registers after calls
- **Return Type Handling**: Set correct result register based on function return type

## Prevention Guidelines

### 1. Always Use ABI-Compliant Functions
- ✅ Use `coerce_arguments_to_abi()` for all function calls
- ❌ Never use manual register assignment for arguments

### 2. Register Usage Rules
- ✅ Use callee-saved registers (X19-X28) for values that must survive calls
- ❌ Never store call-crossing values in caller-saved registers (X0-X18)

### 3. Code Generation Patterns
- ✅ Batch register operations to prevent conflicts
- ✅ Include meaningful parameter comments in generated assembly
- ❌ Release registers before their values are used

### 4. Testing and Validation
- Test with complex nested function calls (constructors calling parent constructors)
- Verify parameter values survive through call chains
- Use assembly output inspection to validate register usage

## Impact Assessment

### Before Fix
- SUPER calls vulnerable to parameter corruption
- Silent data corruption in complex inheritance hierarchies
- Difficult-to-debug runtime failures

### After Fix
- ARM64 ABI compliant register usage across all call types
- Proper parameter preservation through call chains
- Enhanced debugging with meaningful parameter comments
- Consistent behavior across regular, method, and SUPER calls

## Related Files Modified

### Core Implementation
- `NewBCPL/generators/gen_FunctionCall.cpp` - Fixed SUPER call handling
- `NewBCPL/generators/helpers/gen_coerce_to_arg.cpp` - ARM64 ABI implementation

### Supporting Infrastructure
- `NewBCPL/Encoder.h` - Comment-enabled instruction encoders
- `NewBCPL/NewCodeGenerator.h` - Enhanced argument coercion signatures

## Verification

To verify the fix works correctly:

1. **Compile test programs** with SUPER calls in constructor chains
2. **Inspect generated assembly** for proper register usage patterns
3. **Run integration tests** with complex object hierarchies
4. **Validate parameter values** are preserved through call chains

## Conclusion

This ARM64 calling convention bug represents a class of subtle but critical errors in compiler code generation. The fix demonstrates the importance of:

1. **Strict ABI Compliance**: Following calling convention rules precisely
2. **Type-Aware Code Generation**: Considering data types in register allocation
3. **Comprehensive Testing**: Testing complex scenarios like inheritance chains
4. **Clear Documentation**: Making generated code readable and debuggable

The implemented solution provides a robust foundation for ARM64 code generation that properly handles all types of function calls while maintaining strict adherence to the ARM64 ABI specification.
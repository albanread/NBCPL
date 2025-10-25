# Scratch Register Exhaustion Analysis

## Executive Summary

The current function call argument evaluation strategy in NewBCPL has two critical flaws that create serious reliability and scalability issues:

1. **Scratch Register Exhaustion**: The compiler will crash when calling functions with 8+ arguments
2. **Ineffective Clobber Protection**: The current "protection" strategy actually doesn't prevent clobbering

This document provides a detailed analysis of the problem and proposes a robust solution.

## Problem Analysis

### Current Flawed Strategy

The current approach in `gen_FunctionCall.cpp` and related files follows this pattern:

```cpp
// FLAWED APPROACH - DO NOT USE
std::vector<ArgInfo> arg_result_regs;
for (const auto& arg_expr : node.arguments) {
    generate_expression_code(*arg_expr);  // Uses scratch registers
    
    // Copy to "protect" from clobbering
    std::string temp_reg;
    if (register_manager_.is_fp_register(expression_result_reg_)) {
        temp_reg = register_manager_.acquire_fp_scratch_reg();  // ❌ PROBLEM!
    } else {
        temp_reg = register_manager_.acquire_scratch_reg(*this); // ❌ PROBLEM!
    }
    
    arg_result_regs.push_back({temp_reg, true});
}
```

### Why This Fails

#### Problem 1: Register Pool Exhaustion

ARM64 has only **7 scratch registers**: X9, X10, X11, X12, X13, X14, X15

**Failure Scenario:**
```
Function call: foo(a, b, c, d, e, f, g, h, i)  // 9 arguments

Step 1: Evaluate a → result in X9, copy to X10 (acquired scratch)
Step 2: Evaluate b → result in X11, copy to X12 (acquired scratch)  
Step 3: Evaluate c → result in X13, copy to X14 (acquired scratch)
Step 4: Evaluate d → result in X15, copy to ??? (acquired scratch)
Step 5: Evaluate e → CRASH! No more scratch registers available
```

The compiler will crash with "Unable to acquire scratch register" when processing the 8th argument.

#### Problem 2: Clobbering Still Occurs

Even worse, the "protection" doesn't actually work:

```
Function call: foo(simple_var, complex_expression())

Step 1: Evaluate simple_var → result in X9, copy to X10
Step 2: Evaluate complex_expression():
        - This calls generate_expression_code() 
        - generate_expression_code() acquires scratch register X9
        - X9 now contains intermediate results from complex_expression()
        - But we THOUGHT simple_var was "protected" in X10!
        - Actually, complex_expression() might acquire X10 too!
        - Result: simple_var value is CLOBBERED
```

The fundamental issue is that `generate_expression_code()` can acquire **any** scratch register, including ones we're using to "protect" previous arguments.

### Evidence in Codebase

#### File: `generators/gen_FunctionCall.cpp` (Lines 24-45)
Shows the flawed pattern of acquiring scratch registers for each argument.

#### File: `generators/helpers/gen_coerce_to_arg.cpp` (Lines 85-220)
The `coerce_arguments_to_abi()` function correctly handles the final placement, but it receives already-clobbered registers.

#### File: `NewCodeGenerator.cpp` (Various locations)
Multiple instances of `acquire_scratch_reg()` and `acquire_fp_scratch_reg()` showing heavy scratch register usage throughout expression evaluation.

## Impact Assessment

### Current Limitations
- **Maximum 7 arguments** per function call (will crash beyond this)
- **Argument clobbering** in complex expressions leading to wrong results  
- **Unreliable behavior** that's difficult to debug
- **Silent data corruption** when clobbering occurs

### Affected Code Paths
1. `gen_FunctionCall.cpp` - Direct function calls
2. `gen_RoutineCallStatement.cpp` - BCPL routine calls  
3. `gen_Syscall.cpp` - System calls
4. Method calls through `handle_method_call()`
5. Built-in function calls (WRITEF, LEN, etc.)

## Industry Standard Solution

The robust solution used by GCC, Clang, and other production compilers is **right-to-left evaluation with stack storage**:

### Algorithm

```cpp
// CORRECT APPROACH - Industry Standard
void evaluate_arguments_robust(const std::vector<Expression*>& args) {
    // Step 1: Pre-allocate stack space
    size_t total_bytes = args.size() * 8;  // 8 bytes per argument
    emit(SUB SP, SP, #total_bytes);
    
    // Step 2: Evaluate RIGHT-TO-LEFT (reverse order)
    for (int i = args.size() - 1; i >= 0; --i) {
        generate_expression_code(*args[i]);  // Uses scratch registers freely
        
        // Step 3: Immediately store to stack (frees all scratch registers)
        size_t offset = i * 8;
        if (is_float_result()) {
            emit(STR result_reg, [SP, #offset]);  // Store float to stack
        } else {
            emit(STR result_reg, [SP, #offset]);  // Store int to stack  
        }
        
        release_register(result_reg);  // All scratch registers now free
    }
    
    // Step 4: Load from stack to ABI registers when ready to call
    for (int i = 0; i < args.size(); ++i) {
        size_t offset = i * 8;
        std::string abi_reg = get_abi_register(i);
        emit(LDR abi_reg, [SP, #offset]);
    }
    
    // Step 5: Make the call
    emit(BL function_name);
    
    // Step 6: Clean up stack
    emit(ADD SP, SP, #total_bytes);
}
```

### Why This Works

1. **O(1) Register Pressure**: Only uses scratch registers during evaluation of one argument at a time
2. **Clobber-Proof**: Each argument is safely stored on stack before evaluating the next
3. **Scalable**: Works with unlimited arguments (stack space permitting)  
4. **Standard**: Used by all major compilers
5. **Debugger-Friendly**: Arguments remain accessible on stack during debugging

## Recommended Implementation Plan

### Phase 1: Create ArgumentEvaluator Class (STARTED)
✅ `ArgumentEvaluator.h` - Interface defined
✅ `ArgumentEvaluator.cpp` - Implementation in progress

### Phase 2: Integrate with Function Calls
- Modify `gen_FunctionCall.cpp` to use ArgumentEvaluator
- Update `gen_RoutineCallStatement.cpp`  
- Update `gen_Syscall.cpp`
- Handle method calls and built-ins

### Phase 3: Testing and Validation
- Test with 10+ argument functions
- Test with complex nested expressions
- Verify no register pressure issues
- Performance benchmarking

### Phase 4: Cleanup  
- Remove old flawed code paths
- Update documentation
- Add regression tests

## Risk Assessment

### High Priority Fix Required
This is a **critical architectural flaw** that should be fixed immediately because:

1. **Crashes**: Functions with 8+ arguments will crash the compiler
2. **Silent Corruption**: Complex expressions may produce wrong results  
3. **Development Blocker**: Prevents writing complex BCPL programs
4. **Debugging Nightmare**: Hard to trace when clobbering occurs

### Backward Compatibility
The fix is transparent to user code - existing BCPL programs will work correctly without changes.

## Performance Impact

### Stack-Based Approach Benefits
- **Consistent Performance**: No dependency on scratch register availability
- **Better Cache Locality**: Stack accesses are cache-friendly
- **Reduced Register Pressure**: Frees scratch registers for other optimizations

### Benchmark Expectations
- **Minimal Overhead**: 2-3 extra instructions per argument (SUB/STR/LDR/ADD)
- **Better Reliability**: Eliminates crashes and wrong results
- **Net Positive**: More reliable code execution outweighs minimal overhead

## Conclusion

The current scratch register strategy is fundamentally broken and must be replaced with the industry-standard stack-based approach. This is not a nice-to-have optimization - it's a critical bug fix that prevents the compiler from handling real-world code correctly.

The proposed ArgumentEvaluator class provides a clean, robust solution that will eliminate crashes, prevent argument clobbering, and allow the compiler to handle functions with any number of arguments reliably.

**Recommendation: Implement immediately as highest priority bug fix.**
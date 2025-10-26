# Function Argument Optimization Plan

## Executive Summary
Optimize function call parameter handling by using direct register allocation for functions with ≤3 arguments, while maintaining the current stack-based approach for functions with more arguments. This will reduce overhead and improve performance for the majority of function calls.

## Current State Analysis

### Current Implementation (Stack-Based for All Calls)
```cpp
// Current approach for ALL function calls:
1. Pre-allocate stack space (args * 8 bytes, 16-byte aligned)
2. Evaluate arguments right-to-left
3. Store each argument result immediately to stack
4. Release scratch registers after each store
5. Load arguments from stack into ABI registers (X0-X7)
6. Clean up stack space
```

### Performance Overhead
- **Memory operations**: 2 memory ops per argument (store + load)
- **Stack management**: SP adjustment, alignment calculations
- **Register churn**: Acquire/release scratch registers multiple times

### Resources Available
- **Scratch Registers**: X9, X10, X11, X12, X13, X14, X15 (7 total)
- **ARM64 ABI Registers**: X0-X7 (8 argument registers)
- **Method Call Constraint**: X0 reserved for 'this' → X1-X7 usable (7 args max)

## Optimization Strategy

### Threshold Analysis
**≤3 Arguments**: Safe for register-direct approach
- Worst case: 3 args + 2-3 intermediate expressions = ~6 registers needed
- Available: 7 scratch registers
- Safety margin: 1 register for complex expressions

**4+ Arguments**: Keep stack-based approach
- Register pressure too high for reliable direct allocation
- Risk of register exhaustion during complex expression evaluation

### Implementation Plan

#### Phase 1: Add Register-Direct Path

**Location**: `generators/gen_RoutineCallStatement.cpp`

**New Logic Flow**:
```cpp
if (node.arguments.size() <= 3) {
    // NEW: Register-direct approach
    use_register_direct_argument_handling();
} else {
    // EXISTING: Stack-based approach (unchanged)
    use_stack_based_argument_handling();
}
```

#### Phase 1.1: Register-Direct Implementation

**For Method Calls** (X0 = this, X1-X3 = args):
```cpp
std::vector<std::string> arg_regs;
for (size_t i = 0; i < node.arguments.size(); ++i) {
    // Evaluate argument expression
    generate_expression_code(*node.arguments[i]);
    
    // Move result directly to ABI register
    std::string abi_reg = "X" + std::to_string(i + 1);
    if (expression_result_reg_ != abi_reg) {
        emit(Encoder::create_mov_reg(abi_reg, expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
    }
    arg_regs.push_back(abi_reg);
}
```

**For Regular Calls** (X0-X2 = args):
```cpp
std::vector<std::string> arg_regs;
for (size_t i = 0; i < node.arguments.size(); ++i) {
    generate_expression_code(*node.arguments[i]);
    
    std::string abi_reg = "X" + std::to_string(i);
    if (expression_result_reg_ != abi_reg) {
        emit(Encoder::create_mov_reg(abi_reg, expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
    }
    arg_regs.push_back(abi_reg);
}
```

#### Phase 1.2: Safety Mechanisms

**Register Pressure Detection**:
```cpp
bool can_use_register_direct() {
    int available_scratch = count_free_scratch_registers();
    int estimated_need = node.arguments.size() + 2; // +2 for intermediate values
    return available_scratch >= estimated_need;
}
```

**Fallback Strategy**:
```cpp
if (node.arguments.size() <= 3 && can_use_register_direct()) {
    use_register_direct_argument_handling();
} else {
    // Fall back to stack-based for safety
    use_stack_based_argument_handling();
}
```

#### Phase 1.3: Code Structure

**New Functions to Add**:
```cpp
// In NewCodeGenerator class
private:
    void handle_method_call_arguments_direct(RoutineCallStatement& node);
    void handle_regular_call_arguments_direct(RoutineCallStatement& node);
    void handle_super_call_arguments_direct(RoutineCallStatement& node);
    bool can_use_register_direct_arguments(size_t arg_count);
    int count_available_scratch_registers();
```

### Phase 2: Performance Validation

#### Benchmarking Plan
1. **Micro-benchmarks**: Simple function calls with 1-3 arguments
2. **Real workloads**: Existing test suite performance comparison
3. **Register pressure analysis**: Ensure no register exhaustion

#### Success Metrics
- **Performance**: 15-25% improvement for small function calls
- **Reliability**: No register allocation failures
- **Compatibility**: All existing tests pass

### Phase 3: Advanced Optimizations (Future)

#### Argument Evaluation Order Optimization
- Analyze argument complexity to determine optimal evaluation order
- Evaluate simple arguments (constants, variables) last to minimize register pressure

#### Smart Register Allocation
- Use argument registers directly as scratch registers when possible
- Implement register coalescing for argument preparation

#### Type-Specific Optimizations
- Separate handling for FP arguments (D0-D7 registers)
- Vector argument optimizations

## Implementation Risks & Mitigations

### Risk 1: Register Exhaustion
**Mitigation**: Conservative threshold (≤3 args) + runtime fallback to stack

### Risk 2: Complex Expression Handling
**Mitigation**: Pre-flight check for available scratch registers before starting

### Risk 3: ABI Compliance
**Mitigation**: Extensive testing with different argument types and combinations

### Risk 4: Debugging Complexity
**Mitigation**: Add debug flags to force stack-based mode for troubleshooting

## Testing Strategy

### Unit Tests
```cpp
// Test cases to add:
- Function calls with 0-3 arguments (register-direct path)
- Function calls with 4+ arguments (stack-based path)
- Mixed argument types (int, float, objects)
- Complex expressions as arguments
- Nested function calls
- Method calls vs regular calls vs super calls
```

### Integration Tests
- Run full test suite with new optimization enabled
- Performance regression tests
- Memory usage validation

### Stress Tests
- High register pressure scenarios
- Deep call stacks
- Complex arithmetic expressions as arguments

## Expected Benefits

### Performance Improvements
- **1-3 argument calls**: 15-25% faster (eliminate memory operations)
- **Register utilization**: Better scratch register turnover
- **Cache efficiency**: Reduced memory traffic

### Code Quality
- **Simpler generated code**: Direct register moves instead of stack operations
- **Better optimization opportunities**: Register allocator can see direct data flow
- **Reduced stack frame sizes**: No temporary argument storage needed

## Timeline & Dependencies

### Phase 1 Implementation: 2-3 days
- Day 1: Implement register-direct path for method calls
- Day 2: Add regular calls and super calls support
- Day 3: Add safety mechanisms and fallback logic

### Phase 2 Validation: 1-2 days
- Performance benchmarking
- Comprehensive testing
- Bug fixes and refinements

### Total Timeline: 4-5 days for complete implementation and validation

## Compatibility Notes

- **Backward Compatible**: Existing stack-based logic remains unchanged
- **Configurable**: Can add compile flag to disable optimization if needed
- **Gradual Rollout**: Can adjust threshold (≤2, ≤3, ≤4) based on testing results

## Monitoring & Rollback Plan

### Success Metrics to Monitor
1. Function call performance (should improve 15-25%)
2. Register allocation failure rate (should remain 0%)
3. Overall compilation time (should remain same or improve)
4. Test suite pass rate (must remain 100%)

### Rollback Strategy
If issues arise, can quickly disable optimization by:
```cpp
// Emergency rollback - force all calls to use stack-based approach
const bool ENABLE_REGISTER_DIRECT_OPTIMIZATION = false;
```

This provides immediate fallback to proven stable implementation while issues are resolved.
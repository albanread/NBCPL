# BCPL Compiler Register Pressure Optimization: Problems and Issues

## Overview

This document details the implementation of register pressure optimizations for FOREACH loops in the BCPL compiler, the problems encountered during development, and the current status of various optimization strategies.

## Background: The Register Pressure Problem

The original BCPL compiler's FOREACH loop implementation suffered from excessive register pressure due to:

1. **Redundant Temporary Variables**: Every FOREACH loop created 3 temporary variables:
   - `_forEach_vec_*`: Collection pointer (even when iterating over simple variables)
   - `_forEach_len_*`: Collection length (even for constant-size collections)
   - `_forEach_idx_*`: Loop index counter

2. **Nested Loop Multiplication**: In nested FOREACH loops, temporaries from outer loops remained live throughout inner loops, causing cumulative register pressure

3. **No Temporary Reuse**: Sequential loops created unique temporaries instead of reusing names

4. **Poor Spill Decisions**: The register allocator treated all variables equally, potentially spilling critical loop counters

## Implemented Optimizations

### 1. Collection Variable Reuse (PARTIALLY IMPLEMENTED)

**Goal**: Eliminate redundant `_forEach_vec_*` temporaries when the collection expression is already a simple variable.

**Implementation Status**: ✅ CODED, ❌ TESTED
- Added `is_simple_variable_access()` helper method in `CFGBuilderPass.cpp`
- Modified `build_vector_foreach_cfg()` to detect simple variable access
- When detected, reuses original variable instead of creating temporary

**Code Location**: `CFGBuilderPass.cpp:81-90` and `CFGBuilderPass.cpp:1142-1300`

**Current Issues**: 
- Implementation caused compilation errors and runtime crashes
- Reverted temporarily to maintain stability
- Need to investigate impact on symbol table registration

### 2. Immediate Comparison Optimization (PARTIALLY IMPLEMENTED)

**Goal**: Eliminate `_forEach_len_*` temporaries for constant-size collections by using immediate comparison instructions.

**Implementation Status**: ✅ CODED, ❌ TESTED
- Added `is_constant_size_collection()` helper method
- Supports detection of:
  - `VecInitializerExpression` (VEC [1,2,3] syntax)
  - `TableExpression` (TABLE 1,2,3 syntax)
  - `VecAllocationExpression` with constant size (VEC 5)
  - `FVecAllocationExpression` with constant size (FVEC 3)

**Code Location**: `CFGBuilderPass.cpp:85-116`

**Current Issues**:
- ARM64 immediate range limitations (≤255)
- Need to verify ARM64 instruction encoding supports immediate comparisons
- Not yet integrated due to compilation issues

### 3. Vector Bracket Syntax Support (IMPLEMENTED)

**Goal**: Add `VEC [1,2,3]` syntax support to improve constant size detection.

**Implementation Status**: ✅ IMPLEMENTED, ❌ TESTED
- Added `LBracket` and `RBracket` tokens to `TokenType` enum
- Updated lexer (`lex_operator.cpp`) to recognize `[` and `]` characters
- Added `parse_vec_bracket_initializer_expression()` method to parser
- Updated token string conversion in `lex_utils.cpp`

**Code Locations**:
- `Lexer.h:60` - Token enum
- `lex_operator.cpp:22-23` - Character recognition  
- `lex_utils.cpp:48-49` - String conversion
- `pz_parse_expressions.cpp:414-417` - Parser logic
- `pz_parse_expressions.cpp:494-516` - Bracket syntax parser

**Current Issues**:
- Not yet tested due to other compilation issues
- May need AST visitor updates for new syntax

### 4. Loop-Aware Register Allocator (IMPLEMENTED BUT DISABLED)

**Goal**: Priority-based register allocation that protects loop-critical variables from spilling.

**Implementation Status**: ✅ IMPLEMENTED, ❌ INTEGRATED
- Created `LoopAwareAllocator` class with priority-based spilling
- Variable priority levels: `LOOP_COUNTER > LOOP_LENGTH > COLLECTION_POINTER > FOREACH_TEMP > NORMAL`
- Dedicated register support for loop counters (`X16`, `X17`)
- Smart spill candidate selection

**Code Location**: `analysis/LoopAwareAllocator.h` and `analysis/LoopAwareAllocator.cpp` (REMOVED due to compilation issues)

**Current Issues**:
- Compilation errors with `LiveInterval` equality operator
- Const-correctness issues with priority caching
- Need to properly integrate with existing `LinearScanAllocator`

### 5. Temporary Variable Reuse (PARTIALLY IMPLEMENTED)

**Goal**: Reuse temporary variable names across sequential (non-overlapping) FOREACH loops.

**Implementation Status**: ✅ CODED, ❌ INTEGRATED
- Added tracking structures: `available_temp_names_`, `active_foreach_temps_`
- Implemented `get_reusable_temp_name()`, `mark_temp_name_available()`, `release_foreach_temporaries()`
- Added cleanup logic in `visit(ForEachStatement)`

**Current Issues**:
- Methods temporarily removed due to compilation crashes
- Need to ensure proper lifetime management
- Requires careful analysis of variable liveness

## Problems Encountered

### 1. Compilation Errors

**NumberLiteral Value Access**:
```cpp
// ERROR: 'NumberLiteral' has no member named 'value'
size = static_cast<int>(num_literal->value);

// FIXED: Use correct member name
size = static_cast<int>(num_literal->int_value);
```

**LiveInterval Equality Operator**:
```cpp
// ERROR: No equality operator for std::list::remove()
active_intervals_.remove(*best_candidate);

// FIXED: Added operator== to LiveInterval struct
bool operator==(const LiveInterval& other) const {
    return var_name == other.var_name && 
           start_point == other.start_point && 
           end_point == other.end_point;
}
```

**Const-Correctness in Priority Cache**:
```cpp
// ERROR: Cannot modify const map
variable_priority_cache_[interval.var_name] = priority;

// FIXED: Made cache mutable or removed const from method
mutable std::unordered_map<std::string, int> variable_priority_cache_;
```

### 2. Runtime Issues

**Compiler Crashes/Hangs**:
- Programs hang indefinitely after "SDL2 runtime functions registered"
- Minimal programs crash with "Killed: 9" signal
- Issue appears to be related to optimization code changes

**Variable Registration Issues**:
```
NewBCPL Compiler Error: Cannot store to variable 'val': not found in allocation map or as global.
```
- Loop variables not properly registered in symbol table
- May be related to optimization changes affecting variable lifetime

**WRITEF Format String Errors**:
```
NewBCPL Compiler Error: WRITEF format string expects 0 arguments but 1 provided
```
- Runtime format string validation too strict
- Unrelated to optimizations but affects testing

### 3. ARM64 Instruction Encoding Issues

**STP Instruction Error**:
```
NewBCPL Compiler Error: Invalid immediate for STP pre-index. Must be a multiple of 8 in range [-512, 504].
```
- ARM64 instruction encoding constraints
- May be related to stack frame management changes
- Not directly caused by optimizations but exposed during testing

## Current Status and Recommendations

### Immediate Actions Needed

1. **Restore Basic Functionality**:
   - Revert all optimization changes temporarily
   - Ensure basic FOREACH loops work without crashes
   - Re-enable testing with known working examples

2. **Fix Compilation Issues**:
   - Address `NumberLiteral` member access
   - Fix `LiveInterval` equality operator
   - Resolve const-correctness issues

3. **Test Vector Bracket Syntax**:
   - Verify `VEC [1,2,3]` syntax parsing works
   - Test with simple examples before complex optimizations

### Optimization Implementation Strategy

**Phase 1: Foundation (PRIORITY: HIGH)**
1. Ensure basic FOREACH functionality is stable
2. Add comprehensive test suite for existing FOREACH behavior
3. Fix ARM64 instruction encoding issues

**Phase 2: Simple Optimizations (PRIORITY: MEDIUM)**
1. Implement collection variable reuse optimization carefully
2. Add constant size detection for immediate comparisons
3. Test each optimization in isolation

**Phase 3: Advanced Optimizations (PRIORITY: LOW)**
1. Re-implement `LoopAwareAllocator` with proper integration
2. Add temporary variable reuse for sequential loops
3. Implement register allocation enhancements

### Test Strategy

**Regression Testing**:
- Test all existing FOREACH functionality before adding optimizations
- Use minimal test cases to isolate issues
- Verify compilation and execution separately

**Optimization Testing**:
- Create specific tests for each optimization
- Measure register pressure before and after
- Verify performance improvements without correctness regression

## Code Quality Issues Identified

### 1. Error Handling
- Compilation errors not immediately visible during development
- Need better error reporting for optimization failures
- Missing validation for ARM64 instruction constraints

### 2. Testing Infrastructure
- Limited regression testing for FOREACH functionality
- No automated performance testing for register pressure
- Missing integration tests for optimization combinations

### 3. Code Organization
- Optimization logic mixed with core functionality
- Need cleaner separation between original and optimized paths
- Better modularization of helper methods

## Lessons Learned

### 1. Incremental Development
- Always test basic functionality before adding optimizations
- Implement one optimization at a time with full testing
- Maintain working baseline throughout development

### 2. ARM64 Constraints
- ARM64 has specific instruction encoding requirements
- Immediate value ranges are limited
- Stack alignment and addressing modes have strict rules

### 3. Compiler Pipeline Dependencies
- Changes to symbol registration affect multiple compiler passes
- Variable lifetime management is complex
- Register allocation depends on accurate symbol table information

## Future Work

### Short Term
1. Fix basic FOREACH functionality
2. Add bracket syntax support for vectors
3. Implement simple collection variable reuse

### Medium Term
1. Add immediate comparison optimization for small constants
2. Implement loop-aware register allocation
3. Add comprehensive performance testing

### Long Term
1. Profile-guided optimization for register allocation
2. SIMD optimization opportunities for vector operations
3. Global optimization across function boundaries

## Implementation Files Modified

### Core Files
- `CFGBuilderPass.h/.cpp` - FOREACH loop optimization logic
- `analysis/LinearScanAllocator.h/.cpp` - Register allocation (baseline)

### Lexer/Parser Files
- `Lexer.h` - Added bracket tokens
- `lex_operator.cpp` - Bracket character recognition
- `lex_utils.cpp` - Token string conversion
- `pz_parse_expressions.cpp` - Vector bracket syntax parsing
- `Parser.h` - Method declarations

### Test Files
- `test_register_pressure.bcl` - Register pressure demonstration
- `test_register_pressure_optimized.bcl` - Optimization test cases
- `test_vector_syntax.bcl` - Bracket syntax testing
- `test_basic_old_syntax.bcl` - Regression testing

### Removed Files
- `analysis/LoopAwareAllocator.h/.cpp` - Removed due to compilation issues

## Conclusion

The register pressure optimization project identified significant opportunities for improvement in the BCPL compiler's FOREACH loop implementation. While the optimizations were successfully designed and partially implemented, several technical challenges prevented complete integration:

1. **Compilation Issues**: Type system inconsistencies and const-correctness problems
2. **Runtime Stability**: Optimization changes caused crashes and hangs
3. **ARM64 Constraints**: Instruction encoding limitations not fully considered

The foundation for these optimizations has been laid, and the analysis provides a clear roadmap for future implementation. The key insight is that register pressure can be significantly reduced through careful temporary variable management, but this must be done incrementally with thorough testing at each step.

**Recommended Next Steps**:
1. Restore basic functionality and ensure stability
2. Implement optimizations one at a time with comprehensive testing
3. Focus on the highest-impact optimizations first (collection variable reuse)
4. Consider alternative approaches that don't require extensive changes to core compiler infrastructure

The potential benefits remain significant: 33-67% reduction in temporary variables for FOREACH loops, which translates directly to reduced register pressure and improved performance in loop-intensive BCPL programs.
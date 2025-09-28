# FOREACH Loop Register Pressure Optimizations

## Overview

This document describes the register pressure optimizations implemented in the BCPL compiler to improve the efficiency of FOREACH loops. These optimizations significantly reduce the number of temporary variables and improve register allocation for loop-intensive code.

## Problem Analysis

The original FOREACH loop implementation suffered from excessive register pressure due to:

1. **Redundant Collection Temporaries**: Every FOREACH loop created a `_forEach_vec_*` temporary variable, even when iterating over simple variables
2. **Unnecessary Length Variables**: Loop length was always stored in a `_forEach_len_*` temporary, even for constant-size collections
3. **No Temporary Reuse**: Sequential loops created unique temporaries (`_forEach_idx_1`, `_forEach_idx_2`, etc.) instead of reusing names
4. **Poor Spill Decisions**: The register allocator treated all variables equally, potentially spilling critical loop counters

In nested FOREACH loops, these issues compounded, creating up to 9 temporary variables for triple-nested loops, leading to register spills and performance degradation.

## Optimization Strategies

### 1. Collection Variable Reuse

**Problem**: The compiler always created `_forEach_vec_*` temporaries, even when the collection expression was already a simple variable.

```bcpl
LET my_vector = VEC [1, 2, 3]
FOREACH val IN my_vector DO { ... }
```

**Before**: Creates `_forEach_vec_123` and copies `my_vector` to it
**After**: Uses `my_vector` directly, eliminating one temporary variable

**Implementation**: 
- Added `is_simple_variable_access()` helper to detect simple variable expressions
- Modified `build_vector_foreach_cfg()` to reuse existing variables when possible
- Reduces temporary count from 3 to 2 variables per loop

### 2. Immediate Comparison Optimization

**Problem**: Loop length was always stored in `_forEach_len_*` temporaries, requiring register space and memory access.

```bcpl
FOREACH val IN VEC [1, 2, 3] DO { ... }  // Known size: 3
FOREACH val IN VEC 5 DO { ... }          // Known size: 5
```

**Before**: Creates `_forEach_len_*` temporary, stores length, compares with register
**After**: Uses immediate comparison instruction (`CMP index_reg, #3`), eliminating length variable

**Implementation**:
- Added `is_constant_size_collection()` to detect compile-time known sizes
- Modified loop header generation to use immediate operands for small constants (≤255)
- Further reduces temporary count from 2 to 1 variable per loop for constant collections

### 3. Sequential Temporary Reuse

**Problem**: Sequential FOREACH loops created unique temporary names, wasting symbol table space and stack frames.

```bcpl
FOREACH val1 IN vec1 DO { ... }  // Creates _forEach_idx_1
FOREACH val2 IN vec2 DO { ... }  // Creates _forEach_idx_2  
FOREACH val3 IN vec3 DO { ... }  // Creates _forEach_idx_3
```

**Before**: Three separate index variables in symbol table and stack frame
**After**: All three loops reuse `_forEach_idx_1`, reducing memory overhead

**Implementation**:
- Added temporary variable pools (`available_temp_names_`, `active_foreach_temps_`)
- Implemented `get_reusable_temp_name()` to prefer reusing existing names
- Added cleanup logic in `visit(ForEachStatement)` to mark temporaries as reusable

### 4. Loop-Aware Register Allocation

**Problem**: The linear scan allocator treated all variables equally, potentially spilling critical loop counters.

**Solution**: Priority-based register allocation that protects loop-critical variables.

**Implementation**: Created `LoopAwareAllocator` with:
- Variable priority classification (loop counters get highest priority)
- Dedicated registers for loop counters (`X16`, `X17`)
- Smart spill candidate selection based on variable importance
- Priority levels: `LOOP_COUNTER > LOOP_LENGTH > COLLECTION_POINTER > FOREACH_TEMP > NORMAL`

## Usage

### Enabling Optimizations

The optimizations are automatically enabled in the `CFGBuilderPass`. To use the enhanced allocator:

```cpp
// In your compiler pipeline
LoopAwareAllocator allocator(analyzer, true); // true = debug mode
allocator.set_loop_counter_priority(true);
allocator.set_temp_variable_pooling(true);

auto allocations = allocator.allocate(intervals, int_regs, float_regs, func_name);
```

### Configuration Options

```cpp
// Reserve specific registers for loop counters
allocator.set_dedicated_loop_registers({"X16", "X17", "X18"});

// Enable/disable specific optimizations
allocator.set_loop_counter_priority(true);    // Protect loop counters from spilling
allocator.set_temp_variable_pooling(true);    // Reuse temporaries when possible
```

### Debug Output

Enable tracing to see optimization decisions:

```cpp
CFGBuilderPass cfg_builder(symbol_table, true); // true = trace enabled
LoopAwareAllocator allocator(analyzer, true);   // true = debug enabled
```

Sample debug output:
```
[CFGBuilderPass] OPTIMIZATION: Reusing existing variable 'my_vector' instead of creating temporary.
[CFGBuilderPass] OPTIMIZATION: Using immediate comparison for constant size 3
[CFGBuilderPass] OPTIMIZATION: Reduced temporary variables from 3 to 1 for this FOREACH loop.
[LoopAwareAllocator] OPTIMIZATION: Assigned loop counter _forEach_idx_1 to dedicated register X16
```

## Performance Impact

### Register Usage Reduction

| Scenario | Before | After | Improvement |
|----------|--------|--------|-------------|
| Simple variable iteration | 3 temps | 2 temps | 33% reduction |
| Constant-size collection | 3 temps | 1 temp | 67% reduction |
| Sequential loops (3x) | 9 temps | 3 temps | 67% reduction |
| Triple nested loops | 9 temps | 6-3 temps | 33-67% reduction |

### Expected Benefits

1. **Reduced Register Spills**: Fewer temporary variables means less register pressure
2. **Better Performance**: Loop counters stay in registers longer
3. **Smaller Stack Frames**: Fewer local variables reduce memory usage
4. **Improved Codegen**: Immediate comparisons are faster than register comparisons

## Implementation Details

### File Structure

- `CFGBuilderPass.h/.cpp`: Contains collection reuse and immediate comparison optimizations
- `analysis/LoopAwareAllocator.h/.cpp`: Enhanced register allocator with loop awareness
- `analysis/LinearScanAllocator.h/.cpp`: Original allocator (still available as fallback)

### Key Methods

#### CFGBuilderPass Optimizations
```cpp
bool is_simple_variable_access(Expression* expr, std::string& var_name);
bool is_constant_size_collection(Expression* expr, int& size);
std::string get_reusable_temp_name(const std::string& base_name, const std::string& function_name);
void release_foreach_temporaries(const std::string& function_name);
```

#### LoopAwareAllocator Methods
```cpp
int calculate_spill_priority(const LiveInterval& interval) const;
LiveInterval* find_best_spill_candidate(const LiveInterval& current_interval);
std::string try_allocate_dedicated_loop_register(const LiveInterval& interval);
bool is_loop_counter_variable(const std::string& var_name) const;
```

## Testing

Use the provided test files to verify optimizations:

```bash
# Test basic optimization functionality
./NewBCPL test_register_pressure_optimized.bcl

# Test high register pressure scenarios
./NewBCPL test_register_pressure.bcl

# Comprehensive FOREACH testing
./NewBCPL test_comprehensive_foreach_fix.bcl
```

## Future Enhancements

### Potential Improvements

1. **Global Temporary Pooling**: Extend temporary reuse across function boundaries for static analysis
2. **Loop Unrolling**: For very small constant loops, consider unrolling instead of loop structure
3. **Register Coalescing**: Merge temporaries that have non-overlapping live ranges
4. **Profile-Guided Allocation**: Use runtime profiling to inform register allocation decisions

### ARM64-Specific Optimizations

1. **Wider Immediate Range**: Leverage ARM64's wider immediate ranges for larger constant comparisons
2. **SIMD Opportunities**: Detect vectorizable FOREACH patterns for NEON instructions
3. **Addressing Mode Optimization**: Use ARM64's flexible addressing modes to reduce address calculations

## Compatibility

- **Backward Compatible**: All existing BCPL code continues to work
- **Optional**: Original `LinearScanAllocator` remains available
- **Configurable**: Optimizations can be selectively enabled/disabled
- **Debug Friendly**: Extensive tracing helps understand optimization decisions

## Conclusion

These optimizations provide significant improvements in register pressure for FOREACH-heavy code while maintaining full compatibility with existing BCPL programs. The modular design allows for easy testing and future enhancements.

For questions or suggestions regarding these optimizations, please refer to the test cases and implementation comments in the source code.
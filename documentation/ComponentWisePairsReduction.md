# Component-wise PAIRS Reduction with NEON Optimization

## Overview

The component-wise PAIRS reduction system extends NewBCPL's modular reducer framework to efficiently handle collections of PAIRS using ARM NEON SIMD instructions. This allows developers to perform reductions on coordinate data, complex numbers, or any paired values with automatic SIMD optimization.

## Key Concepts

### Component-wise vs. Pairwise Operations

- **Component-wise**: Reduces each component independently across all pairs
  - `MIN([{x:1, y:10}, {x:5, y:2}, {x:3, y:8}])` → `{x:1, y:2}`
  - X components: `MIN(1, 5, 3) = 1`
  - Y components: `MIN(10, 2, 8) = 2`

- **Pairwise**: Operates on adjacent elements within vectors
  - `PAIRWISE_ADD([1, 5, 3, 7])` → `[6, 10]` (horizontal operations)

### Memory Layout Challenge

PAIRS collections are stored as Array of Structures (AoS):
```
Memory: [x1, y1, x2, y2, x3, y3, x4, y4, ...]
```

NEON operates efficiently on Structure of Arrays (SoA):
```
X Vector: [x1, x2, x3, x4]
Y Vector: [y1, y2, y3, y4]
```

## NEON Optimization Strategy

### 1. De-interleaving with VLD2

The system uses ARM NEON's `VLD2` instruction to automatically separate interleaved data:

```assembly
; Load 4 pairs (8 values) and de-interleave
vld2.32 {d0, d1}, [r0]!    ; d0 = [x1,x2,x3,x4], d1 = [y1,y2,y3,y4]
```

### 2. Vectorized Reduction

Process 4 pairs simultaneously using NEON vector instructions:

```assembly
; For MIN reduction
vmin.s32 d0, d0, d2        ; X components: min(current_x, new_x)
vmin.s32 d1, d1, d3        ; Y components: min(current_y, new_y)
```

### 3. Horizontal Reduction

Collapse vector accumulators to final scalar values:

```assembly
; Reduce X vector to single minimum
vpmin.s32 d4, d0, d0       ; Pairwise min within X vector
vpmin.s32 d4, d4, d4       ; Final X minimum

; Reduce Y vector to single minimum  
vpmin.s32 d5, d1, d1       ; Pairwise min within Y vector
vpmin.s32 d5, d5, d5       ; Final Y minimum
```

## Developer Experience

### Simple Usage

```bcpl
LET coordinates = PAIRS 8

// Initialize with coordinate data
coordinates!0 := PAIR(10, 50)
coordinates!1 := PAIR(25, 30)
coordinates!2 := PAIR(5, 80)
coordinates!3 := PAIR(40, 15)
// ... more pairs

// Component-wise reductions (automatically NEON-optimized)
LET min_point = MIN(coordinates)    ; Uses MinReducer
LET max_point = MAX(coordinates)    ; Uses MaxReducer  
LET sum_point = SUM(coordinates)    ; Uses SumReducer
```

### No SIMD Knowledge Required

Developers write simple, clean code. The compiler automatically:
- Detects PAIRS type collections
- Generates NEON-optimized loops
- Handles edge cases (collections < 4 elements)
- Falls back to scalar processing for remainders

## Implementation Architecture

### 1. Type Detection

`CFGBuilderPass::isPairsType()` identifies when reductions operate on PAIRS:

```cpp
bool CFGBuilderPass::isPairsType(Expression* expr) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(expr)) {
        Symbol symbol;
        if (symbol_table_->lookup(var_access->name, symbol)) {
            return symbol.type == VarType::PAIRS;
        }
    }
    return false;
}
```

### 2. Dispatch to Specialized CFG Generation

When PAIRS type detected, use component-wise path:

```cpp
void CFGBuilderPass::generateReductionCFG(Expression* left_expr, Expression* right_expr,
                                         const std::string& result_var, const Reducer& reducer) {
    if (isPairsType(left_expr)) {
        generateComponentWiseReductionCFG(left_expr, right_expr, result_var, reducer);
    } else {
        // Regular scalar reduction
        generateReductionCFG(left_expr, right_expr, result_var, reducer.getReductionOp());
    }
}
```

### 3. NEON-Optimized CFG Structure

The system generates a sophisticated CFG with multiple optimization phases:

```
PreHeader
    ↓
[Initialize NEON accumulators with reducer initial values]
    ↓
VectorHeader ←─────────┐
    ↓                  │
VectorBody             │
[VLD2 + Vector Ops]    │
    ↓                  │
VectorIncrement ───────┘
    ↓
RemainderHeader ←──────┐
    ↓                  │
RemainderBody          │
[Scalar Fallback]      │
    ↓                  │
RemainderIncrement ────┘
    ↓
HorizontalReduction
[VPMIN/VPMAX/VPADD to collapse vectors]
    ↓
Exit
```

### 4. Reducer Integration

The existing reducers work seamlessly:

- **MinReducer**: Provides `BinaryOp::Less` → generates `VMIN` instructions
- **MaxReducer**: Provides `BinaryOp::Greater` → generates `VMAX` instructions  
- **SumReducer**: Provides `BinaryOp::Add` → generates `VADD` instructions

No changes needed to reducer classes!

## Performance Benefits

### SIMD Throughput

- **Scalar**: 1 pair per instruction cycle
- **NEON**: 4 pairs per instruction cycle
- **Theoretical speedup**: 4x for large collections

### Memory Efficiency

- **VLD2**: Single instruction loads and de-interleaves 8 values
- **Cache-friendly**: Sequential memory access pattern
- **Reduced instruction count**: Vectorized operations

### Automatic Optimization

- **Large collections**: Full NEON optimization
- **Small collections**: Automatic scalar fallback
- **Mixed sizes**: Vectorized main loop + scalar remainder
- **Edge cases**: Handled transparently

## Code Generation Example

For `MIN(pairs_collection)`, the system generates:

```llvm
; Initialize accumulators
%x_acc = insertelement <4 x i32> undef, i32 2147483647, i32 0  ; INT_MAX
%y_acc = insertelement <4 x i32> undef, i32 2147483647, i32 0

vector_loop:
  ; VLD2 de-interleave 4 pairs
  %loaded = call { <4 x i32>, <4 x i32> } @llvm.arm.neon.vld2.v4i32(i8* %ptr)
  %x_vals = extractvalue { <4 x i32>, <4 x i32> } %loaded, 0
  %y_vals = extractvalue { <4 x i32>, <4 x i32> } %loaded, 1
  
  ; Vector minimum
  %x_acc = call <4 x i32> @llvm.arm.neon.vmins.v4i32(<4 x i32> %x_acc, <4 x i32> %x_vals)
  %y_acc = call <4 x i32> @llvm.arm.neon.vmins.v4i32(<4 x i32> %y_acc, <4 x i32> %y_vals)
  
  ; Loop control
  br i1 %cond, label %vector_loop, label %horizontal

horizontal:
  ; Reduce vectors to scalars
  %final_x = call i32 @llvm.arm.neon.vpmins.v4i32(<4 x i32> %x_acc)
  %final_y = call i32 @llvm.arm.neon.vpmins.v4i32(<4 x i32> %y_acc)
  
  ; Combine into result pair
  %result = call i64 @make_pair(i32 %final_x, i32 %final_y)
```

## Supported Operations

All modular reducers work with component-wise PAIRS:

- **MIN**: Component-wise minimum (finds minimum X and minimum Y)
- **MAX**: Component-wise maximum (finds maximum X and maximum Y)
- **SUM**: Component-wise addition (sums all X values and all Y values)
- **PRODUCT**: Component-wise multiplication
- **BITWISE_AND**: Component-wise bitwise AND
- **BITWISE_OR**: Component-wise bitwise OR

## Future Extensions

### Additional NEON Instructions

- **VLD3/VLD4**: Support for QUAD and OCT collections
- **VCVT**: Automatic type conversions
- **VDUP**: Broadcast optimizations

### Advanced Patterns

- **Predicated operations**: Conditional reductions
- **Saturating arithmetic**: Overflow protection
- **Fixed-point**: High-precision numeric operations

## Summary

The component-wise PAIRS reduction system demonstrates the power of NewBCPL's modular design:

1. **Developer simplicity**: Write `MIN(pairs)` and get NEON optimization
2. **Automatic SIMD**: No manual vectorization required
3. **Reducer modularity**: Existing reducers work unchanged
4. **Performance**: Up to 4x speedup for large collections
5. **Correctness**: Automatic handling of edge cases and remainders

This system bridges the gap between high-level expressiveness and low-level performance, allowing developers to work with coordinate data, complex numbers, and paired values at SIMD speeds with scalar simplicity.
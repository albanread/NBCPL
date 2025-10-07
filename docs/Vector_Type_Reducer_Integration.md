# Vector Type and Reducer Integration Guide

## Overview

This document provides a comprehensive guide to how NewBCPL's rich vector type system integrates with the NeonReducer architecture. NewBCPL supports both traditional dynamic vectors and innovative multi-lane value types, each with specific NEON optimization strategies.

## NewBCPL Vector Type Taxonomy

### Traditional Dynamic Vectors
These are heap-allocated vectors with dynamic sizing, similar to arrays in other languages:

- **VEC**: Dynamic integer vectors (`LET a = [1, 2, 3, 4, 5]`)
- **FVEC**: Dynamic float vectors (`FLET b = [1.5, 2.5, 3.5]`)

### Multi-Lane Value Types (SIMD-Optimized)
These are fixed-size vectors stored as single values, optimized for SIMD operations:

#### Two-Lane Types
- **PAIR**: 2×32-bit integers in single 64-bit value (`LET p = pair(10, 20)`)
- **FPAIR**: 2×32-bit floats in single 64-bit value (`FLET fp = fpair(1.5, 2.5)`)

#### Four-Lane Types  
- **QUAD**: 4×32-bit integers in single 128-bit value (`LET q = quad(1, 2, 3, 4)`)
- **FQUAD**: 4×16-bit half-precision floats in single 64-bit value (`FLET fq = fquad(1.0h, 2.0h, 3.0h, 4.0h)`)

#### Eight-Lane Types (Future)
- **OCT**: 8×32-bit integers in single 256-bit value (`LET o = oct(1, 2, 3, 4, 5, 6, 7, 8)`) - NYIMP
- **FOCT**: 8×32-bit floats in single 256-bit value (`FLET fo = foct(1.0, 2.0, ...)`) - NYIMP

## Vector Type → NEON Mapping Strategy

### Arrangement Mapping Table

| Vector Type | NEON Arrangement | Width | Element Type | Elements | Storage |
|------------|------------------|-------|--------------|----------|---------|
| PAIR       | .2S             | 64-bit | 32-bit int   | 2        | Single register |
| FPAIR      | .2S             | 64-bit | 32-bit float | 2        | Single register |
| QUAD       | .4S             | 128-bit| 32-bit int   | 4        | Single register |
| FQUAD      | .4H             | 64-bit | 16-bit float | 4        | Single register |
| OCT        | .4S (chunked)   | 256-bit| 32-bit int   | 8        | Two registers   |
| FOCT       | .4S (chunked)   | 256-bit| 32-bit float | 8        | Two registers   |
| VEC        | .4S (chunked)   | Dynamic| 32-bit int   | N        | Multiple chunks |
| FVEC       | .4S (chunked)   | Dynamic| 32-bit float | N        | Multiple chunks |

### Processing Strategies by Type

#### Single-Instruction Types (PAIR, FPAIR, QUAD)
These types fit perfectly into ARM64 NEON registers and can be processed with single instructions:

```cpp
// PAIR example: MIN(pair(10, 20), pair(15, 5)) → pair(10, 5)
SMIN.2S v0, v1, v2  // Single instruction processes both lanes

// QUAD example: MAX(quad(1,2,3,4), quad(4,3,2,1)) → quad(4,3,3,4)  
SMAX.4S v0, v1, v2  // Single instruction processes all four lanes
```

#### Half-Precision Types (FQUAD)
Uses ARM64's native half-precision float support:

```cpp
// FQUAD example: SUM(fquad(1.0h, 2.0h, 3.0h, 4.0h), fquad(0.5h, 1.5h, 2.5h, 3.5h))
FADD.4H v0, v1, v2  // Single instruction, half-precision arithmetic
```

#### Chunked Processing Types (OCT, FOCT, VEC, FVEC)
These require multiple NEON instructions due to size limitations:

```cpp
// OCT example: Process 8×32-bit values using two 4S operations
SMIN.4S v0, v1, v2  // Process lanes 0-3
SMIN.4S v3, v4, v5  // Process lanes 4-7

// Dynamic VEC: Process in 4S chunks until complete
// for (i = 0; i < vec_size; i += 4) { SMIN.4S ... }
```

## Reducer Implementation by Vector Type

### Core Reducer Classes and Vector Type Support

#### MinReducer
**Supported Types**: All vector types
**Operation**: Element-wise minimum selection
**NEON Instructions by Type**:
- PAIR/FPAIR: `SMIN.2S` / `FMIN.2S`
- QUAD/FQUAD: `SMIN.4S` / `FMIN.4H`  
- OCT/FOCT: Chunked `SMIN.4S` / `FMIN.4S`
- VEC/FVEC: Loop-based `SMIN.4S` / `FMIN.4S`

#### MaxReducer  
**Supported Types**: All vector types
**Operation**: Element-wise maximum selection
**NEON Instructions by Type**:
- PAIR/FPAIR: `SMAX.2S` / `FMAX.2S`
- QUAD/FQUAD: `SMAX.4S` / `FMAX.4H`
- OCT/FOCT: Chunked `SMAX.4S` / `FMAX.4S`
- VEC/FVEC: Loop-based `SMAX.4S` / `FMAX.4S`

#### SumReducer
**Supported Types**: All vector types
**Operation**: Element-wise addition
**NEON Instructions by Type**:
- PAIR/FPAIR: `ADD.2S` / `FADD.2S`
- QUAD/FQUAD: `ADD.4S` / `FADD.4H`
- OCT/FOCT: Chunked `ADD.4S` / `FADD.4S`
- VEC/FVEC: Loop-based `ADD.4S` / `FADD.4S`

## Type Detection and Dispatch

### AST-Level Type Analysis
The `ASTAnalyzer` determines vector types during compilation:

```cpp
// Type detection in CFGBuilderPass
if (left_type == "PAIR" && right_type == "PAIR") {
    // Use SMIN.2S for integer pair processing
    generatePairwiseReductionCFG(reducer, "llvm.arm.neon.smin.v2i32");
} else if (left_type == "FQUAD" && right_type == "FQUAD") {
    // Use FMIN.4H for half-precision quad processing
    generatePairwiseReductionCFG(reducer, "llvm.arm.neon.fmin.v4f16");
}
```

### NeonReducerRegistry Lookup
The registry maps `(intrinsic_name, vector_type)` pairs to specific encoders:

```cpp
// Registry key examples
"llvm.arm.neon.smin.v2i32:PAIR:2S"    → gen_neon_smin_2s_pair
"llvm.arm.neon.fmax.v4f16:FQUAD:4H"   → gen_neon_fmax_4h_fquad  
"llvm.arm.neon.fadd.v4f32:FOCT:4S"    → gen_neon_fadd_4s_foct_chunked
```

## Code Generation Examples

### PAIR Operations (2×32-bit integers)
```bcpl
LET p1 = pair(100, 200)
LET p2 = pair(150, 50)
LET result = MIN(p1, p2)  // → pair(100, 50)
```

Generated ARM64 Assembly:
```asm
; Load PAIR values into NEON registers
ldr d0, [x1]        ; Load p1 into lower 64 bits of v0
ldr d1, [x2]        ; Load p2 into lower 64 bits of v1

; Single NEON instruction processes both lanes
smin v0.2s, v0.2s, v1.2s  ; v0 = min(p1.first, p2.first), min(p1.second, p2.second)

; Store result
str d0, [x0]        ; Store result PAIR
```

### QUAD Operations (4×32-bit integers)
```bcpl
LET q1 = quad(10, 40, 20, 80)
LET q2 = quad(30, 10, 50, 60)
LET result = MAX(q1, q2)  // → quad(30, 40, 50, 80)
```

Generated ARM64 Assembly:
```asm
; Load QUAD values into NEON registers  
ldr q0, [x1]        ; Load q1 into 128-bit register v0
ldr q1, [x2]        ; Load q2 into 128-bit register v1

; Single NEON instruction processes all four lanes
smax v0.4s, v0.4s, v1.4s  ; v0 = max of all corresponding lanes

; Store result
str q0, [x0]        ; Store result QUAD
```

### FQUAD Operations (4×16-bit half-precision floats)
```bcpl
FLET fq1 = fquad(1.5h, 2.0h, 3.0h, 4.0h)
FLET fq2 = fquad(2.0h, 1.0h, 4.0h, 3.0h)  
FLET result = SUM(fq1, fq2)  // → fquad(3.5h, 3.0h, 7.0h, 7.0h)
```

Generated ARM64 Assembly:
```asm
; Load FQUAD values (4×16-bit floats in 64-bit registers)
ldr d0, [x1]        ; Load fq1 into lower 64 bits of v0
ldr d1, [x2]        ; Load fq2 into lower 64 bits of v1

; Half-precision floating-point addition
fadd v0.4h, v0.4h, v1.4h  ; v0 = sum of all corresponding half-precision lanes

; Store result
str d0, [x0]        ; Store result FQUAD
```

### Dynamic VEC Operations (N×32-bit integers)
```bcpl
LET a = [1, 5, 3, 8, 2, 7, 4, 6]  // 8-element vector
LET b = [2, 4, 6, 7, 3, 5, 8, 1]  // 8-element vector
LET result = MIN(a, b)             // → [1, 4, 3, 7, 2, 5, 4, 1]
```

Generated ARM64 Assembly:
```asm
; Load vector pointers and size
ldr x1, [sp, #vector_a_ptr]
ldr x2, [sp, #vector_b_ptr]  
ldr x3, [sp, #vector_result_ptr]
ldr w4, [x1, #-8]           ; Load vector size from header

; Loop processing 4 elements at a time
loop_start:
    cmp w4, #4
    blt process_remainder
    
    ; Load 4 elements from each vector
    ldr q0, [x1], #16       ; Load 4 elements from a, advance pointer
    ldr q1, [x2], #16       ; Load 4 elements from b, advance pointer
    
    ; Process 4 elements with single NEON instruction
    smin v0.4s, v0.4s, v1.4s
    
    ; Store 4 results
    str q0, [x3], #16       ; Store 4 results, advance pointer
    
    ; Update counter and loop
    sub w4, w4, #4
    b loop_start

process_remainder:
    ; Handle remaining elements (0-3) with scalar operations
    ; ... scalar processing code ...
```

## Performance Characteristics

### Single-Instruction Types (PAIR, FPAIR, QUAD, FQUAD)
- **Latency**: 1-2 cycles per operation
- **Throughput**: 1 operation per cycle  
- **Memory**: Single load/store operations
- **Register Pressure**: Minimal (single V register per operand)
- **Cache**: Optimal locality (contiguous memory layout)

### Chunked Types (OCT, FOCT - Future)
- **Latency**: 2-4 cycles per operation (multiple instructions)
- **Throughput**: 0.5 operations per cycle
- **Memory**: Multiple load/store operations
- **Register Pressure**: Moderate (multiple V registers)
- **Cache**: Good locality (still contiguous)

### Dynamic Types (VEC, FVEC)
- **Latency**: Variable (depends on vector size)
- **Throughput**: High for large vectors (amortized loop overhead)
- **Memory**: Streaming loads/stores with prefetch opportunities
- **Register Pressure**: Low (register reuse in loops)
- **Cache**: Excellent for sequential access patterns

## Error Handling and Edge Cases

### Type Mismatch Detection
```cpp
// Compile-time type checking
if (left_vector_type != right_vector_type) {
    throw CompileError("Vector type mismatch in reduction: " + 
                      left_vector_type + " vs " + right_vector_type);
}

// Integer vs Float operation validation  
if (operation == "BITWISE_AND" && is_float_type(vector_type)) {
    throw CompileError("Bitwise operations not supported on float vectors: " + vector_type);
}
```

### Size Validation
```cpp
// Multi-lane types have fixed sizes (compile-time known)
// Dynamic vectors require runtime size checking

// VEC/FVEC runtime size validation
if (left_vector_size != right_vector_size) {
    runtime_error("Vector size mismatch in MIN operation");
}

// PAIR/QUAD operations have implicit size matching (always valid)
```

### NEON Availability Fallbacks
```cpp
// When NEON is disabled (--no-neon flag)
if (!neon_enabled) {
    // PAIR fallback: process each lane individually
    result.first = min(left.first, right.first);
    result.second = min(left.second, right.second);
    
    // QUAD fallback: process each lane individually  
    result.first = min(left.first, right.first);
    result.second = min(left.second, right.second);
    result.third = min(left.third, right.third);
    result.fourth = min(left.fourth, right.fourth);
}
```

## Future Extensions

### OCT/FOCT Implementation Strategy
When implementing 8-lane vector types:

1. **Chunked Processing**: Use two 4S operations
2. **Register Allocation**: Pair V registers for 256-bit operations
3. **Memory Layout**: Ensure 32-byte alignment for optimal performance
4. **Loop Unrolling**: Process multiple OCT/FOCT values per iteration

### Advanced NEON Features
- **Cross-Lane Operations**: ADDV, MAXV for scalar reductions
- **Lane Insertion**: INS instruction for selective lane updates  
- **Permutation**: TBL instruction for lane reordering
- **Saturation**: SQADD, UQADD for overflow protection

### Platform Portability
The vector type abstraction enables easy porting:

#### x86 SSE/AVX Mapping
```cpp
PAIR   → SSE2 64-bit operations (_mm_min_epi32 on 2×32-bit)
QUAD   → SSE2 128-bit operations (_mm_min_epi32 on 4×32-bit)  
OCT    → AVX2 256-bit operations (_mm256_min_epi32 on 8×32-bit)
```

#### RISC-V Vector Extension
```cpp
PAIR   → vsetvli with LMUL=1/2, process 2 elements
QUAD   → vsetvli with LMUL=1, process 4 elements
Dynamic → vsetvli with computed LMUL, process N elements
```

## Best Practices

### Type Selection Guidelines
- **Use PAIR/FPAIR** for 2D coordinates, complex numbers, stereo audio samples
- **Use QUAD/FQUAD** for RGBA colors, quaternions, small fixed-size datasets  
- **Use VEC/FVEC** for large datasets, algorithms with unknown sizes at compile-time
- **Prefer multi-lane types** when possible for optimal SIMD performance

### Performance Optimization
- **Align data structures** to NEON register boundaries (8/16/32 bytes)
- **Batch operations** when working with multiple multi-lane values
- **Minimize type conversions** between different vector types
- **Use appropriate precision** (FQUAD's half-precision vs FPAIR's single-precision)

### Code Organization
- **Group related operations** on same vector types in CFG basic blocks
- **Minimize register spills** by processing similar vector types consecutively
- **Leverage compiler optimizations** by using consistent vector types in loops

This integration provides NewBCPL with a powerful, type-safe, and performance-optimized vector processing system that scales from simple 2-element operations to large-scale data processing tasks.
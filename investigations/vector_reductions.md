# Design Note: PAIRS Vector MIN/MAX/SUM Operations

**Date**: December 2024  
**Feature**: Extended SIMD operations for PAIRS vectors  
**Target**: ARM64 NEON optimization  
**Status**: Pre-implementation design  

## Overview

Extend the existing PAIRS vector infrastructure to support MIN, MAX, and SUM operations using ARM64 NEON instructions. This builds on the proven architecture in `VectorCodeGen.cpp` that already handles ADD/SUB/MUL operations with 4x SIMD parallelism.

## Current Foundation

The existing `generatePairsVectorBinaryOp()` provides:
- ✅ **Q-register chunking**: Processes 2 PAIRs (16 bytes) per iteration
- ✅ **NEON arrangement**: `.4S` for 4 × 32-bit integers per Q-register  
- ✅ **Memory management**: Automatic result allocation via `GETVEC`
- ✅ **Remainder handling**: Scalar fallback for odd vector sizes
- ✅ **Type detection**: `isPairsVectorOperation()` integration

## Target Operations

### 1. Element-wise MIN/MAX
```bcpl
LET pairs1 = PAIRS 100
LET pairs2 = PAIRS 100
LET result_min = MIN(pairs1, pairs2)  // Element-wise minimum
LET result_max = MAX(pairs1, pairs2)  // Element-wise maximum
```

### 2. Element-wise SUM
```bcpl
LET result_sum = SUM(pairs1, pairs2)  // Element-wise addition (alias for +)
```

## ARM64 NEON Instructions Required

### MIN Operation - SMIN (Signed Minimum)
```assembly
SMIN V0.4S, V1.4S, V2.4S  ; Find minimum of each lane
```
**Encoding**: `0100 1110 1010 0000 0110 11xx xxxx xxxx`
- Bit 30: Q=1 (128-bit)
- Bits 23-22: size=00 (32-bit elements) 
- Bits 15-10: opcode=011011 (SMIN)

### MAX Operation - SMAX (Signed Maximum)  
```assembly
SMAX V0.4S, V1.4S, V2.4S  ; Find maximum of each lane
```
**Encoding**: `0100 1110 1010 0000 0110 01xx xxxx xxxx`
- Same as SMIN but bit 11: o1=0 (SMAX vs SMIN)

### SUM Operation - ADD (Existing)
Already implemented via `create_add_vector_reg()` - SUM becomes semantic alias.

## Implementation Plan

### Phase 1: Architecture - NEW APPROACH ✨
**Decision**: Create dedicated `reductions.cpp` to avoid crowding `VectorCodeGen.cpp`

```cpp
// reductions.h
class ReductionCodeGen {
public:
    ReductionCodeGen(RegisterManager& rm, NewCodeGenerator& cg, ASTAnalyzer& analyzer);
    
    // Main entry point - type-dispatched
    void generateReduction(FunctionCall& node, const std::string& op_name);
    static bool isReductionOperation(const std::string& func_name);
    
private:
    // PAIRS reductions (integer)
    void generatePairsMin(const std::vector<ExprPtr>& args);
    void generatePairsMax(const std::vector<ExprPtr>& args);  
    void generatePairsSum(const std::vector<ExprPtr>& args);
    
    // NEON instruction generators
    Instruction vecgen_smin_4s(const std::string& vd, const std::string& vn, const std::string& vm);
    Instruction vecgen_smax_4s(const std::string& vd, const std::string& vn, const std::string& vm);
};
```

### Phase 2: Parser Integration
In `NewCodeGenerator::visit(FunctionCall& node)`:
```cpp
if (ReductionCodeGen::isReductionOperation(node.function_name)) {
    reduction_codegen_.generateReduction(node, node.function_name);
    return;
}
```

### Phase 3: NEON Encoders
Add new instruction encoders:
```cpp
Instruction ReductionCodeGen::vecgen_smin_4s(const std::string& vd, 
                                             const std::string& vn, 
                                             const std::string& vm) {
    uint32_t base_opcode = 0x4EA06C00; // SMIN.4S base encoding
    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));
    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    return Instruction(instruction, "SMIN " + vd + ".4S, " + vn + ".4S, " + vm + ".4S");
}
```

## Validation Strategy

### 1. Clang Reference Implementation
Generate reference assembly for validation:
```cpp
// test_neon_reference.cpp
#include <arm_neon.h>

void test_pairs_min_max(int32x4_t a, int32x4_t b) {
    int32x4_t min_result = vminq_s32(a, b);  // Should generate SMIN.4S
    int32x4_t max_result = vmaxq_s32(a, b);  // Should generate SMAX.4S
    int32x4_t sum_result = vaddq_s32(a, b);  // Should generate ADD.4S
}
```

### 2. Instruction Verification
Use `InstructionDecoder` to verify generated opcodes match expected patterns.

### 3. End-to-End Testing
```bcpl
LET test_pairs_operations() BE {
    LET p1 = PAIRS 4
    LET p2 = PAIRS 4
    // Initialize test data...
    
    LET min_result = MIN(p1, p2)
    LET max_result = MAX(p1, p2)  
    LET sum_result = SUM(p1, p2)
    
    // Verify results...
}
```

## File Organization

```
NewBCPL/
├── VectorCodeGen.cpp     # Basic vector arithmetic (+, -, *, /)
├── reductions.h          # NEW: Reduction operations interface
├── reductions.cpp        # NEW: MIN, MAX, SUM implementations  
└── investigations/
    └── vector_reductions.md  # This design document
```

## Performance Impact

- **4x parallelism**: Process 4 × 32-bit integers simultaneously
- **Memory efficiency**: 128-bit loads/stores maximize bandwidth
- **Reduced instruction count**: ~4x fewer operations vs scalar
- **Register pressure**: Optimal Q-register utilization

## Implementation Status

### ✅ Completed
- [x] Architecture design with `ReductionCodeGen` class
- [x] Basic structure for type-dispatched reductions
- [x] PAIRS MIN/MAX instruction encoders (`vecgen_smin_4s`, `vecgen_smax_4s`)
- [x] Memory allocation and Q-register management
- [x] Integration points with existing infrastructure

### 🚧 In Progress
- [ ] Instruction encoding validation with clang reference
- [ ] Parser integration for `MIN`/`MAX`/`SUM` function recognition
- [ ] End-to-end testing with PAIRS vectors

### 📋 Future Extensions
- [ ] FPAIRS operations (FMIN/FMAX for floating-point vectors)
- [ ] VEC/FVEC support for regular integer/float vectors
- [ ] Horizontal reductions (sum/min/max across single vector)
- [ ] Advanced operations (DOT product, NORM, etc.)

## Risk Analysis

- **Low risk**: Building on proven `generatePairsVectorBinaryOp()` architecture
- **Encoder validation**: Clang cross-reference ensures correctness  
- **Fallback available**: Scalar implementation for edge cases
- **Testing coverage**: Comprehensive validation pipeline

---

**Implementation Priority**: High - Core mathematical operations for PAIRS  
**Estimated Effort**: 2-3 days (encoder + integration + testing)  
**Performance Gain**: 4x speedup for MIN/MAX operations on large PAIRS vectors  
**Architecture Benefit**: Clean separation enables future vector operation extensions
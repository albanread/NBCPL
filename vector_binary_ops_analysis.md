# Vector Binary Operators Routing Analysis

## Current Issues with Binary Operator Routing

### Problem Summary

The current binary operator routing in `NewCodeGenerator::visit(BinaryOp& node)` has several critical issues that prevent vector types from properly using the new custom `vecgen_*` encoders:

1. **Incomplete Vector Type Detection**: Only OCT/FOCT are routed to VectorCodeGen, while PAIR/FPAIR/QUAD use separate legacy logic
2. **Missing Division Support**: Division operations are not included in vector operation detection
3. **Fragmented Code Paths**: Multiple routing mechanisms instead of unified VectorCodeGen approach
4. **Inconsistent Encoder Usage**: Some vector operations may not use the new custom encoders

### Current Routing Logic Analysis

#### 1. SIMD Operations (OCT, FOCT) ✅
```cpp
if (vector_codegen_ && VectorCodeGen::isSimdOperation(left_type, right_type)) {
    debug_print("Detected SIMD vector operation - using VectorCodeGen");
    vector_codegen_->generateSimdBinaryOp(node, use_neon_);
    return;
}
```
**Status**: ✅ Properly routed to VectorCodeGen
**Encoders Used**: New `vecgen_*` methods

#### 2. PAIR Operations ⚠️
```cpp
if (is_vector_pair_operation(node)) {
    debug_print("Detected vector PAIR operation - using NEON acceleration");
    generate_neon_vector_pair_operation(node);
    return;
}
```
**Status**: ⚠️ Uses separate legacy path
**Encoders Used**: May not use new `vecgen_*` methods consistently
**Limitations**: 
- Only checks `VarType::PAIR` (excludes FPAIR, QUAD)
- Only supports Add, Subtract, Multiply (no Division)
- Separate code path from VectorCodeGen

#### 3. FPAIR and QUAD Operations ❌
**Status**: ❌ Not detected by current routing logic
**Current Behavior**: Falls through to scalar operations
**Impact**: Vector operations processed as scalar, losing SIMD benefits

### Detected Issues

#### Issue 1: Inconsistent Vector Type Detection

**Current `VectorCodeGen::isSimdOperation`**:
```cpp
bool VectorCodeGen::isSimdOperation(VarType left_type, VarType right_type) {
    bool left_is_simd = (left_type == VarType::OCT || left_type == VarType::FOCT);
    bool right_is_simd = (right_type == VarType::OCT || right_type == VarType::FOCT);
    return left_is_simd || right_is_simd;
}
```
**Problem**: Excludes PAIR, FPAIR, QUAD vector types

#### Issue 2: Limited Operation Support

**Current `is_vector_pair_operation`**:
```cpp
if (node.op != BinaryOp::Operator::Add && 
    node.op != BinaryOp::Operator::Subtract && 
    node.op != BinaryOp::Operator::Multiply) {
    return false;
}
```
**Problem**: Division operations not supported for vectors

#### Issue 3: Fragmented Code Paths

- **Path 1**: VectorCodeGen for OCT/FOCT
- **Path 2**: `generate_neon_vector_pair_operation` for PAIR
- **Path 3**: Scalar fallback for FPAIR/QUAD

**Problem**: Inconsistent encoder usage and maintenance complexity

## Recommended Solution

### 1. Unified Vector Detection

**Extend `VectorCodeGen::isSimdOperation` to include all vector types**:
```cpp
bool VectorCodeGen::isSimdOperation(VarType left_type, VarType right_type) {
    auto isVectorType = [](VarType type) {
        return type == VarType::PAIR || type == VarType::FPAIR || 
               type == VarType::QUAD || type == VarType::OCT || 
               type == VarType::FOCT;
    };
    
    bool left_is_vector = isVectorType(left_type);
    bool right_is_vector = isVectorType(right_type);
    
    // Support vector-vector and vector-scalar operations
    return left_is_vector || right_is_vector;
}
```

### 2. Complete Operation Support

**Add division support in VectorCodeGen**:
- Implement `vecgen_fdiv_vector` for floating-point division
- Add scalar division fallback for integer vectors
- Support all four basic arithmetic operations: +, -, *, /

### 3. Unified Routing Logic

**Simplify `NewCodeGenerator::visit(BinaryOp& node)`**:
```cpp
void NewCodeGenerator::visit(BinaryOp& node) {
    debug_print("Visiting BinaryOp node.");

    VarType left_type = infer_expression_type_local(node.left.get());
    VarType right_type = infer_expression_type_local(node.right.get());
    
    // Route ALL vector operations through VectorCodeGen
    if (vector_codegen_ && VectorCodeGen::isSimdOperation(left_type, right_type)) {
        debug_print("Detected vector operation - using VectorCodeGen");
        vector_codegen_->generateSimdBinaryOp(node, use_neon_);
        return;
    }

    // Handle logical operations with short-circuit evaluation
    if (node.op == BinaryOp::Operator::LogicalAnd) {
        generate_short_circuit_and(node);
        return;
    } else if (node.op == BinaryOp::Operator::LogicalOr) {
        generate_short_circuit_or(node);
        return;
    }

    // Continue with existing scalar operation logic...
}
```

### 4. Enhanced VectorCodeGen Implementation

**Required enhancements to VectorCodeGen**:

1. **Support for all vector types**: PAIR, FPAIR, QUAD, OCT, FOCT
2. **Complete operation set**: Add, Subtract, Multiply, Divide
3. **Consistent encoder usage**: All operations use `vecgen_*` methods
4. **Proper register management**: Use appropriate D/Q registers per vector type

## Implementation Priority

### High Priority (Required for Testing)
1. ✅ **Fix `isSimdOperation`**: Include PAIR, FPAIR, QUAD
2. ✅ **Add division support**: Implement vector division operations
3. ✅ **Unified routing**: Remove duplicate detection logic

### Medium Priority (Enhancement)
1. **Scalar-vector operations**: Improve mixed-type operation handling
2. **Error handling**: Better diagnostics for unsupported operations
3. **Performance optimization**: Register allocation improvements

### Low Priority (Future)
1. **Advanced operations**: Vector comparison, bitwise operations
2. **Type promotion**: Automatic type conversion between vector types
3. **Compiler optimizations**: Vector operation fusion

## Expected Benefits

### 1. Consistent Custom Encoder Usage
- All vector operations use tested `vecgen_*` methods
- Unified instruction encoding across vector types
- Consistent assembly output and hex opcodes

### 2. Complete Operation Support
- Addition, subtraction, multiplication, division for all vector types
- Vector-scalar and vector-vector operations
- Proper fallback to scalar operations when needed

### 3. Simplified Maintenance
- Single code path for all vector operations
- Reduced complexity in binary operator routing
- Easier to add new vector operations

### 4. Better Testing Coverage
- All vector types follow same code path
- Consistent behavior between `--exec` and `--run` modes
- Easier to diagnose encoding vs logic issues

## Testing Validation

### Before Changes
```
PAIR operations:  ✅ Working (separate path)
FPAIR operations: ❌ Falling to scalar
QUAD operations:  ❌ Falling to scalar  
OCT operations:   ✅ Working (VectorCodeGen)
FOCT operations:  ✅ Working (VectorCodeGen)
Division ops:     ❌ Not supported for vectors
```

### After Changes
```
PAIR operations:  ✅ Working (VectorCodeGen + vecgen_*)
FPAIR operations: ✅ Working (VectorCodeGen + vecgen_*)
QUAD operations:  ✅ Working (VectorCodeGen + vecgen_*)
OCT operations:   ✅ Working (VectorCodeGen + vecgen_*)
FOCT operations:  ✅ Working (VectorCodeGen + vecgen_*)
Division ops:     ✅ Working (VectorCodeGen + vecgen_*)
```

## Action Items

1. **Update `VectorCodeGen::isSimdOperation`** to include all vector types
2. **Add division operation support** in VectorCodeGen
3. **Remove legacy `is_vector_pair_operation`** logic
4. **Test all vector types** with the unified routing
5. **Verify custom encoder usage** with `--list` flag comparisons
6. **Update documentation** to reflect unified approach

This unified approach will ensure that your custom `vecgen_*` encoders are used consistently across all vector types, providing the performance benefits and instruction accuracy you've achieved in your testing.
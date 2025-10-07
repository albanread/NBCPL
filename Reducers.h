#pragma once

#include "Reducer.h"

/**
 * Concrete Reducer Implementations
 * 
 * These classes implement the Reducer interface for specific reduction operations,
 * forming the core of the modern NeonReducer architecture. Each reducer encapsulates
 * the complete behavior of one reduction type, including:
 * 
 * - Operation semantics (what the reduction does)
 * - Binary operator for scalar fallback
 * - Operation code for NEON encoder dispatch
 * - Initialization requirements (if any)
 * 
 * Design Philosophy:
 * - One class per reduction operation (Single Responsibility Principle)
 * - Immutable behavior (all methods are const)
 * - Zero-cost abstractions (methods inline to simple returns)
 * - Type-safe operation dispatch
 * 
 * Integration Points:
 * - Factory creation via createReducer() in Reducer.cpp
 * - CFG generation via CFGBuilderPass::visit(MinStatement&) etc.
 * - NEON encoding via NeonReducerRegistry lookups
 * - Scalar fallback via getOperator() implementations
 * 
 * Performance Characteristics:
 * - Compile-time polymorphism (virtual calls only during setup)
 * - Zero runtime overhead for operation dispatch
 * - Optimal NEON instruction generation via registry
 * - Automatic vectorization opportunities
 */

/**
 * MinReducer - Element-wise minimum reduction across all NewBCPL vector types
 * 
 * Computes the element-wise minimum between two vectors of equal length,
 * supporting both traditional vectors and multi-lane value types.
 * For each position i, result[i] = min(vector1[i], vector2[i]).
 * 
 * Mathematical Definition:
 *   result[i] = (left[i] < right[i]) ? left[i] : right[i]
 * 
 * NEON Implementation by Vector Type:
 *   - PAIR:   SMIN.2S (2×32-bit integers in single 64-bit value)
 *   - FPAIR:  FMIN.2S (2×32-bit floats in single 64-bit value)
 *   - QUAD:   SMIN.4S (4×32-bit integers in single 128-bit value)
 *   - FQUAD:  FMIN.4H (4×16-bit half-precision floats in single 64-bit value)
 *   - OCT:    SMIN.4S (8×32-bit integers, processed in chunks - NYIMP)
 *   - FOCT:   FMIN.4S (8×32-bit floats, processed in chunks - NYIMP)
 *   - VEC:    SMIN.4S (dynamic integer vectors, processed in 4S chunks)
 *   - FVEC:   FMIN.4S (dynamic float vectors, processed in 4S chunks)
 * 
 * Initialization Strategy:
 *   - Uses first element as starting value (no explicit initialization)
 *   - Natural identity: min(a, +∞) = a
 *   - Multi-lane types process all lanes simultaneously
 * 
 * Vector Type Examples:
 *   PAIR:  LET p1 = pair(10, 20); LET p2 = pair(15, 5) → MIN(p1, p2) = pair(10, 5)
 *   QUAD:  LET q1 = quad(1,4,2,8); LET q2 = quad(3,1,5,6) → MIN(q1, q2) = quad(1,1,2,6)
 *   FQUAD: FLET fq1 = fquad(1.5h, 2.0h, 3.0h, 4.0h) → FMIN.4H processing
 *   VEC:   LET a = [1, 5, 3, 8]; LET b = [2, 4, 6, 7] → MIN(a, b) = [1, 4, 3, 7]
 */
class MinReducer : public Reducer {
public:
    std::string getName() const override { 
        return "MIN"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Less; 
    }
    
    int getReductionOp() const override { 
        return 0; // MIN operation
    }
    
    // MIN doesn't require initialization - uses first element as starting value
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * MaxReducer - Element-wise maximum reduction across all NewBCPL vector types
 * 
 * Computes the element-wise maximum between two vectors of equal length,
 * supporting both traditional vectors and multi-lane value types.
 * For each position i, result[i] = max(vector1[i], vector2[i]).
 * 
 * Mathematical Definition:
 *   result[i] = (left[i] > right[i]) ? left[i] : right[i]
 * 
 * NEON Implementation by Vector Type:
 *   - PAIR:   SMAX.2S (2×32-bit integers in single 64-bit value)
 *   - FPAIR:  FMAX.2S (2×32-bit floats in single 64-bit value)
 *   - QUAD:   SMAX.4S (4×32-bit integers in single 128-bit value)
 *   - FQUAD:  FMAX.4H (4×16-bit half-precision floats in single 64-bit value)
 *   - OCT:    SMAX.4S (8×32-bit integers, processed in chunks - NYIMP)
 *   - FOCT:   FMAX.4S (8×32-bit floats, processed in chunks - NYIMP)
 *   - VEC:    SMAX.4S (dynamic integer vectors, processed in 4S chunks)
 *   - FVEC:   FMAX.4S (dynamic float vectors, processed in 4S chunks)
 * 
 * Initialization Strategy:
 *   - Uses first element as starting value (no explicit initialization)
 *   - Natural identity: max(a, -∞) = a
 *   - Multi-lane types process all lanes simultaneously
 * 
 * Vector Type Examples:
 *   PAIR:  LET p1 = pair(10, 20); LET p2 = pair(15, 5) → MAX(p1, p2) = pair(15, 20)
 *   QUAD:  LET q1 = quad(1,4,2,8); LET q2 = quad(3,1,5,6) → MAX(q1, q2) = quad(3,4,5,8)
 *   FQUAD: FLET fq1 = fquad(1.5h, 2.0h, 3.0h, 4.0h) → FMAX.4H processing
 *   VEC:   LET a = [1, 5, 3, 8]; LET b = [2, 4, 6, 7] → MAX(a, b) = [2, 5, 6, 8]
 */
class MaxReducer : public Reducer {
public:
    std::string getName() const override { 
        return "MAX"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Greater; 
    }
    
    int getReductionOp() const override { 
        return 1; // MAX operation
    }
    
    // MAX doesn't require initialization - uses first element as starting value
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * SumReducer - Element-wise addition reduction across all NewBCPL vector types
 * 
 * Computes the element-wise sum of two vectors of equal length,
 * supporting both traditional vectors and multi-lane value types.
 * For each position i, result[i] = vector1[i] + vector2[i].
 * 
 * Mathematical Definition:
 *   result[i] = left[i] + right[i]
 * 
 * NEON Implementation by Vector Type:
 *   - PAIR:   ADD.2S (2×32-bit integers in single 64-bit value)
 *   - FPAIR:  FADD.2S (2×32-bit floats in single 64-bit value)
 *   - QUAD:   ADD.4S (4×32-bit integers in single 128-bit value)
 *   - FQUAD:  FADD.4H (4×16-bit half-precision floats in single 64-bit value)
 *   - OCT:    ADD.4S (8×32-bit integers, processed in chunks - NYIMP)
 *   - FOCT:   FADD.4S (8×32-bit floats, processed in chunks - NYIMP)
 *   - VEC:    ADD.4S (dynamic integer vectors, processed in 4S chunks)
 *   - FVEC:   FADD.4S (dynamic float vectors, processed in 4S chunks)
 * 
 * Initialization Strategy:
 *   - Explicit initialization to zero (additive identity)
 *   - Required for accumulation patterns: sum += element
 *   - Ensures correct results for empty or single-element vectors
 *   - Multi-lane types initialize all lanes to zero simultaneously
 * 
 * Vector Type Examples:
 *   PAIR:  LET p1 = pair(10, 20); LET p2 = pair(5, 15) → SUM(p1, p2) = pair(15, 35)
 *   QUAD:  LET q1 = quad(1,2,3,4); LET q2 = quad(5,6,7,8) → SUM(q1, q2) = quad(6,8,10,12)
 *   FQUAD: FLET fq1 = fquad(1.0h, 2.0h, 3.0h, 4.0h) → FADD.4H processing
 *   VEC:   LET a = [1, 2, 3, 4]; LET b = [5, 6, 7, 8] → SUM(a, b) = [6, 8, 10, 12]
 */
class SumReducer : public Reducer {
public:
    std::string getName() const override { 
        return "SUM"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Add; 
    }
    
    int getReductionOp() const override { 
        return 2; // SUM operation
    }
    
    // SUM requires initialization to zero
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(0));
    }
};

/**
 * ProductReducer - Element-wise multiplication reduction across all vector types (future extension)
 * 
 * Computes the element-wise product of two vectors of equal length,
 * supporting both traditional vectors and multi-lane value types.
 * For each position i, result[i] = vector1[i] * vector2[i].
 * 
 * Mathematical Definition:
 *   result[i] = left[i] * right[i]
 * 
 * NEON Implementation (Future) by Vector Type:
 *   - PAIR:   MUL.2S (2×32-bit integers in single 64-bit value)
 *   - FPAIR:  FMUL.2S (2×32-bit floats in single 64-bit value)
 *   - QUAD:   MUL.4S (4×32-bit integers in single 128-bit value)
 *   - FQUAD:  FMUL.4H (4×16-bit half-precision floats in single 64-bit value)
 *   - OCT:    MUL.4S (8×32-bit integers, processed in chunks - NYIMP)
 *   - FOCT:   FMUL.4S (8×32-bit floats, processed in chunks - NYIMP)
 *   - VEC:    MUL.4S (dynamic integer vectors, processed in 4S chunks)
 *   - FVEC:   FMUL.4S (dynamic float vectors, processed in 4S chunks)
 * 
 * Initialization Strategy:
 *   - Explicit initialization to one (multiplicative identity)
 *   - Required for accumulation patterns: product *= element
 *   - Ensures correct results for empty vectors (product = 1)
 *   - Multi-lane types initialize all lanes to one simultaneously
 * 
 * Current Status:
 *   - Reducer class: ✅ Implemented
 *   - Factory registration: ✅ Complete
 *   - NEON encoders: ⏳ Future work (needs NeonReducerRegistry entries)
 *   - CFG integration: ✅ Ready
 * 
 * Vector Type Examples (Future):
 *   PAIR:  LET p1 = pair(2, 3); LET p2 = pair(4, 5) → PRODUCT(p1, p2) = pair(8, 15)
 *   QUAD:  LET q1 = quad(2,3,4,5); LET q2 = quad(1,2,3,4) → PRODUCT(q1, q2) = quad(2,6,12,20)
 *   VEC:   LET a = [2, 3, 4, 5]; LET b = [1, 2, 3, 4] → PRODUCT(a, b) = [2, 6, 12, 20]
 */
class ProductReducer : public Reducer {
public:
    std::string getName() const override { 
        return "PRODUCT"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Multiply; 
    }
    
    int getReductionOp() const override { 
        return 3; // PRODUCT operation
    }
    
    // PRODUCT requires initialization to one
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(1));
    }
};

/**
 * BitwiseAndReducer - Element-wise bitwise AND reduction for integer vector types (future extension)
 * 
 * Computes the element-wise bitwise AND of two integer vectors,
 * supporting integer-based traditional vectors and multi-lane value types.
 * For each position i, result[i] = vector1[i] & vector2[i].
 * 
 * Mathematical Definition:
 *   result[i] = left[i] & right[i] (bitwise AND operation)
 * 
 * NEON Implementation (Future) by Integer Vector Type:
 *   - PAIR:   AND.16B (reinterpret 2×32-bit integers as 8 bytes)
 *   - QUAD:   AND.16B (4×32-bit integers as 16 bytes in single operation)
 *   - OCT:    AND.16B (8×32-bit integers, processed in chunks - NYIMP)
 *   - VEC:    AND.16B (dynamic integer vectors, processed in 16B chunks)
 *   - ⚠️ FPAIR/FQUAD/FOCT/FVEC: Not applicable (bitwise ops invalid on floats)
 * 
 * Initialization Strategy:
 *   - Explicit initialization to 0xFFFFFFFF (all bits set)
 *   - AND identity: a & 0xFFFFFFFF = a
 *   - Required for accumulation patterns: mask &= element
 *   - Multi-lane types initialize all lanes to 0xFFFFFFFF simultaneously
 * 
 * Use Cases:
 *   - Bit mask operations across vector lanes
 *   - Feature vector filtering with boolean masks
 *   - Boolean array operations (when represented as integers)
 *   - Multi-lane bit manipulation (PAIR/QUAD as wide bitmasks)
 * 
 * Current Status:
 *   - Reducer class: ✅ Implemented
 *   - Factory registration: ✅ Complete
 *   - NEON encoders: ⏳ Future work (needs NeonReducerRegistry entries)
 *   - Type validation: ⚠️ Integer-only operation (compile-time checks needed)
 * 
 * Vector Type Examples (Future):
 *   PAIR:  LET p1 = pair(0xFF00, 0x0FF0); LET p2 = pair(0xABCD, 0x1234) → AND = pair(0xAB00, 0x0230)
 *   QUAD:  LET q1 = quad(0xF0F0, 0x0F0F, 0xFF00, 0x00FF) → bitwise operations across all lanes
 *   VEC:   LET masks = [0xFF00, 0x0FF0, 0x00FF]; LET data = [0xABCD, 0x1234, 0x5678] → element-wise AND
 */
class BitwiseAndReducer : public Reducer {
public:
    std::string getName() const override { 
        return "BITWISE_AND"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::BitwiseAnd; 
    }
    
    int getReductionOp() const override { 
        return 4; // BITWISE_AND operation
    }
    
    // Bitwise AND requires initialization to all 1s (0xFFFFFFFF for 32-bit)
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(-1)); // All bits set
    }
};

/**
 * BitwiseOrReducer - Element-wise bitwise OR reduction for integer vector types (future extension)
 * 
 * Computes the element-wise bitwise OR of two integer vectors,
 * supporting integer-based traditional vectors and multi-lane value types.
 * For each position i, result[i] = vector1[i] | vector2[i].
 * 
 * Mathematical Definition:
 *   result[i] = left[i] | right[i] (bitwise OR operation)
 * 
 * NEON Implementation (Future) by Integer Vector Type:
 *   - PAIR:   ORR.16B (reinterpret 2×32-bit integers as 8 bytes)
 *   - QUAD:   ORR.16B (4×32-bit integers as 16 bytes in single operation)
 *   - OCT:    ORR.16B (8×32-bit integers, processed in chunks - NYIMP)
 *   - VEC:    ORR.16B (dynamic integer vectors, processed in 16B chunks)
 *   - ⚠️ FPAIR/FQUAD/FOCT/FVEC: Not applicable (bitwise ops invalid on floats)
 * 
 * Initialization Strategy:
 *   - Explicit initialization to 0x00000000 (no bits set)
 *   - OR identity: a | 0x00000000 = a
 *   - Required for accumulation patterns: flags |= element
 *   - Multi-lane types initialize all lanes to 0x00000000 simultaneously
 * 
 * Use Cases:
 *   - Bit flag combination across vector lanes
 *   - Feature vector merging with boolean flags
 *   - Boolean array union operations
 *   - Multi-lane flag accumulation (PAIR/QUAD as wide flag sets)
 * 
 * Current Status:
 *   - Reducer class: ✅ Implemented
 *   - Factory registration: ✅ Complete
 *   - NEON encoders: ⏳ Future work (needs NeonReducerRegistry entries)
 *   - Type validation: ⚠️ Integer-only operation (compile-time checks needed)
 * 
 * Vector Type Examples (Future):
 *   PAIR:  LET p1 = pair(0x0001, 0x0010); LET p2 = pair(0x0002, 0x0020) → OR = pair(0x0003, 0x0030)
 *   QUAD:  LET q1 = quad(0x01, 0x10, 0x100, 0x1000) → flag operations across all lanes
 *   VEC:   LET flags1 = [0x0001, 0x0010, 0x0100]; LET flags2 = [0x0002, 0x0020, 0x0200] → element-wise OR
 */
class BitwiseOrReducer : public Reducer {
public:
    std::string getName() const override { 
        return "BITWISE_OR"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::BitwiseOr; 
    }
    
    int getReductionOp() const override { 
        return 5; // BITWISE_OR operation
    }
    
    // Bitwise OR requires initialization to zero
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(0));
    }
};

/**
 * PairwiseMinReducer - NEON-accelerated pairwise minimum reduction
 * 
 * Performs pairwise minimum operations within vector lanes using specialized
 * NEON instructions. This is different from element-wise operations as it
 * operates on adjacent pairs within the same vector.
 * 
 * Operation Semantics:
 *   - Takes adjacent pairs (a[0],a[1]), (a[2],a[3]), etc.
 *   - Computes minimum of each pair
 *   - Results in vector with half the elements
 * 
 * NEON Implementation:
 *   - FMINP.4S for 32-bit floats (pairwise minimum)
 *   - SMINP.4S for 32-bit integers (pairwise minimum)
 *   - Cross-lane operations within 128-bit SIMD registers
 * 
 * Initialization Strategy:
 *   - No explicit initialization required
 *   - Operates on existing vector data
 *   - Natural reduction pattern for hierarchical minimization
 * 
 * Use Cases:
 *   - Hierarchical reduction trees
 *   - Fast minimum finding in small vectors
 *   - Parallel reduction algorithms
 * 
 * Vector Transformation:
 *   Input:  [a, b, c, d, e, f, g, h]
 *   Output: [min(a,b), min(c,d), min(e,f), min(g,h)]
 * 
 * Example Usage:
 *   LET data = [3, 1, 4, 2, 8, 5, 7, 6]
 *   LET result = PAIRWISE_MIN(data)  // [1, 2, 5, 6]
 */
class PairwiseMinReducer : public Reducer {
public:
    std::string getName() const override { 
        return "PAIRWISE_MIN"; 
    }
    
    // For pairwise reductions, the binary operator is not applied in the same way
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Less; // Placeholder
    }
    
    int getReductionOp() const override { 
        return 10; // New operation code for pairwise min
    }
    
    // Pairwise operations don't typically need initialization
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * PairwiseMaxReducer - NEON-accelerated pairwise maximum reduction
 * 
 * Performs pairwise maximum operations within vector lanes using specialized
 * NEON instructions. This is different from element-wise operations as it
 * operates on adjacent pairs within the same vector.
 * 
 * Operation Semantics:
 *   - Takes adjacent pairs (a[0],a[1]), (a[2],a[3]), etc.
 *   - Computes maximum of each pair
 *   - Results in vector with half the elements
 * 
 * NEON Implementation:
 *   - FMAXP.4S for 32-bit floats (pairwise maximum)
 *   - SMAXP.4S for 32-bit integers (pairwise maximum)
 *   - Cross-lane operations within 128-bit SIMD registers
 * 
 * Initialization Strategy:
 *   - No explicit initialization required
 *   - Operates on existing vector data
 *   - Natural reduction pattern for hierarchical maximization
 * 
 * Use Cases:
 *   - Hierarchical reduction trees
 *   - Fast maximum finding in small vectors
 *   - Parallel reduction algorithms
 * 
 * Vector Transformation:
 *   Input:  [a, b, c, d, e, f, g, h]
 *   Output: [max(a,b), max(c,d), max(e,f), max(g,h)]
 * 
 * Example Usage:
 *   LET data = [3, 1, 4, 2, 8, 5, 7, 6]
 *   LET result = PAIRWISE_MAX(data)  // [3, 4, 8, 7]
 */
class PairwiseMaxReducer : public Reducer {
public:
    std::string getName() const override { 
        return "PAIRWISE_MAX"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Greater; // Placeholder
    }
    
    int getReductionOp() const override { 
        return 11; // New operation code for pairwise max
    }
    
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * PairwiseAddReducer - NEON-accelerated pairwise addition reduction
 * 
 * Performs pairwise addition operations within vector lanes using specialized
 * NEON instructions. This is different from element-wise operations as it
 * operates on adjacent pairs within the same vector.
 * 
 * Operation Semantics:
 *   - Takes adjacent pairs (a[0],a[1]), (a[2],a[3]), etc.
 *   - Computes sum of each pair
 *   - Results in vector with half the elements
 * 
 * NEON Implementation:
 *   - FADDP.4S for 32-bit floats (pairwise addition)
 *   - ADDP.4S for 32-bit integers (pairwise addition)
 *   - Cross-lane operations within 128-bit SIMD registers
 * 
 * Initialization Strategy:
 *   - No explicit initialization required
 *   - Operates on existing vector data
 *   - Natural reduction pattern for hierarchical summation
 * 
 * Use Cases:
 *   - Fast dot product computation (when combined with multiplication)
 *   - Hierarchical summation trees
 *   - Parallel accumulation algorithms
 *   - Complex number operations (real + imaginary pairs)
 * 
 * Vector Transformation:
 *   Input:  [a, b, c, d, e, f, g, h]
 *   Output: [a+b, c+d, e+f, g+h]
 * 
 * Example Usage:
 *   LET data = [1, 2, 3, 4, 5, 6, 7, 8]
 *   LET result = PAIRWISE_ADD(data)  // [3, 7, 11, 15]
 */
class PairwiseAddReducer : public Reducer {
public:
    std::string getName() const override { 
        return "PAIRWISE_ADD"; 
    }
    
    BinaryOp::Operator getOperator() const override { 
        return BinaryOp::Operator::Add; // Placeholder
    }
    
    int getReductionOp() const override { 
        return 12; // New operation code for pairwise add
    }
    
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};
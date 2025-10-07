#pragma once

#include "AST.h"
#include <string>
#include <memory>

/**
 * Reducer Interface - Abstract base class for reduction operations
 * 
 * This interface is the cornerstone of the modern NeonReducer architecture,
 * providing a clean, type-safe way to define vector reduction operations.
 * It replaces the legacy ReductionCodeGen system with a modular design
 * that integrates seamlessly with the compiler's CFG infrastructure.
 * 
 * Key Design Principles:
 * - Each reduction operation (MIN, MAX, SUM, etc.) has its own class
 * - Operations are defined abstractly, separate from NEON implementation
 * - Easy extensibility through factory pattern
 * - Integration with CFG optimization passes
 * 
 * Architecture Benefits:
 * - Type Safety: Compile-time validation of operations
 * - Modularity: Easy to add new reduction types
 * - Performance: NEON encoders registered via NeonReducerRegistry
 * - Safety: CFG integration eliminates register clobbering bugs
 * 
 * NewBCPL Vector Type System Support:
 * - PAIR: 2×32-bit integers (SMIN.2S, SMAX.2S, ADD.2S)
 * - FPAIR: 2×32-bit floats (FMIN.2S, FMAX.2S, FADD.2S)
 * - QUAD: 4×32-bit integers (SMIN.4S, SMAX.4S, ADD.4S)
 * - FQUAD: 4×16-bit half-precision floats (FMIN.4H, FMAX.4H, FADD.4H)
 * - VEC: Dynamic integer vectors (chunked 4S processing)
 * - FVEC: Dynamic float vectors (chunked 4S processing)
 * - OCT/FOCT: 8-element vectors (NYIMP - chunked processing planned)
 * 
 * @see NeonReducerRegistry for NEON-specific implementations
 * @see CFGBuilderPass::visit(MinStatement&) for CFG integration
 * @see docs/Vector_Type_Reducer_Integration.md for complete type system guide
 */
class Reducer {
public:
    virtual ~Reducer() = default;

    /**
     * Get the name of the reduction operation (e.g., "MIN", "MAX", "SUM")
     * 
     * This name is used for:
     * - Factory dispatch in createReducer()
     * - Error messages and debugging output
     * - NEON intrinsic name generation
     * - CFG basic block naming
     * 
     * @return String name of the operation (must be uppercase)
     */
    virtual std::string getName() const = 0;

    /**
     * Get the binary operator to use in the reduction loop
     * 
     * This defines the core scalar operation that combines two elements
     * when generating CFG-based reduction loops. For example:
     * - MIN uses BinaryOp::Operator::Less for (a < b ? a : b)
     * - MAX uses BinaryOp::Operator::Greater for (a > b ? a : b)  
     * - SUM uses BinaryOp::Operator::Add for (a + b)
     * 
     * Note: NEON implementations may use different instructions
     * (e.g., SMIN.4S for MIN) but the scalar fallback uses this operator.
     * 
     * @return BinaryOp::Operator for the reduction's scalar operation
     */
    virtual BinaryOp::Operator getOperator() const = 0;

    /**
     * Get the reduction operation code for NEON encoder dispatch
     * 
     * This integer code is used by:
     * - NeonReducerRegistry to find appropriate NEON encoders
     * - CFG metadata to preserve operation type through optimization passes
     * - Code generation to select correct instruction sequences
     * 
     * Standard operation codes:
     * - 0: MIN (minimum reduction)
     * - 1: MAX (maximum reduction)
     * - 2: SUM (addition reduction)
     * - 3: PRODUCT (multiplication reduction)
     * - 4: BITWISE_AND (bitwise AND reduction)
     * - 5: BITWISE_OR (bitwise OR reduction)
     * - 10-12: Pairwise operations (NEON-specific)
     * 
     * @return Reduction operation code (integer)
     */
    virtual int getReductionOp() const = 0;

    /**
     * Get an optional initial value for the result variable
     * 
     * Some reduction operations require explicit initialization:
     * - SUM: Initialize to 0 (additive identity)
     * - PRODUCT: Initialize to 1 (multiplicative identity)
     * - BITWISE_AND: Initialize to 0xFFFFFFFF (all bits set)
     * - BITWISE_OR: Initialize to 0 (no bits set)
     * 
     * Others use the first element as the starting value:
     * - MIN: Start with first element, compare with rest
     * - MAX: Start with first element, compare with rest
     * 
     * This initialization is used by:
     * - CFGBuilderPass to generate proper loop prologue
     * - Scalar fallback implementations
     * - NEON encoders that need accumulator initialization
     * 
     * @return nullptr if no initialization needed, or Expression for initial value
     */
    virtual std::unique_ptr<Expression> getInitialValue() const {
        return nullptr;
    }

    /**
     * Check if this reduction requires initialization of the result
     * 
     * Convenience method that checks if getInitialValue() returns a value.
     * Used by code generation to determine whether to emit initialization code.
     * 
     * @return true if getInitialValue() should be used, false otherwise
     */
    virtual bool requiresInitialization() const {
        return getInitialValue() != nullptr;
    }

    /**
     * Get a string representation of the initial value for debugging
     * 
     * Provides human-readable representation of the initialization value
     * for error messages, debugging output, and compiler tracing.
     * 
     * @return String representation, or "none" if no initialization
     */
    virtual std::string getInitialValueString() const {
        if (auto init = getInitialValue()) {
            if (auto* num_lit = dynamic_cast<NumberLiteral*>(init.get())) {
                if (num_lit->literal_type == NumberLiteral::LiteralType::Integer) {
                    return std::to_string(num_lit->int_value);
                } else {
                    return std::to_string(num_lit->float_value);
                }
            }
            return "complex_expression";
        }
        return "none";
    }
};

/**
 * Factory function to create reducer instances
 * 
 * This is the main entry point for creating reducer objects from string names.
 * Used by:
 * - Parser when processing MIN(), MAX(), SUM() function calls
 * - CFGBuilderPass when creating reduction CFG nodes
 * - Error checking and validation passes
 * 
 * The factory uses a static map with lambda factories to avoid static
 * initialization order issues and provide fast lookup.
 * 
 * Supported operations:
 * - "MIN": MinReducer for element-wise minimum
 * - "MAX": MaxReducer for element-wise maximum
 * - "SUM": SumReducer for element-wise addition
 * - "PRODUCT": ProductReducer for element-wise multiplication
 * - "BITWISE_AND": BitwiseAndReducer for element-wise AND
 * - "BITWISE_OR": BitwiseOrReducer for element-wise OR
 * - "PAIRWISE_MIN": PairwiseMinReducer for NEON pairwise minimum
 * - "PAIRWISE_MAX": PairwiseMaxReducer for NEON pairwise maximum
 * - "PAIRWISE_ADD": PairwiseAddReducer for NEON pairwise addition
 * 
 * @param operation_name Name of the operation (case-sensitive, uppercase)
 * @return Unique pointer to appropriate reducer, or nullptr if unknown operation
 */
std::unique_ptr<Reducer> createReducer(const std::string& operation_name);

/**
 * Check if a given name is a supported reduction operation
 * 
 * Convenience function that attempts to create a reducer and checks
 * if the result is valid. Used for validation in parser and error checking.
 * 
 * @param operation_name Name to check (case-sensitive, uppercase)
 * @return true if supported, false otherwise
 */
bool isReductionOperation(const std::string& operation_name);
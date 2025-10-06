#pragma once

#include "AST.h"
#include <string>
#include <memory>

/**
 * Reducer Interface - Abstract base class for reduction operations
 * 
 * This interface encapsulates the logic of a specific reduction operation,
 * making the system modular and extensible. Each reduction type (MIN, MAX, SUM, etc.)
 * implements this interface to define its specific behavior.
 */
class Reducer {
public:
    virtual ~Reducer() = default;

    /**
     * Get the name of the reduction operation (e.g., "MIN", "MAX", "SUM")
     * Used for debugging and error messages
     * @return String name of the operation
     */
    virtual std::string getName() const = 0;

    /**
     * Get the binary operator to use in the reduction loop
     * This defines the core operation that combines two elements
     * @return BinaryOp::Operator for the reduction
     */
    virtual BinaryOp::Operator getOperator() const = 0;

    /**
     * Get the reduction operation enum value
     * Used by the existing ReductionCodeGen system for NEON optimizations
     * @return ReductionOp enum value
     */
    virtual int getReductionOp() const = 0;

    /**
     * Get an optional initial value for the result variable
     * Some reductions (like SUM) need initialization, others don't
     * @return nullptr if no initialization needed, or Expression for initial value
     */
    virtual std::unique_ptr<Expression> getInitialValue() const {
        return nullptr;
    }

    /**
     * Check if this reduction requires initialization of the result
     * @return true if getInitialValue() should be used
     */
    virtual bool requiresInitialization() const {
        return getInitialValue() != nullptr;
    }

    /**
     * Get a string representation of the initial value for debugging
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
 * @param operation_name Name of the operation ("MIN", "MAX", "SUM", etc.)
 * @return Unique pointer to appropriate reducer, or nullptr if unknown operation
 */
std::unique_ptr<Reducer> createReducer(const std::string& operation_name);

/**
 * Check if a given name is a supported reduction operation
 * @param operation_name Name to check
 * @return true if supported, false otherwise
 */
bool isReductionOperation(const std::string& operation_name);
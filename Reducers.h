#pragma once

#include "Reducer.h"
#include "reductions.h"

/**
 * Concrete Reducer Implementations
 * 
 * These classes implement the Reducer interface for specific reduction operations.
 * Each reducer encapsulates the behavior of one reduction type, making the system
 * modular and easily extensible.
 */

/**
 * MinReducer - Element-wise minimum reduction
 * Finds the minimum value at each position between two vectors
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
        return static_cast<int>(ReductionOp::MIN); 
    }
    
    // MIN doesn't require initialization - uses first element as starting value
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * MaxReducer - Element-wise maximum reduction  
 * Finds the maximum value at each position between two vectors
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
        return static_cast<int>(ReductionOp::MAX); 
    }
    
    // MAX doesn't require initialization - uses first element as starting value
    std::unique_ptr<Expression> getInitialValue() const override {
        return nullptr;
    }
};

/**
 * SumReducer - Element-wise addition reduction
 * Adds corresponding elements from two vectors
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
        return static_cast<int>(ReductionOp::SUM); 
    }
    
    // SUM requires initialization to zero
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(0));
    }
};

/**
 * ProductReducer - Element-wise multiplication reduction (future extension)
 * Multiplies corresponding elements from two vectors
 * 
 * Note: This requires extending the ReductionOp enum in reductions.h
 * and adding NEON support in ReductionCodeGen
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
        // This would need to be added to ReductionOp enum
        return 3; // Placeholder for ReductionOp::PRODUCT
    }
    
    // PRODUCT requires initialization to one
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(1));
    }
};

/**
 * BitwiseAndReducer - Element-wise bitwise AND reduction (future extension)
 * Performs bitwise AND on corresponding elements from two vectors
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
        // This would need to be added to ReductionOp enum
        return 4; // Placeholder for ReductionOp::BITWISE_AND
    }
    
    // Bitwise AND requires initialization to all 1s (0xFFFFFFFF for 32-bit)
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(-1)); // All bits set
    }
};

/**
 * BitwiseOrReducer - Element-wise bitwise OR reduction (future extension)
 * Performs bitwise OR on corresponding elements from two vectors
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
        // This would need to be added to ReductionOp enum
        return 5; // Placeholder for ReductionOp::BITWISE_OR
    }
    
    // Bitwise OR requires initialization to zero
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(static_cast<int64_t>(0));
    }
};

/**
 * PairwiseMinReducer - NEON-accelerated pairwise minimum reduction
 * Uses vpmin NEON intrinsic for finding minimum of pairs within vectors
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
 * Uses vpmax NEON intrinsic for finding maximum of pairs within vectors
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
 * Uses vpadd NEON intrinsic for adding pairs within vectors
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
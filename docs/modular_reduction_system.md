# Modular Reduction System Documentation

## Overview

The NewBCPL compiler has been refactored to use a modular reduction system that makes adding new reduction operations much easier and more maintainable. This document explains the new architecture and how to use it.

## Architecture

### Before: Monolithic Design

The original implementation had several drawbacks:

```cpp
// Separate AST nodes for each operation
class MinStatement : public Statement { /* ... */ };
class MaxStatement : public Statement { /* ... */ };
class SumStatement : public Statement { /* ... */ };

// Separate visitor methods
void CFGBuilderPass::visit(MinStatement& node) { /* ... */ }
void CFGBuilderPass::visit(MaxStatement& node) { /* ... */ }
void CFGBuilderPass::visit(SumStatement& node) { /* ... */ }

// Switch statement in generateReductionCFG
void generateReductionCFG(/* ... */, int op) {
    switch(op) {
        case 0: /* MIN logic */ break;
        case 1: /* MAX logic */ break;
        case 2: /* SUM logic */ break;
    }
}
```

**Problems:**
- Adding new operations required changes to multiple files
- Integer operation codes were error-prone
- Reduction logic was tightly coupled with AST/CFG construction
- Large switch statements became hard to maintain

### After: Modular Design

The new system uses a **Reducer Interface** with concrete implementations:

```cpp
// Single AST node for all reductions
class ReductionStatement : public Statement {
    std::unique_ptr<Reducer> reducer;
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
};

// Single visitor method
void CFGBuilderPass::visit(ReductionStatement& node) {
    // Handle any reduction type using the reducer interface
    generateReductionCFG(/*...*/, *node.reducer);
}

// Pluggable reducer implementations
class MinReducer : public Reducer { /* ... */ };
class MaxReducer : public Reducer { /* ... */ };
class SumReducer : public Reducer { /* ... */ };
```

## Core Components

### 1. Reducer Interface (`Reducer.h`)

The abstract base class that defines the contract for all reduction operations:

```cpp
class Reducer {
public:
    virtual ~Reducer() = default;
    
    // Operation metadata
    virtual std::string getName() const = 0;
    virtual BinaryOp::Operator getOperator() const = 0;
    virtual int getReductionOp() const = 0;
    
    // Optional initialization
    virtual std::unique_ptr<Expression> getInitialValue() const;
    virtual bool requiresInitialization() const;
    virtual std::string getInitialValueString() const;
};
```

### 2. Concrete Reducers (`Reducers.h`)

Implementations for specific reduction operations:

#### MinReducer
```cpp
class MinReducer : public Reducer {
public:
    std::string getName() const override { return "MIN"; }
    BinaryOp::Operator getOperator() const override { return BinaryOp::Operator::Less; }
    int getReductionOp() const override { return static_cast<int>(ReductionOp::MIN); }
    // No initialization needed - uses first element as starting value
    std::unique_ptr<Expression> getInitialValue() const override { return nullptr; }
};
```

#### SumReducer
```cpp
class SumReducer : public Reducer {
public:
    std::string getName() const override { return "SUM"; }
    BinaryOp::Operator getOperator() const override { return BinaryOp::Operator::Add; }
    int getReductionOp() const override { return static_cast<int>(ReductionOp::SUM); }
    // SUM requires initialization to zero
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(0);
    }
};
```

### 3. Factory Pattern (`Reducer.cpp`)

Creates reducer instances by name:

```cpp
std::unique_ptr<Reducer> createReducer(const std::string& operation_name);
bool isReductionOperation(const std::string& operation_name);
```

### 4. Unified AST Node (`AST.h`)

Single ReductionStatement that works with any reducer:

```cpp
class ReductionStatement : public Statement {
public:
    std::unique_ptr<Reducer> reducer;
    std::string result_variable;
    ExprPtr left_operand;
    ExprPtr right_operand;
    
    ReductionStatement(std::unique_ptr<Reducer> reducer, 
                      std::string result_var, 
                      ExprPtr left, 
                      ExprPtr right);
};
```

## Usage Examples

### Creating Reduction Statements

```cpp
// Using factory pattern
auto min_reducer = createReducer("MIN");
auto min_statement = std::make_unique<ReductionStatement>(
    std::move(min_reducer),
    "result_var",
    std::make_unique<VariableAccess>("vector_a"),
    std::make_unique<VariableAccess>("vector_b")
);

// Direct instantiation
auto sum_statement = std::make_unique<ReductionStatement>(
    std::make_unique<SumReducer>(),
    "sum_result",
    std::make_unique<VariableAccess>("data1"),
    std::make_unique<VariableAccess>("data2")
);
```

### CFG Building

The CFGBuilderPass automatically handles initialization and CFG generation:

```cpp
void CFGBuilderPass::visit(ReductionStatement& node) {
    // Handle initialization if needed
    if (node.reducer->requiresInitialization()) {
        // Generate: result_var := initial_value
        // (e.g., sum_result := 0)
    }
    
    // Generate the reduction CFG
    generateReductionCFG(node.left_operand.get(), 
                        node.right_operand.get(),
                        node.result_variable, 
                        *node.reducer);
}
```

## Adding New Reduction Operations

Adding a new reduction operation is now straightforward and requires no changes to existing code:

### Step 1: Create the Reducer Class

```cpp
// In Reducers.h
class ProductReducer : public Reducer {
public:
    std::string getName() const override { return "PRODUCT"; }
    BinaryOp::Operator getOperator() const override { return BinaryOp::Operator::Multiply; }
    int getReductionOp() const override { return static_cast<int>(ReductionOp::PRODUCT); }
    
    // PRODUCT requires initialization to 1
    std::unique_ptr<Expression> getInitialValue() const override {
        return std::make_unique<NumberLiteral>(1);
    }
};
```

### Step 2: Add to Factory

```cpp
// In Reducer.cpp
std::unordered_map<std::string, std::function<std::unique_ptr<Reducer>()>> reducer_factories = {
    {"MIN", []() { return std::make_unique<MinReducer>(); }},
    {"MAX", []() { return std::make_unique<MaxReducer>(); }},
    {"SUM", []() { return std::make_unique<SumReducer>(); }},
    {"PRODUCT", []() { return std::make_unique<ProductReducer>(); }}, // New!
};
```

### Step 3: Extend Backend Support (Optional)

For full functionality, you may need to:

1. **Extend ReductionOp enum** in `reductions.h`:
```cpp
enum class ReductionOp {
    MIN,
    MAX,
    SUM,
    PRODUCT  // New!
};
```

2. **Add NEON support** in `ReductionCodeGen`:
```cpp
void ReductionCodeGen::generateReduction(/*...*/) {
    switch(op) {
        case ReductionOp::MIN: /* SMIN.4S */ break;
        case ReductionOp::MAX: /* SMAX.4S */ break;
        case ReductionOp::SUM: /* ADD.4S */ break;
        case ReductionOp::PRODUCT: /* MUL.4S */ break; // New!
    }
}
```

## Benefits

### 1. Extensibility
- Adding new reductions requires minimal code changes
- No modification of AST visitor pattern
- No changes to CFG building infrastructure

### 2. Reusability
- Reducer classes can be used in other parts of the compiler
- Logic is self-contained and portable
- Easy to create libraries of reduction operations

### 3. Separation of Concerns
- Reduction logic separated from AST structure
- CFG building separated from operation specifics
- Clear interfaces between components

### 4. Maintainability
- No large switch statements
- Type-safe operation dispatch
- Clear, self-documenting code

### 5. Testability
- Individual reducers can be unit tested
- Mock reducers can be created for testing
- Easier to test edge cases

## Migration Guide

### For Existing Code

The old MinStatement, MaxStatement, and SumStatement classes are still supported but marked as deprecated:

```cpp
// OLD (still works, but deprecated)
auto min_stmt = std::make_unique<MinStatement>("result", left_expr, right_expr);

// NEW (recommended)
auto min_stmt = std::make_unique<ReductionStatement>(
    createReducer("MIN"), "result", left_expr, right_expr
);
```

### For Parser Integration

Update your parser to generate ReductionStatement nodes:

```cpp
// In parser
if (token == "MIN" || token == "MAX" || token == "SUM") {
    auto reducer = createReducer(token);
    if (!reducer) {
        throw ParseError("Unknown reduction operation: " + token);
    }
    
    return std::make_unique<ReductionStatement>(
        std::move(reducer), result_var, left_expr, right_expr
    );
}
```

## Performance Considerations

The modular system has minimal performance overhead:

- **Compile time**: Virtual function calls are resolved at compile time when possible
- **Runtime**: Same NEON instruction generation as before
- **Memory**: Slight increase due to vtable pointers, but negligible

For performance-critical code, consider the template-based approach mentioned in the original proposal:

```cpp
template<typename ReducerFunc>
void generateReductionCFG(Expression* left, Expression* right, 
                         const std::string& result, ReducerFunc reducer_op);
```

## Future Extensions

The modular system enables many future enhancements:

### 1. Composite Reductions
```cpp
class MeanReducer : public Reducer {
    // Internally uses SumReducer + division
};
```

### 2. Conditional Reductions
```cpp
class ConditionalSumReducer : public Reducer {
    std::unique_ptr<Expression> condition;
    // Only sum elements that meet condition
};
```

### 3. Multi-Input Reductions
```cpp
class WeightedAverageReducer : public Reducer {
    // Takes values and weights as separate inputs
};
```

### 4. Custom Initialization
```cpp
class ClampedMinReducer : public Reducer {
    int min_allowed_value;
    // Initialize with custom minimum value
};
```

## Debugging and Tracing

The new system provides better debugging capabilities:

```cpp
// Enable tracing in CFGBuilderPass
CFGBuilderPass pass(symbol_table, true /* trace_enabled */);

// Output shows:
// [CFGBuilderPass] visit(ReductionStatement) entered for operation: MIN
// [CFGBuilderPass] Building CFG for MIN reduction operation (modern interface)
// [CFGBuilderPass] visit(ReductionStatement) exiting for operation: MIN
```

Each reducer provides debugging information:
```cpp
std::cout << "Reducer: " << reducer->getName() << std::endl;
std::cout << "Requires init: " << reducer->requiresInitialization() << std::endl;
std::cout << "Initial value: " << reducer->getInitialValueString() << std::endl;
```

## Conclusion

The modular reduction system represents a significant improvement in the NewBCPL compiler architecture. It demonstrates how careful application of design patterns can make a system more maintainable, extensible, and robust while preserving performance.

The system is ready for production use and provides a solid foundation for future enhancements to the reduction capabilities of the NewBCPL language.
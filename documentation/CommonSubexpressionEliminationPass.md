<file_path>
NewBCPL/CommonSubexpressionEliminationPass.md
</file_path>

<edit_description>
Document CommonSubexpressionEliminationPass class
</edit_description>

```
# CommonSubexpressionEliminationPass Class Documentation

## Overview

The `CommonSubexpressionEliminationPass` class is an optimization pass designed to identify and eliminate redundant computations in the Abstract Syntax Tree (AST). By detecting common subexpressions, it reduces the number of redundant operations, improving the efficiency of the generated code.

---

## Key Features

- **Expression Canonicalization**: Converts expressions into a canonical string representation for comparison.
- **Temporary Variable Generation**: Creates temporary variables to store the results of common subexpressions.
- **Hoisting**: Moves common subexpressions out of their original context and stores them in temporary variables for reuse.
- **Integration with AST**: Traverses and modifies the AST to replace redundant expressions with variable accesses.

---

## Methods

### `CommonSubexpressionEliminationPass(std::unordered_map<std::string, int64_t>& manifests)`
Constructor that initializes the optimizer with manifest constants.

### `std::string generate_temp_var_name()`
Generates a unique name for temporary variables used to store common subexpressions.

### `std::string expression_to_string(Expression* expr)`
Converts an expression into a canonical string representation for comparison.

### `void visit(FunctionDeclaration& node)`
Processes a function declaration, resetting the state for each function and hoisting common subexpressions.

### `void visit(RoutineDeclaration& node)`
Processes a routine declaration, resetting the state for each routine and hoisting common subexpressions.

### `void visit(AssignmentStatement& node)`
Detects common subexpressions in assignment statements and replaces them with variable accesses.

### `void visit(BinaryOp& node)`
Optimizes binary operations by recursively visiting their operands.

### `void visit(UnaryOp& node)`
Optimizes unary operations by recursively visiting their operand.

### `void visit(FunctionCall& node)`
Optimizes function calls by recursively visiting the function expression and arguments.

### `void visit(VectorAccess& node)`
Optimizes vector access expressions by recursively visiting their components.

### `void visit(CharIndirection& node)`
Optimizes character indirection expressions by recursively visiting their components.

### `void visit(FloatVectorIndirection& node)`
Optimizes floating-point vector indirection expressions by recursively visiting their components.

### `void visit(ConditionalExpression& node)`
Optimizes conditional expressions by recursively visiting their components.

### `void visit(ValofExpression& node)`
Processes `valof` expressions, which are treated as unique due to their block nature.

---

## Example Workflow

1. **Canonicalization**: Expressions are converted into a canonical string representation using `expression_to_string`.
2. **Detection**: The optimizer checks if the canonicalized expression already exists in the `available_expressions_` map.
3. **Hoisting**: If a common subexpression is detected, it is hoisted into a temporary variable using `generate_temp_var_name`.
4. **Replacement**: The original expression is replaced with a `VariableAccess` to the temporary variable.
5. **Insertion**: Hoisted declarations are inserted at the beginning of the function or routine body.

---

## Limitations

- **Side Effects**: The optimizer does not handle expressions with side effects, such as function calls with external dependencies.
- **Complex Expressions**: Certain complex expressions, like `valof`, are treated as unique and not optimized.
- **Non-Block Bodies**: Hoisted declarations are not inserted for functions or routines with non-block bodies.

---

## Summary

The `CommonSubexpressionEliminationPass` class is a powerful optimization tool that enhances the efficiency of the generated code by reducing redundant computations. Its integration with the AST and ability to hoist common subexpressions make it a critical component of the optimization pipeline.
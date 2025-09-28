# Plan for Common Subexpression Elimination (CSE) Pass

## Goal
Implement a basic CSE pass that identifies and replaces common subexpressions within a single function/routine scope. This implementation will adhere to the documented limitation of not handling control flow (labels, function calls, variable reassignments within expressions) correctly, meaning it's primarily safe for straight-line code within a function.

## Key Concepts from Documentation
*   **Identify:** Create a unique, canonical representation of a subexpression.
*   **Hoist:** On the first encounter, generate a new temporary variable and assign the subexpression's result to it.
*   **Replace:** On all encounters (including the first), replace the subexpression with an access to the temporary variable.
*   **Limitation:** The current implementation is only safe for use within a single basic block (or rather, within the current traversal context, acknowledging the limitation regarding control flow).

## Steps

1.  **Create `CommonSubexpressionEliminationPass.h`:**
    *   Define `class CommonSubexpressionEliminationPass : public Optimizer`.
    *   Override `getName()`.
    *   Declare `visit` methods for relevant nodes:
        *   `visit(FunctionDeclaration& node)` and `visit(RoutineDeclaration& node)`: To reset the CSE state (clear `available_expressions_` and `temp_var_counter_`) at the beginning of each function/routine.
        *   `visit(AssignmentStatement& node)`: This is where the core logic for identifying and replacing subexpressions will reside.
        *   `visit(BinaryOp& node)`, `visit(UnaryOp& node)`, `visit(FunctionCall& node)`, `visit(VectorAccess& node)`, `visit(CharIndirection& node)`, `visit(FloatVectorIndirection& node)`: These will be the expression types that we attempt to identify as common subexpressions. Their `visit` methods will primarily ensure their children are optimized.
    *   Declare private members:
        *   `std::unordered_map<std::string, std::string> available_expressions_`: Maps canonical expression string to the name of the temporary variable holding its result.
        *   `int temp_var_counter_`: To generate unique temporary variable names.
        *   `std::string generate_temp_var_name()`: Helper to create unique temporary variable names (e.g., `_cse_temp_0`).
        *   `std::string expression_to_string(Expression* expr)`: Helper to create a canonical string representation of an expression. This will be a crucial part.
        *   `std::vector<DeclPtr> hoisted_declarations_`: A list to collect `LetDeclaration` nodes for hoisted temporary variables. These will be inserted at the beginning of the function/routine body.

2.  **Create `CommonSubexpressionEliminationPass.cpp`:**

    *   **Constructor:** Initialize `Optimizer` base and `temp_var_counter_`.
    *   **`getName()`:** Return "Common Subexpression Elimination Pass".
    *   **`generate_temp_var_name()`:** Simple implementation to return `_cse_temp_N` and increment counter.

    *   **`expression_to_string(Expression* expr)`:**
        *   Implement this as a recursive helper function.
        *   For `NumberLiteral`, `StringLiteral`, `CharLiteral`, `BooleanLiteral`: Return their literal value as a string.
        *   For `VariableAccess`: Return `(VAR <name>)`.
        *   For `BinaryOp`: Return `(<OP> <left_str> <right_str>)`.
        *   For `UnaryOp`: Return `(<OP> <operand_str>)`.
        *   For `FunctionCall`: Return `(CALL <func_str> <arg1_str> <arg2_str> ...)`.
        *   For `VectorAccess`, `CharIndirection`, `FloatVectorIndirection`: Return `(ACCESS <base_str> <index_str>)`.
        *   **Crucial:** This function must produce a *stable* and *unique* string for identical expressions.

    *   **`visit(FunctionDeclaration& node)` and `visit(RoutineDeclaration& node)`:**
        *   Clear `available_expressions_`.
        *   Reset `temp_var_counter_ = 0`.
        *   Clear `hoisted_declarations_`.
        *   Call `Optimizer::visit(node)` to continue traversal of the function/routine body.
        *   **After traversal of the body:** Insert `hoisted_declarations_` at the beginning of the function/routine's body (if it's a `BlockStatement` or `CompoundStatement`). This will require modifying the AST structure.

    *   **`visit(AssignmentStatement& node)`:**
        *   Iterate through `node.rhs` (right-hand side expressions).
        *   For each `rhs_expr`:
            *   Call `rhs_expr = visit_expr(std::move(rhs_expr))` to ensure its children are optimized first.
            *   Generate `canonical_expr_str = expression_to_string(rhs_expr.get())`.
            *   Check `available_expressions_`:
                *   If `available_expressions_.count(canonical_expr_str)`:
                    *   `temp_var_name = available_expressions_[canonical_expr_str]`.
                    *   Replace `rhs_expr` with `std::make_unique<VariableAccess>(temp_var_name)`.
                *   Else (new common subexpression):
                    *   `temp_var_name = generate_temp_var_name()`.
                    *   `available_expressions_[canonical_expr_str] = temp_var_name`.
                    *   **Hoist:** Create a new `LetDeclaration` for `temp_var_name` and an `AssignmentStatement` to assign `rhs_expr` to `temp_var_name`. Add these to `hoisted_declarations_`.
                    *   Replace `rhs_expr` with `std::make_unique<VariableAccess>(temp_var_name)`.

    *   **`visit(BinaryOp& node)`, `visit(UnaryOp& node)`, etc. (Expression nodes):**
        *   These `visit` methods will primarily call `Optimizer::visit` on their children to ensure they are processed. The actual CSE logic for these expressions will be handled by `expression_to_string` and the `AssignmentStatement` visitor.

## Considerations and Challenges

*   **Side Effects:** The current plan does not account for side effects (e.g., function calls, I/O operations) within expressions. An expression with side effects should not be hoisted. This is a known limitation of the basic block CSE.
*   **Variable Liveness/Reassignment:** The documentation explicitly mentions this limitation: "The availableExpressions map is not cleared at labels, after function calls with potential side effects, or when a variable within a subexpression is reassigned." This means the current CSE will be very basic and might produce incorrect results if these conditions are met. This should be clearly documented as a known limitation.
*   **Insertion of Hoisted Declarations:** Carefully inserting the `hoisted_declarations_` at the correct place (beginning of the function/routine body) will be crucial. This might involve modifying `visit(BlockStatement& node)` or `visit(CompoundStatement& node)` to prepend the new declarations/assignments.

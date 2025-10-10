#include "LoopInvariantCodeMotionPass.h"
#include "analysis/Visitors/VariableUsageVisitor.h"
#include <iostream>
#include <vector>
#include <set>
#include <string>

// --- Public Methods ---

/**
 * @brief Constructs the pass with necessary references to other compiler components.
 */
LoopInvariantCodeMotionPass::LoopInvariantCodeMotionPass(
    std::unordered_map<std::string, int64_t>& manifests,
    SymbolTable& symbol_table,
    ASTAnalyzer& analyzer,
    bool enable_tracing
) : Optimizer(manifests),
    symbol_table_(symbol_table),
    analyzer_(analyzer),
    temp_var_factory_(),
    enable_tracing_(enable_tracing) {}

/**
 * @brief Main entry point for applying the optimization pass to the AST.
 */
ProgramPtr LoopInvariantCodeMotionPass::apply(ProgramPtr program) {
    if (program) {
        program->accept(*this);
    }
    return program;
}

// --- Visitor Overrides ---

/**
 * @brief Visits a FunctionDeclaration to set the current function context.
 */
void LoopInvariantCodeMotionPass::visit(FunctionDeclaration& node) {
    current_function_name_ = node.name;
    if (node.body) {
        node.body->accept(*this);
    }
}

/**
 * @brief Visits a RoutineDeclaration to set the current function context.
 */
void LoopInvariantCodeMotionPass::visit(RoutineDeclaration& node) {
    current_function_name_ = node.name;
    if (node.body) {
        node.body->accept(*this);
    }
}

/**
 * @brief Analyzes a FOR loop for invariant code.
 */
void LoopInvariantCodeMotionPass::visit(ForStatement& node) {
    if (!node.body) return;

    // 1. Identify all variables that are modified within this loop.
    VariableUsageVisitor def_visitor(&symbol_table_);
    node.body->accept(def_visitor);
    std::set<std::string> loop_defined_vars = def_visitor.getVariables();
    loop_defined_vars.insert(node.loop_variable); // The loop counter is also modified.

    // 2. Find and hoist invariant code out of the loop body.
    optimize_loop_body(node.body, loop_defined_vars);
}

/**
 * @brief Analyzes a WHILE loop for invariant code.
 */
void LoopInvariantCodeMotionPass::visit(WhileStatement& node) {
    if (!node.body) return;

    // 1. Identify all variables that are modified within this loop.
    VariableUsageVisitor def_visitor(&symbol_table_);
    node.body->accept(def_visitor);
    std::set<std::string> loop_defined_vars = def_visitor.getVariables();

    // 2. Find and hoist invariant code out of the loop body.
    optimize_loop_body(node.body, loop_defined_vars);
}

/**
 * @brief Analyzes a REPEAT loop for invariant code.
 */
void LoopInvariantCodeMotionPass::visit(RepeatStatement& node) {
    if (!node.body) return;

    // 1. Identify all variables that are modified within this loop.
    VariableUsageVisitor def_visitor(&symbol_table_);
    node.body->accept(def_visitor);
    std::set<std::string> loop_defined_vars = def_visitor.getVariables();

    // 2. Find and hoist invariant code out of the loop body.
    optimize_loop_body(node.body, loop_defined_vars);
}


// --- Private Helper Methods ---

/**
 * @brief Shared logic to find invariants and rewrite the loop body if any are found.
 * @param body A reference to the loop's body statement pointer.
 * @param loop_defined_vars A set of variable names that are modified within the loop.
 */
void LoopInvariantCodeMotionPass::optimize_loop_body(StmtPtr& body, const std::set<std::string>& loop_defined_vars) {
    hoisted_statements_.clear();
    findAndHoistInvariants(body, loop_defined_vars);

    // If any statements were hoisted, prepend them to the original loop body
    // by wrapping both in a new CompoundStatement.
    if (!hoisted_statements_.empty()) {
        std::vector<StmtPtr> new_body_stmts = std::move(hoisted_statements_);
        new_body_stmts.push_back(std::move(body));
        body = std::make_unique<CompoundStatement>(std::move(new_body_stmts));
    }
}

/**
 * @brief Determines if an expression is loop-invariant.
 * An expression is invariant if all variables it uses are defined outside the loop.
 */
bool LoopInvariantCodeMotionPass::isInvariant(Expression* expr, const std::set<std::string>& loop_defined_vars) {
    if (!expr) return true; // A null expression is trivially invariant.

    VariableUsageVisitor usage_visitor(&symbol_table_);
    expr->accept(usage_visitor);
    const auto& used_vars = usage_visitor.getVariables();

    // Check if any variable used in the expression is in the set of variables
    // modified inside the loop.
    for (const auto& var : used_vars) {
        if (loop_defined_vars.count(var)) {
            return false; // Dependency found, not invariant.
        }
    }
    return true; // No dependencies found, expression is invariant.
}

/**
 * @brief Traverses a statement's AST to find and hoist invariant expressions.
 */
void LoopInvariantCodeMotionPass::findAndHoistInvariants(StmtPtr& stmt, const std::set<std::string>& loop_defined_vars) {
    if (!stmt) return;

    // Case 1: Assignment Statement (the primary candidate for hoisting)
    if (auto* assign = dynamic_cast<AssignmentStatement*>(stmt.get())) {
        for (auto& rhs : assign->rhs) {
            if (isInvariant(rhs.get(), loop_defined_vars)) {

                // ✅ **FIX**: Correctly infer the type of the expression to be hoisted.
                VarType expr_type = analyzer_.infer_expression_type(rhs.get());

                // Create a new temporary variable to hold the invariant result.
                std::string temp_var = temp_var_factory_.create(
                    current_function_name_,
                    expr_type,
                    symbol_table_,
                    analyzer_
                );

                // ✅ **BUG FIX**: Check if temporary variable creation failed
                if (temp_var.empty()) {
                    std::cerr << "[LICM] Error: Failed to create temporary variable for function " 
                              << current_function_name_ << ". Skipping hoisting." << std::endl;
                    continue; // Skip this optimization to avoid AST corruption
                }

                if (enable_tracing_) {
                    std::cout << "[LICM] Hoisting invariant expression in function " << current_function_name_ << std::endl;
                }

                // Create a new LET statement: LET _tempN = <invariant_expr>
                std::vector<std::string> names = {temp_var};
                std::vector<ExprPtr> inits;
                // Clone the expression instead of moving it to preserve the original
                inits.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(rhs->clone().release())));
                // Lower LetDeclaration to AssignmentStatement before pushing to statements vector
                std::vector<ExprPtr> lhs_vec;
                for (const auto& name : names) {
                    lhs_vec.push_back(std::make_unique<VariableAccess>(name));
                }
                hoisted_statements_.push_back(std::make_unique<AssignmentStatement>(
                    std::move(lhs_vec),
                    std::move(inits)
                ));

                // Replace the original expression with a reference to the temporary variable.
                rhs = std::make_unique<VariableAccess>(temp_var);
            }
        }
    // Case 2 & 3: Recursively search inside blocks of statements.
    } else if (auto* block = dynamic_cast<BlockStatement*>(stmt.get())) {
        for (auto& s : block->statements) {
            findAndHoistInvariants(s, loop_defined_vars);
        }
    } else if (auto* compound = dynamic_cast<CompoundStatement*>(stmt.get())) {
        for (auto& s : compound->statements) {
            findAndHoistInvariants(s, loop_defined_vars);
        }
    }
}

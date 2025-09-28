#include "ASTAnalyzer.h"

// Visitor implementation for ValofExpression nodes
void ASTAnalyzer::visit(ValofExpression& node) {
    // Visit the body of the ValofExpression if present
    if (node.body) {
        // Record that we're in a VALOF expression
        std::string previous_function_scope = current_function_scope_;

        // Find any RESULTIS statements within the VALOF block
        node.body->accept(*this);

        // --- PRIORITIZE EXPLICIT RETURN TYPE ANNOTATION ---
        if (!current_function_scope_.empty()) {
            if (node.explicit_return_type != VarType::UNKNOWN) {
                function_return_types_[current_function_scope_] = node.explicit_return_type;
            } else {
                // Fallback to old logic: try to infer from RESULTIS statements
                bool has_float_ops = false;
                if (auto* block = dynamic_cast<BlockStatement*>(node.body.get())) {
                    for (const auto& stmt : block->statements) {
                        if (auto* resultis = dynamic_cast<ResultisStatement*>(stmt.get())) {
                            if (resultis->expression) {
                                if (auto* num_lit = dynamic_cast<NumberLiteral*>(resultis->expression.get())) {
                                    if (num_lit->literal_type == NumberLiteral::LiteralType::Float) {
                                        has_float_ops = true;
                                    }
                                }
                                else if (infer_expression_type(resultis->expression.get()) == VarType::FLOAT) {
                                    has_float_ops = true;
                                }
                                else if (auto* func_call = dynamic_cast<FunctionCall*>(resultis->expression.get())) {
                                    if (auto* var_access = dynamic_cast<VariableAccess*>(func_call->function_expr.get())) {
                                        if (var_access->name == "FLOAT") {
                                            has_float_ops = true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
                if (has_float_ops) {
                    function_return_types_[current_function_scope_] = VarType::FLOAT;
                }
            }
        }

        // Restore the previous function scope
        current_function_scope_ = previous_function_scope;
    }
}

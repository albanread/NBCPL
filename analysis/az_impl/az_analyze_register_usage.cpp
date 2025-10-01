#include "../ASTAnalyzer.h"
#include "../../AST.h"
#include <iostream>

void ASTAnalyzer::analyze_callee_saved_register_usage(const std::string& function_name, ASTNode* body) {
    if (!body) return;
    
    auto& metrics = function_metrics_[function_name];
    
    if (trace_enabled_) {
        std::cout << "[ASTAnalyzer] Analyzing callee-saved register usage for function: " << function_name << std::endl;
    }
    
    // Reset counters
    metrics.required_callee_saved_temps = 0;
    metrics.has_call_preserving_expressions = false;
    metrics.required_callee_saved_regs.clear();
    
    // Analyze the function body recursively
    analyze_node_for_register_usage(body, metrics);
    
    if (trace_enabled_) {
        std::cout << "[ASTAnalyzer] Function " << function_name 
                  << " requires " << metrics.required_callee_saved_temps 
                  << " callee-saved temp registers" << std::endl;
        if (!metrics.required_callee_saved_regs.empty()) {
            std::cout << "[ASTAnalyzer] Specific registers required: ";
            for (const auto& reg : metrics.required_callee_saved_regs) {
                std::cout << reg << " ";
            }
            std::cout << std::endl;
        }
    }
}

void ASTAnalyzer::analyze_node_for_register_usage(ASTNode* node, FunctionMetrics& metrics) {
    if (!node) return;
    
    // Check different node types that might need register preservation
    if (auto* binary_op = dynamic_cast<BinaryOp*>(node)) {
        analyze_binary_op_register_usage(binary_op, metrics);
    } else if (auto* resultis_stmt = dynamic_cast<ResultisStatement*>(node)) {
        analyze_node_for_register_usage(resultis_stmt->expression.get(), metrics);
    } else if (auto* assignment = dynamic_cast<AssignmentStatement*>(node)) {
        for (const auto& rhs_expr : assignment->rhs) {
            analyze_node_for_register_usage(rhs_expr.get(), metrics);
        }
    } else if (auto* if_stmt = dynamic_cast<IfStatement*>(node)) {
        analyze_node_for_register_usage(if_stmt->condition.get(), metrics);
        analyze_node_for_register_usage(if_stmt->then_branch.get(), metrics);
    } else if (auto* test_stmt = dynamic_cast<TestStatement*>(node)) {
        analyze_node_for_register_usage(test_stmt->condition.get(), metrics);
        analyze_node_for_register_usage(test_stmt->then_branch.get(), metrics);
        if (test_stmt->else_branch) {
            analyze_node_for_register_usage(test_stmt->else_branch.get(), metrics);
        }
    } else if (auto* block_stmt = dynamic_cast<BlockStatement*>(node)) {
        for (const auto& stmt : block_stmt->statements) {
            analyze_node_for_register_usage(stmt.get(), metrics);
        }
    } else if (auto* compound_stmt = dynamic_cast<CompoundStatement*>(node)) {
        for (const auto& stmt : compound_stmt->statements) {
            analyze_node_for_register_usage(stmt.get(), metrics);
        }
    } else if (auto* valof_expr = dynamic_cast<ValofExpression*>(node)) {
        analyze_node_for_register_usage(valof_expr->body.get(), metrics);
    } else if (auto* conditional_expr = dynamic_cast<ConditionalExpression*>(node)) {
        analyze_node_for_register_usage(conditional_expr->condition.get(), metrics);
        analyze_node_for_register_usage(conditional_expr->true_expr.get(), metrics);
        analyze_node_for_register_usage(conditional_expr->false_expr.get(), metrics);
    } else if (auto* unary_op = dynamic_cast<UnaryOp*>(node)) {
        analyze_node_for_register_usage(unary_op->operand.get(), metrics);
    } else if (auto* vec_access = dynamic_cast<VectorAccess*>(node)) {
        analyze_node_for_register_usage(vec_access->vector_expr.get(), metrics);
        analyze_node_for_register_usage(vec_access->index_expr.get(), metrics);
    } else if (auto* pair_expr = dynamic_cast<PairExpression*>(node)) {
        analyze_node_for_register_usage(pair_expr->first_expr.get(), metrics);
        analyze_node_for_register_usage(pair_expr->second_expr.get(), metrics);
    } else if (auto* fpair_expr = dynamic_cast<FPairExpression*>(node)) {
        analyze_node_for_register_usage(fpair_expr->first_expr.get(), metrics);
        analyze_node_for_register_usage(fpair_expr->second_expr.get(), metrics);
    }
    // Function calls don't need analysis here - they're the terminators we're looking for
}

void ASTAnalyzer::analyze_binary_op_register_usage(BinaryOp* binary_op, FunctionMetrics& metrics) {
    if (!binary_op) return;
    
    // Check if this is a call-preserving expression pattern like: variable * FUNCTION_CALL()
    bool left_has_call = expression_contains_function_call(binary_op->left.get());
    bool right_has_call = expression_contains_function_call(binary_op->right.get());
    
    if (left_has_call && right_has_call) {
        // Both sides have calls - need even more registers
        metrics.required_callee_saved_temps += 2;
        metrics.has_call_preserving_expressions = true;
        
        // Reserve specific registers for this complex expression
        metrics.required_callee_saved_regs.insert("X20");
        metrics.required_callee_saved_regs.insert("X21");
        
        if (trace_enabled_) {
            std::cout << "[ASTAnalyzer] Found complex binary operation with calls on both sides - needs 2 temp registers" << std::endl;
        }
    } else if ((left_has_call && has_variables_or_complex_expr(binary_op->right.get())) ||
               (right_has_call && has_variables_or_complex_expr(binary_op->left.get()))) {
        // One side has a call, the other has variables/expressions that need preservation
        metrics.required_callee_saved_temps += 1;
        metrics.has_call_preserving_expressions = true;
        
        // Reserve a specific register for this pattern
        metrics.required_callee_saved_regs.insert("X20");
        
        if (trace_enabled_) {
            std::cout << "[ASTAnalyzer] Found call-preserving binary operation (e.g., N * FUNC(N-1)) - needs 1 temp register" << std::endl;
        }
    }
    
    // Recursively analyze both sides
    analyze_node_for_register_usage(binary_op->left.get(), metrics);
    analyze_node_for_register_usage(binary_op->right.get(), metrics);
}

bool ASTAnalyzer::expression_contains_function_call(ASTNode* expr) const {
    if (!expr) return false;
    
    if (dynamic_cast<FunctionCall*>(expr)) {
        return true;
    }
    
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr)) {
        return expression_contains_function_call(binary_op->left.get()) ||
               expression_contains_function_call(binary_op->right.get());
    }
    
    if (auto* unary_op = dynamic_cast<UnaryOp*>(expr)) {
        return expression_contains_function_call(unary_op->operand.get());
    }
    
    if (auto* conditional = dynamic_cast<ConditionalExpression*>(expr)) {
        return expression_contains_function_call(conditional->condition.get()) ||
               expression_contains_function_call(conditional->true_expr.get()) ||
               expression_contains_function_call(conditional->false_expr.get());
    }
    
    if (auto* vec_access = dynamic_cast<VectorAccess*>(expr)) {
        return expression_contains_function_call(vec_access->vector_expr.get()) ||
               expression_contains_function_call(vec_access->index_expr.get());
    }
    
    if (auto* pair_expr = dynamic_cast<PairExpression*>(expr)) {
        return expression_contains_function_call(pair_expr->first_expr.get()) ||
               expression_contains_function_call(pair_expr->second_expr.get());
    }
    
    if (auto* fpair_expr = dynamic_cast<FPairExpression*>(expr)) {
        return expression_contains_function_call(fpair_expr->first_expr.get()) ||
               expression_contains_function_call(fpair_expr->second_expr.get());
    }
    
    if (auto* pair_access = dynamic_cast<PairAccessExpression*>(expr)) {
        return expression_contains_function_call(pair_access->pair_expr.get());
    }
    
    if (auto* fpair_access = dynamic_cast<FPairAccessExpression*>(expr)) {
        return expression_contains_function_call(fpair_access->pair_expr.get());
    }
    
    if (auto* member_access = dynamic_cast<MemberAccessExpression*>(expr)) {
        return expression_contains_function_call(member_access->object_expr.get());
    }
    
    return false;
}

bool ASTAnalyzer::has_variables_or_complex_expr(ASTNode* expr) const {
    if (!expr) return false;
    
    // Variables need preservation
    if (dynamic_cast<VariableAccess*>(expr)) {
        return true;
    }
    
    // Complex expressions need preservation
    if (dynamic_cast<BinaryOp*>(expr) || 
        dynamic_cast<UnaryOp*>(expr) ||
        dynamic_cast<VectorAccess*>(expr) ||
        dynamic_cast<PairAccessExpression*>(expr) ||
        dynamic_cast<FPairAccessExpression*>(expr) ||
        dynamic_cast<MemberAccessExpression*>(expr)) {
        return true;
    }
    
    // Literals don't need preservation (they can be regenerated)
    if (dynamic_cast<NumberLiteral*>(expr) ||
        dynamic_cast<StringLiteral*>(expr) ||
        dynamic_cast<CharLiteral*>(expr) ||
        dynamic_cast<BooleanLiteral*>(expr)) {
        return false;
    }
    
    return true; // Conservative - assume it needs preservation
}

int ASTAnalyzer::count_callee_saved_temps_in_expression(ASTNode* expr) const {
    if (!expr) return 0;
    
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr)) {
        bool left_has_call = expression_contains_function_call(binary_op->left.get());
        bool right_has_call = expression_contains_function_call(binary_op->right.get());
        
        if (left_has_call && right_has_call) {
            return 2; // Need to preserve both sides
        } else if ((left_has_call && has_variables_or_complex_expr(binary_op->right.get())) ||
                   (right_has_call && has_variables_or_complex_expr(binary_op->left.get()))) {
            return 1; // Need to preserve one side across call
        }
    }
    
    return 0;
}

bool ASTAnalyzer::expression_needs_call_preservation(ASTNode* expr) const {
    return count_callee_saved_temps_in_expression(expr) > 0;
}
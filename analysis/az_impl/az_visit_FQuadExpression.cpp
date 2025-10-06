#include "ASTAnalyzer.h"

void ASTAnalyzer::visit(FQuadExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting FQuadExpression" << std::endl;
    }
    
    // Track that we're processing an FQUAD expression
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // FQUAD operations may use multiple registers for intermediate calculations
        metrics.instruction_count += 6; // Approximate instructions for 4-element float vector construction
        metrics.num_float_variables++; // Track float vector usage
    }

    // Analyze all four expressions in the fquad
    if (node.first_expr) {
        node.first_expr->accept(*this);
    }
    if (node.second_expr) {
        node.second_expr->accept(*this);
    }
    if (node.third_expr) {
        node.third_expr->accept(*this);
    }
    if (node.fourth_expr) {
        node.fourth_expr->accept(*this);
    }
}
#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(FOctExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting FOctExpression" << std::endl;
    }
    
    // Track that we're processing a FOCT expression
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // FOCT operations may use multiple registers for intermediate calculations
        metrics.instruction_count += 8; // Approximate instructions for 8-element float vector construction
        metrics.num_float_variables++; // Track float vector usage
    }
    
    // Visit all eight component expressions
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
    if (node.fifth_expr) {
        node.fifth_expr->accept(*this);
    }
    if (node.sixth_expr) {
        node.sixth_expr->accept(*this);
    }
    if (node.seventh_expr) {
        node.seventh_expr->accept(*this);
    }
    if (node.eighth_expr) {
        node.eighth_expr->accept(*this);
    }
    
    // Validate component values are valid float values
    if (node.is_literal()) {
        auto validate_float = [this](Expression* expr, const std::string& position) {
            if (auto* num_lit = dynamic_cast<NumberLiteral*>(expr)) {
                if (num_lit->literal_type != NumberLiteral::LiteralType::Float) {
                    std::string error = "FOCT " + position + " component must be a float literal";
                    semantic_errors_.push_back(error);
                }
            }
        };
        
        validate_float(node.first_expr.get(), "first");
        validate_float(node.second_expr.get(), "second");
        validate_float(node.third_expr.get(), "third");
        validate_float(node.fourth_expr.get(), "fourth");
        validate_float(node.fifth_expr.get(), "fifth");
        validate_float(node.sixth_expr.get(), "sixth");
        validate_float(node.seventh_expr.get(), "seventh");
        validate_float(node.eighth_expr.get(), "eighth");
    }
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] FOctExpression analysis complete" << std::endl;
    }
}
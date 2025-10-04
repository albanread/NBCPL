#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(OctExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting OctExpression" << std::endl;
    }
    
    // Track that we're processing an OCT expression
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // OCT operations may use multiple registers for intermediate calculations
        metrics.instruction_count += 8; // Approximate instructions for 8-element vector construction
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
    
    // Validate component values are within 8-bit signed integer range
    if (node.is_literal()) {
        auto validate_8bit = [this](Expression* expr, const std::string& position) {
            if (auto* num_lit = dynamic_cast<NumberLiteral*>(expr)) {
                int64_t value = num_lit->int_value;
                if (value < -128 || value > 127) {
                    std::string error = "OCT " + position + " component value " + std::to_string(value) + 
                                      " is out of 8-bit signed range (-128 to 127)";
                    semantic_errors_.push_back(error);
                }
            }
        };
        
        validate_8bit(node.first_expr.get(), "first");
        validate_8bit(node.second_expr.get(), "second");
        validate_8bit(node.third_expr.get(), "third");
        validate_8bit(node.fourth_expr.get(), "fourth");
        validate_8bit(node.fifth_expr.get(), "fifth");
        validate_8bit(node.sixth_expr.get(), "sixth");
        validate_8bit(node.seventh_expr.get(), "seventh");
        validate_8bit(node.eighth_expr.get(), "eighth");
    }
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] OctExpression analysis complete" << std::endl;
    }
}
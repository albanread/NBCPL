#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(QuadExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting QuadExpression" << std::endl;
    }
    
    // Track that we're processing a QUAD expression
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // QUAD operations may use multiple registers for intermediate calculations
        metrics.instruction_count += 5; // Approximate instructions for BFI operations
    }
    
    // Visit all four component expressions
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
    
    // Validate component values are within 16-bit signed integer range
    if (node.is_literal()) {
        auto validate_16bit = [this](Expression* expr, const std::string& position) {
            if (auto* num_lit = dynamic_cast<NumberLiteral*>(expr)) {
                int64_t value = num_lit->int_value;
                if (value < -32768 || value > 32767) {
                    std::string error = "QUAD " + position + " component value " + std::to_string(value) + 
                                      " is out of 16-bit signed range (-32768 to 32767)";
                    semantic_errors_.push_back(error);
                }
            }
        };
        
        validate_16bit(node.first_expr.get(), "first");
        validate_16bit(node.second_expr.get(), "second");
        validate_16bit(node.third_expr.get(), "third");
        validate_16bit(node.fourth_expr.get(), "fourth");
    }
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] QuadExpression analysis complete" << std::endl;
    }
}
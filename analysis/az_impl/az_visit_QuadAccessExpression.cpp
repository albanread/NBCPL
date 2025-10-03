#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(QuadAccessExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting QuadAccessExpression ("
                  << (node.access_type == QuadAccessExpression::FIRST ? "first" : 
                     node.access_type == QuadAccessExpression::SECOND ? "second" :
                     node.access_type == QuadAccessExpression::THIRD ? "third" : "fourth") 
                  << ")" << std::endl;
    }
    
    // Track that we're accessing a QUAD component
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // QUAD access requires bit field extraction operations
        metrics.instruction_count += 2; // Approximate cost of SBFX instruction
    }
    
    // Visit the underlying quad expression
    if (node.quad_expr) {
        node.quad_expr->accept(*this);
        
        // Validate that the expression being accessed is a valid packed type
        VarType expr_type = infer_expression_type(node.quad_expr.get());
        std::string access_name = (node.access_type == QuadAccessExpression::FIRST ? "first" : 
                                 node.access_type == QuadAccessExpression::SECOND ? "second" :
                                 node.access_type == QuadAccessExpression::THIRD ? "third" : "fourth");
        
        if (expr_type != VarType::UNKNOWN) {
            // Check if this is a valid access for the given type
            if (expr_type == VarType::PAIR || expr_type == VarType::FPAIR) {
                // PAIR/FPAIR only support .first and .second
                if (node.access_type == QuadAccessExpression::THIRD || node.access_type == QuadAccessExpression::FOURTH) {
                    std::string error = "Cannot access ." + access_name + " on " + 
                                      var_type_to_string(expr_type) + " type (only .first and .second are supported)";
                    semantic_errors_.push_back(error);
                }
            } else if (expr_type != VarType::QUAD) {
                // Not a packed type at all
                std::string error = "Cannot access ." + access_name + " on non-packed type: " + 
                                  var_type_to_string(expr_type);
                semantic_errors_.push_back(error);
            }
            // QUAD supports all four components, so no additional validation needed
        }
    } else {
        semantic_errors_.push_back("QuadAccessExpression missing quad expression");
    }
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] QuadAccessExpression analysis complete" << std::endl;
    }
}
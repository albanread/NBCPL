#include "ASTAnalyzer.h"

void ASTAnalyzer::visit(FQuadAccessExpression& node) {
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Visiting FQuadAccessExpression ("
                  << (node.access_type == FQuadAccessExpression::FIRST ? "first" : 
                     node.access_type == FQuadAccessExpression::SECOND ? "second" :
                     node.access_type == FQuadAccessExpression::THIRD ? "third" : "fourth") 
                  << ")" << std::endl;
    }
    
    // Track that we're accessing an FQUAD component
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];
        // FQUAD component access is relatively lightweight
        metrics.instruction_count += 2; // Approximate instructions for bit field extraction
        metrics.num_float_variables++; // Track float usage from vector access
    }

    // Analyze the underlying fquad expression
    if (node.quad_expr) {
        node.quad_expr->accept(*this);
    } else {
        throw std::runtime_error("FQuadAccessExpression has null quad_expr");
    }
    
    // Validate access bounds at compile time if possible
    // FQUAD supports .first, .second, .third, .fourth access
    switch (node.access_type) {
        case FQuadAccessExpression::FIRST:
        case FQuadAccessExpression::SECOND:
        case FQuadAccessExpression::THIRD:
        case FQuadAccessExpression::FOURTH:
            // All four access types are valid for FQUAD
            break;
        default:
            throw std::runtime_error("Invalid FQUAD access type");
    }
}
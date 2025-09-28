#include "ASTAnalyzer.h"

// Visitor implementation for StringAllocationExpression nodes
void ASTAnalyzer::visit(StringAllocationExpression& node) {
    // Mark that the current function/routine performs heap allocation
    if (current_function_scope_ != "Global") {
        // SAMM Optimization: Mark that this function performs heap allocation
        function_metrics_[current_function_scope_].performs_heap_allocation = true;
    }
    
    // Visit the size expression if present
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}

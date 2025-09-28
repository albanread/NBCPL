#include "ASTAnalyzer.h"

// Visitor implementation for FVecAllocationExpression nodes
void ASTAnalyzer::visit(FVecAllocationExpression& node) {
    // Mark that the current function/routine performs heap allocation
    if (current_function_scope_ != "Global") {
        // SAMM Optimization: Mark that this function performs heap allocation
        function_metrics_[current_function_scope_].performs_heap_allocation = true;
        
        // Float vector allocation requires a call to the runtime, which uses the 
        // global data pointer (X28). Therefore, we must flag this function
        // as one that accesses globals so the code generator will initialize X28.
        function_metrics_[current_function_scope_].accesses_globals = true;
    }
    
    // Visit the size expression if present
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}
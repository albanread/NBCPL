#include "ASTAnalyzer.h"

// Visitor implementation for NewExpression nodes
void ASTAnalyzer::visit(NewExpression& node) {
    // Mark that the current function/routine performs heap allocation
    if (current_function_scope_ != "Global") {
        // SAMM Optimization: Mark that this function performs heap allocation
        function_metrics_[current_function_scope_].performs_heap_allocation = true;
        
        // Object allocation requires runtime support and accesses globals
        function_metrics_[current_function_scope_].accesses_globals = true;
    }
    
    // Visit constructor arguments for further analysis
    for (auto& arg : node.constructor_arguments) {
        if (arg) {
            arg->accept(*this);
        }
    }
}
#include "ASTAnalyzer.h"
#include <iostream>

// Visitor implementation for UntilStatement nodes
void ASTAnalyzer::visit(UntilStatement& node) {
    // Push UNTIL loop context to prevent FOR loop state interference
    loop_context_stack_.push(LoopContext::UNTIL_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed UNTIL loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    if (node.condition) {
        node.condition->accept(*this);
    }
    if (node.body) {
        node.body->accept(*this);
    }

    // Pop UNTIL loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped UNTIL loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
}

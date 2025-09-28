#include "ASTAnalyzer.h"
#include <iostream>

// Visitor implementation for WhileStatement nodes
void ASTAnalyzer::visit(WhileStatement& node) {
    // Push WHILE loop context to prevent FOR loop state interference
    loop_context_stack_.push(LoopContext::WHILE_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed WHILE loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);

    // Pop WHILE loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped WHILE loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
}

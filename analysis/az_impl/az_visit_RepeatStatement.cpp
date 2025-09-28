#include "ASTAnalyzer.h"
#include <iostream>

// Visitor implementation for RepeatStatement nodes
void ASTAnalyzer::visit(RepeatStatement& node) {
    // Push REPEAT loop context to prevent FOR loop state interference
    loop_context_stack_.push(LoopContext::REPEAT_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed REPEAT loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    // Visit the body of the repeat loop, if present
    if (node.body) {
        node.body->accept(*this);
    }
    // Visit the condition of the repeat loop, if present
    if (node.condition) {
        node.condition->accept(*this);
    }

    // Pop REPEAT loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped REPEAT loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
}

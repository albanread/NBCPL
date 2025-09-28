#include "ASTAnalyzer.h"

// Implements ASTAnalyzer::visit for BinaryOp nodes.
void ASTAnalyzer::visit(BinaryOp& node) {
    // If the right operand is a function call, increment the required_callee_saved_temps metric.
    if (node.right && node.right->getType() == ASTNode::NodeType::FunctionCallExpr && !current_function_scope_.empty()) {
        function_metrics_[current_function_scope_].required_callee_saved_temps += 1;
    }

    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
}

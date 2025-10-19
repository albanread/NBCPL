#include "ASTAnalyzer.h"
#include "../../DataTypes.h"

// Implements ASTAnalyzer::visit for BinaryOp nodes.
void ASTAnalyzer::visit(BinaryOp& node) {
    // If the right operand is a function call, increment the required_callee_saved_temps metric.
    if (node.right && node.right->getType() == ASTNode::NodeType::FunctionCallExpr && !current_function_scope_.empty()) {
        function_metrics_[current_function_scope_].required_callee_saved_temps += 1;
    }

    // === Stage 2: Vector Operation Detection ===
    // Visit children first to gather type information
    if (node.left) {
        node.left->accept(*this);
    }
    if (node.right) {
        node.right->accept(*this);
    }
    
    // After visiting children, check if this is a vector operation that needs lowering
    if (node.left && node.right) {
        VarType left_type = infer_expression_type(node.left.get());
        VarType right_type = infer_expression_type(node.right.get());
        
        // Detect vector arithmetic operations that will need lowering in Stage 3
        bool is_vector_operation = false;
        if ((left_type == VarType::PAIRS && right_type == VarType::PAIRS) ||
            (left_type == VarType::FPAIRS && right_type == VarType::FPAIRS) ||
            (left_type == VarType::QUADS && right_type == VarType::QUADS) ||
            (left_type == VarType::FQUADS && right_type == VarType::FQUADS)) {
            
            if (node.op == BinaryOp::Operator::Add ||
                node.op == BinaryOp::Operator::Subtract ||
                node.op == BinaryOp::Operator::Multiply ||
                node.op == BinaryOp::Operator::Divide) {
                is_vector_operation = true;
            }
        }
        
        if (is_vector_operation && !current_function_scope_.empty()) {
            if (trace_enabled_) {
                std::cout << "[ANALYZER TRACE] Detected vector operation: " 
                          << vartype_to_string(left_type) << " + " << vartype_to_string(right_type)
                          << " (will need lowering in Stage 3)" << std::endl;
            }
            
            // Mark this function as having vector operations (for future lowering pass)
            function_metrics_[current_function_scope_].required_callee_saved_temps += 1;
            
            // TODO Stage 3: Store vector operation nodes for lowering pass
            // For now, just mark the function as needing additional processing
        }
    }
}

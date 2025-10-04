#include "../NewCodeGenerator.h"
#include <iostream>

void NewCodeGenerator::visit(LaneAccessExpression& node) {
    if (debug_enabled_) {
        std::cout << "[CODE GEN] Generating code for LaneAccessExpression" << std::endl;
    }
    
    // Check if we have VectorCodeGen available
    if (!vector_codegen_) {
        throw std::runtime_error("VectorCodeGen not initialized for lane access expression");
    }
    
    // Use VectorCodeGen to generate code for lane read
    std::string result_reg = vector_codegen_->generateLaneRead(node, use_neon_);
    
    // Set the result register for the expression
    expression_result_reg_ = result_reg;
    
    // Get the vector type to determine element type
    VarType vector_type = infer_expression_type_local(node.vector_expr.get());
    VarType element_type = VarType::INTEGER;
    
    if (vector_type == VarType::FPAIR || vector_type == VarType::FOCT) {
        element_type = VarType::FLOAT;
    }
    

    
    if (debug_enabled_) {
        std::cout << "[CODE GEN] Lane access expression result in register: " << result_reg 
                  << " (element type: " << vartype_to_string(element_type) << ")" << std::endl;
    }
}
#include "../NewCodeGenerator.h"
#include <iostream>

void NewCodeGenerator::visit(OctExpression& node) {
    if (debug_enabled_) {
        std::cout << "[CODE GEN] Generating code for OctExpression" << std::endl;
    }
    
    // Check if we have VectorCodeGen available
    if (!vector_codegen_) {
        throw std::runtime_error("VectorCodeGen not initialized for OCT expression");
    }
    
    // Use VectorCodeGen to generate code for OCT construction
    std::string result_reg = vector_codegen_->generateOctConstruction(node, use_neon_);
    
    // Set the result register for the expression
    expression_result_reg_ = result_reg;
    

    
    if (debug_enabled_) {
        std::cout << "[CODE GEN] OCT expression result in register: " << result_reg << std::endl;
    }
}
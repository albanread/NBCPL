#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(QuadAccessExpression& node) {
    // Determine the type of the expression to handle PAIR vs QUAD access properly
    VarType expr_type = infer_expression_type_local(node.quad_expr.get());
    
    std::string access_name = (node.access_type == QuadAccessExpression::FIRST ? "first" : 
                              node.access_type == QuadAccessExpression::SECOND ? "second" :
                              node.access_type == QuadAccessExpression::THIRD ? "third" : "fourth");
    
    debug_print("Generating code for QuadAccessExpression (." + access_name + ") on " + 
                vartype_to_string(expr_type) + " type");
    
    // Generate code for the packed expression (which is a 64-bit word)
    if (!node.quad_expr) {
        debug_print("ERROR: QuadAccessExpression missing expression");
        return;
    }
    
    node.quad_expr->accept(*this);
    std::string packed_reg = expression_result_reg_;
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // Handle different packed types
    if (expr_type == VarType::PAIR || expr_type == VarType::FPAIR) {
        // PAIR/FPAIR: 32-bit components (only .first and .second are valid)
        if (node.access_type == QuadAccessExpression::THIRD || node.access_type == QuadAccessExpression::FOURTH) {
            throw std::runtime_error("Invalid component access: ." + access_name + " is not valid for " + 
                                   vartype_to_string(expr_type) + " type (only .first and .second are supported)");
        }
        
        if (node.access_type == QuadAccessExpression::FIRST) {
            // Extract lower 32 bits (bits 0-31) using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 0, 32));
            debug_print("Extracted first (bits 0-31) from " + vartype_to_string(expr_type) + " using SBFX for sign extension");
        } else { // SECOND
            // Extract upper 32 bits (bits 32-63) using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 32, 32));
            debug_print("Extracted second (bits 32-63) from " + vartype_to_string(expr_type) + " using SBFX for sign extension");
        }
    } else if (expr_type == VarType::QUAD) {
        // QUAD: 16-bit components (all four components are valid)
        if (node.access_type == QuadAccessExpression::FIRST) {
            // Extract lower 16 bits (bits 0-15) using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 0, 16));
            debug_print("Extracted first (bits 0-15) from QUAD using SBFX for sign extension");
        } else if (node.access_type == QuadAccessExpression::SECOND) {
            // Extract bits 16-31 using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 16, 16));
            debug_print("Extracted second (bits 16-31) from QUAD using SBFX for sign extension");
        } else if (node.access_type == QuadAccessExpression::THIRD) {
            // Extract bits 32-47 using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 32, 16));
            debug_print("Extracted third (bits 32-47) from QUAD using SBFX for sign extension");
        } else { // FOURTH
            // Extract upper 16 bits (bits 48-63) using SBFX for signed values
            emit(Encoder::opt_create_sbfx(result_reg, packed_reg, 48, 16));
            debug_print("Extracted fourth (bits 48-63) from QUAD using SBFX for sign extension");
        }
    } else {
        throw std::runtime_error("Invalid component access: ." + access_name + " can only be used on PAIR, FPAIR, or QUAD types, got " + 
                               vartype_to_string(expr_type));
    }
    
    // Release the packed register
    register_manager.release_register(packed_reg);
    
    // The extracted value is our result
    expression_result_reg_ = result_reg;
    
    debug_print("QuadAccessExpression code generation complete using ARM64 bit field extraction");
}
#include "../NewCodeGenerator.h"
#include "../DataGenerator.h"
#include "../RegisterManager.h"
#include "../Encoder.h"

void NewCodeGenerator::visit(FQuadAccessExpression& node) {
    debug_print("Generating code for FQuadAccessExpression");
    
    // First, evaluate the underlying fquad expression
    if (!node.quad_expr) {
        throw std::runtime_error("FQuadAccessExpression has no underlying quad expression");
    }
    
    node.quad_expr->accept(*this);
    std::string fquad_reg = expression_result_reg_;
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // For FQUAD access, we extract 32-bit float values from the packed 64-bit representation
    // Currently we only support accessing the first two elements (first and second)
    // since we store FQUAD as a 64-bit value containing two 32-bit floats
    
    switch (node.access_type) {
        case FQuadAccessExpression::FIRST:
            debug_print("Accessing .first element of FQUAD");
            // Extract bits 0-31 (first float)
            emit(Encoder::opt_create_ubfx(result_reg, fquad_reg, 0, 32));
            break;
            
        case FQuadAccessExpression::SECOND:
            debug_print("Accessing .second element of FQUAD");
            // Extract bits 32-63 (second float)
            emit(Encoder::opt_create_ubfx(result_reg, fquad_reg, 32, 32));
            break;
            
        case FQuadAccessExpression::THIRD:
            debug_print("Accessing .third element of FQUAD");
            // TODO: Implement when full 128-bit FQUAD support is added
            throw std::runtime_error("FQUAD .third access not yet implemented (requires 128-bit support)");
            break;
            
        case FQuadAccessExpression::FOURTH:
            debug_print("Accessing .fourth element of FQUAD");
            // TODO: Implement when full 128-bit FQUAD support is added
            throw std::runtime_error("FQUAD .fourth access not yet implemented (requires 128-bit support)");
            break;
            
        default:
            throw std::runtime_error("Invalid FQUAD access type");
    }
    
    // Clean up
    register_manager.release_register(fquad_reg);
    
    expression_result_reg_ = result_reg;
    debug_print("FQuadAccessExpression result in register: " + result_reg);
}
#include "../NewCodeGenerator.h"
#include "../DataGenerator.h"
#include "../RegisterManager.h"
#include "../Encoder.h"

void NewCodeGenerator::visit(FQuadExpression& node) {
    debug_print("Generating code for FQuadExpression using ARM64 bit manipulation");
    
    // An FQUAD combines four 32-bit floats into a 128-bit value
    // For now, we'll represent this as two 64-bit registers or use stack storage
    // This is a simplified implementation that stores to memory and loads back
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // Check if this is a literal fquad (all expressions are literals)
    if (node.first_expr && node.second_expr && node.third_expr && node.fourth_expr) {
        bool all_literals = true;
        float first_val = 0.0f, second_val = 0.0f, third_val = 0.0f, fourth_val = 0.0f;
        
        if (auto* first_lit = dynamic_cast<NumberLiteral*>(node.first_expr.get())) {
            first_val = (first_lit->literal_type == NumberLiteral::LiteralType::Float) ? 
                       static_cast<float>(first_lit->float_value) : static_cast<float>(first_lit->int_value);
        } else {
            all_literals = false;
        }
        
        if (auto* second_lit = dynamic_cast<NumberLiteral*>(node.second_expr.get())) {
            second_val = (second_lit->literal_type == NumberLiteral::LiteralType::Float) ? 
                        static_cast<float>(second_lit->float_value) : static_cast<float>(second_lit->int_value);
        } else {
            all_literals = false;
        }
        
        if (auto* third_lit = dynamic_cast<NumberLiteral*>(node.third_expr.get())) {
            third_val = (third_lit->literal_type == NumberLiteral::LiteralType::Float) ? 
                       static_cast<float>(third_lit->float_value) : static_cast<float>(third_lit->int_value);
        } else {
            all_literals = false;
        }
        
        if (auto* fourth_lit = dynamic_cast<NumberLiteral*>(node.fourth_expr.get())) {
            fourth_val = (fourth_lit->literal_type == NumberLiteral::LiteralType::Float) ? 
                        static_cast<float>(fourth_lit->float_value) : static_cast<float>(fourth_lit->int_value);
        } else {
            all_literals = false;
        }
        
        if (all_literals) {
            debug_print("FQuadExpression is literal - generating packed representation");
            
            // For literal FQUAD, we can generate the packed data directly
            // Pack the first two floats into the lower 64 bits
            uint32_t first_bits, second_bits, third_bits, fourth_bits;
            memcpy(&first_bits, &first_val, sizeof(uint32_t));
            memcpy(&second_bits, &second_val, sizeof(uint32_t));
            memcpy(&third_bits, &third_val, sizeof(uint32_t));
            memcpy(&fourth_bits, &fourth_val, sizeof(uint32_t));
            
            // Combine first two floats into lower 64 bits
            uint64_t lower_bits = static_cast<uint64_t>(first_bits) | (static_cast<uint64_t>(second_bits) << 32);
            
            // Load the lower 64 bits into result register
            emit(Encoder::create_movz_imm(result_reg, lower_bits & 0xFFFF));
            if ((lower_bits >> 16) & 0xFFFF) {
                emit(Encoder::create_movk_imm(result_reg, (lower_bits >> 16) & 0xFFFF, 16));
            }
            if ((lower_bits >> 32) & 0xFFFF) {
                emit(Encoder::create_movk_imm(result_reg, (lower_bits >> 32) & 0xFFFF, 32));
            }
            if ((lower_bits >> 48) & 0xFFFF) {
                emit(Encoder::create_movk_imm(result_reg, (lower_bits >> 48) & 0xFFFF, 48));
            }
            
            // For now, we only handle the lower 64 bits (first two floats)
            // TODO: Implement full 128-bit support when needed
            
            expression_result_reg_ = result_reg;
            debug_print("Literal FQuadExpression result in register: " + result_reg);
            return;
        }
    }
    
    // Non-literal case: evaluate each expression and construct FQUAD at runtime
    debug_print("FQuadExpression is non-literal - constructing at runtime");
    
    // For simplicity, we'll construct the FQUAD by evaluating each component
    // and combining them using bit manipulation (similar to QUAD and FPAIR)
    
    // Initialize result register to 0
    emit(Encoder::create_movz_imm(result_reg, 0));
    
    // Generate code for first expression (bits 0-31)
    if (node.first_expr) {
        node.first_expr->accept(*this);
        std::string first_reg = expression_result_reg_;
        
        // Convert to float bits and insert into lower 32 bits
        // For now, assume the value is already in the right format
        emit(Encoder::opt_create_bfi(result_reg, first_reg, 0, 32));
        
        register_manager.release_register(first_reg);
        debug_print("Inserted first expression into bits 0-31 using BFI");
    }
    
    // Generate code for second expression (bits 32-63)
    if (node.second_expr) {
        node.second_expr->accept(*this);
        std::string second_reg = expression_result_reg_;
        
        // Convert to float bits and insert into upper 32 bits
        emit(Encoder::opt_create_bfi(result_reg, second_reg, 32, 32));
        
        register_manager.release_register(second_reg);
        debug_print("Inserted second expression into bits 32-63 using BFI");
    }
    
    // For now, we only handle the first two elements (64-bit representation)
    // The third and fourth elements would require additional register or memory storage
    // TODO: Implement full 128-bit FQUAD when register allocation supports it
    
    if (node.third_expr || node.fourth_expr) {
        debug_print("Warning: Third and fourth FQUAD elements not fully implemented yet");
    }
    
    expression_result_reg_ = result_reg;
    debug_print("Non-literal FQuadExpression result in register: " + result_reg);
}
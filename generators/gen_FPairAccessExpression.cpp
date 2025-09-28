#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(FPairAccessExpression& node) {
    debug_print("Generating code for FPairAccessExpression using ARM64 bit field extraction and float conversion (" + 
                std::string(node.access_type == FPairAccessExpression::FIRST ? "first" : "second") + 
                " - extracting 32-bit float field from 64-bit word)");
    
    // Generate code for the fpair expression (which is a 64-bit word containing two packed floats)
    if (!node.pair_expr) {
        debug_print("ERROR: FPairAccessExpression missing fpair expression");
        return;
    }
    
    node.pair_expr->accept(*this);
    std::string fpair_reg = expression_result_reg_;
    
    auto& register_manager = RegisterManager::getInstance();
    
    // Extract the appropriate 32-bit field from the 64-bit word using UBFX
    std::string temp_gen_reg = register_manager.get_free_register(*this);
    
    if (node.access_type == FPairAccessExpression::FIRST) {
        // Extract lower 32 bits (bits 0-31) using UBFX
        // UBFX rd, rn, #lsb, #width
        // Extract 32 bits starting from bit 0
        emit(Encoder::opt_create_ubfx(temp_gen_reg, fpair_reg, 0, 32));
        debug_print("Extracted first float bits (bits 0-31) from fpair using UBFX");
    } else { // SECOND
        // Extract upper 32 bits (bits 32-63) using UBFX
        // Extract 32 bits starting from bit 32
        emit(Encoder::opt_create_ubfx(temp_gen_reg, fpair_reg, 32, 32));
        debug_print("Extracted second float bits (bits 32-63) from fpair using UBFX");
    }
    
    // Convert the extracted 32-bit bit pattern back to a float register
    // FMOV D_reg, X_reg moves the bit pattern from general register to float register
    std::string result_float_reg = register_manager.acquire_fp_scratch_reg();
    emit(Encoder::create_fmov_x_to_d(result_float_reg, temp_gen_reg));
    debug_print("Converted extracted bits to float register: " + result_float_reg);
    
    // Release temporary registers
    register_manager.release_register(fpair_reg);
    register_manager.release_register(temp_gen_reg);
    
    // The extracted float value is our result
    expression_result_reg_ = result_float_reg;
    
    debug_print("FPairAccessExpression code generation complete using ARM64 bit field extraction and float conversion");
}
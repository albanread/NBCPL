#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(PairAccessExpression& node) {
    debug_print("Generating code for PairAccessExpression using ARM64 bit field extraction (" + 
                std::string(node.access_type == PairAccessExpression::FIRST ? "first" : "second") + 
                " - extracting 32-bit field from 64-bit word)");
    
    // Generate code for the pair expression (which is a 64-bit word)
    if (!node.pair_expr) {
        debug_print("ERROR: PairAccessExpression missing pair expression");
        return;
    }
    
    node.pair_expr->accept(*this);
    std::string pair_reg = expression_result_reg_;
    
    auto& register_manager = RegisterManager::getInstance();
    
    // Extract the appropriate 32-bit field from the 64-bit word using UBFX
    std::string result_reg = register_manager.get_free_register(*this);
    
    if (node.access_type == PairAccessExpression::FIRST) {
        // Extract lower 32 bits (bits 0-31) using UBFX
        // UBFX rd, rn, #lsb, #width
        // Extract 32 bits starting from bit 0
        emit(Encoder::opt_create_ubfx(result_reg, pair_reg, 0, 32));
        debug_print("Extracted first (bits 0-31) from pair using UBFX");
    } else { // SECOND
        // Extract upper 32 bits (bits 32-63) using UBFX
        // Extract 32 bits starting from bit 32
        emit(Encoder::opt_create_ubfx(result_reg, pair_reg, 32, 32));
        debug_print("Extracted second (bits 32-63) from pair using UBFX");
    }
    
    // Release the pair register
    register_manager.release_register(pair_reg);
    
    // The extracted 32-bit value is our result
    expression_result_reg_ = result_reg;
    
    debug_print("PairAccessExpression code generation complete using ARM64 bit field extraction");
}
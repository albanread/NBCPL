#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RegisterManager.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>

void NewCodeGenerator::visit(FPairExpression& node) {
    debug_print("Generating code for FPairExpression using ARM64 bit manipulation");
    
    // An FPAIR combines two 32-bit floats into a single 64-bit word
    // First float goes into bits 0-31 (lower 32 bits)
    // Second float goes into bits 32-63 (upper 32 bits)
    
    auto& register_manager = RegisterManager::getInstance();
    std::string result_reg = register_manager.get_free_register(*this);
    
    // Initialize result register to 0
    emit(Encoder::create_movz_imm(result_reg, 0));
    
    // Generate code for first expression (bits 0-31)
    if (node.first_expr) {
        node.first_expr->accept(*this);
        std::string first_reg = expression_result_reg_;
        
        // Convert double to single precision float first
        std::string single_precision_reg = register_manager.acquire_fp_scratch_reg(); // This gives us D register
        std::string s_reg = "S" + single_precision_reg.substr(1); // Convert D0 -> S0
        debug_print("FCVT registers: s_reg=" + s_reg + ", first_reg=" + first_reg);
        emit(Encoder::create_fcvt_d_to_s(s_reg, first_reg));
        
        // For float values, we need to move from FP register to general register
        // FMOV W_reg, S_reg moves the bit pattern of a 32-bit float
        std::string temp_gen_reg = register_manager.get_free_register(*this);
        std::string w_reg = "W" + temp_gen_reg.substr(1); // Convert X0 -> W0
        emit(Encoder::create_fmov_s_to_w(w_reg, s_reg));
        
        // Use BFXIL to insert first float bits into bits 0-31 of result
        // BFXIL rd, rn, #lsb, #width  
        // Insert 32 bits from temp_gen_reg starting at bit 0
        emit(Encoder::opt_create_bfxil(result_reg, temp_gen_reg, 0, 32));
        
        register_manager.release_register(first_reg);
        register_manager.release_fp_register(single_precision_reg);
        register_manager.release_register(temp_gen_reg);
        debug_print("Converted double to float and inserted into bits 0-31 using BFXIL");
    }
    
    // Generate code for second expression (bits 32-63)
    if (node.second_expr) {
        node.second_expr->accept(*this);
        std::string second_reg = expression_result_reg_;
        
        // Convert double to single precision float first
        std::string single_precision_reg = register_manager.acquire_fp_scratch_reg(); // This gives us D register
        std::string s_reg = "S" + single_precision_reg.substr(1); // Convert D0 -> S0
        emit(Encoder::create_fcvt_d_to_s(s_reg, second_reg));
        
        // For float values, we need to move from FP register to general register
        std::string temp_gen_reg = register_manager.get_free_register(*this);
        std::string w_reg = "W" + temp_gen_reg.substr(1); // Convert X0 -> W0
        emit(Encoder::create_fmov_s_to_w(w_reg, s_reg));
        
        // Use BFI to insert second float bits into bits 32-63 of result
        // Insert 32 bits from temp_gen_reg starting at bit 32
        emit(Encoder::opt_create_bfi(result_reg, temp_gen_reg, 32, 32));
        
        register_manager.release_register(second_reg);
        register_manager.release_fp_register(single_precision_reg);
        register_manager.release_register(temp_gen_reg);
        debug_print("Converted double to float and inserted into bits 32-63 using BFI");
    }
    
    // The combined 64-bit value containing two 32-bit floats is our result
    expression_result_reg_ = result_reg;
    
    debug_print("FPairExpression code generation complete - used ARM64 bit manipulation");
}
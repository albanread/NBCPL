#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_str_vec_imm(const std::string& qt, const std::string& xn, int immediate, const std::string& variable_name) {
    uint32_t qt_encoding = get_reg_encoding(qt);
    uint32_t xn_encoding = get_reg_encoding(xn);
    
    // Check immediate range - must be 0-4080 and multiple of 16 for Q registers
    if (immediate < 0 || immediate > 4080 || immediate % 16 != 0) {
        throw std::runtime_error("STR (vector) immediate offset must be 0-4080 and multiple of 16 for Q registers.");
    }
    
    // Scale immediate for encoding (divide by 16)
    uint32_t scaled_imm = immediate / 16;
    
    // Base encoding for STR Qt, [Xn, #imm] - matches clang pattern 3d80xxxx
    uint32_t encoding = 0x3D800000;
    
    // Set fields exactly like LDR but with STR base
    encoding |= (scaled_imm << 10);  // imm12 at bits 21:10
    encoding |= (xn_encoding << 5);  // Rn at bits 9:5  
    encoding |= qt_encoding;         // Rt at bits 4:0
    
    std::stringstream ss;
    if (!variable_name.empty()) {
        ss << "STR " << qt << ", [" << xn << ", #" << immediate << "] ; " << variable_name;
    } else {
        ss << "STR " << qt << ", [" << xn << ", #" << immediate << "]";
    }
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::STR_FP;
    instr.dest_reg = qt_encoding;
    instr.base_reg = xn_encoding;
    instr.immediate = immediate;
    instr.is_mem_op = true;
    instr.uses_immediate = true;
    
    return instr;
}
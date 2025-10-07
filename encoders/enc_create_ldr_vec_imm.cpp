// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_ldr_vec_imm(const std::string& qt, const std::string& xn, int immediate, const std::string& variable_name) {
    uint32_t qt_encoding = get_reg_encoding(qt);
    uint32_t xn_encoding = get_reg_encoding(xn);
    
    // Check immediate range - must be 0-4080 and multiple of 16 for Q registers
    if (immediate < 0 || immediate > 4080 || immediate % 16 != 0) {
        throw std::runtime_error("LDR (vector) immediate offset must be 0-4080 and multiple of 16 for Q registers.");
    }
    
    // Scale immediate for encoding (divide by 16)
    uint32_t scaled_imm = immediate / 16;
    
    // Base encoding matches clang exactly
    uint32_t encoding = 0x3DC00000;
    
    // Set fields exactly like the working test
    encoding |= (scaled_imm << 10);  // imm12 at bits 21:10
    encoding |= (xn_encoding << 5);  // Rn at bits 9:5  
    encoding |= qt_encoding;         // Rt at bits 4:0
    
    std::stringstream ss;
    if (!variable_name.empty()) {
        ss << "LDR " << qt << ", [" << xn << ", #" << immediate << "] ; " << variable_name;
    } else {
        ss << "LDR " << qt << ", [" << xn << ", #" << immediate << "]";
    }
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::LDR_FP;
    instr.dest_reg = qt_encoding;
    instr.base_reg = xn_encoding;
    instr.immediate = immediate;
    instr.is_mem_op = true;
    instr.uses_immediate = true;
    
    return instr;
}
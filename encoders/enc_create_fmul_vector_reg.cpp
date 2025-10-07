// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_fmul_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // Base encoding for FMUL vector - corrected opcode and bit pattern
    uint32_t encoding = 0x6E20DC00 | (rm << 16) | (rn << 5) | rd;
    
    // Set arrangement-specific bits
    if (arrangement == "2S") {
        encoding &= ~(1U << 30); // Clear Q bit for 64-bit operation (2S)
    } else if (arrangement == "2D") {
        encoding |= (0b01 << 22); // Set sz=01 for double-precision
        encoding &= ~(1U << 29); // Clear bit 29 for 2D
    }

    std::stringstream ss;
    ss << "FMUL " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::FMUL_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}

// Status: PASS - All FADD vector variations tested by NewBCPL --test-encoders
#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_fadd_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // Base encoding for FADD (vector) - corrected opcode from ADD to FADD
    uint32_t encoding = 0x4E20D400 | (rm << 16) | (rn << 5) | rd;

    // Set the size bits based on the arrangement
    if (arrangement == "4S" || arrangement == "2S") {
        // size bits (22, 23) = 00 for single-precision, Q bit determines 2S vs 4S
        encoding &= ~(0b11 << 22); // Clear size bits (set to 00)
        if (arrangement == "2S") {
            encoding &= ~(1 << 30); // Clear Q bit for 64-bit operation
        }
    } else if (arrangement == "2D") {
        // size bits (22, 23) = 01 for double-precision
        encoding |= (0b01 << 22);
    } else {
        throw std::runtime_error("Unsupported arrangement for FADD: " + arrangement);
    }

    std::stringstream ss;
    ss << "FADD " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::FADD_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}

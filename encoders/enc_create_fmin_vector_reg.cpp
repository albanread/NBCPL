#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_fmin_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    uint32_t encoding;

    // FMIN vector encodings based on arrangement (corrected to match Clang)
    if (arrangement == "2S") {
        // 64-bit version: 0ea0f400 base
        encoding = 0x0ea0f400;
    } else if (arrangement == "4S") {
        // 128-bit version: 4ea0f400 base
        encoding = 0x4ea0f400;
    } else if (arrangement == "2D") {
        // 128-bit double-precision: 4ee0f400 base
        encoding = 0x4ee0f400;
    } else {
        throw std::runtime_error("Unsupported arrangement for FMIN: " + arrangement);
    }

    // Add register fields: Rm[20:16], Rn[9:5], Rd[4:0]
    encoding |= (rm << 16) | (rn << 5) | rd;

    std::stringstream ss;
    ss << "FMIN " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::FMIN_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}
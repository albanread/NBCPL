// Status: PASS - All SUB vector variations tested by NewBCPL --test-encoders
#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_sub_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    uint32_t encoding;

    // SUB vector encodings based on arrangement (corrected to match Clang)
    if (arrangement == "2S") {
        // 64-bit version: 2ea08400 base
        encoding = 0x2ea08400;
    } else if (arrangement == "4S") {
        // 128-bit version: 6ea08400 base  
        encoding = 0x6ea08400;
    } else if (arrangement == "8B") {
        // 64-bit 8-byte: 2e208400 base  
        encoding = 0x2e208400;
    } else if (arrangement == "16B") {
        // 128-bit 16-byte: 6e208400 base
        encoding = 0x6e208400;
    } else if (arrangement == "4H") {
        // 64-bit 4-halfword: 2e608400 base
        encoding = 0x2e608400;
    } else if (arrangement == "8H") {
        // 128-bit 8-halfword: 6e608400 base
        encoding = 0x6e608400;
    } else if (arrangement == "2D") {
        // 128-bit 2-doubleword: 6ee08400 base
        encoding = 0x6ee08400;
    } else {
        throw std::runtime_error("Unsupported arrangement for SUB vector: " + arrangement);
    }

    // Add register fields: Rm[20:16], Rn[9:5], Rd[4:0]
    encoding |= (rm << 16) | (rn << 5) | rd;

    std::stringstream ss;
    ss << "SUB " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::SUB_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}
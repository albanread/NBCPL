#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_add_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // Base encoding for ADD vector instruction
    // Q | 0 | 0 | 01110 | size[1:0] | 1 | Rm[4:0] | 100001 | Rn[4:0] | Rd[4:0]
    // For integer ADD: opcode = 100001 (bits 15-10)
    uint32_t encoding = 0x0E008400; // Base with Q=0, opcode=100001

    // Set Q bit, size, and mandatory bit 21 based on arrangement
    if (arrangement == "2S") {
        // Q=0 (64-bit), size=10 for 32-bit elements
        encoding |= (0x2 << 22); // size = 10
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "4S") {
        // Q=1 (128-bit), size=10 for 32-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x2 << 22);  // size = 10
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "8B") {
        // Q=0 (64-bit), size=00 for 8-bit elements
        encoding |= (0x0 << 22); // size = 00
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "16B") {
        // Q=1 (128-bit), size=00 for 8-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x0 << 22);  // size = 00
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "4H") {
        // Q=0 (64-bit), size=01 for 16-bit elements
        encoding |= (0x1 << 22); // size = 01
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "8H") {
        // Q=1 (128-bit), size=01 for 16-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x1 << 22);  // size = 01
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "2D") {
        // Q=1 (128-bit), size=11 for 64-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x3 << 22);  // size = 11
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else {
        throw std::runtime_error("Unsupported arrangement for ADD vector: " + arrangement);
    }

    // Set register fields
    encoding |= (rm << 16) | (rn << 5) | rd;

    std::stringstream ss;
    ss << "ADD " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::ADD_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}
#include "Encoder.h"
#include "BitPatcher.h"
#include <stdexcept>
#include <string>

// ARM64 FMOV (32-bit variant, scalar, from general-purpose register to vector register)
// This instruction moves the contents of a W register to an S register
// Encoding: FMOV Sd, Wn
// 0x1E270000 | (Rn << 5) | Rd
// Where Rn is the W register (bits 5-9) and Rd is the S register (bits 0-4)

Instruction Encoder::create_fmov_w_to_s(const std::string& sd, const std::string& wn) {
    // Parse S register number from "S0", "S1", etc.
    auto parse_s_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'S') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    // Parse W register number from "W0", "W1", etc.
    auto parse_w_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'W') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    int s_dst = parse_s_reg(sd);
    int w_src = parse_w_reg(wn);

    if (s_dst < 0 || s_dst > 31 || w_src < 0 || w_src > 31) {
        throw std::runtime_error("Invalid register for FMOV W to S: " + sd + ", " + wn);
    }

    // FMOV Sd, Wn encoding (32-bit scalar)
    BitPatcher patcher(0x1E270000);
    patcher.patch(w_src, 5, 5); // bits 5-9: source W register
    patcher.patch(s_dst, 0, 5); // bits 0-4: destination S register

    Instruction instr(patcher.get_value(), "FMOV " + sd + ", " + wn);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(sd);
    instr.src_reg1 = Encoder::get_reg_encoding(wn);

    return instr;
}
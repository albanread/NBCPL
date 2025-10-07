// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "../Encoder.h"
#include "../BitPatcher.h"
#include <stdexcept>
#include <string>

// ARM64 FMOV (32-bit variant, scalar, from vector register to general-purpose register)
// This instruction moves the contents of an S register to a W register
// Encoding: FMOV Wd, Sn
// 0x1E260000 | (Rn << 5) | Rd
// Where Rn is the S register (bits 5-9) and Rd is the W register (bits 0-4)

Instruction Encoder::create_fmov_s_to_w(const std::string& wd, const std::string& sn) {
    // Parse W register number from "W0", "W1", etc.
    auto parse_w_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'W') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    // Parse S register number from "S0", "S1", etc.
    auto parse_s_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'S') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    int w_dst = parse_w_reg(wd);
    int s_src = parse_s_reg(sn);

    if (w_dst < 0 || w_dst > 31 || s_src < 0 || s_src > 31) {
        throw std::runtime_error("Invalid register for FMOV S to W: " + wd + ", " + sn);
    }

    // FMOV Wd, Sn encoding (32-bit scalar)
    BitPatcher patcher(0x1E260000);
    patcher.patch(s_src, 5, 5); // bits 5-9: source S register
    patcher.patch(w_dst, 0, 5); // bits 0-4: destination W register

    Instruction instr(patcher.get_value(), "FMOV " + wd + ", " + sn);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(wd);
    instr.src_reg1 = Encoder::get_reg_encoding(sn);

    return instr;
}
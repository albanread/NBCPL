// This encoder is present in the test schedule and has passed automated validation.
#include "Encoder.h"
#include "BitPatcher.h"
#include <stdexcept>
#include <string>
#include <algorithm>
#include <cctype>

// ARM64 FMOV (64-bit variant, scalar, from vector register to general-purpose register)
// This instruction moves the contents of a D register to an X register
// Encoding: FMOV Xd, Dn
// 0x9E660000 | (Rn << 5) | Rd
// Where Rn is the D register (bits 5-9) and Rd is the X register (bits 0-4)

Instruction Encoder::create_fmov_d_to_x(const std::string& xd, const std::string& dn) {
    // Parse X register number from "X0", "X1", etc. (case-insensitive)
    auto parse_x_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2) return -1;
        std::string upper_reg = reg;
        std::transform(upper_reg.begin(), upper_reg.end(), upper_reg.begin(), ::toupper);
        if (upper_reg[0] != 'X') return -1;
        try {
            return std::stoi(upper_reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    // Parse D register number from "D0", "D1", etc. (case-insensitive)
    auto parse_d_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2) return -1;
        std::string upper_reg = reg;
        std::transform(upper_reg.begin(), upper_reg.end(), upper_reg.begin(), ::toupper);
        if (upper_reg[0] != 'D') return -1;
        try {
            return std::stoi(upper_reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    int x_dst = parse_x_reg(xd);
    int d_src = parse_d_reg(dn);

    if (x_dst < 0 || x_dst > 31 || d_src < 0 || d_src > 31) {
        throw std::runtime_error("Invalid register for FMOV D to X: " + xd + ", " + dn);
    }

    // FMOV Xd, Dn encoding (64-bit scalar)
    BitPatcher patcher(0x9E660000);
    patcher.patch(d_src, 5, 5); // bits 5-9: source D register
    patcher.patch(x_dst, 0, 5); // bits 0-4: destination X register

    Instruction instr(patcher.get_value(), "FMOV " + xd + ", " + dn);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(dn);

    return instr;
}
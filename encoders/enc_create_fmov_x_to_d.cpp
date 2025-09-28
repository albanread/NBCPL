#include "Encoder.h"
#include "BitPatcher.h"
#include <stdexcept>
#include <string>

// ARM64 FMOV (64-bit variant, scalar, from general-purpose register to vector register)
// This instruction moves the contents of an X register to a D register
// Encoding: FMOV Dd, Xn
// 0x9E670000 | (Rn << 5) | Rd
// Where Rn is the X register (bits 5-9) and Rd is the D register (bits 0-4)

Instruction Encoder::create_fmov_x_to_d(const std::string& dd, const std::string& xn) {
    // Parse D register number from "D0", "D1", etc.
    auto parse_d_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'D') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    // Parse X register number from "X0", "X1", etc.
    auto parse_x_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'X') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    int d_dst = parse_d_reg(dd);
    int x_src = parse_x_reg(xn);

    if (d_dst < 0 || d_dst > 31 || x_src < 0 || x_src > 31) {
        throw std::runtime_error("Invalid register for FMOV X to D: " + dd + ", " + xn);
    }

    // FMOV Dd, Xn encoding (64-bit scalar)
    BitPatcher patcher(0x9E670000);
    patcher.patch(x_src, 5, 5); // bits 5-9: source X register
    patcher.patch(d_dst, 0, 5); // bits 0-4: destination D register

    Instruction instr(patcher.get_value(), "FMOV " + dd + ", " + xn);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(dd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);

    return instr;
}
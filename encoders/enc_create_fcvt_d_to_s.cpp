#include "../Encoder.h"
#include "../BitPatcher.h"
#include <stdexcept>
#include <string>

// ARM64 FCVT (Floating-point Convert precision)
// This instruction converts from double precision (D) to single precision (S)
// Encoding: FCVT Sd, Dn
// 0x1E624000 | (Rn << 5) | Rd
// Where Rn is the D register (bits 5-9) and Rd is the S register (bits 0-4)

Instruction Encoder::create_fcvt_d_to_s(const std::string& sd, const std::string& dn) {
    // Parse S register number from "S0", "S1", etc.
    auto parse_s_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'S') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    // Parse D register number from "D0", "D1", etc.
    auto parse_d_reg = [](const std::string& reg) -> int {
        if (reg.size() < 2 || reg[0] != 'D') return -1;
        try {
            return std::stoi(reg.substr(1));
        } catch (...) {
            return -1;
        }
    };

    int s_dst = parse_s_reg(sd);
    int d_src = parse_d_reg(dn);

    if (s_dst < 0 || s_dst > 31 || d_src < 0 || d_src > 31) {
        throw std::runtime_error("Invalid register for FCVT D to S: " + sd + ", " + dn);
    }

    // FCVT Sd, Dn encoding (double to single precision)
    BitPatcher patcher(0x1E624000);
    patcher.patch(d_src, 5, 5); // bits 5-9: source D register
    patcher.patch(s_dst, 0, 5); // bits 0-4: destination S register

    Instruction instr(patcher.get_value(), "FCVT " + sd + ", " + dn +" ;encoder.");
    instr.opcode = InstructionDecoder::OpType::FCVT;
    instr.dest_reg = Encoder::get_reg_encoding(sd);
    instr.src_reg1 = Encoder::get_reg_encoding(dn);

    return instr;
}

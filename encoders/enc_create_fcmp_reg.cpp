#include "Encoder.h"
#include <sstream>

Instruction Encoder::create_fcmp_reg(const std::string& dn, const std::string& dm) {
    uint32_t rn = get_reg_encoding(dn);
    uint32_t rm = get_reg_encoding(dm);

    // Detect register type and use appropriate base encoding
    uint32_t base_encoding;
    if (dn[0] == 'd' || dn[0] == 'D') {
        // Double-precision (64-bit): FCMP Dn, Dm
        base_encoding = 0x1E602008;
    } else if (dn[0] == 's' || dn[0] == 'S') {
        // Single-precision (32-bit): FCMP Sn, Sm
        base_encoding = 0x1E202000;
    } else {
        throw std::invalid_argument("FCMP register type must be 'D' (double) or 'S' (single)");
    }
    
    uint32_t encoding = base_encoding | (rm << 16) | (rn << 5);

    std::stringstream ss;
    ss << "FCMP " << dn << ", " << dm;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::FCMP;
    instr.src_reg1 = Encoder::get_reg_encoding(dn);
    instr.src_reg2 = Encoder::get_reg_encoding(dm);
    return instr;
}

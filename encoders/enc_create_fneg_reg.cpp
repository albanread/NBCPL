// Status: PASS - Tested by NewBCPL --test-encoders
#include "Encoder.h"
#include <sstream>
#include "BitPatcher.h"

// Implements FNEG Dd, Dn and FNEG Sd, Sn (floating-point negate)
// FNEG (double-precision) has base encoding 0x1E614000
// FNEG (single-precision) has base encoding 0x1E214000
Instruction Encoder::create_fneg_reg(const std::string& dd, const std::string& dn) {
    uint32_t rd = get_reg_encoding(dd);
    uint32_t rn = get_reg_encoding(dn);

    // Detect register type and use appropriate base encoding
    uint32_t base_encoding;
    if (dd[0] == 'd' || dd[0] == 'D') {
        // Double-precision (64-bit): FNEG Dd, Dn
        base_encoding = 0x1E614000;
    } else if (dd[0] == 's' || dd[0] == 'S') {
        // Single-precision (32-bit): FNEG Sd, Sn
        base_encoding = 0x1E214000;
    } else {
        throw std::invalid_argument("FNEG register type must be 'D' (double) or 'S' (single)");
    }
    
    BitPatcher patcher(base_encoding);
    patcher.patch(rn, 5, 5); // Dn at bits [9:5]
    patcher.patch(rd, 0, 5); // Dd at bits [4:0]

    std::stringstream ss;
    ss << "FNEG " << dd << ", " << dn;
    Instruction instr(patcher.get_value(), ss.str());
    instr.opcode = InstructionDecoder::OpType::FMOV; // No dedicated FNEG OpType, FMOV is often used for unary float ops
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    return instr;
}

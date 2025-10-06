#include "Encoder.h"
#include "BitPatcher.h"

// Implements the ARM64 FSQRT Dd, Dn and FSQRT Sd, Sn instruction encoding.
// FSQRT (double-precision) has base encoding 0x1E61C000.
// FSQRT (single-precision) has base encoding 0x1E21C000.
// Dd/Sd: destination FP register (0-31), Dn/Sn: source FP register (0-31).
Instruction Encoder::create_fsqrt_reg(const std::string& dd, const std::string& dn) {
    uint32_t rd = get_reg_encoding(dd);
    uint32_t rn = get_reg_encoding(dn);

    // Detect register type and use appropriate base encoding
    uint32_t base_encoding;
    if (dd[0] == 'd' || dd[0] == 'D') {
        // Double-precision (64-bit): FSQRT Dd, Dn
        base_encoding = 0x1E61C000;
    } else if (dd[0] == 's' || dd[0] == 'S') {
        // Single-precision (32-bit): FSQRT Sd, Sn
        base_encoding = 0x1E21C000;
    } else {
        throw std::invalid_argument("FSQRT register type must be 'D' (double) or 'S' (single)");
    }
    
    BitPatcher patcher(base_encoding);
    patcher.patch(rn, 5, 5);  // Dn at bits [9:5]
    patcher.patch(rd, 0, 5);  // Dd at bits [4:0]

    std::string asm_text = "FSQRT " + dd + ", " + dn;
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FSQRT;
    instr.dest_reg = rd + 32; // FP register encoding offset
    instr.src_reg1 = rn + 32; // FP register encoding offset
    return instr;
}

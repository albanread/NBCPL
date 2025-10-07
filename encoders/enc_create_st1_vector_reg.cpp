// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_st1_vector_reg(const std::string& vt, const std::string& xn, const std::string& arrangement) {
    uint32_t rt = get_reg_encoding(vt);
    uint32_t rn = get_reg_encoding(xn);

    // Encoding for ST1 {Vt.4S}, [Xn]
    // This is the store counterpart to LD1
    // Q(1) | 0 | 0 | 0110 | 1 | L(0) | M(0) | 1 | Rm(11111) | 1100 | Rn | Rt
    // L=0 for store (vs L=1 for load in LD1)
    uint32_t encoding = 0x4C008C00 | (rn << 5) | rt;

    std::stringstream ss;
    ss << "ST1 {" << vt << "." << arrangement << "}, [" << xn << "]";
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::ST1_VECTOR;
    instr.dest_reg = rt;
    instr.base_reg = rn;
    instr.is_mem_op = true;
    instr.uses_immediate = false;

    return instr;
}
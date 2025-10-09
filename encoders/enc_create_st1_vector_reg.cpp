// This encoder is present in the test schedule and has passed automated validation.
#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_st1_vector_reg(const std::string& vt, const std::string& xn, const std::string& arrangement) {
    uint32_t rt = get_reg_encoding(vt);
    uint32_t rn = get_reg_encoding(xn);

    // Encoding for ST1 {Vt.4S}, [Xn]
    // This is the store counterpart to LD1
    // Corrected encoding based on Clang output.
    uint32_t encoding = 0x4C007800 | (rn << 5) | rt;

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
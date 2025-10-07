// Status: PASS - All SMAX vector variations tested by NewBCPL --test-encoders
#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_smax_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // SMAX (Signed Maximum) vector instruction
    // SMAX Vd.<T>, Vn.<T>, Vm.<T>
    // Element-wise signed maximum
    
    if (arrangement != "8B" && arrangement != "16B" && arrangement != "4H" && 
        arrangement != "8H" && arrangement != "2S" && arrangement != "4S") {
        throw std::runtime_error("Invalid arrangement for SMAX vector: " + arrangement + 
                                 " (expected 8B, 16B, 4H, 8H, 2S, or 4S)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "8B") {
        // SMAX Vd.8B, Vn.8B, Vm.8B (64-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=0, size=00
        encoding = 0x0E206400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "16B") {
        // SMAX Vd.16B, Vn.16B, Vm.16B (128-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=1, size=00
        encoding = 0x4E206400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // SMAX Vd.4H, Vn.4H, Vm.4H (64-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=0, size=01
        encoding = 0x0E606400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // SMAX Vd.8H, Vn.8H, Vm.8H (128-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=1, size=01
        encoding = 0x4E606400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2S") {
        // SMAX Vd.2S, Vn.2S, Vm.2S (64-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=0, size=10
        encoding = 0x0EA06400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // SMAX Vd.4S, Vn.4S, Vm.4S (128-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm011001nnnnnddddd where Q=1, size=10
        encoding = 0x4EA06400 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "SMAX " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::SMAX_VECTOR;
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    instr.src_reg2 = rm;
    instr.is_mem_op = false;
    instr.uses_immediate = false;
    
    return instr;
}
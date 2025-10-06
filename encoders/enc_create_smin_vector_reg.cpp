#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_smin_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // SMIN (Signed Minimum) vector instruction
    // SMIN Vd.<T>, Vn.<T>, Vm.<T>
    // Element-wise signed minimum
    
    if (arrangement != "8B" && arrangement != "16B" && arrangement != "4H" && 
        arrangement != "8H" && arrangement != "2S" && arrangement != "4S") {
        throw std::runtime_error("Invalid arrangement for SMIN vector: " + arrangement + 
                                 " (expected 8B, 16B, 4H, 8H, 2S, or 4S)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "8B") {
        // SMIN Vd.8B, Vn.8B, Vm.8B (64-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=0, size=00
        encoding = 0x0E206C00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "16B") {
        // SMIN Vd.16B, Vn.16B, Vm.16B (128-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=1, size=00
        encoding = 0x4E206C00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // SMIN Vd.4H, Vn.4H, Vm.4H (64-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=0, size=01
        encoding = 0x0E606C00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // SMIN Vd.8H, Vn.8H, Vm.8H (128-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=1, size=01
        encoding = 0x4E606C00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2S") {
        // SMIN Vd.2S, Vn.2S, Vm.2S (64-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=0, size=10
        encoding = 0x0EA06C00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // SMIN Vd.4S, Vn.4S, Vm.4S (128-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm011011nnnnnddddd where Q=1, size=10
        encoding = 0x4EA06C00 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "SMIN " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::SMIN_VECTOR;
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    instr.src_reg2 = rm;
    instr.is_mem_op = false;
    instr.uses_immediate = false;
    
    return instr;
}
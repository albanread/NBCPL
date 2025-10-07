// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_sminp_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // SMINP (Signed Minimum Pairwise) vector instruction
    // SMINP Vd.<T>, Vn.<T>, Vm.<T>
    // Pairwise signed minimum of elements
    
    if (arrangement != "8B" && arrangement != "16B" && arrangement != "4H" && 
        arrangement != "8H" && arrangement != "2S" && arrangement != "4S") {
        throw std::runtime_error("Invalid arrangement for SMINP vector: " + arrangement + 
                                 " (expected 8B, 16B, 4H, 8H, 2S, or 4S)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "8B") {
        // SMINP Vd.8B, Vn.8B, Vm.8B (64-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=0, size=00
        encoding = 0x0E20AC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "16B") {
        // SMINP Vd.16B, Vn.16B, Vm.16B (128-bit vector, 8-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=1, size=00
        encoding = 0x4E20AC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // SMINP Vd.4H, Vn.4H, Vm.4H (64-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=0, size=01
        encoding = 0x0E60AC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // SMINP Vd.8H, Vn.8H, Vm.8H (128-bit vector, 16-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=1, size=01
        encoding = 0x4E60AC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2S") {
        // SMINP Vd.2S, Vn.2S, Vm.2S (64-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=0, size=10
        encoding = 0x0EA0AC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // SMINP Vd.4S, Vn.4S, Vm.4S (128-bit vector, 32-bit elements)
        // 0Q001110xx1mmmmm101011nnnnnddddd where Q=1, size=10
        encoding = 0x4EA0AC00 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "SMINP " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::SMINP_VECTOR;
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    instr.src_reg2 = rm;
    instr.is_mem_op = false;
    instr.uses_immediate = false;
    
    return instr;
}
// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_uzp2_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // UZP2 (Unzip 2) vector instruction
    // UZP2 Vd.<T>, Vn.<T>, Vm.<T>
    // Extracts odd-numbered elements from two vectors
    
    if (arrangement != "8B" && arrangement != "16B" && arrangement != "4H" && 
        arrangement != "8H" && arrangement != "2S" && arrangement != "4S" && arrangement != "2D") {
        throw std::runtime_error("Invalid arrangement for UZP2 vector: " + arrangement + 
                                 " (expected 8B, 16B, 4H, 8H, 2S, 4S, or 2D)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "8B") {
        // UZP2 Vd.8B, Vn.8B, Vm.8B (64-bit vector, 8-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=0, size=00
        encoding = 0x0E005800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "16B") {
        // UZP2 Vd.16B, Vn.16B, Vm.16B (128-bit vector, 8-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=1, size=00
        encoding = 0x4E005800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // UZP2 Vd.4H, Vn.4H, Vm.4H (64-bit vector, 16-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=0, size=01
        encoding = 0x0E405800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // UZP2 Vd.8H, Vn.8H, Vm.8H (128-bit vector, 16-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=1, size=01
        encoding = 0x4E405800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2S") {
        // UZP2 Vd.2S, Vn.2S, Vm.2S (64-bit vector, 32-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=0, size=10
        encoding = 0x0E805800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // UZP2 Vd.4S, Vn.4S, Vm.4S (128-bit vector, 32-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=1, size=10
        encoding = 0x4E805800 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2D") {
        // UZP2 Vd.2D, Vn.2D, Vm.2D (128-bit vector, 64-bit elements)
        // 0Q001110xx0mmmmm010110nnnnnddddd where Q=1, size=11
        encoding = 0x4EC05800 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "UZP2 " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::UZP2_VECTOR;
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    instr.src_reg2 = rm;
    instr.is_mem_op = false;
    instr.uses_immediate = false;
    
    return instr;
}
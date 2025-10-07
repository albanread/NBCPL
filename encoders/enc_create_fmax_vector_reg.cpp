// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_fmax_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // FMAX (Floating-point Maximum) vector instruction
    // FMAX Vd.<T>, Vn.<T>, Vm.<T>
    // Element-wise floating-point maximum
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "2D" && arrangement != "4H" && arrangement != "8H") {
        throw std::runtime_error("Invalid arrangement for FMAX vector: " + arrangement + 
                                 " (expected 2S, 4S, 2D, 4H, or 8H)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "2S") {
        // FMAX Vd.2S, Vn.2S, Vm.2S (64-bit vector, single precision)
        // 0Q001110xx1mmmmm111101nnnnnddddd where Q=0, sz=00
        encoding = 0x0E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // FMAX Vd.4S, Vn.4S, Vm.4S (128-bit vector, single precision)
        // 0Q001110xx1mmmmm111101nnnnnddddd where Q=1, sz=00
        encoding = 0x4E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2D") {
        // FMAX Vd.2D, Vn.2D, Vm.2D (128-bit vector, double precision)
        // 0Q001110xx1mmmmm111101nnnnnddddd where Q=1, sz=01
        encoding = 0x4E60F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // FMAX Vd.4H, Vn.4H, Vm.4H (64-bit vector, half precision)
        // 0Q001110xx1mmmmm001101nnnnnddddd where Q=0, sz=10
        encoding = 0x0E403400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // FMAX Vd.8H, Vn.8H, Vm.8H (128-bit vector, half precision)
        // 0Q001110xx1mmmmm001101nnnnnddddd where Q=1, sz=10
        encoding = 0x4E403400 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "FMAX " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::FMAX_VECTOR;
    instr.dest_reg = rd;
    instr.src_reg1 = rn;
    instr.src_reg2 = rm;
    instr.is_mem_op = false;
    instr.uses_immediate = false;
    
    return instr;
}
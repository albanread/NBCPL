// Status: PASS - All FMAXP vector variations tested by NewBCPL --test-encoders
#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_fmaxp_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // FMAXP (Floating-point Maximum Pairwise) vector instruction
    // FMAXP Vd.<T>, Vn.<T>, Vm.<T>
    // Floating-point maximum of pairs of elements
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "4H" && arrangement != "2D") {
        throw std::runtime_error("Invalid arrangement for FMAXP vector: " + arrangement + " (expected 2S, 4S, 4H, or 2D)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "2S") {
        // FMAXP Vd.2S, Vn.2S, Vm.2S (64-bit vector)
        // 00101110001mmmmm111101nnnnnddddd
        encoding = 0x2E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // FMAXP Vd.4S, Vn.4S, Vm.4S (128-bit vector)
        // 01101110001mmmmm111101nnnnnddddd
        encoding = 0x6E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // FMAXP Vd.4H, Vn.4H, Vm.4H (64-bit vector, half precision)
        // 00101110010mmmmm001101nnnnnddddd
        encoding = 0x2E403400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2D") {
        // FMAXP Vd.2D, Vn.2D, Vm.2D (128-bit vector, double precision)
        // 01101110111mmmmm111100nnnnnddddd
        encoding = 0x6E60F000 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "FMAXP " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    return Instruction(encoding, ss.str());
}
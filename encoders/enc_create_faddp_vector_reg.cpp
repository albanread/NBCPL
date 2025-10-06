#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_faddp_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // FADDP (Floating-point Add Pairwise) vector instruction
    // FADDP Vd.<T>, Vn.<T>, Vm.<T>
    // Floating-point addition of pairs of elements
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "4H" && arrangement != "2D") {
        throw std::runtime_error("Invalid arrangement for FADDP vector: " + arrangement + " (expected 2S, 4S, 4H, or 2D)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "2S") {
        // FADDP Vd.2S, Vn.2S, Vm.2S (64-bit vector)
        // 01101110001mmmmm110100nnnnnddddd
        encoding = 0x2E20D400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // FADDP Vd.4S, Vn.4S, Vm.4S (128-bit vector)
        // 01101110101mmmmm110100nnnnnddddd
        encoding = 0x6E20D400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // FADDP Vd.4H, Vn.4H, Vm.4H (64-bit vector, half precision)
        // 00101110010mmmmm000100nnnnnddddd
        encoding = 0x2E401400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2D") {
        // FADDP Vd.2D, Vn.2D, Vm.2D (128-bit vector, double precision)
        // 01101110111mmmmm110100nnnnnddddd
        encoding = 0x6E60D400 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "FADDP " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    return Instruction(encoding, ss.str());
}
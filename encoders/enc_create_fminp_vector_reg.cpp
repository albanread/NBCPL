#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_fminp_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // FMINP (Floating-point Minimum Pairwise) vector instruction
    // FMINP Vd.<T>, Vn.<T>, Vm.<T>
    // Floating-point minimum of pairs of elements
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "2D") {
        throw std::runtime_error("Invalid arrangement for FMINP vector: " + arrangement + " (expected 2S, 4S, or 2D)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "2S") {
        // FMINP Vd.2S, Vn.2S, Vm.2S (64-bit vector)
        // 01101110001mmmmm111101nnnnnddddd
        encoding = 0x2E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // FMINP Vd.4S, Vn.4S, Vm.4S (128-bit vector)
        // 01101110101mmmmm111101nnnnnddddd
        encoding = 0x6E20F400 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2D") {
        // FMINP Vd.2D, Vn.2D, Vm.2D (128-bit vector, double precision)
        // 01101110111mmmmm111101nnnnnddddd
        encoding = 0x6E60F400 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "FMINP " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    return Instruction(encoding, ss.str());
}
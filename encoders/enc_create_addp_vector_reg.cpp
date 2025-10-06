#include "../Encoder.h"
#include <sstream>

Instruction Encoder::create_addp_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    // ADDP (Add Pairwise) vector instruction
    // ADDP Vd.<T>, Vn.<T>, Vm.<T>
    // Integer addition of pairs of elements
    
    if (arrangement != "8B" && arrangement != "16B" && arrangement != "4H" && arrangement != "8H" && arrangement != "2S" && arrangement != "4S") {
        throw std::runtime_error("Invalid arrangement for ADDP vector: " + arrangement + " (expected 8B, 16B, 4H, 8H, 2S, or 4S)");
    }
    
    uint32_t encoding;
    
    if (arrangement == "8B") {
        // ADDP Vd.8B, Vn.8B, Vm.8B (64-bit vector, 8-bit elements)
        // 00001110001mmmmm101111nnnnnddddd
        encoding = 0x0E20BC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "16B") {
        // ADDP Vd.16B, Vn.16B, Vm.16B (128-bit vector, 8-bit elements)
        // 01001110001mmmmm101111nnnnnddddd
        encoding = 0x4E20BC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4H") {
        // ADDP Vd.4H, Vn.4H, Vm.4H (64-bit vector, 16-bit elements)
        // 00001110011mmmmm101111nnnnnddddd
        encoding = 0x0E60BC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "8H") {
        // ADDP Vd.8H, Vn.8H, Vm.8H (128-bit vector, 16-bit elements)
        // 01001110011mmmmm101111nnnnnddddd
        encoding = 0x4E60BC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "2S") {
        // ADDP Vd.2S, Vn.2S, Vm.2S (64-bit vector, 32-bit elements)
        // 00001110101mmmmm101111nnnnnddddd
        encoding = 0x0EA0BC00 | (rm << 16) | (rn << 5) | rd;
    } else if (arrangement == "4S") {
        // ADDP Vd.4S, Vn.4S, Vm.4S (128-bit vector, 32-bit elements)
        // 01001110101mmmmm101111nnnnnddddd
        encoding = 0x4EA0BC00 | (rm << 16) | (rn << 5) | rd;
    }

    std::stringstream ss;
    ss << "ADDP " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    
    return Instruction(encoding, ss.str());
}
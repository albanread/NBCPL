#include "../Encoder.h"
#include <stdexcept>
#include <iostream>

Instruction Encoder::enc_create_fsub_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // FSUB vector register instruction
    // FSUB Vd.<T>, Vn.<T>, Vm.<T>
    // Floating-point subtract (vector)
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "2D") {
        throw std::runtime_error("Invalid arrangement for FSUB vector: " + arrangement + " (expected 2S, 4S, or 2D)");
    }
    
    // Extract register numbers
    int vd_num = get_reg_encoding(vd);
    int vn_num = get_reg_encoding(vn);
    int vm_num = get_reg_encoding(vm);
    
    if (vd_num < 0 || vd_num > 31 || vn_num < 0 || vn_num > 31 || vm_num < 0 || vm_num > 31) {
        throw std::runtime_error("Invalid vector register in FSUB vector instruction");
    }
    
    // FSUB vector encoding (Advanced SIMD three same)
    // 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0
    //  0| Q|   0|01110| sz|1|      Rm      |100010|      Rn      |      Rd      |
    //
    // sz=0 for 32-bit (S), sz=1 for 64-bit (D)
    // Q=0 for 64-bit vector (2S/1D), Q=1 for 128-bit vector (4S/2D)
    
    uint32_t instruction = 0x0E200C00; // Base encoding: 0|0|0|01110|0|1|00000|100010|00000|00000
    
    // Set Q bit based on arrangement
    if (arrangement == "4S" || arrangement == "2D") {
        instruction |= (1U << 30); // Set Q=1 for 128-bit
    }
    
    // Set sz bit based on element size
    if (arrangement == "2D") {
        instruction |= (1U << 22); // Set sz=1 for 64-bit elements
    }
    // sz=0 for 32-bit elements (2S, 4S)
    
    // Set register fields
    instruction |= (vm_num << 16);  // Rm field
    instruction |= (vn_num << 5);   // Rn field  
    instruction |= vd_num;          // Rd field
    
    return Instruction(instruction, "FSUB " + vd + "." + arrangement + ", " + vn + "." + arrangement + ", " + vm + "." + arrangement);
}
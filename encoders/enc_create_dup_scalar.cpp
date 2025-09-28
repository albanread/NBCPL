#include "../Encoder.h"
#include <stdexcept>
#include <iostream>

Instruction Encoder::enc_create_dup_scalar(const std::string& vd, const std::string& rn, const std::string& arrangement) {
    // DUP (scalar) instruction
    // DUP Vd.<T>, Rn
    // Duplicate general-purpose register to vector
    
    if (arrangement != "2S" && arrangement != "4S" && arrangement != "8H" && arrangement != "16B" && 
        arrangement != "2D" && arrangement != "4H" && arrangement != "8B") {
        throw std::runtime_error("Invalid arrangement for DUP: " + arrangement + " (expected 2S, 4S, 8H, 16B, 2D, 4H, 8B)");
    }
    
    // Extract register numbers
    int vd_num = get_reg_encoding(vd);
    int rn_num = get_reg_encoding(rn);
    
    if (vd_num < 0 || vd_num > 31 || rn_num < 0 || rn_num > 31) {
        throw std::runtime_error("Invalid register in DUP instruction");
    }
    
    // DUP (scalar) encoding (Advanced SIMD copy)
    // 31|30|29|28|27|26|25|24|23|22|21|20|19|18|17|16|15|14|13|12|11|10|9|8|7|6|5|4|3|2|1|0
    //  0| Q| 0|01110000|    imm5   | 0| 0| 0| 0| 1|   Rn   |   Rd   |
    //
    // Q=0 for 64-bit vector, Q=1 for 128-bit vector
    // imm5 encodes the element size and index
    
    uint32_t instruction = 0x0E000C00; // Base encoding: 0|0|0|01110000|00000|0|0|0|0|1|00000|00000
    
    // Set Q bit and imm5 based on arrangement
    uint32_t imm5 = 0;
    bool is_128bit = false;
    
    if (arrangement == "8B") {
        imm5 = 0x01; // 00001 for 8-bit elements, 64-bit vector
        is_128bit = false;
    } else if (arrangement == "16B") {
        imm5 = 0x01; // 00001 for 8-bit elements, 128-bit vector
        is_128bit = true;
    } else if (arrangement == "4H") {
        imm5 = 0x02; // 00010 for 16-bit elements, 64-bit vector
        is_128bit = false;
    } else if (arrangement == "8H") {
        imm5 = 0x02; // 00010 for 16-bit elements, 128-bit vector
        is_128bit = true;
    } else if (arrangement == "2S") {
        imm5 = 0x04; // 00100 for 32-bit elements, 64-bit vector
        is_128bit = false;
    } else if (arrangement == "4S") {
        imm5 = 0x04; // 00100 for 32-bit elements, 128-bit vector
        is_128bit = true;
    } else if (arrangement == "2D") {
        imm5 = 0x08; // 01000 for 64-bit elements, 128-bit vector
        is_128bit = true;
    }
    
    // Set Q bit for 128-bit vector
    if (is_128bit) {
        instruction |= (1U << 30); // Set Q=1
    }
    
    // Set imm5 field (bits 16-20)
    instruction |= (imm5 << 16);
    
    // Set register fields
    instruction |= (rn_num << 5);   // Rn field
    instruction |= vd_num;          // Rd field
    
    return Instruction(instruction, "DUP " + vd + "." + arrangement + ", " + rn);
}
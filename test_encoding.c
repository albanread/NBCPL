#include <stdint.h>
#include <stdio.h>

// Test to manually verify FCVT instruction encoding
// Based on ARM64 Architecture Reference Manual

void test_fcvt_encoding() {
    printf("Testing FCVT instruction encoding\n\n");
    
    // FCVT Dd, Sn encoding: 0001 1110 0010 0010 1100 00nn nnnd dddd
    // Base opcode: 0x1E22C000
    uint32_t base = 0x1E22C000;
    
    printf("Base FCVT S->D opcode: 0x%08X\n", base);
    
    // Test various register combinations
    for (int s_reg = 0; s_reg <= 2; s_reg++) {
        for (int d_reg = 0; d_reg <= 2; d_reg++) {
            uint32_t instruction = base;
            instruction |= (d_reg & 0x1F);           // Bits 0-4: destination register
            instruction |= ((s_reg & 0x1F) << 5);    // Bits 5-9: source register
            
            printf("fcvt d%d, s%d -> 0x%08X\n", d_reg, s_reg, instruction);
        }
    }
    
    printf("\n");
    
    // Check the specific encoding we're generating: fcvt d0, s2
    uint32_t our_encoding = base | (0 << 0) | (2 << 5);
    printf("Our encoding (fcvt d0, s2): 0x%08X\n", our_encoding);
    
    // Check the encoding from the original question: 0x1e224001
    uint32_t mystery_encoding = 0x1e224001;
    printf("Mystery encoding: 0x%08X\n", mystery_encoding);
    
    // Decode the mystery encoding
    int mystery_d_reg = mystery_encoding & 0x1F;
    int mystery_s_reg = (mystery_encoding >> 5) & 0x1F;
    uint32_t mystery_opcode = mystery_encoding & 0xFFFFFFE0;
    
    printf("Mystery decoded: fcvt d%d, s%d (opcode: 0x%08X)\n", 
           mystery_d_reg, mystery_s_reg, mystery_opcode);
    
    // Check if mystery_opcode matches any known FCVT variants
    if ((mystery_opcode & 0xFFFFFF00) == 0x1E224000) {
        printf("Mystery appears to be FCVT D->S (0x1E624000 base)\n");
    } else if ((mystery_opcode & 0xFFFFFF00) == 0x1E22C000) {
        printf("Mystery appears to be FCVT S->D (0x1E22C000 base)\n");
    } else {
        printf("Mystery opcode doesn't match standard FCVT patterns\n");
    }
    
    printf("\n");
    
    // Test bit manipulation similar to our FPAIR extraction
    printf("Testing FPAIR-like bit patterns:\n");
    
    // Create test FPAIR: 1.0f (0x3F800000) and 2.0f (0x40000000)
    uint32_t f1_bits = 0x3F800000;  // 1.0f
    uint32_t f2_bits = 0x40000000;  // 2.0f
    uint64_t fpair = ((uint64_t)f2_bits << 32) | f1_bits;
    
    printf("FPAIR(1.0, 2.0): 0x%016llX\n", fpair);
    
    // Extract lanes like our UBFX operations
    uint32_t lane0_bits = (uint32_t)(fpair & 0xFFFFFFFF);
    uint32_t lane1_bits = (uint32_t)(fpair >> 32);
    
    printf("Lane 0 bits: 0x%08X\n", lane0_bits);
    printf("Lane 1 bits: 0x%08X\n", lane1_bits);
    
    // Reinterpret as floats
    float lane0_float = *(float*)&lane0_bits;
    float lane1_float = *(float*)&lane1_bits;
    
    printf("Lane 0 as float: %f\n", lane0_float);
    printf("Lane 1 as float: %f\n", lane1_float);
    
    // Convert to double (what FCVT should do)
    double lane0_double = (double)lane0_float;
    double lane1_double = (double)lane1_float;
    
    printf("Lane 0 as double: %f\n", lane0_double);
    printf("Lane 1 as double: %f\n", lane1_double);
}

int main() {
    test_fcvt_encoding();
    return 0;
}
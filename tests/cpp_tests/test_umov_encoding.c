#include <stdio.h>
#include <stdint.h>

// Test program to encode UMOV instruction correctly
// Target: mov.s w20, v0[0] -> 0x0e043c14
// Target: mov.s w20, v0[1] -> 0x0e0c3c14

uint32_t encode_umov_try1(int wd_reg, int vn_reg, int lane) {
    // Try 1: Current failing approach
    uint32_t instruction = 0;
    instruction |= (0x0E << 24);  // opcode
    instruction |= (0 << 22);     // SS=00
    instruction |= ((lane == 0 ? 0x04 : 0x0C) << 16);  // imm5
    instruction |= (0x0F << 10);  // fixed field
    instruction |= ((vn_reg & 0x1F) << 5);
    instruction |= (wd_reg & 0x1F);
    return instruction;
}

uint32_t encode_umov_try2(int wd_reg, int vn_reg, int lane) {
    // Try 2: Different base opcode - maybe it's not 0x0E
    uint32_t instruction = 0;
    instruction |= (0x4E << 24);  // Try 0x4E instead
    instruction |= (0 << 22);     // SS=00
    instruction |= ((lane == 0 ? 0x04 : 0x0C) << 16);
    instruction |= (0x0F << 10);
    instruction |= ((vn_reg & 0x1F) << 5);
    instruction |= (wd_reg & 0x1F);
    return instruction;
}

uint32_t encode_umov_try3(int wd_reg, int vn_reg, int lane) {
    // Try 3: Maybe SS should be 10 (2) for 32-bit
    uint32_t instruction = 0;
    instruction |= (0x0E << 24);
    instruction |= (2 << 22);     // SS=10 instead of 00
    instruction |= ((lane == 0 ? 0x04 : 0x0C) << 16);
    instruction |= (0x0F << 10);
    instruction |= ((vn_reg & 0x1F) << 5);
    instruction |= (wd_reg & 0x1F);
    return instruction;
}

uint32_t encode_umov_try4(int wd_reg, int vn_reg, int lane) {
    // Try 4: Different imm5 values
    uint32_t instruction = 0;
    instruction |= (0x0E << 24);
    instruction |= (0 << 22);
    instruction |= ((lane == 0 ? 0x10 : 0x12) << 16);  // Try 10000, 10010
    instruction |= (0x0F << 10);
    instruction |= ((vn_reg & 0x1F) << 5);
    instruction |= (wd_reg & 0x1F);
    return instruction;
}

uint32_t encode_umov_try5(int wd_reg, int vn_reg, int lane) {
    // Try 5: Fixed field might be wrong
    uint32_t instruction = 0;
    instruction |= (0x0E << 24);
    instruction |= (0 << 22);
    instruction |= ((lane == 0 ? 0x04 : 0x0C) << 16);
    instruction |= (0x3C << 10);  // Try 0x3C instead of 0x0F
    instruction |= ((vn_reg & 0x1F) << 5);
    instruction |= (wd_reg & 0x1F);
    return instruction;
}

int main() {
    printf("Testing multiple UMOV instruction encodings to find the correct one:\n");
    printf("Target: mov.s w20, v0[0] should be 0x0e043c14\n");
    printf("Target: mov.s w20, v0[1] should be 0x0e0c3c14\n\n");
    
    printf("Try 1 (current approach):\n");
    uint32_t try1_0 = encode_umov_try1(20, 0, 0);
    uint32_t try1_1 = encode_umov_try1(20, 0, 1);
    printf("  Lane 0: 0x%08x, Lane 1: 0x%08x\n", try1_0, try1_1);
    
    printf("Try 2 (0x4E opcode):\n");
    uint32_t try2_0 = encode_umov_try2(20, 0, 0);
    uint32_t try2_1 = encode_umov_try2(20, 0, 1);
    printf("  Lane 0: 0x%08x, Lane 1: 0x%08x\n", try2_0, try2_1);
    
    printf("Try 3 (SS=10):\n");
    uint32_t try3_0 = encode_umov_try3(20, 0, 0);
    uint32_t try3_1 = encode_umov_try3(20, 0, 1);
    printf("  Lane 0: 0x%08x, Lane 1: 0x%08x\n", try3_0, try3_1);
    
    printf("Try 4 (different imm5):\n");
    uint32_t try4_0 = encode_umov_try4(20, 0, 0);
    uint32_t try4_1 = encode_umov_try4(20, 0, 1);
    printf("  Lane 0: 0x%08x, Lane 1: 0x%08x\n", try4_0, try4_1);
    
    printf("Try 5 (0x3C fixed field):\n");
    uint32_t try5_0 = encode_umov_try5(20, 0, 0);
    uint32_t try5_1 = encode_umov_try5(20, 0, 1);
    printf("  Lane 0: 0x%08x, Lane 1: 0x%08x\n", try5_0, try5_1);
    
    printf("\nLooking for which one matches 0x0e043c14 and 0x0e0c3c14...\n");
    
    return 0;
}
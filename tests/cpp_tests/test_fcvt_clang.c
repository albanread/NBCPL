#include <stdint.h>
#include <stdio.h>

// Test function that does the exact same operations as our FPAIR lane access:
// 1. Extract 32-bit value from 64-bit word
// 2. Move to S register via FMOV W->S 
// 3. Convert S->D via FCVT
double extract_and_convert(uint64_t value, int lane) {
    uint32_t bits;
    if (lane == 0) {
        // Extract lower 32 bits
        bits = (uint32_t)(value & 0xFFFFFFFF);
    } else {
        // Extract upper 32 bits  
        bits = (uint32_t)(value >> 32);
    }
    
    // Reinterpret bits as float and convert to double
    float f = *(float*)&bits;
    return (double)f;
}

int main() {
    printf("Testing FCVT S->D encoding with clang\n");
    
    // Test with the same values that fail in our BCPL code
    uint32_t f1_bits = 0x3F800000; // 1.0f
    uint32_t f2_bits = 0x40000000; // 2.0f
    uint64_t fpair = ((uint64_t)f2_bits << 32) | f1_bits;
    
    printf("FPAIR bits: 0x%016llX\n", fpair);
    
    // Extract both lanes
    double lane0 = extract_and_convert(fpair, 0);
    double lane1 = extract_and_convert(fpair, 1);
    
    printf("Lane 0: %f\n", lane0);
    printf("Lane 1: %f\n", lane1);
    
    // Test with values that work in BCPL
    uint32_t f3_bits = 0x40490FD0; // ~3.14159f
    uint32_t f4_bits = 0x402DF854; // ~2.71828f
    uint64_t fpair2 = ((uint64_t)f4_bits << 32) | f3_bits;
    
    printf("\nFPAIR2 bits: 0x%016llX\n", fpair2);
    
    double lane0_2 = extract_and_convert(fpair2, 0);
    double lane1_2 = extract_and_convert(fpair2, 1);
    
    printf("Lane 0: %f\n", lane0_2);
    printf("Lane 1: %f\n", lane1_2);
    
    return 0;
}
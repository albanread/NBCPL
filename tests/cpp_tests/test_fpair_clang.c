#include <stdint.h>
#include <stdio.h>

// Simulate FPAIR structure - 64-bit value with two 32-bit floats
typedef struct {
    uint64_t bits;
} fpair_t;

// Create FPAIR from two floats
fpair_t make_fpair(float first, float second) {
    fpair_t result;
    uint32_t first_bits = *(uint32_t*)&first;
    uint32_t second_bits = *(uint32_t*)&second;
    result.bits = ((uint64_t)second_bits << 32) | first_bits;
    return result;
}

// Extract lane 0 (first element, bits 0-31)
float fpair_lane0(fpair_t fp) {
    uint32_t bits = (uint32_t)(fp.bits & 0xFFFFFFFF);
    return *(float*)&bits;
}

// Extract lane 1 (second element, bits 32-63)  
float fpair_lane1(fpair_t fp) {
    uint32_t bits = (uint32_t)(fp.bits >> 32);
    return *(float*)&bits;
}

int main() {
    printf("Testing FPAIR-like operations with clang\n");
    
    // Test with known values
    fpair_t f1 = make_fpair(1.0f, 2.0f);
    printf("Created fpair(1.0, 2.0)\n");
    printf("Bit pattern: 0x%016lX\n", f1.bits);
    
    // Extract lanes
    float lane0 = fpair_lane0(f1);
    float lane1 = fpair_lane1(f1);
    
    printf("Lane 0: %f\n", lane0);
    printf("Lane 1: %f\n", lane1);
    
    // Test with the same values as our BCPL test
    fpair_t f2 = make_fpair(42.0f, 99.0f);
    printf("\nCreated fpair(42.0, 99.0)\n");
    printf("Bit pattern: 0x%016lX\n", f2.bits);
    
    float lane0_2 = fpair_lane0(f2);
    float lane1_2 = fpair_lane1(f2);
    
    printf("Lane 0: %f\n", lane0_2);
    printf("Lane 1: %f\n", lane1_2);
    
    return 0;
}
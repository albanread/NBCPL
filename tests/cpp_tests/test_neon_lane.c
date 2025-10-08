#include <stdint.h>
#include <stdio.h>
#include <arm_neon.h>

// Test NEON lane extraction to see correct ARM64 syntax
double extract_lane_0(uint64_t fpair_bits) {
    // Load the 64-bit value into a D register (viewed as V register)
    uint64x1_t u64_vec = vcreate_u64(fpair_bits);
    float32x2_t vec = vreinterpret_f32_u64(u64_vec);
    
    // Extract lane 0 and convert to double
    float lane0_f32 = vget_lane_f32(vec, 0);
    return (double)lane0_f32;
}

double extract_lane_1(uint64_t fpair_bits) {
    // Load the 64-bit value into a D register (viewed as V register)
    uint64x1_t u64_vec = vcreate_u64(fpair_bits);
    float32x2_t vec = vreinterpret_f32_u64(u64_vec);
    
    // Extract lane 1 and convert to double
    float lane1_f32 = vget_lane_f32(vec, 1);
    return (double)lane1_f32;
}

// Alternative: Manual bit manipulation like our UBFX approach
double extract_lane_0_manual(uint64_t fpair_bits) {
    uint32_t bits = (uint32_t)(fpair_bits & 0xFFFFFFFF);
    float f = *(float*)&bits;
    return (double)f;
}

double extract_lane_1_manual(uint64_t fpair_bits) {
    uint32_t bits = (uint32_t)(fpair_bits >> 32);
    float f = *(float*)&bits;
    return (double)f;
}

int main() {
    printf("Testing NEON lane extraction vs manual extraction\n");
    
    // Create test value: 1.0f and 2.0f
    uint32_t f1_bits = 0x3F800000; // 1.0f
    uint32_t f2_bits = 0x40000000; // 2.0f
    uint64_t fpair = ((uint64_t)f2_bits << 32) | f1_bits;
    
    printf("FPAIR bits: 0x%016llX\n", fpair);
    
    // Test NEON approach
    double lane0_neon = extract_lane_0(fpair);
    double lane1_neon = extract_lane_1(fpair);
    
    printf("NEON lane 0: %f\n", lane0_neon);
    printf("NEON lane 1: %f\n", lane1_neon);
    
    // Test manual approach
    double lane0_manual = extract_lane_0_manual(fpair);
    double lane1_manual = extract_lane_1_manual(fpair);
    
    printf("Manual lane 0: %f\n", lane0_manual);
    printf("Manual lane 1: %f\n", lane1_manual);
    
    return 0;
}
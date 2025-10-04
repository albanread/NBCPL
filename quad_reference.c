// Reference C code to generate correct UMOV opcodes for QUAD (16-bit elements)
// Compile with: clang -target aarch64-apple-darwin -c quad_reference.c -o quad_reference.o
// Disassemble with: objdump -d quad_reference.o

#include <arm_neon.h>

void test_quad_umov_reference() {
    // Create a test vector with known values
    int16x4_t v0 = {10, 20, 30, 40};
    
    // Extract each lane to see the correct opcodes
    int lane0 = vget_lane_s16(v0, 0);  // Should generate umov w, v.h[0]
    int lane1 = vget_lane_s16(v0, 1);  // Should generate umov w, v.h[1] 
    int lane2 = vget_lane_s16(v0, 2);  // Should generate umov w, v.h[2]
    int lane3 = vget_lane_s16(v0, 3);  // Should generate umov w, v.h[3]
    
    // Use the values to prevent optimization
    volatile int result = lane0 + lane1 + lane2 + lane3;
}

// Alternative approach using inline assembly to get exact opcodes
void test_quad_umov_asm() {
    int16x4_t v0 = {10, 20, 30, 40};
    int result;
    
    // Direct assembly for lane 0
    __asm__("umov %w0, %1.h[0]" : "=r"(result) : "w"(v0));
    
    // Direct assembly for lane 1  
    __asm__("umov %w0, %1.h[1]" : "=r"(result) : "w"(v0));
    
    // Direct assembly for lane 2
    __asm__("umov %w0, %1.h[2]" : "=r"(result) : "w"(v0));
    
    // Direct assembly for lane 3
    __asm__("umov %w0, %1.h[3]" : "=r"(result) : "w"(v0));
}
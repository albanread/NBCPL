// testing/encoder_validation/TestableEncoders.cpp
#include "../../Encoder.h"
#include "../../AST.h"

// ===== WRAPPERS FOR ENCODERS FOLDER =====

/**
 * @brief Wrapper for Encoder::create_add_reg for direct testing.
 */
Instruction test_create_add_reg() {
    // Example: ADD X0, X1, X2
    return Encoder::create_add_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_and_reg for direct testing.
 */
Instruction test_create_and_reg() {
    // Example: AND X0, X1, X2
    return Encoder::create_and_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_cmp_imm for direct testing.
 */
Instruction test_create_cmp_imm() {
    // Example: CMP X0, #5
    return Encoder::create_cmp_imm("x0", 5);
}

/**
 * @brief Wrapper for Encoder::create_cmp_reg for direct testing.
 */
Instruction test_create_cmp_reg() {
    // Example: CMP X0, X1
    return Encoder::create_cmp_reg("x0", "x1");
}

/**
 * @brief Wrapper for Encoder::create_eor_reg for direct testing.
 */
Instruction test_create_eor_reg() {
    // Example: EOR X0, X1, X2
    return Encoder::create_eor_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_lsl_imm for direct testing.
 */
Instruction test_create_lsl_imm() {
    // Example: LSL X0, X1, #16
    return Encoder::create_lsl_imm("x0", "x1", 16);
}

/**
 * @brief Wrapper for Encoder::create_lsl_reg for direct testing.
 */
Instruction test_create_lsl_reg() {
    // Example: LSL X0, X1, X2
    return Encoder::create_lsl_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_lsr_reg for direct testing.
 */
Instruction test_create_lsr_reg() {
    // Example: LSR X0, X1, X2
    return Encoder::create_lsr_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_mov_reg for direct testing.
 */
Instruction test_create_mov_reg() {
    // Example: MOV X0, X1
    return Encoder::create_mov_reg("x0", "x1");
}

/**
 * @brief Wrapper for Encoder::create_movk_imm for direct testing.
 */
Instruction test_create_movk_imm() {
    // Example: MOVK X0, #0x1234, LSL #16
    return Encoder::create_movk_imm("x0", 0x1234, 16);
}

/**
 * @brief Wrapper for Encoder::create_movz_imm for direct testing.
 */
Instruction test_create_movz_imm() {
    // Example: MOVZ X0, #0x1234, LSL #16
    return Encoder::create_movz_imm("x0", 0x1234, 16);
}

/**
 * @brief Wrapper for Encoder::create_mul_reg for direct testing.
 */
Instruction test_create_mul_reg() {
    // Example: MUL X0, X1, X2
    return Encoder::create_mul_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_orr_reg for direct testing.
 */
Instruction test_create_orr_reg() {
    // Example: ORR X0, X1, X2
    return Encoder::create_orr_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_sdiv_reg for direct testing.
 */
Instruction test_create_sdiv_reg() {
    // Example: SDIV X0, X1, X2
    return Encoder::create_sdiv_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_sub_imm for direct testing.
 */
Instruction test_create_sub_imm() {
    // Example: SUB X0, X1, #16
    return Encoder::create_sub_imm("x0", "x1", 16);
}

/**
 * @brief Wrapper for Encoder::create_sub_reg for direct testing.
 */
Instruction test_create_sub_reg() {
    // Example: SUB X0, X1, X2
    return Encoder::create_sub_reg("x0", "x1", "x2");
}

/**
 * @brief Wrapper for Encoder::create_brk for direct testing.
 */
Instruction test_create_brk() {
    // Example: BRK #0
    return Encoder::create_brk(0);
}

/**
 * @brief Wrapper for Encoder::create_cset_eq for direct testing.
 */
Instruction test_create_cset_eq() {
    // Example: CSET X0, EQ
    return Encoder::create_cset_eq("x0");
}

/**
 * @brief Wrapper for Encoder::create_cset for direct testing.
 */
Instruction test_create_cset() {
    // Example: CSET X0, NE
    return Encoder::create_cset("x0", "NE");
}

/**
 * @brief Wrapper for Encoder::create_csetm_eq for direct testing.
 */
Instruction test_create_csetm_eq() {
    // Example: CSETM X0, EQ
    return Encoder::create_csetm_eq("x0");
}

/**
 * @brief Refactored NEON encoder functions that return Instruction objects directly.
 * These are pure functions suitable for automated testing against Clang output.
 * 
 * Each function takes the minimal required parameters and returns a complete
 * Instruction object with both assembly text and binary encoding.
 */

// ===== FMINP ENCODERS =====

Instruction gen_neon_fminp_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMINP.4S: Vector pairwise minimum of single-precision floats
    // Expected encoding from Clang: 6ea2f420 for fminp v0.4s, v1.4s, v2.4s
    // This uses the actual ARM64 encoding with proper register layout
    
    return Encoder::create_fminp_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_fminp_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMINP.2S: Vector pairwise minimum of 2x single-precision floats  
    // Expected encoding from Clang: 2ea2f420 for fminp v0.2s, v1.2s, v2.2s
    
    return Encoder::create_fminp_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_neon_fminp_4h(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMINP.4H: Vector pairwise minimum of 4x half-precision floats (FQUAD)
    // Expected encoding from Clang: 2ec23420 for fminp v0.4h, v1.4h, v2.4h
    // Note: 4H arrangement uses different opcode pattern than 2S/4S
    
    return Encoder::create_fminp_vector_reg("v0", "v1", "v2", "4H");
}

// ===== FMAXP ENCODERS =====

Instruction gen_neon_fmaxp_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMAXP.4S: Vector pairwise maximum of single-precision floats
    // Expected encoding from Clang: 6e22f420 for fmaxp v0.4s, v1.4s, v2.4s
    
    return Encoder::create_fmaxp_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_fmaxp_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMAXP.2S: Vector pairwise maximum of 2x single-precision floats
    // Expected encoding from Clang: 2e22f420 for fmaxp v0.2s, v1.2s, v2.2s
    
    return Encoder::create_fmaxp_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_neon_fmaxp_4h(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMAXP.4H: Vector pairwise maximum of 4x half-precision floats (FQUAD)
    // Expected encoding from Clang: 2e423420 for fmaxp v0.4h, v1.4h, v2.4h
    
    return Encoder::create_fmaxp_vector_reg("v0", "v1", "v2", "4H");
}

// ===== FADDP ENCODERS =====

Instruction gen_neon_faddp_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADDP.4S: Vector pairwise addition of single-precision floats
    // Expected encoding from Clang: 6e22d420 for faddp v0.4s, v1.4s, v2.4s
    
    return Encoder::create_faddp_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_faddp_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADDP.2S: Vector pairwise addition of 2x single-precision floats
    // Expected encoding from Clang: 2e22d420 for faddp v0.2s, v1.2s, v2.2s
    
    return Encoder::create_faddp_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_neon_faddp_4h(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADDP.4H: Vector pairwise addition of 4x half-precision floats (FQUAD)
    // Expected encoding from Clang: 2e421420 for faddp v0.4h, v1.4h, v2.4h
    
    return Encoder::create_faddp_vector_reg("v0", "v1", "v2", "4H");
}

// ===== ADDP ENCODERS (Integer) =====

Instruction gen_neon_addp_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADDP.4S: Vector pairwise addition of 4x 32-bit integers
    // Expected encoding from Clang: 4ea2bc20 for addp v0.4s, v1.4s, v2.4s
    
    return Encoder::create_addp_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_addp_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADDP.2S: Vector pairwise addition of 2x 32-bit integers
    // Expected encoding from Clang: 0ea2bc20 for addp v0.2s, v1.2s, v2.2s
    
    return Encoder::create_addp_vector_reg("v0", "v1", "v2", "2S");
}

// ===== SMINP ENCODERS (Signed Integer Minimum) =====

Instruction gen_neon_sminp_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMINP.4S: Vector pairwise minimum of 4x signed 32-bit integers
    // Expected encoding from Clang: 4ea2ac20 for sminp v0.4s, v1.4s, v2.4s
    
    return Encoder::create_sminp_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_sminp_8h(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMINP.8H: Vector pairwise minimum of 8x signed 16-bit integers
    // Expected encoding from Clang: 4e62ac20 for sminp v0.8h, v1.8h, v2.8h
    
    return Encoder::create_sminp_vector_reg("v0", "v1", "v2", "8H");
}

Instruction gen_neon_sminp_16b(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMINP.16B: Vector pairwise minimum of 16x signed 8-bit integers
    // Expected encoding from Clang: 4e22ac20 for sminp v0.16b, v1.16b, v2.16b
    
    return Encoder::create_sminp_vector_reg("v0", "v1", "v2", "16B");
}

// ===== SMAX ENCODERS (Signed Integer Maximum) =====

Instruction gen_neon_smax_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMAX.4S: Vector element-wise maximum of 4x signed 32-bit integers
    // Expected encoding from Clang: 4ea26420 for smax v0.4s, v1.4s, v2.4s
    
    return Encoder::create_smax_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_smax_8h(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMAX.8H: Vector element-wise maximum of 8x signed 16-bit integers
    // Expected encoding from Clang: 4e626420 for smax v0.8h, v1.8h, v2.8h
    
    return Encoder::create_smax_vector_reg("v0", "v1", "v2", "8H");
}

Instruction gen_neon_smax_16b(const PairwiseReductionLoopStatement& node) {
    // ARM64 SMAX.16B: Vector element-wise maximum of 16x signed 8-bit integers
    // Expected encoding from Clang: 4e226420 for smax v0.16b, v1.16b, v2.16b
    
    return Encoder::create_smax_vector_reg("v0", "v1", "v2", "16B");
}

// ===== ADD ENCODERS (Integer Addition) =====

Instruction gen_neon_add_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADD.4S: Vector element-wise addition of 4x signed 32-bit integers
    // Expected encoding from Clang: 4ea18402 for add v2.4s, v0.4s, v1.4s
    
    return Encoder::create_add_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_add_8h(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADD.8H: Vector element-wise addition of 8x signed 16-bit integers
    // Expected encoding from Clang: 4e618402 for add v2.8h, v0.8h, v1.8h
    
    return Encoder::create_add_vector_reg("v0", "v1", "v2", "8H");
}

Instruction gen_neon_add_16b(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADD.16B: Vector element-wise addition of 16x signed 8-bit integers
    // Expected encoding from Clang: 4e218402 for add v2.16b, v0.16b, v1.16b
    
    return Encoder::create_add_vector_reg("v0", "v1", "v2", "16B");
}

// ===== SUB ENCODERS (Integer Subtraction) =====

Instruction gen_neon_sub_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 SUB.4S: Vector element-wise subtraction of 4x signed 32-bit integers
    // Expected encoding from Clang: 4ea28420 for sub v0.4s, v1.4s, v2.4s
    
    return Encoder::create_sub_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_sub_8h(const PairwiseReductionLoopStatement& node) {
    // ARM64 SUB.8H: Vector element-wise subtraction of 8x signed 16-bit integers  
    // Expected encoding from Clang: 4e628420 for sub v0.8h, v1.8h, v2.8h
    
    return Encoder::create_sub_vector_reg("v0", "v1", "v2", "8H");
}

Instruction gen_neon_sub_16b(const PairwiseReductionLoopStatement& node) {
    // ARM64 SUB.16B: Vector element-wise subtraction of 16x signed 8-bit integers
    // Expected encoding from Clang: 4e228420 for sub v0.16b, v1.16b, v2.16b
    
    return Encoder::create_sub_vector_reg("v0", "v1", "v2", "16B");
}

// ===== FMIN ENCODERS (Floating-point Minimum) =====

Instruction gen_neon_fmin_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMIN.4S: Vector element-wise minimum of 4x single-precision floats
    // Expected encoding from Clang: 4ea2f420 for fmin v0.4s, v1.4s, v2.4s
    
    return Encoder::create_fmin_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_neon_fmin_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMIN.2S: Vector element-wise minimum of 2x single-precision floats
    // Expected encoding from Clang: 2ea2f420 for fmin v0.2s, v1.2s, v2.2s
    
    return Encoder::create_fmin_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_neon_fmin_2d(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMIN.2D: Vector element-wise minimum of 2x double-precision floats
    // Expected encoding from Clang: 4ee2f420 for fmin v0.2d, v1.2d, v2.2d
    
    return Encoder::create_fmin_vector_reg("v0", "v1", "v2", "2D");
}

// ===== LEGACY SCALAR ENCODERS =====

Instruction gen_scalar_nop(const PairwiseReductionLoopStatement& node) {
    // ARM64 NOP instruction - well-known encoding for testing
    return Encoder::create_nop();
}

Instruction gen_scalar_add_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADD register instruction: ADD X0, X1, X2
    return Encoder::create_add_reg("x0", "x1", "x2");
}

Instruction gen_scalar_sub_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 SUB register instruction: SUB X0, X1, X2
    return Encoder::create_sub_reg("x0", "x1", "x2");
}

Instruction gen_scalar_mul_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 MUL register instruction: MUL X0, X1, X2
    return Encoder::create_mul_reg("x0", "x1", "x2");
}

Instruction gen_scalar_mov_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 MOV register instruction: MOV X0, X1
    return Encoder::create_mov_reg("x0", "x1");
}

// ===== LEGACY FLOATING POINT ENCODERS =====

Instruction gen_fp_fadd_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADD scalar instruction: FADD D0, D1, D2
    return Encoder::create_fadd_reg("d0", "d1", "d2");
}

Instruction gen_fp_fsub_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FSUB scalar instruction: FSUB D0, D1, D2
    return Encoder::create_fsub_reg("d0", "d1", "d2");
}

Instruction gen_fp_fmul_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMUL scalar instruction: FMUL D0, D1, D2
    return Encoder::create_fmul_reg("d0", "d1", "d2");
}

Instruction gen_fp_fdiv_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FDIV scalar instruction: FDIV D0, D1, D2
    return Encoder::create_fdiv_reg("d0", "d1", "d2");
}

// ===== LEGACY VECTOR FLOATING POINT ENCODERS =====

Instruction gen_vec_fadd_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADD vector instruction: FADD V0.4S, V1.4S, V2.4S
    return Encoder::create_fadd_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_vec_fadd_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADD vector instruction: FADD V0.2S, V1.2S, V2.2S
    return Encoder::create_fadd_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_vec_fadd_2d(const PairwiseReductionLoopStatement& node) {
    // ARM64 FADD vector instruction: FADD V0.2D, V1.2D, V2.2D
    return Encoder::create_fadd_vector_reg("v0", "v1", "v2", "2D");
}

Instruction gen_vec_fsub_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FSUB vector instruction: FSUB V0.4S, V1.4S, V2.4S
    return Encoder::enc_create_fsub_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_vec_fsub_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FSUB vector instruction: FSUB V0.2S, V1.2S, V2.2S
    return Encoder::enc_create_fsub_vector_reg("v0", "v1", "v2", "2S");
}

Instruction gen_vec_fmul_4s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMUL vector instruction: FMUL V0.4S, V1.4S, V2.4S
    return Encoder::create_fmul_vector_reg("v0", "v1", "v2", "4S");
}

Instruction gen_vec_fmul_2s(const PairwiseReductionLoopStatement& node) {
    // ARM64 FMUL vector instruction: FMUL V0.2S, V1.2S, V2.2S
    return Encoder::create_fmul_vector_reg("v0", "v1", "v2", "2S");
}

// ===== LEGACY LOAD/STORE ENCODERS =====

Instruction gen_mem_ldr_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 LDR immediate instruction: LDR X0, [X1, #8]
    return Encoder::create_ldr_imm("x0", "x1", 8);
}

Instruction gen_mem_str_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 STR immediate instruction: STR X0, [X1, #8]
    return Encoder::create_str_imm("x0", "x1", 8);
}

Instruction gen_mem_ldp_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 LDP immediate instruction: LDP X0, X1, [X2, #16]
    return Encoder::create_ldp_imm("x0", "x1", "x2", 16);
}

Instruction gen_mem_stp_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 STP immediate instruction: STP X0, X1, [X2, #16]
    return Encoder::create_stp_imm("x0", "x1", "x2", 16);
}

// ===== LEGACY ARITHMETIC ENCODERS =====

Instruction gen_scalar_add_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 ADD immediate instruction: ADD X0, X1, #42
    return Encoder::create_add_imm("x0", "x1", 42);
}

Instruction gen_scalar_sub_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 SUB immediate instruction: SUB X0, X1, #16
    return Encoder::create_sub_imm("x0", "x1", 16);
}

Instruction gen_scalar_cmp_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 CMP register instruction: CMP X0, X1
    return Encoder::create_cmp_reg("x0", "x1");
}

Instruction gen_scalar_cmp_imm(const PairwiseReductionLoopStatement& node) {
    // ARM64 CMP immediate instruction: CMP X0, #5
    return Encoder::create_cmp_imm("x0", 5);
}

// ===== LEGACY FLOATING POINT ENCODERS =====

Instruction gen_fp_fcmp_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FCMP register instruction: FCMP S0, S1 (single-precision)
    return Encoder::create_fcmp_reg("s0", "s1");
}

Instruction gen_fp_fsqrt_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FSQRT register instruction: FSQRT S0, S1 (single-precision)
    return Encoder::create_fsqrt_reg("s0", "s1");
}

Instruction gen_fp_fneg_reg(const PairwiseReductionLoopStatement& node) {
    // ARM64 FNEG register instruction: FNEG S0, S1 (single-precision)
    return Encoder::create_fneg_reg("s0", "s1");
}

/**
 * @brief Helper function to create a test Instruction for validation
 * This provides a known-good test case to verify the validation framework works.
 */
Instruction create_test_nop_instruction() {
    // ARM64 NOP instruction - well-known encoding for testing
    Instruction instr;
    instr.encoding = 0xd503201f;  // Real ARM64 NOP encoding
    instr.assembly_text = "nop";
    return instr;
}
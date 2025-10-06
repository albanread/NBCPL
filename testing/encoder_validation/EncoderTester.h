// testing/encoder_validation/EncoderTester.h
#pragma once

#include <string>
#include <vector>
#include <functional>
#include "../../Encoder.h"
#include "../../AST.h"

/**
 * @brief Automated encoder validation framework for NewBCPL
 * 
 * This class provides a comprehensive testing system that verifies NEON encoder
 * functions produce assembly and binary code identical to Clang's output.
 * 
 * Usage:
 *   EncoderTester tester;
 *   bool success = tester.run_all_tests();
 */
class EncoderTester {
public:
    /**
     * @brief Main entry point for encoder validation
     * @return true if all tests pass, false if any fail
     */
    bool run_all_tests();

    /**
     * @brief Test a specific encoder by name
     * @param encoder_name Name of the encoder to test (e.g., "fminp_4s")
     * @return true if test passes, false if it fails
     */
    bool run_single_test(const std::string& encoder_name);

    /**
     * @brief Test encoders matching a wildcard pattern
     * @param pattern Wildcard pattern (e.g., "fminp_*", "*_4s")
     * @return true if all matching tests pass, false if any fail
     */
    bool run_pattern_tests(const std::string& pattern);

    /**
     * @brief List all available encoder names
     * @return vector of encoder names
     */
    std::vector<std::string> list_available_encoders();

    /**
     * @brief Find encoders matching a wildcard pattern
     * @param pattern Wildcard pattern to match
     * @return vector of matching encoder names
     */
    std::vector<std::string> find_matching_encoders(const std::string& pattern);

    /**
     * @brief Test individual FMINP encoders
     */
    bool test_gen_neon_fminp_4s();
    bool test_gen_neon_fminp_2s(); 
    bool test_gen_neon_fminp_4h();

    /**
     * @brief Test individual FMAXP encoders
     */
    bool test_gen_neon_fmaxp_4s();
    bool test_gen_neon_fmaxp_2s();
    bool test_gen_neon_fmaxp_4h();

    /**
     * @brief Test individual FADDP encoders
     */
    bool test_gen_neon_faddp_4s();
    bool test_gen_neon_faddp_2s();
    bool test_gen_neon_faddp_4h();

    /**
     * @brief Test individual ADDP encoders  
     */
    bool test_gen_neon_addp_4s();
    bool test_gen_neon_addp_2s();

    /**
     * @brief Test individual SMINP encoders
     */
    bool test_gen_neon_sminp_4s();
    bool test_gen_neon_sminp_8h();
    bool test_gen_neon_sminp_16b();

    /**
     * @brief Test legacy scalar encoders
     */
    bool test_gen_scalar_nop();
    bool test_gen_scalar_add_reg();
    bool test_gen_scalar_sub_reg();
    bool test_gen_scalar_mul_reg();
    bool test_gen_scalar_mov_reg();

    /**
     * @brief Test legacy floating point encoders
     */
    bool test_gen_fp_fadd_reg();
    bool test_gen_fp_fsub_reg();
    bool test_gen_fp_fmul_reg();
    bool test_gen_fp_fdiv_reg();

    /**
     * @brief Test legacy vector floating point encoders
     */
    bool test_gen_vec_fadd_4s();
    bool test_gen_vec_fadd_2s();
    bool test_gen_vec_fadd_2d();
    bool test_gen_vec_fsub_4s();
    bool test_gen_vec_fsub_2s();
    bool test_gen_vec_fmul_4s();
    bool test_gen_vec_fmul_2s();

    /**
     * @brief Test legacy load/store encoders
     */
    bool test_gen_mem_ldr_imm();
    bool test_gen_mem_str_imm();
    bool test_gen_mem_ldp_imm();
    bool test_gen_mem_stp_imm();

    /**
     * @brief Test legacy branch encoders
     */
    bool test_gen_scalar_add_imm();
    bool test_gen_scalar_sub_imm();
    bool test_gen_scalar_cmp_reg();
    bool test_gen_scalar_cmp_imm();
    bool test_gen_fp_fcmp_reg();
    bool test_gen_fp_fsqrt_reg();
    bool test_gen_fp_fneg_reg();

    /**
     * @brief Test individual SMAX encoders
     */
    bool test_gen_neon_smax_4s();
    bool test_gen_neon_smax_8h();
    bool test_gen_neon_smax_16b();

    /**
     * @brief Test individual ADD encoders
     */
    bool test_gen_neon_add_4s();
    bool test_gen_neon_add_8h();
    bool test_gen_neon_add_16b();

    /**
     * @brief Test individual SUB encoders
     */
    bool test_gen_neon_sub_4s();
    bool test_gen_neon_sub_8h();
    bool test_gen_neon_sub_16b();

    /**
     * @brief Test individual FMIN encoders
     */
    bool test_gen_neon_fmin_4s();
    bool test_gen_neon_fmin_2s();
    bool test_gen_neon_fmin_2d();

    /**
     * @brief Core validation helper that compares encoder output to Clang
     * @param test_name Human-readable test name for reporting
     * @param instruction The Instruction object produced by our encoder
     * @return true if validation passes, false otherwise
     */
    bool runValidation(const std::string& test_name, const Instruction& instruction);

private:

    /**
     * @brief Creates temporary validation directory if needed
     */
    void ensureValidationDirectory();

    /**
     * @brief Validates assembly by compiling with clang
     * @param assembly_text The assembly string to validate
     * @param output_file Path where clang should write the object file
     * @return true if assembly compiles successfully
     */
    bool validateAssembly(const std::string& assembly_text, const std::string& output_file);

    /**
     * @brief Extracts binary encoding from clang-compiled object file
     * @param object_file Path to the object file produced by clang
     * @return The hex encoding as produced by clang, or empty string on failure
     */
    std::string extractClangBinaryEncoding(const std::string& object_file);

    /**
     * @brief Creates mock AST nodes needed for encoder testing
     */
    PairwiseReductionLoopStatement createMockReductionStatement();

    /**
     * @brief Converts uint32_t encoding to hex string for comparison
     */
    std::string encodingToHexString(uint32_t encoding);

    /**
     * @brief Reports test results with detailed failure information
     */
    void reportResult(const std::string& test_name, bool passed, 
                     const std::string& expected_hex = "", 
                     const std::string& actual_hex = "");

    // Test state
    int tests_run = 0;
    int tests_passed = 0;
    std::string validation_dir = "validation";

    // Encoder test mapping
    std::map<std::string, std::function<bool()>> encoder_test_map;

    /**
     * @brief Initialize the encoder test mapping
     */
    void initialize_test_map();

    /**
     * @brief Check if pattern matches name (supports * wildcards)
     */
    bool matches_pattern(const std::string& name, const std::string& pattern);

    /**
     * @brief Suggest similar encoder names for typos
     */
    void suggest_similar_encoders(const std::string& invalid_name);
};

// Forward declarations of refactored encoder functions
// These will be modified to return Instruction objects directly
Instruction gen_neon_fminp_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fminp_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fminp_4h(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_fmaxp_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fmaxp_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fmaxp_4h(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_faddp_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_faddp_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_faddp_4h(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_addp_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_addp_2s(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_sminp_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_sminp_8h(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_sminp_16b(const PairwiseReductionLoopStatement& node);

// Legacy encoder function declarations
Instruction gen_scalar_nop(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_add_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_sub_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_mul_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_mov_reg(const PairwiseReductionLoopStatement& node);

Instruction gen_fp_fadd_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fsub_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fmul_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fdiv_reg(const PairwiseReductionLoopStatement& node);

Instruction gen_vec_fadd_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fadd_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fadd_2d(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fsub_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fsub_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fmul_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_vec_fmul_2s(const PairwiseReductionLoopStatement& node);

Instruction gen_mem_ldr_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_mem_str_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_mem_ldp_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_mem_stp_imm(const PairwiseReductionLoopStatement& node);

Instruction gen_scalar_add_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_sub_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_cmp_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_scalar_cmp_imm(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fcmp_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fsqrt_reg(const PairwiseReductionLoopStatement& node);
Instruction gen_fp_fneg_reg(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_smax_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_smax_8h(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_smax_16b(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_add_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_add_8h(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_add_16b(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_sub_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_sub_8h(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_sub_16b(const PairwiseReductionLoopStatement& node);

Instruction gen_neon_fmin_4s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fmin_2s(const PairwiseReductionLoopStatement& node);
Instruction gen_neon_fmin_2d(const PairwiseReductionLoopStatement& node);
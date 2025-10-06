// codegen/neon_reducers/arm64_fminp_encoders.cpp
#include "NeonReducerRegistry.h"
#include "../../NewCodeGenerator.h"
#include "../../AST.h"
#include "../../Symbol.h"
#include "../../Encoder.h"

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_4s_basic.bcl
 * @notes  Processes two 8-element 32-bit float vectors (FVEC 8).
 */
void gen_neon_fminp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FMINP 4S not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: Generated FMINP.4S for FVEC8 pairwise minimum via registry
}

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 2S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_2s_basic.bcl
 * @notes  Processes two FPAIR (2x32-bit float) values.
 */
void gen_neon_fminp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FMINP 2S not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: Generated FMINP.2S for FPAIR pairwise minimum via registry
}

/**
 * @brief  Generates ARM64 NEON FMINP instruction for a 4H vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_fminp_4h_fquad.bcl
 * @notes  Processes FQUAD (4x16-bit float) values - NEW for FQUAD support.
 *         This is a key encoder for the new FQUAD vector type implementation.
 */
void gen_neon_fminp_4h(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FMINP 4H not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: Generated FMINP.4H for FQUAD pairwise minimum via registry
}
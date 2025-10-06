// codegen/neon_reducers/arm64_faddp_encoders.cpp
#include "NeonReducerRegistry.h"
#include "../../NewCodeGenerator.h"
#include "../../AST.h"
#include "../../Symbol.h"
#include "../../Encoder.h"

/**
 * @brief  Generates ARM64 NEON FADDP instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_faddp_4s_basic.bcl
 * @notes  Processes two 8-element 32-bit float vectors (FVEC 8).
 */
void gen_neon_faddp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FADDP not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: FADDP.4S for FVEC8 pairwise addition via registry
}

/**
 * @brief  Generates ARM64 NEON FADDP instruction for a 2S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_faddp_2s_basic.bcl
 * @notes  Processes two FPAIR (2x32-bit float) values.
 */
void gen_neon_faddp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FADDP not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: FADDP.2S for FPAIR pairwise addition via registry
}

/**
 * @brief  Generates ARM64 NEON FADDP instruction for a 4H vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_faddp_4h_fquad.bcl
 * @notes  Processes FQUAD (4x16-bit float) values - NEW for FQUAD support.
 *         This is a key encoder for the new FQUAD vector type implementation.
 */
void gen_neon_faddp_4h(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - FADDP 4H not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: FADDP.4H for FQUAD pairwise addition via registry
}
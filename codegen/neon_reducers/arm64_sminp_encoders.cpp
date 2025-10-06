// codegen/neon_reducers/arm64_sminp_encoders.cpp
#include "NeonReducerRegistry.h"
#include "../../NewCodeGenerator.h"
#include "../../AST.h"
#include "../../Symbol.h"
#include "../../Encoder.h"

/**
 * @brief  Generates ARM64 NEON SMIN instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_sminp_4s_basic.bcl
 * @notes  Processes two 8-element 32-bit integer vectors (VEC 8).
 *         Uses SMIN (element-wise) since SMINP may not be available on all targets.
 */
void gen_neon_sminp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - SMIN 4S not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: Generated SMIN.4S for VEC8 pairwise minimum via registry
}

/**
 * @brief  Generates ARM64 NEON SMIN instruction for a 2S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_sminp_2s_basic.bcl
 * @notes  Processes two PAIR (2x32-bit integer) values.
 *         Uses SMIN (element-wise) for 2-element integer pairs.
 */
void gen_neon_sminp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - SMIN 2S not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: Generated SMIN.2S for PAIR pairwise minimum via registry
}
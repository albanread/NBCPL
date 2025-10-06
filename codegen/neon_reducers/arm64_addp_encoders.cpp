// codegen/neon_reducers/arm64_addp_encoders.cpp
#include "NeonReducerRegistry.h"
#include "../../NewCodeGenerator.h"
#include "../../AST.h"
#include "../../Symbol.h"
#include "../../Encoder.h"

/**
 * @brief  Generates ARM64 NEON ADDP instruction for a 4S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_addp_4s_basic.bcl
 * @notes  Processes two 8-element 32-bit integer vectors (VEC 8).
 */
void gen_neon_addp_4s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - ADDP not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: ADDP.4S for VEC8 pairwise addition via registry
}

/**
 * @brief  Generates ARM64 NEON ADDP instruction for a 2S vector layout.
 * @compliance
 * - clang_assembly_valid: false
 * - binary_encoding_valid: false
 * - test_cases: test_addp_2s_basic.bcl
 * @notes  Processes two PAIR (2x32-bit integer) values.
 */
void gen_neon_addp_2s(NewCodeGenerator& cg, const PairwiseReductionLoopStatement& node) {
    // Use the same pattern as existing generators - get variable registers directly
    std::string vector_a_reg = cg.get_variable_register(node.vector_a_name);
    std::string vector_b_reg = cg.get_variable_register(node.vector_b_name);
    std::string result_reg = cg.get_variable_register(node.result_vector_name);

    // For now, placeholder implementation - ADDP not yet implemented
    // This will be replaced with direct NEON code once register access is resolved
    // PLACEHOLDER: ADDP.2S for PAIR pairwise addition via registry
}
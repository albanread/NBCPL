#include "NewCodeGenerator.h"

void NewCodeGenerator::visit(PairsAllocationExpression& node) {
    debug_print("Visiting PairsAllocationExpression node.");
    // `PAIRS size_expr`
    // This allocates a vector (array) of PAIR words on the heap and returns its address.
    // Each PAIR is 8 bytes (2 x 32-bit integers), so we need size_expr * 2 words.
    // However, we store the PAIR count in the header, not the word count.

    // 1. Evaluate the size_expr (number of PAIRs).
    generate_expression_code(*node.size_expr);
    std::string size_pairs_reg = expression_result_reg_; // Register holding the number of PAIRs
    

    // 2. Pass element count directly to GETVEC (GETVEC handles internal sizing)
    if (size_pairs_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", size_pairs_reg));
    }
    register_manager_.release_register(size_pairs_reg);

    // 4. Call the GETVEC runtime function.
    // GETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("GETVEC"));

    // 5. GETVEC already sets the correct length, just return the result
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting PairsAllocationExpression node. Result in " + expression_result_reg_);
}
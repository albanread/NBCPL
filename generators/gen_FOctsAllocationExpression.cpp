#include "NewCodeGenerator.h"

void NewCodeGenerator::visit(FOctsAllocationExpression& node) {
    debug_print("Visiting FOctsAllocationExpression node.");
    // `FOCTS size_expr`
    // This allocates a vector (array) of FOCT words on the heap and returns its address.
    // Each FOCT is 8 bytes (8 x 8-bit floats), so we need size_expr * 2 words.
    // However, we store the FOCT count in the header, not the word count.

    // 1. Evaluate the size_expr (number of FOCTs).
    generate_expression_code(*node.size_expr);
    std::string size_focts_reg = expression_result_reg_; // Register holding the number of FOCTs
    

    // 2. Pass element count directly to FGETVEC (FGETVEC handles internal sizing)
    if (size_focts_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", size_focts_reg));
    }
    register_manager_.release_register(size_focts_reg);

    // 4. Call the FGETVEC runtime function.
    // FGETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("FGETVEC"));

    // 5. FGETVEC already sets the correct length, just return the result
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting FOctsAllocationExpression node. Result in " + expression_result_reg_);
}
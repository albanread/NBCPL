#include "NewCodeGenerator.h"

void NewCodeGenerator::visit(QuadsAllocationExpression& node) {
    debug_print("Visiting QuadsAllocationExpression node.");
    // `QUADS size_expr`
    // This allocates a vector (array) of QUAD words on the heap and returns its address.
    // Each QUAD is 8 bytes (4 x 16-bit integers), so we need size_expr * 2 words.
    // However, we store the QUAD count in the header, not the word count.

    // 1. Evaluate the size_expr (number of QUADs).
    generate_expression_code(*node.size_expr);
    std::string size_quads_reg = expression_result_reg_; // Register holding the number of QUADs
    

    // 2. Pass element count directly to GETVEC (GETVEC handles internal sizing)
    if (size_quads_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", size_quads_reg));
    }
    register_manager_.release_register(size_quads_reg);

    // 4. Call the GETVEC runtime function.
    // GETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("GETVEC"));

    // 5. GETVEC already sets the correct length, just return the result
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting QuadsAllocationExpression node. Result in " + expression_result_reg_);
}
#include "NewCodeGenerator.h"

void NewCodeGenerator::visit(FQuadsAllocationExpression& node) {
    debug_print("Visiting FQuadsAllocationExpression node.");
    // `FQUADS size_expr`
    // This allocates a vector (array) of FQUAD words on the heap and returns its address.
    // Each FQUAD is 8 bytes (4 x 16-bit floats), so we need size_expr * 2 words.
    // However, we store the FQUAD count in the header, not the word count.

    // 1. Evaluate the size_expr (number of FQUADs).
    generate_expression_code(*node.size_expr);
    std::string size_fquads_reg = expression_result_reg_; // Register holding the number of FQUADs
    

    // 2. Pass element count directly to FGETVEC (FGETVEC handles internal sizing)
    if (size_fquads_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", size_fquads_reg));
    }
    register_manager_.release_register(size_fquads_reg);

    // 4. Call the FGETVEC runtime function.
    // FGETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("FGETVEC"));

    // 5. FGETVEC already sets the correct length, just return the result
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting FQuadsAllocationExpression node. Result in " + expression_result_reg_);
}
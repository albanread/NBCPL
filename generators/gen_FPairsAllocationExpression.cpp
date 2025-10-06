#include "NewCodeGenerator.h"

void NewCodeGenerator::visit(FPairsAllocationExpression& node) {
    debug_print("Visiting FPairsAllocationExpression node.");
    // `FPAIRS size_expr`
    // This allocates a vector (array) of FPAIR words on the heap and returns its address.
    // Each FPAIR is 8 bytes (2 x 32-bit floats), so we need size_expr * 2 words.
    // However, we store the FPAIR count in the header, not the word count.

    // 1. Evaluate the size_expr (number of FPAIRs).
    generate_expression_code(*node.size_expr);
    std::string size_fpairs_reg = expression_result_reg_; // Register holding the number of FPAIRs
    
    // Keep a copy of the fpair count for the header
    std::string fpair_count_reg = register_manager_.acquire_spillable_temp_reg(*this);
    emit(Encoder::create_mov_reg(fpair_count_reg, size_fpairs_reg));

    // 2. Calculate total words needed: size_fpairs * 2 (each FPAIR = 2 words)
    std::string total_words_reg = register_manager_.acquire_spillable_temp_reg(*this);
    emit(Encoder::create_lsl_imm(total_words_reg, size_fpairs_reg, 1)); // total_words = size_fpairs << 1
    
    // 3. Move the total number of words to X0 (GETVEC expects word count in X0).
    if (total_words_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", total_words_reg));
    }
    register_manager_.release_register(total_words_reg);
    register_manager_.release_register(size_fpairs_reg);

    // 4. Call the FGETVEC runtime function for float vectors.
    // FGETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("FGETVEC"));

    // 5. Fix the length header: Replace word count with FPAIR count
    // The vector header is at [result_ptr - 8], and currently contains total_words
    // We need to replace it with fpair_count (the number of FPAIRs)
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }
    
    // Store the FPAIR count in the length header at [result_ptr - 8]
    std::string header_addr_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_sub_imm(header_addr_reg, result_reg, 8)); // header_addr = result_ptr - 8
    emit(Encoder::create_str_imm(fpair_count_reg, header_addr_reg, 0)); // Store FPAIR count in header
    register_manager_.release_register(header_addr_reg);
    register_manager_.release_register(fpair_count_reg);

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting FPairsAllocationExpression node. Result in " + expression_result_reg_);
}
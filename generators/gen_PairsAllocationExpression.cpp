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
    
    // Keep a copy of the pair count for the header
    std::string pair_count_reg = register_manager_.acquire_spillable_temp_reg(*this);
    emit(Encoder::create_mov_reg(pair_count_reg, size_pairs_reg));

    // 2. Calculate total words needed: size_pairs * 2 (each PAIR = 2 words)
    std::string total_words_reg = register_manager_.acquire_spillable_temp_reg(*this);
    emit(Encoder::create_lsl_imm(total_words_reg, size_pairs_reg, 1)); // total_words = size_pairs << 1
    
    // 3. Move the total number of words to X0 (GETVEC expects word count in X0).
    if (total_words_reg != "X0") {
        emit(Encoder::create_mov_reg("X0", total_words_reg));
    }
    register_manager_.release_register(total_words_reg);
    register_manager_.release_register(size_pairs_reg);

    // 4. Call the GETVEC runtime function.
    // GETVEC allocates memory and returns the base address in X0.
    emit(Encoder::create_branch_with_link("GETVEC"));

    // 5. Fix the length header: Replace word count with PAIR count
    // The vector header is at [result_ptr - 8], and currently contains total_words
    // We need to replace it with pair_count (the number of PAIRs)
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(*this);
    if (result_reg != "X0") {
        emit(Encoder::create_mov_reg(result_reg, "X0"));
    }
    
    // Store the PAIR count in the length header at [result_ptr - 8]
    std::string header_addr_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_sub_imm(header_addr_reg, result_reg, 8)); // header_addr = result_ptr - 8
    emit(Encoder::create_str_imm(pair_count_reg, header_addr_reg, 0)); // Store PAIR count in header
    register_manager_.release_register(header_addr_reg);
    register_manager_.release_register(pair_count_reg);

    expression_result_reg_ = result_reg;
    debug_print("Finished visiting PairsAllocationExpression node. Result in " + expression_result_reg_);
}
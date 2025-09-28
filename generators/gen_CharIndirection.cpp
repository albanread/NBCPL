#include "NewCodeGenerator.h"
#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"


void NewCodeGenerator::visit(CharIndirection& node) {
    debug_print("Visiting CharIndirection node.");
    auto& register_manager = register_manager_;
    // Char indirection like `string_expr % index_expr` or `string_expr ! index_expr` for characters.
    // This typically means `*(string_base + index_in_bytes)`.
    // Assuming `string_expr` evaluates to the base address (X register),
    // and `index_expr` evaluates to an integer index (X register).

    generate_expression_code(*node.string_expr);
    std::string string_base_reg = expression_result_reg_; // Holds the base address of the string

    generate_expression_code(*node.index_expr);
    std::string index_reg = expression_result_reg_; // Holds the index (in bytes)

    // --- BOUNDS CHECKING ---
    if (bounds_checking_enabled_) {
        debug_print("Generating bounds check for string character access.");
        
        // Load string length from offset -8 (stored just before the data)
        // Since LDR immediate doesn't support negative offsets, subtract 8 first
        std::string length_reg = register_manager.get_free_register(*this);
        std::string length_addr_reg = register_manager.get_free_register(*this);
        
        emit(Encoder::create_sub_imm(length_addr_reg, string_base_reg, 8));
        emit(Encoder::create_ldr_imm(length_reg, length_addr_reg, 0, "Load string length for bounds check"));
        register_manager.release_register(length_addr_reg);
        
        // Compare index with length (unsigned comparison)
        emit(Encoder::create_cmp_reg(index_reg, length_reg));
        
        // Branch to error handler if index >= length (unsigned higher or same)
        std::string error_label = get_bounds_error_label_for_current_function();
        emit(Encoder::create_branch_conditional("HS", error_label));
        
        register_manager.release_register(length_reg);
        
        debug_print("Bounds check generated for string access.");
    }

    // Scale index by 4 for 32-bit character access
    // FIX: Copy index to temp register to avoid destructive modification
    std::string offset_reg = register_manager.acquire_scratch_reg(*this);
    emit(Encoder::create_mov_reg(offset_reg, index_reg));
    emit(Encoder::create_lsl_imm(offset_reg, offset_reg, 2)); // offset_reg <<= 2

    // Add the offset to the base address to get the effective memory address
    std::string effective_addr_reg = register_manager.get_free_register(*this);
    emit(Encoder::create_add_reg(effective_addr_reg, string_base_reg, offset_reg));
    register_manager.release_register(string_base_reg);
    register_manager.release_register(index_reg);
    register_manager.release_register(offset_reg);

    // Load the 32-bit character value from the effective address into a W register
    std::string x_dest_reg = register_manager.get_free_register(*this); // Get X register
    std::string w_dest_reg = "W" + x_dest_reg.substr(1); // Convert "Xn" to "Wn"
    emit(Encoder::create_ldr_word_imm(w_dest_reg, effective_addr_reg, 0)); // Load 32-bit value
    register_manager.release_register(effective_addr_reg);

    // ✅ FIX: Return the 64-bit X register. Writing to the W register
    // automatically zero-extends the value into the full X register.
    expression_result_reg_ = x_dest_reg;
    debug_print("Finished visiting CharIndirection node.");
}

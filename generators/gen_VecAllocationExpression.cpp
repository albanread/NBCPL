#include "../NewCodeGenerator.h"



#include <stdexcept>

void NewCodeGenerator::visit(VecAllocationExpression& node) {
    debug_print("Visiting VecAllocationExpression node.");
    // `VEC size_expr`
    // This allocates a vector (array) of words on the heap and returns its address.
    // This typically translates to a call to the GETVEC runtime function.

    // 1. Evaluate the size_expr (number of words).
    generate_expression_code(*node.size_expr);
    std::string size_words_reg = expression_result_reg_; // Register holding the number of words

    // 2. Move the number of words to X0 (GETVEC expects word count in X0).
    auto& register_manager = register_manager_;
    emit(Encoder::create_mov_reg("X0", size_words_reg));
    register_manager.release_register(size_words_reg);

    // 3. Call the runtime `GETVEC` function using the veneer system.
    if (veneer_manager_.has_veneer("GETVEC")) {
        std::string veneer_label = veneer_manager_.get_veneer_label("GETVEC");
        Instruction bl_instr = Encoder::create_branch_with_link(veneer_label);
        bl_instr.jit_attribute = JITAttribute::JitCall;
        bl_instr.target_label = "GETVEC";
        emit(bl_instr);
    } else {
        // Fallback to RuntimeManager method
        size_t offset = RuntimeManager::instance().get_function_offset("GETVEC");
        std::string addr_reg = register_manager_.acquire_scratch_reg(*this);
        Instruction ldr_instr = Encoder::create_ldr_imm(addr_reg, "X19", offset);
        ldr_instr.jit_attribute = JITAttribute::JitAddress;
        emit(ldr_instr);
        Instruction blr_instr = Encoder::create_branch_with_link_register(addr_reg);
        blr_instr.jit_attribute = JITAttribute::JitCall;
        blr_instr.target_label = "GETVEC";
        emit(blr_instr);
        register_manager_.release_register(addr_reg);
    }

    // The result of the allocation is the address in X0.
    std::string result_reg = register_manager_.acquire_scratch_reg(*this);
    if (result_reg != "X0") {
        Instruction mov_instr = Encoder::create_mov_reg(result_reg, "X0");
        mov_instr.nopeep = true; // nopeep = true for allocation results
        emit(mov_instr);
        expression_result_reg_ = result_reg;
    } else {
        expression_result_reg_ = "X0";
        register_manager_.mark_register_as_used("X0");
    }

    debug_print("Finished visiting VecAllocationExpression node.");
}

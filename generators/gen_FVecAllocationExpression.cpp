#include "../NewCodeGenerator.h"

#include <stdexcept>

void NewCodeGenerator::visit(FVecAllocationExpression& node) {
    debug_print("Visiting FVecAllocationExpression node.");
    // `FVEC size_expr`
    // This allocates a vector (array) of float words on the heap and returns its address.
    // This typically translates to a call to the FGETVEC runtime function.

    // 1. Evaluate the size_expr (number of words).
    generate_expression_code(*node.size_expr);
    std::string size_words_reg = expression_result_reg_; // Register holding the number of words

    // 2. Move the number of words to X0 (FGETVEC expects float count in X0).
    auto& register_manager = register_manager_;
    emit(Encoder::create_mov_reg("X0", size_words_reg));
    register_manager.release_register(size_words_reg);

    // 3. Call the runtime `FGETVEC` function using the veneer system.
    if (veneer_manager_.has_veneer("FGETVEC")) {
        std::string veneer_label = veneer_manager_.get_veneer_label("FGETVEC");
        Instruction bl_instr = Encoder::create_branch_with_link(veneer_label);
        bl_instr.jit_attribute = JITAttribute::JitCall;
        bl_instr.target_label = "FGETVEC";
        emit(bl_instr);
    } else {
        // Fallback to RuntimeManager method
        size_t offset = RuntimeManager::instance().get_function_offset("FGETVEC");
        std::string addr_reg = register_manager.acquire_scratch_reg(*this);
        Instruction ldr_instr = Encoder::create_ldr_imm(addr_reg, "X19", offset);
        ldr_instr.jit_attribute = JITAttribute::JitAddress;
        emit(ldr_instr);
        Instruction blr_instr = Encoder::create_branch_with_link_register(addr_reg);
        blr_instr.jit_attribute = JITAttribute::JitCall;
        blr_instr.target_label = "FGETVEC";
        emit(blr_instr);
        register_manager.release_register(addr_reg);
    }

    // The result of the float vector allocation is the address in X0.
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

    debug_print("Finished visiting FVecAllocationExpression node.");
}

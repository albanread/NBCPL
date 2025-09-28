#include "NewCodeGenerator.h"
#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"
#include <iostream>
#include <stdexcept>

void NewCodeGenerator::visit(VectorAccess& node) {
    debug_print("Visiting VectorAccess node.");

    // First, check the type of the base expression
    VarType base_type = infer_expression_type_local(node.vector_expr.get());

    if (
        base_type == VarType::POINTER_TO_ANY_LIST ||
        base_type == VarType::POINTER_TO_INT_LIST ||
        base_type == VarType::POINTER_TO_FLOAT_LIST
    ) {
        // --- NEW LOGIC FOR LISTS ---
        debug_print("Handling VectorAccess as List Element Access.");

        // 1. Evaluate the list expression to get the header pointer in a register
        generate_expression_code(*node.vector_expr);
        std::string list_header_reg = expression_result_reg_;

        // 2. Evaluate the index expression
        generate_expression_code(*node.index_expr);
        std::string index_reg = expression_result_reg_;

        // 3. Set up arguments for the runtime call: list in X0, index in X1
        emit(Encoder::create_mov_reg("X0", list_header_reg));
        emit(Encoder::create_mov_reg("X1", index_reg));
        register_manager_.release_register(list_header_reg);
        register_manager_.release_register(index_reg);

        // 4. Call the runtime function
        emit(Encoder::create_branch_with_link("BCPL_LIST_GET_NTH"));

        // 5. The result (pointer to the sublist or NULL) is in X0.
        expression_result_reg_ = "X0";
        register_manager_.mark_register_as_used("X0");
        debug_print("Finished visiting VectorAccess node (list case).");
        return;
    }

    // --- EXISTING LOGIC FOR VECTORS ---
    generate_expression_code(*node.vector_expr);
    std::string vector_base_reg = expression_result_reg_; // Holds the base address of the vector

    generate_expression_code(*node.index_expr);
    std::string index_reg = expression_result_reg_; // Holds the index

    auto& register_manager = register_manager_;

    // --- BOUNDS CHECKING ---
    if (bounds_checking_enabled_) {
        debug_print("Generating bounds check for vector access.");
        
        // Load vector length from offset -8 (stored just before the data)
        // Since LDR immediate doesn't support negative offsets, subtract 8 first
        std::string length_reg = register_manager.get_free_register(*this);
        std::string length_addr_reg = register_manager.get_free_register(*this);
        
        emit(Encoder::create_sub_imm(length_addr_reg, vector_base_reg, 8));
        emit(Encoder::create_ldr_imm(length_reg, length_addr_reg, 0, "Load vector length for bounds check"));
        register_manager.release_register(length_addr_reg);
        
        // Compare index with length (unsigned comparison)
        emit(Encoder::create_cmp_reg(index_reg, length_reg));
        
        // Branch to error handler if index >= length (unsigned higher or same)
        std::string error_label = get_bounds_error_label_for_current_function();
        emit(Encoder::create_branch_conditional("HS", error_label));
        
        register_manager.release_register(length_reg);
        debug_print("Bounds check generated.");
    }

    // Calculate the byte offset: index * 8 (since BCPL words are 8 bytes)
    // FIX: Copy index to temp register to avoid destructive modification
    std::string offset_reg = register_manager.acquire_scratch_reg(*this);
    emit(Encoder::create_mov_reg(offset_reg, index_reg));
    emit(Encoder::create_lsl_imm(offset_reg, offset_reg, 3)); // LSL by 3 (left shift by 3 is multiply by 8)
    debug_print("Calculated byte offset for vector access.");

    // Add the offset to the base address to get the effective memory address
    // ADD Xeff_addr, vector_base_reg, offset_reg
    std::string effective_addr_reg = register_manager.get_free_register(*this);
    emit(Encoder::create_add_reg(effective_addr_reg, vector_base_reg, offset_reg));
    register_manager.release_register(vector_base_reg);
    register_manager.release_register(index_reg);
    register_manager.release_register(offset_reg);
    bool use_float_load = false;

    // 1. First, try the authoritative check using the AST Analyzer.
    VarType vec_type = infer_expression_type_local(node.vector_expr.get());
    if (vec_type == VarType::POINTER_TO_FLOAT_VEC) {
        use_float_load = true;
        debug_print("VectorAccess type determined as POINTER_TO_FLOAT_VEC by ASTAnalyzer.");
    } else {
        // 2. If the analyzer fails, fall back to checking the CallFrameManager.
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.vector_expr.get())) {
            const std::string& var_name = var_access->name;
            if (current_frame_manager_ && current_frame_manager_->is_float_variable(var_name)) {
                use_float_load = true;
                debug_print("VectorAccess type determined as 'float' by CallFrameManager for variable '" + var_name + "'.");
            }
        }
    }

    if (use_float_load) {
        // It's a float vector, so use a floating-point load into a D register.
        std::string dest_reg = register_manager_.acquire_fp_scratch_reg();
        emit(Encoder::create_ldr_fp_imm(dest_reg, effective_addr_reg, 0));
        expression_result_reg_ = dest_reg;
    } else {
        // It's an integer vector, so use the existing general-purpose load into an X register.
        std::string dest_reg = register_manager_.get_free_register(*this);
        emit(Encoder::create_ldr_imm(dest_reg, effective_addr_reg, 0));
        expression_result_reg_ = dest_reg;
    }
    
    register_manager_.release_register(effective_addr_reg);
    debug_print("Finished visiting VectorAccess node.");
}

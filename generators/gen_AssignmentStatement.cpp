#include "../NewCodeGenerator.h"
#include "../LabelManager.h"
#include "../analysis/ASTAnalyzer.h"

// In NewCodeGenerator.cpp or gen_AssignmentStatement.cpp

void NewCodeGenerator::visit(AssignmentStatement& node) {
    debug_print("Visiting AssignmentStatement node.");
    debug_print("  [CSE DEBUG] *** PROCESSING ASSIGNMENT STATEMENT ***");
    debug_print("  [CSE DEBUG] LHS count: " + std::to_string(node.lhs.size()) + ", RHS count: " + std::to_string(node.rhs.size()));
    
    // Check if this looks like a CSE-generated assignment
    if (node.lhs.size() == 1 && node.rhs.size() == 1) {
        if (auto* lhs_var = dynamic_cast<VariableAccess*>(node.lhs[0].get())) {
            if (lhs_var->name.find("_cse_temp_") == 0) {
                debug_print("  [CSE DEBUG] *** CSE-GENERATED ASSIGNMENT DETECTED: " + lhs_var->name + " ***");
                if (auto* rhs_str = dynamic_cast<StringLiteral*>(node.rhs[0].get())) {
                    debug_print("  [CSE DEBUG] *** ASSIGNING STRING LITERAL: \"" + rhs_str->value + "\" ***");
                }
            }
        }
    }

    // Check for destructuring assignment pattern (2/4 LHS, 1 RHS with PAIR/FPAIR/QUAD)
    if ((node.lhs.size() == 2 && node.rhs.size() == 1) || 
        (node.lhs.size() == 4 && node.rhs.size() == 1)) {
        VarType rhs_type = infer_expression_type_local(node.rhs[0].get());
        
        if (rhs_type == VarType::PAIR || rhs_type == VarType::FPAIR || rhs_type == VarType::QUAD) {
            std::string type_name = (rhs_type == VarType::PAIR) ? "PAIR" : 
                                   (rhs_type == VarType::FPAIR) ? "FPAIR" : "QUAD";
            debug_print("Processing destructuring assignment for " + type_name);
            
            // Evaluate the single RHS expression (PAIR/FPAIR/QUAD)
            generate_expression_code(*node.rhs[0]);
            std::string packed_reg = expression_result_reg_;
            
            if (rhs_type == VarType::QUAD) {
                // QUAD destructuring: extract four 16-bit components
                std::vector<std::string> component_regs;
                
                for (int i = 0; i < 4; i++) {
                    std::string component_reg = register_manager_.acquire_scratch_reg(*this);
                    emit(Encoder::opt_create_sbfx(component_reg, packed_reg, i * 16, 16));
                    component_regs.push_back(component_reg);
                }
                
                // Assign to LHS variables
                for (int i = 0; i < 4; i++) {
                    if (auto* var = dynamic_cast<VariableAccess*>(node.lhs[i].get())) {
                        handle_variable_assignment(var, component_regs[i]);
                    } else {
                        throw std::runtime_error("Destructuring assignment: LHS element " + std::to_string(i) + " must be a variable");
                    }
                    register_manager_.release_register(component_regs[i]);
                }
                
                register_manager_.release_register(packed_reg);
                debug_print("Finished QUAD destructuring assignment");
                return;
            } else {
                // PAIR/FPAIR destructuring: extract two 32-bit components
                std::string first_component_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::opt_create_sbfx(first_component_reg, packed_reg, 0, 32));
                
                std::string second_component_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::opt_create_sbfx(second_component_reg, packed_reg, 32, 32));
                
                // If FPAIR, convert bit patterns to floating-point registers
                if (rhs_type == VarType::FPAIR) {
                    std::string first_fp_reg = register_manager_.acquire_fp_scratch_reg();
                    std::string second_fp_reg = register_manager_.acquire_fp_scratch_reg();
                    
                    emit(Encoder::create_fmov_w_to_s(first_fp_reg, first_component_reg));
                    emit(Encoder::create_fmov_w_to_s(second_fp_reg, second_component_reg));
                    
                    register_manager_.release_register(first_component_reg);
                    register_manager_.release_register(second_component_reg);
                    
                    first_component_reg = first_fp_reg;
                    second_component_reg = second_fp_reg;
                }
                
                // Assign to LHS variables
                if (auto* var1 = dynamic_cast<VariableAccess*>(node.lhs[0].get())) {
                    handle_variable_assignment(var1, first_component_reg);
                } else {
                    throw std::runtime_error("Destructuring assignment: first LHS must be a variable");
                }
                
                if (auto* var2 = dynamic_cast<VariableAccess*>(node.lhs[1].get())) {
                    handle_variable_assignment(var2, second_component_reg);
                } else {
                    throw std::runtime_error("Destructuring assignment: second LHS must be a variable");
                }
                
                // Clean up registers
                register_manager_.release_register(packed_reg);
                register_manager_.release_register(first_component_reg);
                register_manager_.release_register(second_component_reg);
                
                debug_print("Finished PAIR/FPAIR destructuring assignment");
                return;
            }
        }
    }
    
    // Regular assignment: validate size match
    if (node.lhs.size() != node.rhs.size()) {
        throw std::runtime_error("AssignmentStatement: Mismatch in number of LHS and RHS expressions.");
    }

    // Evaluate all RHS expressions first, storing results in temporary registers.
    std::vector<std::string> rhs_result_regs;
    for (const auto& rhs_expr : node.rhs) {
        generate_expression_code(*rhs_expr);
        rhs_result_regs.push_back(expression_result_reg_);
        // Do NOT release expression_result_reg_ yet.
    }

    // Now perform the assignments from RHS result registers to LHS locations.
    for (size_t i = 0; i < node.lhs.size(); ++i) {
        const auto& lhs_expr = node.lhs[i];
        const std::string& value_to_store_reg = rhs_result_regs[i];

        if (auto* bitfield_lhs = dynamic_cast<BitfieldAccessExpression*>(lhs_expr.get())) {
            auto* start_lit = dynamic_cast<NumberLiteral*>(bitfield_lhs->start_bit_expr.get());
            auto* width_lit = dynamic_cast<NumberLiteral*>(bitfield_lhs->width_expr.get());

            // Optimized Path: Use BFI if start and width are constants.
            if (start_lit && width_lit) {
                debug_print("Handling value-based bitfield assignment (BFI optimized path).");

                // 1. Get the CURRENT VALUE of the base variable (e.g., 'm').
                generate_expression_code(*bitfield_lhs->base_expr);
                std::string dest_reg = expression_result_reg_; // This is Xd.

                // 2. The value to store is already in 'value_to_store_reg'. This is Xn.

                // 3. Emit the BFI instruction.
                emit(Encoder::opt_create_bfi(dest_reg, value_to_store_reg,
                                             start_lit->int_value, width_lit->int_value));

                // 4. Store the modified value back into the original variable.
                if (auto* base_var = dynamic_cast<VariableAccess*>(bitfield_lhs->base_expr.get())) {
                    handle_variable_assignment(base_var, dest_reg);
                } else {
                    throw std::runtime_error("Bitfield base must be a simple variable.");
                }

                // Release the registers.
                register_manager_.release_register(dest_reg);
                register_manager_.release_register(value_to_store_reg);

            } else {
                // Fallback Path: For variable start/width, use the manual read-modify-write.
                debug_print("Handling value-based bitfield assignment (Fallback path).");

                generate_expression_code(*bitfield_lhs->base_expr);
                std::string word_reg = expression_result_reg_; // Holds current value of 'm'

                generate_expression_code(*bitfield_lhs->start_bit_expr);
                std::string start_reg = expression_result_reg_;

                generate_expression_code(*bitfield_lhs->width_expr);
                std::string width_reg = expression_result_reg_;
                
                // Allocate registers for the masks
                std::string mask_reg = register_manager_.acquire_scratch_reg(*this);
                std::string one_reg = register_manager_.acquire_scratch_reg(*this);
                std::string write_mask_reg = register_manager_.acquire_scratch_reg(*this);

                // 1. Create mask for the given width: mask = (1 << width) - 1
                emit(Encoder::create_movz_imm(one_reg, 1));
                emit(Encoder::create_lsl_reg(mask_reg, one_reg, width_reg));
                emit(Encoder::create_sub_imm(mask_reg, mask_reg, 1));

                // 2. Shift the mask to the start position: write_mask = mask << start
                emit(Encoder::create_lsl_reg(write_mask_reg, mask_reg, start_reg));

                // 3. Clear the target bits in the word's value (destructive)
                emit(Encoder::create_bic_reg(word_reg, word_reg, write_mask_reg));

                // 4. Truncate and shift the new value to insert it (destructive)
                emit(Encoder::create_and_reg(value_to_store_reg, value_to_store_reg, mask_reg));
                emit(Encoder::create_lsl_reg(value_to_store_reg, value_to_store_reg, start_reg));

                // 5. Combine the cleared word with the new value (destructive)
                emit(Encoder::create_orr_reg(word_reg, word_reg, value_to_store_reg));

                // 6. Store the final result from word_reg back into the variable
                if (auto* base_var = dynamic_cast<VariableAccess*>(bitfield_lhs->base_expr.get())) {
                    handle_variable_assignment(base_var, word_reg);
                } else {
                    throw std::runtime_error("Bitfield base must be a simple variable.");
                }
                
                // 7. Release all registers
                register_manager_.release_register(word_reg);
                register_manager_.release_register(start_reg);
                register_manager_.release_register(width_reg);
                register_manager_.release_register(mask_reg);
                register_manager_.release_register(one_reg);
                register_manager_.release_register(write_mask_reg);
                register_manager_.release_register(value_to_store_reg);
            }
        } else if (auto* var_access = dynamic_cast<VariableAccess*>(lhs_expr.get())) {
            // --- LOCAL VALUE TRACKING: Register canonical address if RHS is a literal ---
            std::string canonical_form = get_expression_canonical_form(node.rhs[i].get());
            if (!canonical_form.empty()) {
                register_canonical_address(var_access->name, canonical_form);
                debug_print("Registered canonical address: " + var_access->name + " = " + canonical_form);
                debug_print("  [CSE INTEGRATION] This variable now holds a known address");
            } else {
                // If RHS is not a trackable literal, invalidate any existing tracking
                invalidate_variable_tracking(var_access->name);
            }
            
            handle_variable_assignment(var_access, value_to_store_reg);
        } else if (auto* member_access = dynamic_cast<MemberAccessExpression*>(lhs_expr.get())) {
            handle_member_access_assignment(member_access, value_to_store_reg);
        } else if (auto* vec_access = dynamic_cast<VectorAccess*>(lhs_expr.get())) {
            // --- FVEC/FTABLE support: check type and emit correct store ---
            VarType vec_type = infer_expression_type_local(vec_access->vector_expr.get());
            if (vec_type == VarType::POINTER_TO_FLOAT_VEC) {
                // Ensure value is in a float register; if not, convert
                std::string store_reg = value_to_store_reg;
                if (!register_manager_.is_fp_register(store_reg)) {
                    // Move to a float register (SCVTF if needed)
                    std::string fp_reg = register_manager_.acquire_fp_scratch_reg();
                    emit(Encoder::create_scvtf_reg(fp_reg, store_reg));
                    register_manager_.release_register(store_reg);
                    store_reg = fp_reg;
                }
                // Evaluate base and index
                generate_expression_code(*vec_access->vector_expr);
                std::string base_reg = expression_result_reg_;
                generate_expression_code(*vec_access->index_expr);
                std::string index_reg = expression_result_reg_;
                emit(Encoder::create_lsl_imm(index_reg, index_reg, 3));
                std::string effective_addr_reg = register_manager_.get_free_register(*this);
                emit(Encoder::create_add_reg(effective_addr_reg, base_reg, index_reg));
                register_manager_.release_register(base_reg);
                register_manager_.release_register(index_reg);
                emit(Encoder::create_str_fp_imm(store_reg, effective_addr_reg, 0)); // FTABLE: floating-point store
                register_manager_.release_register(effective_addr_reg);
                register_manager_.release_register(store_reg);
            } else {
                handle_vector_assignment(vec_access, value_to_store_reg);
            }
        } else if (auto* char_indirection = dynamic_cast<CharIndirection*>(lhs_expr.get())) {
            handle_char_indirection_assignment(char_indirection, value_to_store_reg);
        } else if (auto* float_vec_indirection = dynamic_cast<FloatVectorIndirection*>(lhs_expr.get())) {
            handle_float_vector_indirection_assignment(float_vec_indirection, value_to_store_reg);
        } else if (auto* unary_op = dynamic_cast<UnaryOp*>(lhs_expr.get())) {
            handle_indirection_assignment(unary_op, value_to_store_reg);
        } else {
            throw std::runtime_error("Unsupported LHS type for assignment.");
        }
    }

    debug_print("Finished visiting AssignmentStatement node.");
}

// Implementation for pointer indirection assignment (!P := ...)
void NewCodeGenerator::handle_indirection_assignment(UnaryOp* unary_op, const std::string& value_to_store_reg) {
    // Ensure this is an indirection operation ('!')
    if (unary_op->op != UnaryOp::Operator::Indirection) {
        throw std::runtime_error("Unsupported unary operator on the LHS of an assignment.");
    }

    debug_print("Handling indirection assignment (pointer dereference).");

    // 1. Evaluate the operand of the unary op (e.g., 'P' in '!P').
    //    This gives us the register holding the memory address to write to.
    generate_expression_code(*unary_op->operand);
    std::string pointer_addr_reg = expression_result_reg_;

    // 2. Emit a STRore instruction to store the value into the memory
    //    location pointed to by the address register.
    emit(Encoder::create_str_imm(value_to_store_reg, pointer_addr_reg, 0));

    // 3. Release the register that held the pointer address.
    register_manager_.release_register(pointer_addr_reg);
}

// Implementation for float vector indirection assignment
void NewCodeGenerator::handle_float_vector_indirection_assignment(FloatVectorIndirection* float_vec_indirection, const std::string& value_to_store_reg) {
    // Evaluate the vector base address
    generate_expression_code(*float_vec_indirection->vector_expr);
    std::string base_reg = expression_result_reg_;
    // Evaluate the index
    generate_expression_code(*float_vec_indirection->index_expr);
    std::string index_reg = expression_result_reg_;

    // Multiply index by 3 (since double is 8 bytes, float is 4 bytes so use 2 if float, 3 if double)
    emit(Encoder::create_lsl_imm(index_reg, index_reg, 3)); // LSL by 3 (multiply by 8 for double)

    // Add the offset to the base address to get the effective memory address
    std::string effective_addr_reg = register_manager_.get_free_register(*this);
    emit(Encoder::create_add_reg(effective_addr_reg, base_reg, index_reg));
    register_manager_.release_register(base_reg);
    register_manager_.release_register(index_reg);

    // Store the 64-bit floating-point value from value_to_store_reg into the effective address
    emit(Encoder::create_str_fp_imm(value_to_store_reg, effective_addr_reg, 0));
    register_manager_.release_register(effective_addr_reg);
}

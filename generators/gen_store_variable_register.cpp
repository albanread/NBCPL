#include "../NewCodeGenerator.h"

void NewCodeGenerator::store_variable_register(const std::string& var_name, const std::string& value_reg) {
    debug_print("store_variable_register for '" + var_name + "' from register '" + value_reg + "'");

    // --- PRIORITY 1: Check for globals FIRST ---
    Symbol symbol;
    if (lookup_symbol(var_name, symbol) && symbol.is_global()) {
         debug_print("  [GLOBAL] Storing to global variable '" + var_name + "'.");
         size_t offset = symbol.location.data_offset * 8;
         // Use appropriate store instruction based on variable type
         if (symbol.type == VarType::FLOAT) {
             emit(Encoder::create_str_fp_imm(value_reg, "X28", offset));
             debug_print("Generated STR (FP) for global " + var_name + " at offset " + std::to_string(offset));
         } else {
             emit(Encoder::create_str_imm(value_reg, "X28", offset, var_name));
             debug_print("Generated STR for global " + var_name + " at offset " + std::to_string(offset));
         }
         return;
    }

    // --- PRIORITY 2: Check for class member variables ---
    if (!current_class_name_.empty() && class_table_) {
        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
        if (class_entry) {
            auto member_it = class_entry->member_variables.find(var_name);
            if (member_it != class_entry->member_variables.end()) {
                debug_print("  ✅ CLASS MEMBER: Variable '" + var_name + "' is a class member. Generating member store via _this.");
                
                // Get the _this pointer
                std::string this_reg = get_variable_register("_this");
                
                // Calculate member offset
                size_t member_offset = member_it->second.offset;
                debug_print("  Member offset: " + std::to_string(member_offset) + " bytes");
                
                // Emit STR instruction
                if (member_it->second.type == VarType::FLOAT) {
                    emit(Encoder::create_str_fp_imm(value_reg, this_reg, member_offset));
                    debug_print("Generated STR (FP) for class member " + var_name + " at offset " + std::to_string(member_offset));
                } else {
                    emit(Encoder::create_str_imm(value_reg, this_reg, member_offset, var_name));
                    debug_print("Generated STR for class member " + var_name + " at offset " + std::to_string(member_offset));
                }
                
                return;
            }
        }
    }

    // --- PRIORITY 3: Check the Linear Scan Allocator's final decision ---
    auto func_alloc_it = all_allocations_.find(current_function_name_);
    if (func_alloc_it != all_allocations_.end()) {
        const auto& current_function_allocs = func_alloc_it->second;
        auto var_alloc_it = current_function_allocs.find(var_name);

        if (var_alloc_it != current_function_allocs.end()) {
            const LiveInterval& allocation = var_alloc_it->second;

            if (!allocation.is_spilled && !allocation.assigned_register.empty()) {
                // The variable lives in a register. Move the value there.
                const std::string& home_reg = allocation.assigned_register;
                debug_print("  [ALLOCATOR HIT] Variable '" + var_name + "' lives in " + home_reg + ". Emitting MOV.");
                if (value_reg != home_reg) {
                     if (register_manager_.is_fp_register(value_reg)) {
                        emit(Encoder::create_fmov_reg(home_reg, value_reg));
                    } else {
                        emit(Encoder::create_mov_reg(home_reg, value_reg));
                    }
                }
                // Mark the home register as dirty since it now holds a new value.
                register_manager_.mark_dirty(home_reg, true);
                // Mark the home register as initialized since it now holds a valid value.
                register_manager_.set_initialized(home_reg, true);

            } else {
                // SPILLED: The variable lives on the stack. Store the value there.
                debug_print("  [ALLOCATOR SPILLED] Variable '" + var_name + "' lives on the stack. Emitting STR.");
                int offset = current_frame_manager_->get_offset(var_name);
                if (register_manager_.is_fp_register(value_reg)) {
                    emit(Encoder::create_str_fp_imm(value_reg, "X29", offset));
                } else {
                    emit(Encoder::create_str_imm(value_reg, "X29", offset, var_name));
                }
            }
            return;
        }
    }

    throw std::runtime_error("Cannot store to variable '" + var_name + "': not found in allocation map or as global.");
}
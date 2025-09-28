#include "../NewCodeGenerator.h"
#include "../AST.h"
#include "../RuntimeManager.h"
#include "../Encoder.h"

// This is the definitive, corrected implementation.
std::string NewCodeGenerator::get_variable_register(const std::string& var_name) {
    debug_print("get_variable_register for: '" + var_name + "' in function '" + current_function_name_ + "'");
    debug_print("  [DEBUG] Current function allocations exist: " + std::string(all_allocations_.find(current_function_name_) != all_allocations_.end() ? "YES" : "NO"));

    // --- PRIORITY 1: Check for globals FIRST ---
    Symbol symbol;
    if (lookup_symbol(var_name, symbol) && symbol.is_global()) {
        size_t offset = symbol.location.data_offset * 8;
        debug_print("  ✅ SYMBOL TABLE: Variable '" + var_name + "' is GLOBAL. Forcing load from data segment.");
        debug_print("  Global offset: " + std::to_string(offset) + " bytes");
        
        // Acquire appropriate register type and emit correct LDR instruction
        std::string reg;
        if (symbol.type == VarType::FLOAT) {
            reg = register_manager_.acquire_fp_scratch_reg();
            emit(Encoder::create_ldr_fp_imm(reg, "X28", offset));
            debug_print("Generated LDR (FP) for global " + var_name + " at offset " + std::to_string(offset));
        } else {
            reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(reg, "X28", offset, var_name));
            debug_print("Generated LDR for global " + var_name + " at offset " + std::to_string(offset));
        }
        
        register_manager_.set_initialized(reg, true);
        return reg;
    }



    // --- PRIORITY 2: Check for class member variables ---
    if (!current_class_name_.empty() && class_table_) {
        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
        if (class_entry) {
            auto member_it = class_entry->member_variables.find(var_name);
            if (member_it != class_entry->member_variables.end()) {
                debug_print("  ✅ CLASS MEMBER: Variable '" + var_name + "' is a class member. Generating member access via _this.");
                
                // Get the _this pointer
                std::string this_reg = get_variable_register("_this");
                
                // Calculate member offset
                size_t member_offset = member_it->second.offset;
                debug_print("  Member offset: " + std::to_string(member_offset) + " bytes");
                
                // Acquire appropriate register and emit LDR instruction
                std::string result_reg;
                if (member_it->second.type == VarType::FLOAT) {
                    result_reg = register_manager_.acquire_fp_scratch_reg();
                    emit(Encoder::create_ldr_fp_imm(result_reg, this_reg, member_offset));
                    debug_print("Generated LDR (FP) for class member " + var_name + " at offset " + std::to_string(member_offset));
                } else {
                    result_reg = register_manager_.acquire_scratch_reg(*this);
                    emit(Encoder::create_ldr_imm(result_reg, this_reg, member_offset, var_name));
                    debug_print("Generated LDR for class member " + var_name + " at offset " + std::to_string(member_offset));
                }
                
                register_manager_.set_initialized(result_reg, true);
                return result_reg;
            }
        }
    }

    // --- START OF FIX ---
    // The generator now relies on a complete, pre-computed allocation map.
    // There is no more fallback to perform on-the-fly allocation.

    // --- PRIORITY 3: Check the pre-computed allocation plan ---
    auto func_alloc_it = all_allocations_.find(current_function_name_);
    if (func_alloc_it == all_allocations_.end()) {
        // This is a critical internal error.
        throw std::runtime_error("FATAL: No register allocation plan found for function '" + current_function_name_ + "'.");
    }

    const auto& current_function_allocs = func_alloc_it->second;
    auto var_alloc_it = current_function_allocs.find(var_name);

    if (var_alloc_it == current_function_allocs.end()) {
        // This means the variable was never used or seen by the liveness pass.
        // It could be an unused parameter or a logic error.
        // For safety, we'll try a final lookup in the frame manager for parameters that
        // might have been optimized out but are still referenced.
        if (current_frame_manager_->is_parameter(var_name)) {
            debug_print("  [WARNING] Variable '" + var_name + "' not in allocation plan, but is a parameter. Loading from initial stack slot.");
            int offset = current_frame_manager_->get_offset(var_name);
            VarType var_type = current_frame_manager_->get_variable_type(var_name);
            std::string temp_reg;
            if (var_type == VarType::FLOAT) {
                temp_reg = register_manager_.acquire_fp_scratch_reg();
                emit(Encoder::create_ldr_fp_imm(temp_reg, "X29", offset));
            } else {
                temp_reg = register_manager_.acquire_scratch_reg(*this);
                emit(Encoder::create_ldr_imm(temp_reg, "X29", offset, var_name));
            }
            return temp_reg;
        }
        
        throw std::runtime_error("FATAL: Variable '" + var_name + "' has no allocation entry in function '" + current_function_name_ + "'.");
    }

    // We have a valid allocation plan for this variable.
    const LiveInterval& allocation = var_alloc_it->second;
    std::string assigned_reg = allocation.assigned_register;

    if (allocation.is_spilled) {
        debug_print("  [ALLOCATOR SPILLED] Variable '" + var_name + "' lives on the stack.");
        
        // --- PRIORITY 3.5: Check for known address values (LOCAL VALUE TRACKING) for spilled variables ---
        if (has_known_address_value(var_name)) {
            std::string canonical_form = get_canonical_address_form(var_name);
            debug_print("  ✅ LOCAL VALUE TRACKING: Spilled variable '" + var_name + "' holds known address: " + canonical_form);
            
            if (canonical_form.find("STRING_LITERAL:") == 0) {
                debug_print("  [OPTIMIZATION] Loading string address via ADRP/ADD instead of from stack");
                
                // Extract string value from canonical form: STRING_LITERAL:"Hello World"
                size_t quote_start = canonical_form.find('"');
                size_t quote_end = canonical_form.find('"', quote_start + 1);
                if (quote_start != std::string::npos && quote_end != std::string::npos) {
                    std::string string_value = canonical_form.substr(quote_start + 1, quote_end - quote_start - 1);
                    
                    // Get the actual label from data generator
                    std::string string_label = data_generator_.add_string_literal(string_value);
                    
                    // Emit ADRP/ADD/ADD sequence to calculate address
                    std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
                    emit(Encoder::create_adrp(dest_reg, string_label));
                    emit(Encoder::create_add_literal(dest_reg, dest_reg, string_label));
                    emit(Encoder::create_add_imm(dest_reg, dest_reg, 8)); // Skip length prefix
                    
                    register_manager_.set_initialized(dest_reg, true);
                    debug_print("Generated ADRP/ADD sequence for known string address: " + string_value);
                    return dest_reg;
                }
            }
        }
        
        // Fall back to normal spilled variable loading
        debug_print("  Emitting LDR for spilled variable.");
        int offset = current_frame_manager_->get_offset(var_name);
        VarType var_type = current_frame_manager_->get_variable_type(var_name);
        std::string temp_reg;

        if (var_type == VarType::FLOAT) {
            temp_reg = register_manager_.acquire_fp_scratch_reg();
            emit(Encoder::create_ldr_fp_imm(temp_reg, "X29", offset));
        } else {
            temp_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(temp_reg, "X29", offset, var_name));
        }
        return temp_reg;

    } else {
        if (assigned_reg.empty()) {
            throw std::runtime_error("FATAL: Variable '" + var_name + "' was not spilled but has no assigned register.");
        }
        // For non-spilled variables, the assigned register is its definitive home.
        debug_print("  ✅ REGISTER ALLOCATED: Variable '" + var_name + "' lives in register " + assigned_reg);
        return assigned_reg;
    }
    // --- END OF FIX ---
}
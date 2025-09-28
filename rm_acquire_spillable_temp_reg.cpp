#include "RegisterManager.h"
#include "NewCodeGenerator.h"
#include "Encoder.h"
#include <stdexcept>

std::string RegisterManager::acquire_spillable_temp_reg(NewCodeGenerator& code_gen) {
    // Generate a new, UNIQUE name for this temporary.
    std::string temp_name = "_temp_" + std::to_string(temp_variable_counter_++);

    // 1. Try to find a free register in the variable pool.
    std::string reg = find_free_register(VARIABLE_REGS);
    if (!reg.empty()) {
        // Proactively clean any stale mappings for this register.
        cleanup_stale_mappings_for_reg(reg);
        registers[reg] = {IN_USE_VARIABLE, temp_name, false};
        variable_to_reg_map[temp_name] = reg;
        variable_reg_lru_order_.push_front(temp_name);
        return reg;
    }

    // 2. If none free, try cleanup strategies before giving up.
    if (variable_reg_lru_order_.empty()) {
        // Try to clean up stale variable mappings first
        force_cleanup_stale_variable_mappings();
        
        // Try again after cleanup
        reg = find_free_register(VARIABLE_REGS);
        if (!reg.empty()) {
            cleanup_stale_mappings_for_reg(reg);
            registers[reg] = {IN_USE_VARIABLE, temp_name, false};
            variable_to_reg_map[temp_name] = reg;
            variable_reg_lru_order_.push_front(temp_name);
            return reg;
        }
        
        // If still no luck, try expression boundary cleanup
        cleanup_expression_boundary();
        
        // Final attempt after all cleanup
        reg = find_free_register(VARIABLE_REGS);
        if (!reg.empty()) {
            cleanup_stale_mappings_for_reg(reg);
            registers[reg] = {IN_USE_VARIABLE, temp_name, false};
            variable_to_reg_map[temp_name] = reg;
            variable_reg_lru_order_.push_front(temp_name);
            return reg;
        }
        
        throw std::runtime_error("No spillable registers available for temporary.");
    }
    std::string victim_var = variable_reg_lru_order_.back();
    variable_reg_lru_order_.pop_back();
    std::string victim_reg = variable_to_reg_map.at(victim_var);

    if (registers.at(victim_reg).dirty) {
        Instruction spill_instr = generate_spill_code(victim_reg, victim_var, *code_gen.get_current_frame_manager());
        code_gen.emit(spill_instr);
    }

    variable_to_reg_map.erase(victim_var);
    spilled_variables_.insert(victim_var);

    // 3. Assign the now-free register to our new, unique temporary.
    // Proactively clean any stale mappings for the victim register.
    cleanup_stale_mappings_for_reg(victim_reg);
    registers[victim_reg] = {IN_USE_VARIABLE, temp_name, false};
    variable_to_reg_map[temp_name] = victim_reg;
    variable_reg_lru_order_.push_front(temp_name);

    return victim_reg;
}
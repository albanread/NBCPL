#include "RegisterManager.h"
#include "NewCodeGenerator.h"
#include "Encoder.h"
#include <stdexcept>
#include <iostream>

std::string RegisterManager::acquire_spillable_temp_reg(NewCodeGenerator& code_gen) {
    // Generate a new, UNIQUE name for this temporary.
    std::string temp_name = "_temp_" + std::to_string(temp_variable_counter_++);

    if (debug_enabled_) {
        std::cout << "[REGISTER_MGR] acquire_spillable_temp_reg called, temp_name=" << temp_name << std::endl;
        std::cout << "[REGISTER_MGR] temp_variable_counter_ incremented to: " << temp_variable_counter_ << std::endl;
        std::cout << "[REGISTER_MGR] Current variable_to_reg_map size: " << variable_to_reg_map.size() << std::endl;
        for (const auto& pair : variable_to_reg_map) {
            std::cout << "[REGISTER_MGR]   " << pair.first << " -> " << pair.second << std::endl;
        }
        std::cout << "[REGISTER_MGR] VARIABLE_REGS pool status:" << std::endl;
        for (const auto& reg : VARIABLE_REGS) {
            if (registers.count(reg)) {
                auto& info = registers[reg];
                std::cout << "[REGISTER_MGR]   " << reg << " status=" << info.status 
                         << " bound_to='" << info.bound_to << "'" << std::endl;
            } else {
                std::cout << "[REGISTER_MGR]   " << reg << " not in registry" << std::endl;
            }
        }
    }

    // 1. Try to find a free register in the variable pool.
    std::string reg = find_free_register(VARIABLE_REGS);
    if (debug_enabled_) {
        std::cout << "[REGISTER_MGR] find_free_register returned: " << (reg.empty() ? "NONE" : reg) << std::endl;
    }
    if (!reg.empty()) {
        // Proactively clean any stale mappings for this register.
        cleanup_stale_mappings_for_reg(reg);
        registers[reg] = {IN_USE_VARIABLE, temp_name, false};
        variable_to_reg_map[temp_name] = reg;
        variable_reg_lru_order_.push_front(temp_name);
        if (debug_enabled_) {
            std::cout << "[REGISTER_MGR] Allocated " << reg << " for " << temp_name << std::endl;
            std::cout << "[REGISTER_MGR] Updated variable_to_reg_map[" << temp_name << "] = " << reg << std::endl;
            std::cout << "[REGISTER_MGR] Added " << temp_name << " to LRU front" << std::endl;
            
            // Debug: Verify the register state was actually updated
            if (registers.count(reg)) {
                auto& info = registers[reg];
                std::cout << "[REGISTER_MGR] POST-ALLOC: " << reg << " status=" << info.status 
                         << " bound_to='" << info.bound_to << "' (expected: status=1, bound_to='" << temp_name << "')" << std::endl;
            } else {
                std::cout << "[REGISTER_MGR] ERROR: " << reg << " not found in registers map after allocation!" << std::endl;
            }
        }
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

    if (debug_enabled_) {
        std::cout << "[REGISTER_MGR] Spilled " << victim_var << " from " << victim_reg << ", allocated to " << temp_name << std::endl;
        std::cout << "[REGISTER_MGR] Updated variable_to_reg_map[" << temp_name << "] = " << victim_reg << std::endl;
        
        // Debug: Verify the register state was actually updated
        if (registers.count(victim_reg)) {
            auto& info = registers[victim_reg];
            std::cout << "[REGISTER_MGR] POST-SPILL-ALLOC: " << victim_reg << " status=" << info.status 
                     << " bound_to='" << info.bound_to << "' (expected: status=1, bound_to='" << temp_name << "')" << std::endl;
        } else {
            std::cout << "[REGISTER_MGR] ERROR: " << victim_reg << " not found in registers map after spill allocation!" << std::endl;
        }
    }
    return victim_reg;
}
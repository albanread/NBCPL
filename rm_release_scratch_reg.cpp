#include "RegisterManager.h"

void RegisterManager::release_scratch_reg(const std::string& reg_name) {
    if (registers.count(reg_name) && registers.at(reg_name).status != FREE) {
        // Get the name of the variable that was bound to this register.
        const std::string& bound_var = registers.at(reg_name).bound_to;
        // If a variable was actually bound, remove all of its tracking info.
        if (!bound_var.empty()) {
            // Remove it from the variable-to-register map.
            variable_to_reg_map.erase(bound_var);
            // Correctly remove it from the LRU list, wherever it may be.
            variable_reg_lru_order_.remove(bound_var);
        }
        // Now that all mappings are gone, mark the register as free.
        registers[reg_name] = {FREE, "", false};
    }
}
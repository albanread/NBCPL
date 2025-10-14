#include "RegisterManager.h"
#include <iostream>

void RegisterManager::release_register(const std::string& reg_name) {
    if (debug_enabled_) {
        std::cout << "[REGISTER_MGR] release_register called for: " << reg_name << std::endl;
    }
    if (!registers.count(reg_name)) {
        return;
    }
    
    if (is_fp_register(reg_name)) {
        release_fp_register(reg_name);
        return;
    }

    RegisterInfo& info = registers.at(reg_name);
    if (debug_enabled_) {
        std::cout << "[REGISTER_MGR] " << reg_name << " status=" << info.status << " bound_to=" << info.bound_to << std::endl;
    }

    if (info.status == IN_USE_VARIABLE) {
        // This was a named variable or a spillable temp (_temp_N)
        if (debug_enabled_) {
            std::cout << "[REGISTER_MGR] Releasing variable register: " << reg_name << " (" << info.bound_to << ")" << std::endl;
        }
        release_reg_for_variable(info.bound_to);
    } else if (info.status == IN_USE_SCRATCH) {
        // This was a temp from the scratch pool OR borrowed from the variable pool.
        // A simple release is correct for both cases.
        if (debug_enabled_) {
            std::cout << "[REGISTER_MGR] Releasing scratch register: " << reg_name << std::endl;
        }
        release_scratch_reg(reg_name);
    }
}
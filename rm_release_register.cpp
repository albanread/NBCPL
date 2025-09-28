#include "RegisterManager.h"

void RegisterManager::release_register(const std::string& reg_name) {
    if (!registers.count(reg_name)) {
        return;
    }
    
    if (is_fp_register(reg_name)) {
        release_fp_register(reg_name);
        return;
    }

    RegisterInfo& info = registers.at(reg_name);

    if (info.status == IN_USE_VARIABLE) {
        // This was a named variable or a spillable temp (_temp_N)
        release_reg_for_variable(info.bound_to);
    } else if (info.status == IN_USE_SCRATCH) {
        // This was a temp from the scratch pool OR borrowed from the variable pool.
        // A simple release is correct for both cases.
        release_scratch_reg(reg_name);
    }
}
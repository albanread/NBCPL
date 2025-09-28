
#include "RegisterManager.h"
#include "NewCodeGenerator.h"
#include "Encoder.h"
#include <stdexcept>

std::string RegisterManager::acquire_spillable_fp_temp_reg(NewCodeGenerator& code_gen) {
    // 1. Try to find a free FP variable register first.
    for (const auto& reg : FP_VARIABLE_REGS) {
        if (registers[reg].status == FREE) {
            // Mark as a variable, bind to a special temp name, and track it.
            registers[reg].status = IN_USE_VARIABLE;
            registers[reg].bound_to = "_temp_fp_";
            fp_variable_to_reg_map_["_temp_fp_"] = reg;
            fp_variable_reg_lru_order_.push_front("_temp_fp_");
            return reg;
        }
    }

    // 2. If no registers are free, spill the least recently used FP variable.
    if (fp_variable_reg_lru_order_.empty()) {
        throw std::runtime_error("No spillable FP registers available for temporary.");
    }
    std::string victim_var = fp_variable_reg_lru_order_.back();
    fp_variable_reg_lru_order_.pop_back();
    std::string victim_reg = fp_variable_to_reg_map_.at(victim_var);

    if (registers.at(victim_reg).dirty) {
        Instruction spill_instr = Encoder::create_str_fp_imm(victim_reg, "X29", code_gen.get_current_frame_manager()->get_spill_offset(victim_var));
        code_gen.emit(spill_instr);
    }

    fp_variable_to_reg_map_.erase(victim_var);
    // No explicit spilled_variables_ for FP, but you could add if needed

    // 3. Assign the now-free register to our new temporary and track it.
    registers[victim_reg].status = IN_USE_VARIABLE;
    registers[victim_reg].bound_to = "_temp_fp_";
    fp_variable_to_reg_map_["_temp_fp_"] = victim_reg;
    fp_variable_reg_lru_order_.push_front("_temp_fp_");

    return victim_reg;
}

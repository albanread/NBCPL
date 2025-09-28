#ifndef CODE_GEN_UTILS_H
#define CODE_GEN_UTILS_H

#include <cstdint>
#include <string>

class NewCodeGenerator;

namespace codegen_utils {

    /**
     * @brief Checks if a target address is within direct branch range.
     * 
     * ARM64 direct branch instructions (BL) use a 26-bit signed offset,
     * giving a range of approximately +/- 128MB.
     *
     * @param current_address The address of the current instruction
     * @param target_address The target address to branch to
     * @return true if the target is within range for a direct branch
     * @return false if the target requires an indirect branch
     */
    inline bool is_within_branch_range(uint64_t current_address, uint64_t target_address) {
        // 26-bit signed immediate for branch instructions
        // Maximum range is 2^25 - 1 (positive) and -2^25 (negative)
        const int64_t MAX_BRANCH_RANGE = (1LL << 25) - 1;
        const int64_t MIN_BRANCH_RANGE = -(1LL << 25);
        
        // Calculate offset (in bytes, must be aligned to 4 bytes)
        int64_t offset = static_cast<int64_t>(target_address - current_address);
        
        // Check if offset is within range
        return (offset >= MIN_BRANCH_RANGE && offset <= MAX_BRANCH_RANGE);
    }

    /**
     * @brief Push a register onto the stack (decrement SP, store register)
     */
    inline void emit_push_reg(NewCodeGenerator* gen, const std::string& reg) {
        // SUB SP, SP, #16
        gen->emit(Encoder::create_sub_imm("SP", "SP", 16));
        // STR reg, [SP, #0]
        gen->emit(Encoder::create_str_imm(reg, "SP", 0));
    }

    /**
     * @brief Pop a register from the stack (load register, increment SP)
     */
    inline void emit_pop_reg(NewCodeGenerator* gen, const std::string& reg) {
        // LDR reg, [SP, #0]
        gen->emit(Encoder::create_ldr_imm(reg, "SP", 0));
        // ADD SP, SP, #16
        gen->emit(Encoder::create_add_imm("SP", "SP", 16));
        // Register marking is done in the main method
    }

} // namespace codegen_utils

#endif // CODE_GEN_UTILS_H
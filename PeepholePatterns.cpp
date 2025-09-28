#include "PeepholeOptimizer.h"
#include "EncoderExtended.h"
#include "InstructionDecoder.h"
#include "InstructionComparator.h"

#include <regex.h>
#include <algorithm>
#include "BitPatcher.h"
#include "optimizer/PeepholePatterns.h"
/**
 * @brief Creates a pattern to recognize multiplication by powers of two and convert to left shifts.
 * Pattern: MUL Xd, Xn, #power_of_two -> LSL Xd, Xn, #log2(power_of_two)
 * @return A unique pointer to an InstructionPattern.
 */
std::unique_ptr<InstructionPattern> PeepholeOptimizer::createMultiplyByPowerOfTwoPattern() {
    return std::make_unique<InstructionPattern>(
        1,  // Pattern size: 1 instruction (MUL or MADD acting as MUL)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            const auto& instr = instrs[pos];

            // Check if the instruction is semantically a MUL
            if (InstructionDecoder::getOpcode(instr) != InstructionDecoder::OpType::MUL) {
                return {false, 0};
            }

            // Get the multiplier register (Rm)
            int rm_reg = InstructionDecoder::getRm(instr);

            // Check if Rm is a zero register (XZR/WZR) or an immediate value
            // If it's a zero register, it's not a multiplication by power of two
            if (rm_reg == 31) { // XZR/WZR
                return {false, 0};
            }

            // We need to check if the value in Rm is a power of two.
            // This is tricky without knowing the runtime value of the register.
            // For now, we'll assume that if it's a MUL instruction, and not a zero register,
            // we can't determine if it's a power of two without more advanced analysis.
            // This pattern is primarily for constant multiplication, which would be handled
            // by a MOVZ + MUL sequence or directly by the compiler.
            // For now, we'll disable this pattern until we have a way to determine if Rm holds a power of two.
            return {false, 0}; // Temporarily disable until we can properly check Rm's value
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& instr = instrs[pos];

            // Get destination, multiplicand (Rn), and multiplier (Rm) registers
            int dest_reg = InstructionDecoder::getDestReg(instr);
            int rn_reg = InstructionDecoder::getRn(instr);
            int rm_reg = InstructionDecoder::getRm(instr);

            // This part needs to be re-evaluated once the matcher can identify power-of-two multipliers.
            // For now, returning the original instruction as a placeholder.
            return { instr };
        },
        "Multiply by power of two converted to shift"
    );
}

/**
 * @brief Creates a pattern to recognize division by powers of two and convert to right shifts.
 * Pattern: SDIV Xd, Xn, #power_of_two -> ASR Xd, Xn, #log2(power_of_two)
 * @return A unique pointer to an InstructionPattern.
 */
std::unique_ptr<InstructionPattern> PeepholeOptimizer::createDivideByPowerOfTwoPattern() {
    return std::make_unique<InstructionPattern>(
        2,  // Pattern size: needs 2 instructions (MOVZ + SDIV)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            // Check if we have at least 2 instructions
            if (pos + 1 >= instrs.size()) return {false, 0};

            const auto& mov_instr = instrs[pos];
            const auto& div_instr = instrs[pos + 1];

            // Check if first instruction loads a constant (MOVZ)
            if (InstructionDecoder::getOpcode(mov_instr) != InstructionDecoder::OpType::MOV ||
                !InstructionDecoder::usesImmediate(mov_instr)) {
                return {false, 0};
            }

            // Get the immediate value from the MOV
            int64_t immediate = InstructionDecoder::getImmediate(mov_instr);

            // Check if immediate is a power of two
            if (!EncoderExtended::isPowerOfTwo(immediate)) {
                return {false, 0};
            }

            // Check if second instruction is a divide
            if (InstructionDecoder::getOpcode(div_instr) != InstructionDecoder::OpType::DIV) {
                return {false, 0};
            }

            // Get registers
            int tmp_reg = InstructionDecoder::getDestReg(mov_instr);
            int dest_reg = InstructionDecoder::getDestReg(div_instr);
            int src_reg = InstructionDecoder::getSrcReg1(div_instr);
            int div_reg = InstructionDecoder::getSrcReg2(div_instr);

            // Check if the register with the constant is used as a source in the divide
            if (tmp_reg == div_reg && tmp_reg != dest_reg && tmp_reg != src_reg) {
                return {true, 2};
            }
            return {false, 0};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& mov_instr = instrs[pos];
            const auto& div_instr = instrs[pos + 1];

            // Get the immediate value from the MOV
            int64_t immediate = InstructionDecoder::getImmediate(mov_instr);

            // Calculate shift amount (log base 2)
            int shift_amount = EncoderExtended::log2OfPowerOfTwo(immediate);

            // Get destination and source registers
            int dest_reg = InstructionDecoder::getDestReg(div_instr);
            int src_reg = InstructionDecoder::getSrcReg1(div_instr);

            // Format register names
            std::string dest_reg_name = (dest_reg < 32 ? "x" : "w") + std::to_string(dest_reg % 32);
            std::string src_reg_name = (src_reg < 32 ? "x" : "w") + std::to_string(src_reg % 32);

            // Create ASR instruction
            Instruction asr_instr = Encoder::opt_create_asr_imm(dest_reg_name, src_reg_name, shift_amount);

            return { asr_instr };
        },
        "Division by power of two converted to arithmetic shift"
    );
}

/**
 * @brief Creates a pattern to fuse compare zero and branch instructions.
 * Pattern: CMP Xn, #0 + B.EQ label -> CBZ Xn, label
 * @return A unique pointer to an InstructionPattern.
 */
std::unique_ptr<InstructionPattern> PeepholeOptimizer::createCompareZeroBranchPattern() {
    return std::make_unique<InstructionPattern>(
        2,  // Pattern size: 2 instructions (CMP + B.cond)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            // Check if we have at least 2 instructions
            if (pos + 1 >= instrs.size()) return {false, 0};

            const auto& cmp_instr = instrs[pos];
            const auto& br_instr = instrs[pos + 1];

            // Check if first instruction is a compare
            if (InstructionDecoder::getOpcode(cmp_instr) != InstructionDecoder::OpType::CMP) {
                return {false, 0};
            }

            // Check if it's a compare with zero (immediate #0 or XZR/WZR register)
            bool is_zero_compare = false;
            if (InstructionDecoder::usesImmediate(cmp_instr)) {
                is_zero_compare = (InstructionDecoder::getImmediate(cmp_instr) == 0);
            } else {
                std::string src_reg2 = InstructionDecoder::getSrcReg1AsString(cmp_instr);
                is_zero_compare = (src_reg2 == "xzr" || src_reg2 == "wzr");
            }

            if (!is_zero_compare) {
                return {false, 0};
            }

            // Check if second instruction is a conditional branch
            // (For simplicity, we'll check if the assembly text contains "b." case-insensitively)
            std::string br_text = br_instr.assembly_text;
            std::transform(br_text.begin(), br_text.end(), br_text.begin(), ::tolower);

            if (br_text.find("b.") != 0 && br_text.find(" b.") == std::string::npos) {
                return {false, 0};
            }

            // It's a compare with zero followed by a conditional branch - the pattern matches
            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& cmp_instr = instrs[pos];
            const auto& br_instr = instrs[pos + 1];

            // Get the register being compared
            std::string reg = InstructionDecoder::getDestRegAsString(cmp_instr);

            // Extract branch condition and label
            std::string br_text = br_instr.assembly_text;
            std::transform(br_text.begin(), br_text.end(), br_text.begin(), ::tolower);

            // Find the position after "b."
            size_t cond_start = br_text.find("b.") + 2;

            // Find the position of the comma or space after the condition
            size_t cond_end = br_text.find_first_of(" ,", cond_start);
            if (cond_end == std::string::npos) cond_end = br_text.length();

            // Extract the condition
            std::string condition = br_text.substr(cond_start, cond_end - cond_start);

            // Find the label (everything after the first comma or last space)
            size_t label_start = br_text.find_first_of(",", cond_end);
            if (label_start != std::string::npos) {
                label_start = br_text.find_first_not_of(" ,", label_start);
            } else {
                label_start = br_text.find_last_of(" ") + 1;
            }

            std::string label = br_text.substr(label_start);

            // Create appropriate instruction based on condition
            if (condition == "eq" || condition == "z") {
                // CMP Xn, #0 + B.EQ label -> CBZ Xn, label
                return { Encoder::opt_create_cbz(reg, label) };
            } else if (condition == "ne" || condition == "nz") {
                // CMP Xn, #0 + B.NE label -> CBNZ Xn, label
                return { Encoder::opt_create_cbnz(reg, label) };
            }

            // For other conditions, keep the original instructions
            return { cmp_instr, br_instr };
        },
        "Compare zero and branch fused"
    );
}


// Helper to rebuild assembly text without using regex.
// It finds the first register operand after the mnemonic and replaces it.
static std::string rebuild_assembly_with_new_dest(const Instruction& instr, const std::string& new_dest_reg_name) {
    std::string original_text = instr.assembly_text;
    std::string original_dest_name = InstructionDecoder::getRegisterName(instr.dest_reg);

    // Find the mnemonic (e.g., "MOVZ ")
    size_t first_space = original_text.find(" ");
    if (first_space == std::string::npos) {
        return original_text; // Should not happen
    }

    // Find the original destination register after the mnemonic
    size_t dest_pos = original_text.find(original_dest_name, first_space);
    if (dest_pos == std::string::npos) {
        return original_text; // Fallback if something is unexpected
    }

    // Replace the old register name with the new one
    original_text.replace(dest_pos, original_dest_name.length(), new_dest_reg_name);
    return original_text;
}

// Identical sequential MOV elimination pattern
std::unique_ptr<InstructionPattern> PeepholePatterns::createIdenticalMovePattern() {
    return std::make_unique<InstructionPattern>(
        2, // Window size: look at pairs of instructions

        // Matcher: Look for two identical MOV instructions in a row
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};

            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];

            // Both must be MOV instructions
            if (InstructionDecoder::getOpcode(instr1) != InstructionDecoder::OpType::MOV ||
                InstructionDecoder::getOpcode(instr2) != InstructionDecoder::OpType::MOV) {
                return {false, 0};
            }

            // Both must be register-to-register MOVs (not immediate)
            if (InstructionDecoder::usesImmediate(instr1) || InstructionDecoder::usesImmediate(instr2)) {
                return {false, 0};
            }

            // Both must have the same destination and source registers
            int dest1 = InstructionDecoder::getDestReg(instr1);
            int src1  = InstructionDecoder::getSrcReg1(instr1);
            int dest2 = InstructionDecoder::getDestReg(instr2);
            int src2  = InstructionDecoder::getSrcReg1(instr2);

            if (dest1 == dest2 && src1 == src2) {
                return {true, 2};
            }
            return {false, 0};
        },

        // Transformer: Remove the second, redundant MOV
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            // Only keep the first instruction
            return { instrs[pos] };
        },

        "Identical sequential move elimination (MOV Xd, Xn; MOV Xd, Xn)"
    );
}


std::unique_ptr<InstructionPattern> PeepholeOptimizer::createRedundantMovePattern2() {
    return std::make_unique<InstructionPattern>(
        2, // Fixed-window of 2 instructions.
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};

            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];

            // --- The Middle Path Logic ---
            // Heuristic: If instr1 is an ADD that modifies its own source register,
            // it's likely the second part of an ADRP+ADD pair. Don't touch it.
            if (instr1.opcode == InstructionDecoder::OpType::ADD &&
                InstructionComparator::areSameRegister(instr1.dest_reg, instr1.src_reg1)) {
                return {false, 0};
            }
            // --- End Middle Path Logic ---

            // Pattern: A value-producing instruction followed by a register-to-register MOV.
            if (instr2.opcode != InstructionDecoder::OpType::MOV || instr2.uses_immediate) {
                return {false, 0};
            }

            // Check if the destination of the first instruction is the source of the second.
            if (!InstructionComparator::areSameRegister(instr1.dest_reg, instr2.src_reg1)) {
                return {false, 0};
            }

            // Liveness Check: Ensure the original destination register is not used again.
            for (size_t i = pos + 2; i < instrs.size(); ++i) {
                const auto& future_instr = instrs[i];
                if (InstructionComparator::areSameRegister(future_instr.dest_reg, instr1.dest_reg)) break;
                if (InstructionComparator::areSameRegister(future_instr.src_reg1, instr1.dest_reg) ||
                    InstructionComparator::areSameRegister(future_instr.src_reg2, instr1.dest_reg)) {
                    return {false, 0};
                }
            }
            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            // --- Transformer Logic (this part is correct) ---
            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];
            Instruction optimized_instr = instr1;
            int final_dest_reg_num = instr2.dest_reg;

            BitPatcher patcher(optimized_instr.encoding);
            patcher.patch(final_dest_reg_num, 0, 5);

            optimized_instr.encoding = patcher.get_value();
            optimized_instr.dest_reg = final_dest_reg_num;
            optimized_instr.assembly_text = rebuild_assembly_with_new_dest(instr1, InstructionDecoder::getRegisterName(final_dest_reg_num));

            return { optimized_instr };
        },
        "Redundant Move Elimination"
    );
}

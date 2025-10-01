#include "../PeepholePatterns.h"
#include "../../Encoder.h"
#include "../../InstructionDecoder.h"
#include "../../InstructionComparator.h"

namespace PeepholePatterns {

std::unique_ptr<InstructionPattern> createLdrToLdpXPattern() {
    return std::make_unique<InstructionPattern>(
        2,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& ldr1 = instrs[pos];
            const auto& ldr2 = instrs[pos + 1];

            // Both must be LDR, 64-bit (X registers)
            if (InstructionDecoder::getOpcode(ldr1) != InstructionDecoder::OpType::LDR ||
                InstructionDecoder::getOpcode(ldr2) != InstructionDecoder::OpType::LDR) {
                return {false, 0};
            }
            // Same base register
            if (ldr1.base_reg != ldr2.base_reg) return {false, 0};
            // Different destination registers
            if (ldr1.dest_reg == ldr2.dest_reg) return {false, 0};
            // Offsets must be consecutive and aligned (stride 8 for 64-bit)
            if (ldr2.immediate - ldr1.immediate != 8) return {false, 0};
            // Both must be X registers (0-30, not SP)
            if (ldr1.dest_reg < 0 || ldr1.dest_reg > 30) return {false, 0};
            if (ldr2.dest_reg < 0 || ldr2.dest_reg > 30) return {false, 0};

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& ldr1 = instrs[pos];
            const auto& ldr2 = instrs[pos + 1];
            // Create LDP Xd1, Xd2, [base, #offset]
            Instruction ldp = Encoder::create_ldp_imm(
                InstructionDecoder::getRegisterName(ldr1.dest_reg),
                InstructionDecoder::getRegisterName(ldr2.dest_reg),
                InstructionDecoder::getRegisterName(ldr1.base_reg),
                ldr1.immediate
            );
            return { ldp };
        },
        "Combine adjacent X-register LDRs into LDP"
    );
}

std::unique_ptr<InstructionPattern> createMovSubMovScratchPattern() {
    return std::make_unique<InstructionPattern>(
        3,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 2 >= instrs.size()) return {false, 0};
            const auto& mov1 = instrs[pos];
            const auto& sub  = instrs[pos + 1];
            const auto& mov2 = instrs[pos + 2];

            // MOV Xs, Xt
            if (InstructionDecoder::getOpcode(mov1) != InstructionDecoder::OpType::MOV) return {false, 0};
            // SUB Xs, Xs, #imm
            if (InstructionDecoder::getOpcode(sub) != InstructionDecoder::OpType::SUB) return {false, 0};
            // MOV Xt, Xs
            if (InstructionDecoder::getOpcode(mov2) != InstructionDecoder::OpType::MOV) return {false, 0};

            // Check register usage
            if (mov1.dest_reg != sub.dest_reg) return {false, 0};
            if (mov1.dest_reg != sub.src_reg1) return {false, 0};
            if (mov2.src_reg1 != mov1.dest_reg) return {false, 0};
            if (mov2.dest_reg != mov1.src_reg1) return {false, 0};
            if (mov1.dest_reg == mov1.src_reg1) return {false, 0}; // Don't optimize if src == dest

            // Restrict to scratch registers for mov1.dest_reg (X9/X10/X11)
            if (!(mov1.dest_reg == 9 || mov1.dest_reg == 10 || mov1.dest_reg == 11)) return {false, 0};

            // Critical liveness check: ensure scratch register is not used after the 3-instruction sequence
            int scratch_reg = mov1.dest_reg;
            for (size_t i = pos + 3; i < instrs.size(); ++i) {
                const auto& future_instr = instrs[i];
                
                // If scratch register is redefined, it's safe to optimize
                if (InstructionDecoder::getDestReg(future_instr) == scratch_reg) {
                    break;
                }
                
                // If scratch register is used as source, we cannot optimize
                if (InstructionDecoder::getSrcReg1(future_instr) == scratch_reg ||
                    InstructionDecoder::getSrcReg2(future_instr) == scratch_reg) {
                    return {false, 0};
                }
                
                // Stop checking after a reasonable distance (e.g., 10 instructions)
                if (i - pos > 10) break;
            }

            return {true, 3};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& mov1 = instrs[pos];
            const auto& sub  = instrs[pos + 1];
            const auto& mov2 = instrs[pos + 2];
            // Create optimized SUB: SUB Xt, Xt, #imm
            Instruction optimized_sub = sub;
            optimized_sub.dest_reg = mov2.dest_reg; // Xt
            optimized_sub.src_reg1 = mov2.dest_reg; // Xt
            return { optimized_sub };
        },
        "Eliminate MOV-to-scratch + SUB + MOV-back pattern"
    );
}

 
 
 
// Conservative MOVZ+MOV scratch-to-target pattern for X9/X10/X11 to X19-X27
std::unique_ptr<InstructionPattern> createConservativeMovzScratchPattern() {
    return std::make_unique<InstructionPattern>(
        2, // Pattern size: 2 instructions
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& movz = instrs[pos];
            const auto& mov = instrs[pos + 1];

            // Only match MOVZ followed by MOV
            if (InstructionDecoder::getOpcode(movz) != InstructionDecoder::OpType::MOVZ ||
                InstructionDecoder::getOpcode(mov) != InstructionDecoder::OpType::MOV) {
                return {false, 0};
            }

            // Scratch registers: X9, X10, X11 (register numbers 9, 10, 11)
            if (!(movz.dest_reg == 9 || movz.dest_reg == 10 || movz.dest_reg == 11)) {
                return {false, 0};
            }

            // MOV must move from the scratch register
            if (mov.src_reg1 != movz.dest_reg) {
                return {false, 0};
            }

            // Target registers: X19-X27 (register numbers 19-27)
            if (mov.dest_reg < 19 || mov.dest_reg > 27) {
                return {false, 0};
            }

            // Don't optimize if src == dest
            if (mov.dest_reg == movz.dest_reg) {
                return {false, 0};
            }

            // Critical liveness check: ensure scratch register is not used after the 2-instruction sequence
            int scratch_reg = movz.dest_reg;
            for (size_t i = pos + 2; i < instrs.size(); ++i) {
                const auto& future_instr = instrs[i];
                
                // If scratch register is redefined, it's safe to optimize
                if (InstructionDecoder::getDestReg(future_instr) == scratch_reg) {
                    break;
                }
                
                // If scratch register is used as source, we cannot optimize
                if (InstructionDecoder::getSrcReg1(future_instr) == scratch_reg ||
                    InstructionDecoder::getSrcReg2(future_instr) == scratch_reg) {
                    return {false, 0};
                }
                
                // Stop checking after a reasonable distance (e.g., 10 instructions)
                if (i - pos > 10) break;
            }

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            auto movz = instrs[pos];
            const auto& mov = instrs[pos + 1];
            
            // Change MOVZ destination to MOV's target
            int new_dest_reg = mov.dest_reg;
            movz.dest_reg = new_dest_reg;
            
            // Update encoding to reflect new destination register (bits 0-4)
            // Use Encoder to rebuild instruction instead of text manipulation
            std::string new_dest_reg_name = InstructionDecoder::getRegisterName(new_dest_reg);
            int64_t immediate = InstructionDecoder::getImmediate(movz);
            
            // Rebuild the MOVZ instruction with the new destination register
            Instruction optimized_movz = Encoder::create_movz_imm(new_dest_reg_name, immediate);
            
            return { optimized_movz };
        },
        "Conservative MOVZ+MOV scratch-to-target pattern for X9/X10/X11 to X19-X27"
    );
}



// Eliminate load through scratch register: LDR Xs, [..]; MOV Xt, Xs => LDR Xt, [..]
std::unique_ptr<InstructionPattern> createLoadThroughScratchRegisterPattern() {
    return std::make_unique<InstructionPattern>(
        2, // Pattern size: 2 instructions
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& ldr = instrs[pos];
            const auto& mov = instrs[pos + 1];

            // Check if first is LDR and second is MOV
            if (InstructionDecoder::getOpcode(ldr) != InstructionDecoder::OpType::LDR ||
                InstructionDecoder::getOpcode(mov) != InstructionDecoder::OpType::MOV) {
                return {false, 0};
            }

            // MOV must move from LDR's dest to a different register
            if (mov.src_reg1 != ldr.dest_reg || mov.dest_reg == ldr.dest_reg) {
                return {false, 0};
            }

            // Critical liveness check: ensure scratch register is not used after the 2-instruction sequence
            int scratch_reg = ldr.dest_reg;
            for (size_t i = pos + 2; i < instrs.size(); ++i) {
                const auto& future_instr = instrs[i];
                
                // If scratch register is redefined, it's safe to optimize
                if (InstructionDecoder::getDestReg(future_instr) == scratch_reg) {
                    break;
                }
                
                // If scratch register is used as source, we cannot optimize
                if (InstructionDecoder::getSrcReg1(future_instr) == scratch_reg ||
                    InstructionDecoder::getSrcReg2(future_instr) == scratch_reg) {
                    return {false, 0};
                }
                
                // Stop checking after a reasonable distance (e.g., 10 instructions)
                if (i - pos > 10) break;
            }

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& ldr = instrs[pos];
            const auto& mov = instrs[pos + 1];
            Instruction optimized_ldr = ldr;
            optimized_ldr.dest_reg = mov.dest_reg; // Change destination to MOV's target
            return { optimized_ldr };
        },
        "Eliminate load through scratch register (LDR Xs, [..]; MOV Xt, Xs => LDR Xt, [..])"
    );
}

// Redundant Load Elimination Pattern: LDR Rd, [..]; LDR Rd, [..] => LDR Rd, [..]
std::unique_ptr<InstructionPattern> createRedundantLoadEliminationPattern() {
    return std::make_unique<InstructionPattern>(
        2, // Fixed-window of 2 instructions
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];

            // 1. Both must be LDR instructions.
            if (instr1.opcode != InstructionDecoder::OpType::LDR ||
                instr2.opcode != InstructionDecoder::OpType::LDR) {
                return {false, 0};
            }

            // 2. They must load from the *exact same memory location*.
            if (!InstructionComparator::areSameRegister(instr1.base_reg, instr2.base_reg) ||
                instr1.immediate != instr2.immediate) {
                return {false, 0};
            }

            // 3. CRITICAL FIX: The first load's destination must NOT be used as the base
            //    or index register for the second load. This prevents faulty optimization
            //    of pointer dereferences like LDR X1, [X2]; LDR X3, [X1].
            if (InstructionComparator::areSameRegister(instr1.dest_reg, instr2.base_reg) ||
                InstructionComparator::areSameRegister(instr1.dest_reg, instr2.src_reg2)) { // src_reg2 for scaled/indexed
                return {false, 0}; // This is a dependency, not a redundant load!
            }

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];

            // Keep the first load, and replace the second one with a MOV.
            Instruction new_mov = Encoder::create_mov_reg(
                InstructionDecoder::getRegisterName(instr2.dest_reg),
                InstructionDecoder::getRegisterName(instr1.dest_reg)
            );
            return {instr1, new_mov};
        },
        "Redundant load elimination (LDR Rd, [..]; LDR Rd, [..] => LDR Rd, [..])"
    );
}

std::unique_ptr<InstructionPattern> createLoadAfterStorePattern() {
    return std::make_unique<InstructionPattern>(
        2, // Pattern size: 2 instructions (STR followed by LDR)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& str_instr = instrs[pos];
            const auto& ldr_instr = instrs[pos + 1];

            // STR followed by LDR from the same address
            if (str_instr.opcode != InstructionDecoder::OpType::STR ||
                ldr_instr.opcode != InstructionDecoder::OpType::LDR) {
                return {false, 0};
            }
            // Must be same base register and offset
            if (!InstructionComparator::areSameRegister(str_instr.base_reg, ldr_instr.base_reg) ||
                str_instr.immediate != ldr_instr.immediate) {
                return {false, 0};
            }
            // STR's src_reg must match LDR's dest_reg
            if (!InstructionComparator::areSameRegister(str_instr.src_reg1, ldr_instr.dest_reg)) {
                return {false, 0};
            }
            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            // Remove the redundant LDR, keep only the STR
            return {instrs[pos]};
        },
        "Load-after-store elimination (STR+LDR to STR)"
    );
}

std::unique_ptr<InstructionPattern> createDeadStorePattern() {
    return std::make_unique<InstructionPattern>(
        2, // Pattern size: 2 instructions (STR followed by STR to same address)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& str1 = instrs[pos];
            const auto& str2 = instrs[pos + 1];

            if (str1.opcode != InstructionDecoder::OpType::STR ||
                str2.opcode != InstructionDecoder::OpType::STR) {
                return {false, 0};
            }
            // Must be same base register and offset
            if (!InstructionComparator::areSameRegister(str1.base_reg, str2.base_reg) ||
                str1.immediate != str2.immediate) {
                return {false, 0};
            }
            // The second STR overwrites the first, so the first is dead
            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            // Keep only the second STR
            return {instrs[pos + 1]};
        },
        "Dead store elimination (STR+STR to STR)"
    );
}

std::unique_ptr<InstructionPattern> createRedundantStorePattern() {
    return std::make_unique<InstructionPattern>(
        2, // Pattern size: 2 instructions (STR followed by STR to same address)
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& str1 = instrs[pos];
            const auto& str2 = instrs[pos + 1];

            if (str1.opcode != InstructionDecoder::OpType::STR ||
                str2.opcode != InstructionDecoder::OpType::STR) {
                return {false, 0};
            }
            // Must be same base register and offset
            if (!InstructionComparator::areSameRegister(str1.base_reg, str2.base_reg) ||
                str1.immediate != str2.immediate) {
                return {false, 0};
            }
            // If both STRs store the same value, the first is redundant
            if (InstructionComparator::areSameRegister(str1.src_reg1, str2.src_reg1)) {
                return {true, 2};
            }
            return {false, 0};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            // Keep only one STR (the second)
            return {instrs[pos + 1]};
        },
        "Redundant store elimination (STR+STR to STR)"
    );
}

std::unique_ptr<InstructionPattern> createStrToStpXPattern() {
    return std::make_unique<InstructionPattern>(
        2,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& str1 = instrs[pos];
            const auto& str2 = instrs[pos + 1];

            // Both must be STR, 64-bit (X registers)
            if (InstructionDecoder::getOpcode(str1) != InstructionDecoder::OpType::STR ||
                InstructionDecoder::getOpcode(str2) != InstructionDecoder::OpType::STR) {
                return {false, 0};
            }
            // Same base register
            if (str1.base_reg != str2.base_reg) return {false, 0};
            // Different source registers
            if (str1.src_reg1 == str2.src_reg1) return {false, 0};
            // Offsets must be consecutive and aligned (stride 8 for 64-bit)
            if (str2.immediate - str1.immediate != 8) return {false, 0};
            // Both must be X registers (0-30, not SP)
            if (str1.src_reg1 < 0 || str1.src_reg1 > 30) return {false, 0};
            if (str2.src_reg1 < 0 || str2.src_reg1 > 30) return {false, 0};

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& str1 = instrs[pos];
            const auto& str2 = instrs[pos + 1];
            // Create STP Xs1, Xs2, [base, #offset]
            Instruction stp = Encoder::create_stp_imm(
                InstructionDecoder::getRegisterName(str1.src_reg1),
                InstructionDecoder::getRegisterName(str2.src_reg1),
                InstructionDecoder::getRegisterName(str1.base_reg),
                str1.immediate
            );
            return { stp };
        },
        "Combine adjacent X-register STRs into STP"
    );
}



} // namespace PeepholePatterns

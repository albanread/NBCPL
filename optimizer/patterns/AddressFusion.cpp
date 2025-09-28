#include "../PeepholePatterns.h"
#include "../../Encoder.h"
#include "../../InstructionDecoder.h"
#include "../../InstructionComparator.h"
#include <memory>
#include <string>

namespace PeepholePatterns {




std::unique_ptr<InstructionPattern> createAdrFusionPattern() {
    return std::make_unique<InstructionPattern>(
        2,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 1 >= instrs.size()) return {false, 0};
            const auto& adrp_instr = instrs[pos];
            const auto& add_instr = instrs[pos + 1];

            // 1. Check for ADRP followed by ADD
            if (InstructionDecoder::getOpcode(adrp_instr) != InstructionDecoder::OpType::ADRP ||
                InstructionDecoder::getOpcode(add_instr) != InstructionDecoder::OpType::ADD) {
                return {false, 0};
            }

            // 2. Check register usage (dest_reg of ADRP is src_reg1 and dest_reg of ADD)
            if (!InstructionComparator::areSameRegister(adrp_instr.dest_reg, add_instr.src_reg1) ||
                !InstructionComparator::areSameRegister(adrp_instr.dest_reg, add_instr.dest_reg)) {
                return {false, 0};
            }

            // 3. ADD must have the correct relocation type
            if (add_instr.relocation != RelocationType::ADD_12_BIT_UNSIGNED_OFFSET) {
                return {false, 0};
            }

            // 4. ADRP must have a label
            if (adrp_instr.target_label.empty()) {
                return {false, 0};
            }

            // (Optional) If ADD has a target_label, it must match ADRP's
            if (!add_instr.target_label.empty() && adrp_instr.target_label != add_instr.target_label) {
                return {false, 0};
            }

            return {true, 2};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& adrp_instr = instrs[pos];
            std::string dest_reg = InstructionDecoder::getRegisterName(adrp_instr.dest_reg);
            const std::string& label_name = adrp_instr.target_label;
            Instruction adr_instr = Encoder::create_adr(dest_reg, label_name);
            adr_instr.address = adrp_instr.address;
            return { adr_instr };
        },
        "ADR fusion (ADRP+ADD to ADR, robust matcher)"
    );
}

std::unique_ptr<InstructionPattern> createAdrAddAddFusionPattern() {
    return std::make_unique<InstructionPattern>(
        3,
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 2 >= instrs.size()) return {false, 0};
            const auto& adrp = instrs[pos];
            const auto& add_lo12 = instrs[pos + 1];
            const auto& add_imm = instrs[pos + 2];

            // 1. ADRP, ADD (lo12), ADD (imm)
            if (InstructionDecoder::getOpcode(adrp) != InstructionDecoder::OpType::ADRP ||
                InstructionDecoder::getOpcode(add_lo12) != InstructionDecoder::OpType::ADD ||
                InstructionDecoder::getOpcode(add_imm) != InstructionDecoder::OpType::ADD) {
                return {false, 0};
            }

            // 2. All use the same dest reg, and ADDs use previous result as src1
            if (!InstructionComparator::areSameRegister(adrp.dest_reg, add_lo12.dest_reg) ||
                !InstructionComparator::areSameRegister(adrp.dest_reg, add_lo12.src_reg1) ||
                !InstructionComparator::areSameRegister(add_lo12.dest_reg, add_imm.dest_reg) ||
                !InstructionComparator::areSameRegister(add_lo12.dest_reg, add_imm.src_reg1)) {
                return {false, 0};
            }

            // 3. ADD (lo12) must have correct relocation
            if (add_lo12.relocation != RelocationType::ADD_12_BIT_UNSIGNED_OFFSET) {
                return {false, 0};
            }

            // 4. ADRP must have a label
            if (adrp.target_label.empty()) {
                return {false, 0};
            }

            // (Optional) If ADD (lo12) has a target_label, it must match ADRP's
            if (!add_lo12.target_label.empty() && adrp.target_label != add_lo12.target_label) {
                return {false, 0};
            }

            return {true, 3};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& adrp = instrs[pos];
            const auto& add_imm = instrs[pos + 2];
            std::string dest_reg = InstructionDecoder::getRegisterName(adrp.dest_reg);
            const std::string& label_name = adrp.target_label;
            Instruction adr_instr = Encoder::create_adr(dest_reg, label_name);
            adr_instr.address = adrp.address;
            // Keep the third ADD as is
            return { adr_instr, add_imm };
        },
        "ADR+ADD fusion (ADRP+ADD(lo12)+ADD(imm) → ADR+ADD(imm))"
    );
}

} // namespace PeepholePatterns

#include "../PeepholePatterns.h"
#include "../../Encoder.h"
#include "../../InstructionDecoder.h"
#include "../../InstructionComparator.h"
#include <memory>
#include <string>

namespace PeepholePatterns {

std::unique_ptr<InstructionPattern> createAddFoldingPattern() {
    return std::make_unique<InstructionPattern>(
        3, // We are matching a sequence of 3 instructions

        // --- MATCHER FUNCTION ---
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 2 >= instrs.size()) return {false, 0};

            const auto& adrp = instrs[pos];
            const auto& add_lo12 = instrs[pos + 1];
            const auto& add_imm = instrs[pos + 2];

            // DEBUG: Check nopeep flags
            if (adrp.nopeep || add_lo12.nopeep || add_imm.nopeep) {
                std::cout << "[AddFolding DEBUG] NOPEEP flag set! ADRP:" << adrp.nopeep << " ADD_lo12:" << add_lo12.nopeep << " ADD_imm:" << add_imm.nopeep << std::endl;
                return {false, 0};
            }

            // 1. Check for the ADRP, ADD, ADD sequence
            if (InstructionDecoder::getOpcode(adrp) != InstructionDecoder::OpType::ADRP ||
                InstructionDecoder::getOpcode(add_lo12) != InstructionDecoder::OpType::ADD ||
                InstructionDecoder::getOpcode(add_imm) != InstructionDecoder::OpType::ADD) {
                return {false, 0};
            }

            // 2. Check that the registers form a dependency chain
            // ADRP Xn, label -> ADD Xn, Xn, #:lo12:label -> ADD Xn, Xn, #imm
            if (!InstructionComparator::areSameRegister(adrp.dest_reg, add_lo12.src_reg1) ||
                !InstructionComparator::areSameRegister(adrp.dest_reg, add_lo12.dest_reg) ||
                !InstructionComparator::areSameRegister(add_lo12.dest_reg, add_imm.src_reg1) ||
                !InstructionComparator::areSameRegister(add_lo12.dest_reg, add_imm.dest_reg)) {
                return {false, 0};
            }

            // 3. The second instruction must be ADD with lo12 relocation
            if (add_lo12.relocation != RelocationType::ADD_12_BIT_UNSIGNED_OFFSET) {
                return {false, 0};
            }

            // 4. The third instruction must be an ADD with a small immediate
            // Make this less restrictive - allow any relocation type for the immediate ADD
            if (!InstructionDecoder::usesImmediate(add_imm)) {
                return {false, 0};
            }

            // 5. ADRP must have a target label
            if (adrp.target_label.empty()) {
                return {false, 0};
            }

            // 6. Check that the immediate value is small enough to be combined
            // ARM64 ADD with lo12 uses 12-bit immediate, so we need to ensure
            // the combined offset doesn't overflow
            int64_t immediate_offset = InstructionDecoder::getImmediate(add_imm);
            
            // DEBUG: Print debug information
            std::cout << "[AddFolding DEBUG] Found potential pattern at pos " << pos << std::endl;
            std::cout << "[AddFolding DEBUG] ADRP target: " << adrp.target_label << std::endl;
            std::cout << "[AddFolding DEBUG] ADD lo12 target: " << add_lo12.target_label << std::endl;
            std::cout << "[AddFolding DEBUG] ADD imm value: " << immediate_offset << std::endl;
            std::cout << "[AddFolding DEBUG] ADD imm relocation: " << (int)add_imm.relocation << std::endl;
            std::cout << "[AddFolding DEBUG] ADRP jit_attr: " << (int)adrp.jit_attribute << std::endl;
            std::cout << "[AddFolding DEBUG] ADD lo12 jit_attr: " << (int)add_lo12.jit_attribute << std::endl;
            
            if (immediate_offset < 0 || immediate_offset > 4095) {
                std::cout << "[AddFolding DEBUG] Immediate too large: " << immediate_offset << std::endl;
                return {false, 0}; // Can't safely combine if offset is too large
            }

            // 7. Optional: Check for AddressLoad attribute if present
            // This is more permissive - we don't require it but use it as a hint
            bool has_address_load_hint = 
                (adrp.jit_attribute == JITAttribute::AddressLoad) ||
                (add_lo12.jit_attribute == JITAttribute::AddressLoad);

            std::cout << "[AddFolding DEBUG] Pattern matched! Applying optimization." << std::endl;
            // The pattern matches!
            return {true, 3};
        },

        // --- TRANSFORMER FUNCTION ---
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& adrp = instrs[pos];
            const auto& add_lo12 = instrs[pos + 1];
            const auto& add_imm = instrs[pos + 2];

            // Get the immediate value from the third ADD
            int64_t immediate_offset = InstructionDecoder::getImmediate(add_imm);

            std::cout << "[AddFolding TRANSFORMER] Transforming pattern at pos " << pos << std::endl;
            std::cout << "[AddFolding TRANSFORMER] Converting ADRP+ADD+ADD to ADR+ADD pattern" << std::endl;
            std::cout << "[AddFolding TRANSFORMER] Immediate offset: " << immediate_offset << std::endl;

            // Use the existing ADR+ADD fusion approach from AddressFusion.cpp
            // Create an ADR instruction to replace ADRP+ADD(lo12)
            std::string dest_reg = InstructionDecoder::getRegisterName(adrp.dest_reg);
            const std::string& label_name = adrp.target_label;
            Instruction adr_instr = Encoder::create_adr(dest_reg, label_name);
            adr_instr.address = adrp.address;
            
            // Keep the immediate ADD as-is since ADR gives us the full address
            // and we still need to add the offset
            Instruction final_add = add_imm;
            final_add.address = add_imm.address;

            std::cout << "[AddFolding TRANSFORMER] Created ADR+ADD sequence" << std::endl;

            // Return the optimized two-instruction sequence
            return { adr_instr, final_add };
        },
        "Fold ADD immediate into ADRP+ADD sequence (3→2 instructions)"
    );
}

std::unique_ptr<InstructionPattern> createAdvancedAddFoldingPattern() {
    return std::make_unique<InstructionPattern>(
        3, // Match 3 instructions

        // --- MATCHER FUNCTION ---
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 2 >= instrs.size()) return {false, 0};

            const auto& adrp = instrs[pos];
            const auto& add_lo12 = instrs[pos + 1];
            const auto& add_imm = instrs[pos + 2];

            // Check nopeep attributes
            if (adrp.nopeep || add_lo12.nopeep || add_imm.nopeep) {
                return {false, 0};
            }

            // More sophisticated pattern matching
            // Look for tagged AddressLoad instructions specifically
            if (adrp.jit_attribute != JITAttribute::AddressLoad ||
                add_lo12.jit_attribute != JITAttribute::AddressLoad) {
                return {false, 0};
            }

            // Standard ADRP+ADD+ADD pattern check
            if (InstructionDecoder::getOpcode(adrp) != InstructionDecoder::OpType::ADRP ||
                InstructionDecoder::getOpcode(add_lo12) != InstructionDecoder::OpType::ADD ||
                InstructionDecoder::getOpcode(add_imm) != InstructionDecoder::OpType::ADD) {
                return {false, 0};
            }

            // Check register dependency chain
            if (!InstructionComparator::areSameRegister(adrp.dest_reg, add_lo12.src_reg1) ||
                !InstructionComparator::areSameRegister(add_lo12.dest_reg, add_imm.src_reg1) ||
                !InstructionComparator::areSameRegister(adrp.dest_reg, add_imm.dest_reg)) {
                return {false, 0};
            }

            // Relocation and immediate checks
            if (add_lo12.relocation != RelocationType::ADD_12_BIT_UNSIGNED_OFFSET ||
                !InstructionDecoder::usesImmediate(add_imm)) {
                return {false, 0};
            }

            // Label check
            if (adrp.target_label.empty()) {
                return {false, 0};
            }

            // Ensure safe immediate combining
            int64_t imm = InstructionDecoder::getImmediate(add_imm);
            if (imm < 0 || imm > 2048) { // More conservative limit for advanced pattern
                return {false, 0};
            }

            return {true, 3};
        },

        // --- TRANSFORMER FUNCTION ---
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& adrp = instrs[pos];
            const auto& add_lo12 = instrs[pos + 1];
            const auto& add_imm = instrs[pos + 2];

            // Create optimized instruction sequence
            Instruction optimized_adrp = adrp;
            Instruction optimized_add = add_lo12;

            // Combine the immediate offset into the lo12 ADD
            int64_t combined_offset = InstructionDecoder::getImmediate(add_imm);
            
            std::string reg_name = InstructionDecoder::getRegisterName(optimized_add.dest_reg);
            optimized_add.assembly_text = "ADD " + reg_name + ", " + reg_name + 
                                        ", #:lo12:" + adrp.target_label + "+" + std::to_string(combined_offset);

            // Preserve AddressLoad attribute to indicate this is still part of address loading
            optimized_add.jit_attribute = JITAttribute::AddressLoad;
            
            return { optimized_adrp, optimized_add };
        },
        "Advanced ADD folding with AddressLoad attribute tracking"
    );
}

} // namespace PeepholePatterns
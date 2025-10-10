#include "BitPatcher.h"
#include "Encoder.h"
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

/**
 * @brief Encodes the ARM64 'ADR' instruction with label + offset.
 * @details
 * This function generates the machine code for an ADR instruction that can
 * calculate the address of a label plus an immediate offset. This is useful
 * for accessing string literals with headers (e.g., skipping 8-byte length prefix).
 *
 * The ADR instruction has a range of ±1MB from the current PC, which is sufficient
 * for most code and rodata section accesses. The assembly output will be:
 * ADR xd, label_name+offset
 *
 * The immediate fields (immlo, immhi) are set to 0, and the instruction is
 * tagged with a custom relocation type. The linker calculates the final
 * PC-relative offset including the additional offset.
 *
 * The encoding follows the "PC-relative addressing" format:
 * - **op (bit 31)**: 0 for ADR.
 * - **immlo (bits 30-29)**: Low 2 bits of the 21-bit PC-relative offset.
 * - **Family (bits 28-24)**: `0b10000`.
 * - **immhi (bits 23-5)**: High 19 bits of the 21-bit PC-relative offset.
 * - **Rd (bits 4-0)**: The destination register `xd`.
 *
 * @param xd The destination register (must be a 64-bit 'X' register).
 * @param label_name The target label for the address calculation.
 * @param offset The immediate offset to add to the label address (typically 0 or 8).
 * @return An `Instruction` object with relocation info for the linker.
 * @throw std::invalid_argument if the register is invalid or offset is out of range.
 */
Instruction Encoder::create_adr_with_offset(const std::string& xd, const std::string& label_name, int64_t offset) {
    // Helper lambda to parse the destination register.
    auto parse_register = [](const std::string& reg_str) -> std::pair<uint32_t, bool> {
        if (reg_str.empty()) {
            throw std::invalid_argument("Register string cannot be empty.");
        }
        std::string lower_reg = reg_str;
        std::transform(lower_reg.begin(), lower_reg.end(), lower_reg.begin(), ::tolower);

        // ADR only operates on 64-bit general-purpose registers.
        if (lower_reg.rfind("x", 0) != 0) {
            throw std::invalid_argument("Invalid register for ADR: '" + reg_str + "'. Must be an 'X' register.");
        }

        try {
            uint32_t reg_num = std::stoul(lower_reg.substr(1));
            if (reg_num > 30) { // X31 (SP/XZR) is not a valid destination for ADR.
                throw std::out_of_range("Register X31 (SP/XZR) is not a valid destination for ADR.");
            }
            return {reg_num, true}; // is_64bit is always true
        } catch (const std::logic_error&) {
            throw std::invalid_argument("Invalid register format for ADR: '" + reg_str + "'.");
        }
    };

    // Validate offset range - ADR has ±1MB range, but we'll be conservative
    if (offset < -4096 || offset > 4096) {
        throw std::invalid_argument("Offset out of range: " + std::to_string(offset) + ". Must be within ±4096 for ADR.");
    }

    // (A) Parse and validate the register.
    auto [rd_num, rd_is_64] = parse_register(xd);

    // This check is slightly redundant given the parser's logic, but serves as a safeguard.
    if (!rd_is_64) {
        throw std::invalid_argument("ADR instruction requires a 64-bit 'X' destination register.");
    }

    // (B) Use BitPatcher. The base for ADR is 0x10000000.
    // The immediate fields are left as zero for the linker to patch.
    BitPatcher patcher(0x10000000);

    // Patch the destination register.
    patcher.patch(rd_num, 0, 5); // Rd

    // (C) Format the assembly string with offset.
    std::string assembly_text;
    if (offset == 0) {
        assembly_text = "ADR " + xd + ", " + label_name;
    } else if (offset > 0) {
        assembly_text = "ADR " + xd + ", " + label_name + "+" + std::to_string(offset);
    } else {
        assembly_text = "ADR " + xd + ", " + label_name + std::to_string(offset); // negative sign included
    }

    // (D) Return the completed Instruction object with relocation information.
    Instruction instr(patcher.get_value(), assembly_text);
    instr.opcode = InstructionDecoder::OpType::ADR;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.uses_immediate = true;
    instr.immediate = offset;
    
    // Use appropriate relocation type based on offset
    if (offset == 8) {
        instr.relocation = RelocationType::ADD_12_BIT_UNSIGNED_OFFSET_PLUS_8;
    } else if (offset == 0) {
        instr.relocation = RelocationType::PAGE_21_BIT_PC_RELATIVE; // Reuse existing ADR relocation
    } else {
        throw std::invalid_argument("Unsupported offset: " + std::to_string(offset) + ". Only 0 and 8 are supported.");
    }
    
    instr.target_label = label_name;
    instr.jit_attribute = JITAttribute::AddressLoad;
    return instr;
}
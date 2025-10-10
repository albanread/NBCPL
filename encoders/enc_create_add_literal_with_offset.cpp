#include "Encoder.h"
#include "BitPatcher.h"
#include <string>
#include <algorithm>
#include <stdexcept>

/**
 * @brief Creates an ADD instruction with label + offset relocation.
 * 
 * This function generates an ADD instruction that adds a label's low 12 bits
 * plus an immediate offset to a register. The assembly output will be:
 * ADD xd, xn, #:lo12:label_name + offset
 * 
 * This is useful for cases like string literals where we need to skip
 * a header (e.g., +8 bytes for length prefix).
 * 
 * @param xd The destination register (must be X0-X30).
 * @param xn The source register (must be X0-X30, typically same as xd).
 * @param label_name The target label for relocation.
 * @param offset The immediate offset to add (0-4095).
 * @return Instruction object with proper relocation and assembly text.
 */
Instruction Encoder::create_add_literal_with_offset(const std::string& xd, const std::string& xn, 
                                                    const std::string& label_name, int64_t offset) {
    // Helper lambda to parse register strings
    auto parse_register = [](const std::string& reg_str) -> std::pair<uint32_t, bool> {
        if (reg_str.empty()) {
            throw std::invalid_argument("Register string cannot be empty.");
        }
        std::string lower_reg = reg_str;
        std::transform(lower_reg.begin(), lower_reg.end(), lower_reg.begin(), ::tolower);

        bool is_64bit;
        uint32_t reg_num;
        
        if (lower_reg.rfind("x", 0) == 0) {
            is_64bit = true;
            try {
                reg_num = std::stoul(lower_reg.substr(1));
            } catch (const std::exception& e) {
                throw std::invalid_argument("Invalid register number in '" + reg_str + "'.");
            }
        } else {
            throw std::invalid_argument("Invalid register prefix in '" + reg_str + "'. Must be 'x' for address calculations.");
        }

        if (reg_num > 30) {
            throw std::invalid_argument("Register number out of range in '" + reg_str + "'. Must be X0-X30.");
        }

        return {reg_num, is_64bit};
    };

    // Validate offset range
    if (offset < 0 || offset > 4095) {
        throw std::invalid_argument("Offset out of range: " + std::to_string(offset) + ". Must be 0-4095.");
    }

    // Parse registers
    auto [rd_num, rd_64bit] = parse_register(xd);
    auto [rn_num, rn_64bit] = parse_register(xn);

    if (!rd_64bit || !rn_64bit) {
        throw std::invalid_argument("Both registers must be 64-bit (X registers) for address calculations.");
    }

    // Create the ADD instruction encoding
    // ADD (immediate) encoding: sf=1, op=0, S=0, sh=0, imm12=0 (will be patched by linker)
    BitPatcher patcher(0x91000000);

    // Patch the registers
    patcher.patch(rn_num, 5, 5);  // Rn (source register)
    patcher.patch(rd_num, 0, 5);  // Rd (destination register)

    // Generate assembly text - use special syntax for linker
    std::string assembly_text;
    if (offset == 8) {
        assembly_text = "ADD " + xd + ", " + xn + ", #:lo12:" + label_name + "_plus_8";
    } else if (offset == 0) {
        assembly_text = "ADD " + xd + ", " + xn + ", #:lo12:" + label_name;
    } else {
        throw std::invalid_argument("Unsupported offset: " + std::to_string(offset) + ". Only 0 and 8 are supported.");
    }

    // Create and configure the instruction
    Instruction instr(patcher.get_value(), assembly_text);
    instr.opcode = InstructionDecoder::OpType::ADD;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);
    instr.uses_immediate = true;
    instr.immediate = offset; // Store the offset for internal tracking
    instr.relocation = (offset == 8) ? RelocationType::ADD_12_BIT_UNSIGNED_OFFSET_PLUS_8 : RelocationType::ADD_12_BIT_UNSIGNED_OFFSET;
    instr.target_label = label_name;
    instr.jit_attribute = JITAttribute::AddressLoad;

    return instr;
}
#include "Linker.h"
#include "RuntimeManager.h"
#include <stdexcept>
#include <iostream>

// Default constructor for Linker
Linker::Linker() : next_veneer_address_(0) {}

/**
 * @brief Main entry point to process the instruction stream.
 *
 * This function orchestrates the two main passes of the linking process:
 * 1. Address Assignment: Calculates the final memory address for every instruction and label.
 * 2. Relocation: Patches instructions that depend on the final addresses of labels.
 * @param stream The raw instruction stream from the code generator.
 * @param manager The label manager to store and retrieve label addresses.
 * @param runtime_manager The manager for runtime function lookups.
 * @param code_base_address The starting virtual address for the code segment.
 * @param rodata_base The starting address for read-only data (not used in this model, combined with code).
 * @param data_base The starting virtual address for the read-write data segment.
 * @param enable_tracing A flag to enable verbose logging for debugging.
 * @return A vector of finalized and relocated instructions.
 */
std::vector<Instruction> Linker::process(
    const InstructionStream& stream,
    LabelManager& manager,
    const RuntimeManager& runtime_manager,
    size_t code_base_address,
    void* rodata_base,
    void* data_base,
    bool enable_tracing
) {
    // Pass 1: Assign addresses to all instructions and define labels in the manager.
    std::vector<Instruction> instructions_with_addresses =
        assignAddressesAndResolveLabels(stream, manager, code_base_address, data_base, enable_tracing);

    // Pass 2: Apply relocations to patch instructions with the now-known label addresses.
    performRelocations(instructions_with_addresses, manager, runtime_manager, enable_tracing);

    return instructions_with_addresses;
}

/**
 * @brief (PASS 1) Assigns virtual addresses to instructions and resolves label locations.
 *
 * Iterates through the raw instruction stream, calculating the address for each
 * instruction and data element. It populates the LabelManager with the final
 * address of each label it encounters. This version distinguishes between the
 * code/rodata segment and the read-write data segment.
 *
 * @return A vector of instructions with their `address` field correctly set.
 */

 std::vector<Instruction> Linker::assignAddressesAndResolveLabels(
     const InstructionStream& stream,
     LabelManager& manager,
     size_t code_base_address,
     void* data_base, // This is for .data, which is separate
     bool enable_tracing
 ) {
     if (enable_tracing) std::cerr << "[LINKER-PASS1] Starting address and label assignment...\n";

     std::vector<Instruction> finalized_instructions;
     finalized_instructions.reserve(stream.get_instructions().size());

     // --- Correct Cursor Management ---
     size_t code_cursor = code_base_address;
     size_t data_cursor = reinterpret_cast<size_t>(data_base);
     size_t rodata_cursor = 0; // Will be calculated after code size is known

     // --- Pass 1a: Calculate the total size of the code segment to find where .rodata starts ---
     size_t code_segment_size = 0;
     for (const auto& instr : stream.get_instructions()) {
         if (instr.segment == SegmentType::CODE) {
             if (!instr.is_label_definition) {
                 code_segment_size += 4;
             }
         }
     }

     // --- Calculate the start of the .rodata segment ---
     // Start rodata after the code, plus a gap, aligned to a 4KB page boundary.
     const size_t CODE_RODATA_GAP_BYTES = 16 * 1024;
     rodata_cursor = (code_base_address + code_segment_size + CODE_RODATA_GAP_BYTES + 0xFFF) & ~0xFFF;
     if (enable_tracing) {
         std::cerr << "[LINKER-PASS1] Code segment ends near 0x" << std::hex << (code_base_address + code_segment_size) << std::dec << ".\n";
         std::cerr << "[LINKER-PASS1] Read-only data (.rodata) segment will start at 0x" << std::hex << rodata_cursor << std::dec << ".\n";
     }

     // --- Pass 1b: Assign final addresses to all instructions and define all labels ---
     for (const auto& instr : stream.get_instructions()) {
         Instruction new_instr = instr;

         // Determine which cursor to use based on the segment
         size_t* current_cursor = nullptr;
         switch (instr.segment) {
             case SegmentType::CODE:   current_cursor = &code_cursor;   break;
             case SegmentType::RODATA: current_cursor = &rodata_cursor; break;
             case SegmentType::DATA:   current_cursor = &data_cursor;   break;
         }
         if (!current_cursor) continue;

         // --- START OF FIX ---

         // Step 1: Always define a label if the instruction has one.
         if (instr.is_label_definition) {
             manager.define_label(instr.target_label, *current_cursor);
         }

         // Step 2: Always advance the cursor if the instruction emits data/code.
         // A pure label definition (from Instruction::as_label) has no assembly text and is not a data value.
         bool emits_data_or_code = !instr.assembly_text.empty() || instr.is_data_value;

         if (emits_data_or_code) {
             new_instr.address = *current_cursor;
             *current_cursor += 4; // All entries in the stream are 4 bytes.
         }

         // --- END OF FIX ---

         finalized_instructions.push_back(new_instr);
     }

     return finalized_instructions;
 }



/**
 * @brief (PASS 2) Applies relocations to instructions.
 *
 * Iterates through the address-assigned instructions and patches the machine code
 * for any that have a relocation type set. It uses the addresses resolved in Pass 1.
 */
void Linker::performRelocations(
    std::vector<Instruction>& instructions,
    LabelManager& manager,
    const RuntimeManager& runtime_manager,
    bool enable_tracing
) {
    if (enable_tracing) std::cerr << "[LINKER-PASS2] Starting instruction relocation...\n";

    for (Instruction& instr : instructions) {
        if (instr.relocation == RelocationType::NONE) {
            continue;
        }

        if (instr.target_label.empty()) {
            if (enable_tracing) std::cerr << "[LINKER-WARNING] Instruction at 0x" << std::hex << instr.address << " has relocation type but no target label. Skipping.\n" << std::dec;
            continue;
        }

        size_t target_address;
        instr.resolved_symbol_name = instr.target_label;
        instr.relocation_applied = true;

        if (runtime_manager.is_function_registered(instr.target_label)) {
            target_address = reinterpret_cast<size_t>(runtime_manager.get_function(instr.target_label).address);
        } else if (manager.is_label_defined(instr.target_label)) {
            target_address = manager.get_label_address(instr.target_label);
        } else {
            throw std::runtime_error("Error: Undefined label '" + instr.target_label + "' encountered during linking.");
        }

        instr.resolved_target_address = target_address;

        switch (instr.relocation) {
            case RelocationType::PC_RELATIVE_26_BIT_OFFSET:
                // For BL instructions, check if target is in range
                if (!is_branch_in_range(instr.address, target_address, instr.relocation)) {
                    // Out of range - use or create a veneer
                    if (enable_tracing) {
                        std::cerr << "[LINKER-VENEER] BL call to '" << instr.target_label 
                                  << "' is out of range. Creating veneer.\n";
                    }
                    size_t veneer_address = get_or_create_veneer(instr.target_label, target_address, 
                                                               instr.address, instructions, manager);
                    target_address = veneer_address;
                    instr.resolved_target_address = veneer_address;
                }
                instr.encoding = apply_pc_relative_relocation(instr.encoding, instr.address, target_address, instr.relocation, enable_tracing);
                break;
            case RelocationType::PC_RELATIVE_19_BIT_OFFSET:
            case RelocationType::PAGE_21_BIT_PC_RELATIVE:
            case RelocationType::ADD_12_BIT_UNSIGNED_OFFSET:
                instr.encoding = apply_pc_relative_relocation(instr.encoding, instr.address, target_address, instr.relocation, enable_tracing);
                break;

            case RelocationType::MOVZ_MOVK_IMM_0:
            case RelocationType::MOVZ_MOVK_IMM_16:
            case RelocationType::MOVZ_MOVK_IMM_32:
            case RelocationType::MOVZ_MOVK_IMM_48:
                instr.encoding = apply_movz_movk_relocation(instr.encoding, target_address, instr.relocation);
                break;

            case RelocationType::ABSOLUTE_ADDRESS_LO32: {
                // The target_address is the full 64-bit address of the label.
                // We write the lower 32 bits into this instruction's encoding.
                instr.encoding = static_cast<uint32_t>(target_address & 0xFFFFFFFF);
                break;
            }
            case RelocationType::ABSOLUTE_ADDRESS_HI32: {
                // The target_address is the full 64-bit address of the label.
                // We write the upper 32 bits into this instruction's encoding.
                instr.encoding = static_cast<uint32_t>(target_address >> 32);
                break;
            }
            default:
                // Do nothing for NONE or other unhandled types.
                break;
        }
    }
}

// --- Smart Veneer System Implementation ---

bool Linker::is_branch_in_range(size_t instruction_address, size_t target_address, RelocationType type) {
    int64_t offset = static_cast<int64_t>(target_address) - static_cast<int64_t>(instruction_address);
    
    switch (type) {
        case RelocationType::PC_RELATIVE_26_BIT_OFFSET: {
            // BL instruction: ±128MB range (±134,217,728 bytes)
            const int64_t MAX_RANGE = 0x7FFFFFF * 4; // 26-bit signed * 4 bytes
            return (offset >= -MAX_RANGE && offset <= MAX_RANGE);
        }
        case RelocationType::PC_RELATIVE_19_BIT_OFFSET: {
            // B.cond instruction: ±1MB range (±1,048,576 bytes)
            const int64_t MAX_RANGE = 0x3FFFF * 4; // 19-bit signed * 4 bytes
            return (offset >= -MAX_RANGE && offset <= MAX_RANGE);
        }
        default:
            return true; // Other types don't have range limitations we're concerned with
    }
}

Veneer Linker::create_veneer(const std::string& target_name, size_t target_address, size_t veneer_address) {
    Veneer veneer;
    veneer.address = veneer_address;
    veneer.target_address = target_address;
    veneer.target_name = target_name;
    
    // Create veneer instructions: Load 64-bit address into X16 and branch to it
    // This uses the MOVZ/MOVK sequence to load the full 64-bit address
    
    // MOVZ X16, #(target_address & 0xFFFF)
    Instruction movz_instr;
    movz_instr.address = veneer_address;
    movz_instr.encoding = 0xD2800010 | ((target_address & 0xFFFF) << 5); // MOVZ X16, #imm
    movz_instr.assembly_text = "movz x16, #" + std::to_string(target_address & 0xFFFF);
    movz_instr.segment = SegmentType::CODE;
    veneer.instructions.push_back(movz_instr);
    
    // MOVK X16, #((target_address >> 16) & 0xFFFF), LSL #16
    Instruction movk1_instr;
    movk1_instr.address = veneer_address + 4;
    movk1_instr.encoding = 0xF2A00010 | (((target_address >> 16) & 0xFFFF) << 5); // MOVK X16, #imm, LSL #16
    movk1_instr.assembly_text = "movk x16, #" + std::to_string((target_address >> 16) & 0xFFFF) + ", lsl #16";
    movk1_instr.segment = SegmentType::CODE;
    veneer.instructions.push_back(movk1_instr);
    
    // MOVK X16, #((target_address >> 32) & 0xFFFF), LSL #32
    Instruction movk2_instr;
    movk2_instr.address = veneer_address + 8;
    movk2_instr.encoding = 0xF2C00010 | (((target_address >> 32) & 0xFFFF) << 5); // MOVK X16, #imm, LSL #32
    movk2_instr.assembly_text = "movk x16, #" + std::to_string((target_address >> 32) & 0xFFFF) + ", lsl #32";
    movk2_instr.segment = SegmentType::CODE;
    veneer.instructions.push_back(movk2_instr);
    
    // MOVK X16, #((target_address >> 48) & 0xFFFF), LSL #48
    Instruction movk3_instr;
    movk3_instr.address = veneer_address + 12;
    movk3_instr.encoding = 0xF2E00010 | (((target_address >> 48) & 0xFFFF) << 5); // MOVK X16, #imm, LSL #48
    movk3_instr.assembly_text = "movk x16, #" + std::to_string((target_address >> 48) & 0xFFFF) + ", lsl #48";
    movk3_instr.segment = SegmentType::CODE;
    veneer.instructions.push_back(movk3_instr);
    
    // BLR X16
    Instruction blr_instr;
    blr_instr.address = veneer_address + 16;
    blr_instr.encoding = 0xD63F0200; // BLR X16
    blr_instr.assembly_text = "blr x16";
    blr_instr.segment = SegmentType::CODE;
    veneer.instructions.push_back(blr_instr);
    
    return veneer;
}

size_t Linker::get_or_create_veneer(const std::string& target_name, size_t target_address, 
                                   size_t caller_address, std::vector<Instruction>& instructions,
                                   LabelManager& manager) {
    // Check if we already have a veneer for this target
    auto it = veneer_map_.find(target_name);
    if (it != veneer_map_.end()) {
        return it->second; // Return existing veneer address
    }
    
    // Calculate veneer address - place it at the end of the current code segment
    if (next_veneer_address_ == 0) {
        // Find the highest instruction address and place veneers after it
        size_t max_address = 0;
        for (const auto& instr : instructions) {
            if (instr.segment == SegmentType::CODE && instr.address > max_address) {
                max_address = instr.address;
            }
        }
        next_veneer_address_ = max_address + 4; // Start after the last instruction
        // Align to 16-byte boundary for better performance
        next_veneer_address_ = (next_veneer_address_ + 15) & ~15;
    }
    
    size_t veneer_address = next_veneer_address_;
    
    // Create the veneer
    Veneer veneer = create_veneer(target_name, target_address, veneer_address);
    
    // Add veneer instructions to the instruction stream
    for (const auto& instr : veneer.instructions) {
        instructions.push_back(instr);
    }
    
    // Update the next veneer address (each veneer is 5 instructions = 20 bytes)
    next_veneer_address_ += 20;
    
    // Store the veneer for future reference
    veneers_.push_back(veneer);
    veneer_map_[target_name] = veneer_address;
    
    // Define a label for the veneer so other tools can reference it
    std::string veneer_label = "__veneer_" + target_name;
    manager.define_label(veneer_label, veneer_address);
    
    return veneer_address;
}

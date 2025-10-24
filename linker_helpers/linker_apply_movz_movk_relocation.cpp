#include "Linker.h"
#include "BitPatcher.h"
#include <stdexcept>
#include <cstdint>
#include <iostream>
#include <iomanip>

/**
 * @brief Patches MOVZ/MOVK instructions used for loading absolute 64-bit addresses.
 *
 * @param instruction_encoding The original instruction encoding.
 * @param target_address The absolute address to be loaded.
 * @param type The relocation type (which 16 bits to patch).
 * @return The patched instruction encoding.
 */
uint32_t Linker::apply_movz_movk_relocation(
    uint32_t instruction_encoding,
    size_t target_address,
    RelocationType type,
    bool enable_tracing
) {
    if (enable_tracing) {
        std::cout << "[LINKER_DEBUG] apply_movz_movk_relocation: target_address=0x" 
                  << std::hex << target_address << std::dec << " type=";
    }
    
    BitPatcher patcher(instruction_encoding);
    uint16_t imm16 = 0;

    switch (type) {
        case RelocationType::MOVZ_MOVK_IMM_0:
            if (enable_tracing) std::cout << "MOVZ_MOVK_IMM_0";
            imm16 = (target_address >> 0) & 0xFFFF;
            break;
        case RelocationType::MOVZ_MOVK_IMM_16:
            if (enable_tracing) std::cout << "MOVZ_MOVK_IMM_16";
            imm16 = (target_address >> 16) & 0xFFFF;
            break;
        case RelocationType::MOVZ_MOVK_IMM_32:
            if (enable_tracing) std::cout << "MOVZ_MOVK_IMM_32";
            imm16 = (target_address >> 32) & 0xFFFF;
            break;
        case RelocationType::MOVZ_MOVK_IMM_48:
            if (enable_tracing) std::cout << "MOVZ_MOVK_IMM_48";
            imm16 = (target_address >> 48) & 0xFFFF;
            break;
        default:
            throw std::runtime_error("Unsupported MOVZ/MOVK relocation type in apply_movz_movk_relocation");
    }

    if (enable_tracing) {
        std::cout << " immediate=0x" << std::hex << imm16 << std::dec << std::endl;
    }
    patcher.patch(imm16, 5, 16);
    uint32_t result = patcher.get_value();
    
    // std::cout << "[LINKER_DEBUG] Original encoding: 0x" << std::hex << original_encoding 
    //           << " -> Patched encoding: 0x" << patched_encoding << std::dec << std::endl;
    
    return result;
}

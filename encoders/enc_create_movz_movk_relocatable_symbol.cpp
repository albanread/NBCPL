// Encoder for creating relocatable MOVZ/MOVK sequences that reference a symbol
// This is specifically designed for global base pointer loading where the linker
// should resolve the symbol address and apply relocations.
#include "Encoder.h"
#include <string>
#include <iostream>

std::vector<Instruction> Encoder::create_movz_movk_relocatable_symbol(const std::string& xd, const std::string& symbol) {
    std::vector<Instruction> instructions;

    // Debug output for relocatable symbol encoding
    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] Creating relocatable MOVZ/MOVK sequence for register " 
    //           << xd << " symbol " << symbol << std::endl;

    // Create MOVZ for bits [15:0] with relocation
    auto movz_instr = Encoder::create_movz_imm(xd, 0, 0, RelocationType::MOVZ_MOVK_IMM_0, symbol);
    movz_instr.jit_attribute = JITAttribute::JitAddress;
    instructions.push_back(movz_instr);
    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] MOVZ instruction created with relocation type " 
    //           << static_cast<int>(RelocationType::MOVZ_MOVK_IMM_0) << std::endl;

    // Create MOVK for bits [31:16] with relocation
    auto movk1_instr = Encoder::create_movk_imm(xd, 0, 16, RelocationType::MOVZ_MOVK_IMM_16, symbol);
    movk1_instr.jit_attribute = JITAttribute::JitAddress;
    instructions.push_back(movk1_instr);
    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] MOVK1 instruction created with relocation type " 
    //           << static_cast<int>(RelocationType::MOVZ_MOVK_IMM_16) << std::endl;

    // Create MOVK for bits [47:32] with relocation
    auto movk2_instr = Encoder::create_movk_imm(xd, 0, 32, RelocationType::MOVZ_MOVK_IMM_32, symbol);
    movk2_instr.jit_attribute = JITAttribute::JitAddress;
    instructions.push_back(movk2_instr);
    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] MOVK2 instruction created with relocation type " 
    //           << static_cast<int>(RelocationType::MOVZ_MOVK_IMM_32) << std::endl;

    // Create MOVK for bits [63:48] with relocation
    auto movk3_instr = Encoder::create_movk_imm(xd, 0, 48, RelocationType::MOVZ_MOVK_IMM_48, symbol);
    movk3_instr.jit_attribute = JITAttribute::JitAddress;
    instructions.push_back(movk3_instr);
    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] MOVK3 instruction created with relocation type " 
    //           << static_cast<int>(RelocationType::MOVZ_MOVK_IMM_48) << std::endl;

    // std::cout << "[RELOCATABLE_SYMBOL_DEBUG] Generated " << instructions.size() 
    //           << " relocatable instructions for symbol " << symbol << std::endl;

    return instructions;
}
// This encoder is NOT present in the test schedule. Test will be added via wrapper and results updated here.
#include "Encoder.h"
#include <string>
#include <iostream>

std::vector<Instruction> Encoder::create_movz_movk_jit_addr(const std::string& xd, uint64_t address, const std::string& symbol) {
    std::vector<Instruction> instructions;

    // Debug output for address encoding
    std::cout << "[MOVZ_MOVK_DEBUG] Encoding address 0x" << std::hex << address 
              << " for register " << xd << " symbol " << symbol << std::endl;

    // 1. Split the 64-bit address into four 16-bit chunks.
    uint16_t chunk0 = (address >> 0)  & 0xFFFF;
    uint16_t chunk1 = (address >> 16) & 0xFFFF;
    uint16_t chunk2 = (address >> 32) & 0xFFFF;
    uint16_t chunk3 = (address >> 48) & 0xFFFF;

    std::cout << "[MOVZ_MOVK_DEBUG] Chunks: [0x" << std::hex 
              << chunk0 << ", 0x" << chunk1 << ", 0x" << chunk2 << ", 0x" << chunk3 
              << "]" << std::dec << std::endl;



    // 2. Generate MOVZ for the lowest chunk.
    auto movz_instr = Encoder::create_movz_imm(xd, chunk0, 0, RelocationType::NONE, symbol);
    std::cout << "[MOVZ_MOVK_DEBUG] MOVZ immediate: 0x" << std::hex << movz_instr.immediate 
              << " (expected 0x" << chunk0 << ")" << std::dec << std::endl;
    instructions.push_back(movz_instr);
    instructions.back().jit_attribute = JITAttribute::JitAddress;

    // 3. Generate MOVK for the three remaining chunks, unconditionally.
    auto movk1_instr = Encoder::create_movk_imm(xd, chunk1, 16, RelocationType::NONE, symbol);
    std::cout << "[MOVZ_MOVK_DEBUG] MOVK1 immediate: 0x" << std::hex << movk1_instr.immediate 
              << " (expected 0x" << chunk1 << ")" << std::dec << std::endl;
    instructions.push_back(movk1_instr);
    instructions.back().jit_attribute = JITAttribute::JitAddress;

    auto movk2_instr = Encoder::create_movk_imm(xd, chunk2, 32, RelocationType::NONE, symbol);
    std::cout << "[MOVZ_MOVK_DEBUG] MOVK2 immediate: 0x" << std::hex << movk2_instr.immediate 
              << " (expected 0x" << chunk2 << ")" << std::dec << std::endl;
    instructions.push_back(movk2_instr);
    instructions.back().jit_attribute = JITAttribute::JitAddress;

    auto movk3_instr = Encoder::create_movk_imm(xd, chunk3, 48, RelocationType::NONE, symbol);
    std::cout << "[MOVZ_MOVK_DEBUG] MOVK3 immediate: 0x" << std::hex << movk3_instr.immediate 
              << " (expected 0x" << chunk3 << ")" << std::dec << std::endl;
    instructions.push_back(movk3_instr);
    instructions.back().jit_attribute = JITAttribute::JitAddress;

    std::cout << "[MOVZ_MOVK_DEBUG] Generated " << instructions.size() 
              << " instructions for address encoding" << std::endl;

    return instructions;
}

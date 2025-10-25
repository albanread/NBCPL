#include <iostream>
#include <iomanip>
#include <vector>
#include "encoders/Encoder.h"

void test_movz_movk_encoding() {
    std::cout << "Testing MOVZ/MOVK JIT address encoding..." << std::endl;
    
    // Test address from the debug session: 0x0000004358012928
    uint64_t test_address = 0x0000004358012928ULL;
    
    std::cout << "Input address: 0x" << std::hex << std::setfill('0') << std::setw(16) 
              << test_address << std::dec << std::endl;
    
    // Expected chunks based on manual calculation
    uint16_t expected_chunk0 = (test_address >> 0)  & 0xFFFF;  // 0x2928
    uint16_t expected_chunk1 = (test_address >> 16) & 0xFFFF;  // 0x5801
    uint16_t expected_chunk2 = (test_address >> 32) & 0xFFFF;  // 0x0043
    uint16_t expected_chunk3 = (test_address >> 48) & 0xFFFF;  // 0x0000
    
    std::cout << "Expected chunks:" << std::endl;
    std::cout << "  chunk0 (bits 0-15):   0x" << std::hex << std::setfill('0') << std::setw(4) << expected_chunk0 << std::dec << std::endl;
    std::cout << "  chunk1 (bits 16-31):  0x" << std::hex << std::setfill('0') << std::setw(4) << expected_chunk1 << std::dec << std::endl;
    std::cout << "  chunk2 (bits 32-47):  0x" << std::hex << std::setfill('0') << std::setw(4) << expected_chunk2 << std::dec << std::endl;
    std::cout << "  chunk3 (bits 48-63):  0x" << std::hex << std::setfill('0') << std::setw(4) << expected_chunk3 << std::dec << std::endl;
    
    // Call the encoder function
    std::vector<Instruction> instructions = Encoder::create_movz_movk_jit_addr("x28", test_address, "test_symbol");
    
    std::cout << "\nGenerated instructions:" << std::endl;
    for (size_t i = 0; i < instructions.size(); i++) {
        const auto& instr = instructions[i];
        std::cout << "  [" << i << "] " << instr.assembly_text << std::endl;
        std::cout << "      immediate: 0x" << std::hex << std::setfill('0') << std::setw(4) 
                  << instr.immediate << std::dec << " (" << instr.immediate << ")" << std::endl;
        std::cout << "      encoding:  0x" << std::hex << std::setfill('0') << std::setw(8) 
                  << instr.encoding << std::dec << std::endl;
    }
    
    // Verify the immediates match expected chunks
    std::cout << "\nVerification:" << std::endl;
    bool all_correct = true;
    
    if (instructions.size() != 4) {
        std::cout << "ERROR: Expected 4 instructions, got " << instructions.size() << std::endl;
        all_correct = false;
    } else {
        // MOVZ should have chunk0
        if (instructions[0].immediate != expected_chunk0) {
            std::cout << "ERROR: MOVZ immediate mismatch. Expected 0x" << std::hex << expected_chunk0 
                      << ", got 0x" << instructions[0].immediate << std::dec << std::endl;
            all_correct = false;
        } else {
            std::cout << "OK: MOVZ immediate matches chunk0" << std::endl;
        }
        
        // MOVK instructions should have chunk1, chunk2, chunk3
        uint16_t expected_chunks[] = {expected_chunk1, expected_chunk2, expected_chunk3};
        for (int i = 0; i < 3; i++) {
            if (instructions[i+1].immediate != expected_chunks[i]) {
                std::cout << "ERROR: MOVK[" << i << "] immediate mismatch. Expected 0x" << std::hex << expected_chunks[i] 
                          << ", got 0x" << instructions[i+1].immediate << std::dec << std::endl;
                all_correct = false;
            } else {
                std::cout << "OK: MOVK[" << i << "] immediate matches chunk" << (i+1) << std::endl;
            }
        }
    }
    
    if (all_correct) {
        std::cout << "\nSUCCESS: All immediates match expected chunks!" << std::endl;
    } else {
        std::cout << "\nFAILURE: One or more immediates don't match!" << std::endl;
    }
}

void test_simple_addresses() {
    std::cout << "\n\nTesting simple addresses..." << std::endl;
    
    // Test some simple cases
    uint64_t addresses[] = {
        0x0000000000001234ULL,  // Simple low address
        0x1234567890ABCDEFULL,  // All chunks non-zero
        0x0000000000000000ULL,  // Zero address
        0xFFFFFFFFFFFFFFFFULL   // All bits set
    };
    
    for (auto addr : addresses) {
        std::cout << "\nTesting address: 0x" << std::hex << std::setfill('0') << std::setw(16) 
                  << addr << std::dec << std::endl;
        
        auto instructions = Encoder::create_movz_movk_jit_addr("x28", addr, "test");
        
        // Manual chunk calculation
        uint16_t chunks[4] = {
            static_cast<uint16_t>((addr >> 0)  & 0xFFFF),
            static_cast<uint16_t>((addr >> 16) & 0xFFFF),
            static_cast<uint16_t>((addr >> 32) & 0xFFFF),
            static_cast<uint16_t>((addr >> 48) & 0xFFFF)
        };
        
        std::cout << "Expected chunks: [0x" << std::hex << std::setfill('0') << std::setw(4) << chunks[0];
        for (int i = 1; i < 4; i++) {
            std::cout << ", 0x" << std::setw(4) << chunks[i];
        }
        std::cout << "]" << std::dec << std::endl;
        
        std::cout << "Actual immediates: [0x" << std::hex << std::setfill('0') << std::setw(4) << instructions[0].immediate;
        for (size_t i = 1; i < instructions.size(); i++) {
            std::cout << ", 0x" << std::setw(4) << instructions[i].immediate;
        }
        std::cout << "]" << std::dec << std::endl;
        
        bool match = true;
        for (size_t i = 0; i < 4 && i < instructions.size(); i++) {
            if (instructions[i].immediate != chunks[i]) {
                match = false;
                break;
            }
        }
        std::cout << (match ? "PASS" : "FAIL") << std::endl;
    }
}

int main() {
    test_movz_movk_encoding();
    test_simple_addresses();
    return 0;
}
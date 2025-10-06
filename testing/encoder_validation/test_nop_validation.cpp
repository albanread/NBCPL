// testing/encoder_validation/test_nop_validation.cpp
#include "EncoderTester.h"
#include "../../Encoder.h"
#include <iostream>

/**
 * @brief Simple test to verify the encoder validation framework works correctly
 * 
 * This test validates a known-good NOP instruction against Clang to ensure
 * the validation framework itself is working properly before testing complex
 * NEON encoders.
 */

// Test function for the known-good NOP instruction
Instruction create_test_nop() {
    // ARM64 NOP instruction - well-known encoding
    Instruction instr;
    instr.encoding = 0xd503201f;  // Real ARM64 NOP encoding
    instr.assembly_text = "nop";
    return instr;
}

int main() {
    std::cout << "=== NOP Instruction Validation Test ===" << std::endl;
    std::cout << "Testing validation framework with known-good NOP instruction...\n" << std::endl;
    
    // Create encoder tester
    EncoderTester tester;
    
    // Test the NOP instruction
    Instruction nop_instr = create_test_nop();
    bool result = tester.runValidation("test_nop", nop_instr);
    
    if (result) {
        std::cout << "\n✅ SUCCESS: Validation framework is working correctly!" << std::endl;
        std::cout << "The framework correctly validated the NOP instruction against Clang." << std::endl;
        return 0;
    } else {
        std::cout << "\n❌ FAILURE: Validation framework has issues." << std::endl;
        std::cout << "Check clang installation or framework implementation." << std::endl;
        return 1;
    }
}
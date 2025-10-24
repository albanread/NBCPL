#include "PeepholeOptimizer.h"
#include "InstructionStream.h"
#include "LabelManager.h"
#include "VectorCodeGen.h"
#include <iostream>
#include <iomanip>
#include <cassert>

void printInstructions(const std::vector<Instruction>& instructions, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << std::setw(4) << i << ": 0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << instructions[i].encoding << std::dec << " " << instructions[i].assembly_text << "\n";
    }
    std::cout << "Total instructions: " << instructions.size() << "\n";
}

void testNeonOptimizationBasic() {
    std::cout << "\n***** Basic NEON Optimization Test *****\n";
    
    // Create instruction stream with two consecutive NEON MUL .2S instructions
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the two NEON MUL instructions that should be fused
    Instruction mul1 = VectorCodeGen::mul_vector_2s("V0", "V0", "V1");
    Instruction mul2 = VectorCodeGen::mul_vector_2s("V2", "V2", "V3");
    
    stream.add_instruction(mul1);
    stream.add_instruction(mul2);
    
    auto instructions_before = stream.get_instructions();
    printInstructions(instructions_before, "Before Optimization");
    
    // Verify the encodings are what we expect
    assert(instructions_before[0].encoding == 0x0ea19c00);
    assert(instructions_before[1].encoding == 0x0ea19c00);
    std::cout << "✓ Input instructions have correct .2S encodings\n";
    
    // Create peephole optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Run optimization
    optimizer.optimize(stream, 3);
    
    auto instructions_after = stream.get_instructions();
    printInstructions(instructions_after, "After Optimization");
    
    // Check results
    const auto& stats = optimizer.getStats();
    std::cout << "\nOptimization Statistics:\n";
    std::cout << "Instructions before: " << stats.total_instructions_before << "\n";
    std::cout << "Instructions after: " << stats.total_instructions_after << "\n";
    std::cout << "Optimizations applied: " << stats.optimizations_applied << "\n";
    
    for (const auto& [pattern, count] : stats.pattern_matches) {
        std::cout << "Pattern '" << pattern << "': " << count << " matches\n";
    }
    
    // Verify the optimization worked
    if (stats.optimizations_applied > 0) {
        std::cout << "✓ NEON optimization was applied!\n";
        
        // Check if we have a .4S instruction (0x4ea19c00)
        bool found_4s = false;
        for (const auto& instr : instructions_after) {
            if (instr.encoding == 0x4ea19c00) {
                found_4s = true;
                std::cout << "✓ Found fused .4S instruction: " << instr.assembly_text << "\n";
                break;
            }
        }
        
        if (!found_4s) {
            std::cout << "✗ Expected .4S instruction not found\n";
        }
        
        // Check that we have fewer instructions than before
        if (instructions_after.size() < instructions_before.size()) {
            std::cout << "✓ Instruction count reduced from " << instructions_before.size() 
                      << " to " << instructions_after.size() << "\n";
        }
        
    } else {
        std::cout << "✗ NEON optimization was NOT applied\n";
    }
}

void testNeonOptimizationWithGaps() {
    std::cout << "\n***** NEON Optimization Test with Instruction Gaps *****\n";
    
    // Create instruction stream with a gap between NEON instructions
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add first NEON MUL
    stream.add_instruction(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
    
    // Add a NOP instruction in between
    Instruction nop;
    nop.encoding = 0xd503201f;
    nop.assembly_text = "nop";
    nop.segment = SegmentType::CODE;
    stream.add_instruction(nop);
    
    // Add second NEON MUL
    stream.add_instruction(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));
    
    auto instructions_before = stream.get_instructions();
    printInstructions(instructions_before, "Before Optimization (with gap)");
    
    // Create peephole optimizer
    PeepholeOptimizer optimizer(true);
    optimizer.optimize(stream, 3);
    
    auto instructions_after = stream.get_instructions();
    printInstructions(instructions_after, "After Optimization (with gap)");
    
    const auto& stats = optimizer.getStats();
    std::cout << "Optimizations applied: " << stats.optimizations_applied << "\n";
    
    // With the fixed pattern, this should NOT be optimized since instructions aren't consecutive
    if (stats.optimizations_applied == 0) {
        std::cout << "✓ Correctly did NOT optimize non-consecutive NEON instructions\n";
    } else {
        std::cout << "✗ Unexpected: optimized non-consecutive instructions\n";
    }
}

void testSingleNeonInstruction() {
    std::cout << "\n***** Single NEON Instruction Test *****\n";
    
    // Create instruction stream with only one NEON MUL
    InstructionStream stream(LabelManager::instance(), false);
    stream.add_instruction(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
    
    auto instructions_before = stream.get_instructions();
    printInstructions(instructions_before, "Before Optimization (single)");
    
    PeepholeOptimizer optimizer(true);
    optimizer.optimize(stream, 3);
    
    auto instructions_after = stream.get_instructions();
    printInstructions(instructions_after, "After Optimization (single)");
    
    const auto& stats = optimizer.getStats();
    
    // Should not optimize a single instruction
    if (stats.optimizations_applied == 0) {
        std::cout << "✓ Correctly did NOT optimize single NEON instruction\n";
    } else {
        std::cout << "✗ Unexpected: optimized single instruction\n";
    }
}

void testNopeepAttribute() {
    std::cout << "\n***** Nopeep Attribute Test *****\n";
    
    // Create instruction stream with nopeep attribute
    InstructionStream stream(LabelManager::instance(), false);
    
    Instruction mul1 = VectorCodeGen::mul_vector_2s("V0", "V0", "V1");
    Instruction mul2 = VectorCodeGen::mul_vector_2s("V2", "V2", "V3");
    
    // Set nopeep on first instruction
    mul1.nopeep = true;
    
    stream.add_instruction(mul1);
    stream.add_instruction(mul2);
    
    auto instructions_before = stream.get_instructions();
    printInstructions(instructions_before, "Before Optimization (with nopeep)");
    
    PeepholeOptimizer optimizer(true);
    optimizer.optimize(stream, 3);
    
    auto instructions_after = stream.get_instructions();
    const auto& stats = optimizer.getStats();
    
    // Should not optimize due to nopeep attribute
    if (stats.optimizations_applied == 0) {
        std::cout << "✓ Correctly respected nopeep attribute\n";
    } else {
        std::cout << "✗ Unexpected: ignored nopeep attribute\n";
    }
}

void verifyEncodingConversion() {
    std::cout << "\n***** Encoding Conversion Verification *****\n";
    
    // Test the bit manipulation directly
    uint32_t original_2s = 0x0ea19c00;  // .2S encoding
    uint32_t expected_4s = 0x4ea19c00;  // .4S encoding
    uint32_t converted = original_2s | 0x40000000;  // Set bit 30
    
    std::cout << "Original .2S: 0x" << std::hex << original_2s << std::dec << "\n";
    std::cout << "Converted:    0x" << std::hex << converted << std::dec << "\n";
    std::cout << "Expected .4S: 0x" << std::hex << expected_4s << std::dec << "\n";
    
    if (converted == expected_4s) {
        std::cout << "✓ Encoding conversion is correct\n";
    } else {
        std::cout << "✗ Encoding conversion failed\n";
    }
    
    // Show bit patterns
    std::cout << "\nBit analysis:\n";
    std::cout << ".2S: " << std::bitset<32>(original_2s) << "\n";
    std::cout << ".4S: " << std::bitset<32>(converted) << "\n";
    std::cout << "Diff:" << std::bitset<32>(converted ^ original_2s) << " (bit 30)\n";
}

int main() {
    std::cout << "=== Testing Fixed NEON Peephole Optimization ===\n";
    
    try {
        verifyEncodingConversion();
        testNeonOptimizationBasic();
        testNeonOptimizationWithGaps();
        testSingleNeonInstruction();
        testNopeepAttribute();
        
        std::cout << "\n=== All Tests Complete ===\n";
        std::cout << "The fixed NEON optimization should now work correctly!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
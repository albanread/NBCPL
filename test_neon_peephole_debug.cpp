#include "PeepholeOptimizer.h"
#include "InstructionStream.h"
#include "LabelManager.h"
#include "VectorCodeGen.h"
#include <iostream>
#include <vector>

void printInstructions(const std::vector<Instruction>& instructions, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << std::setw(4) << i << ": 0x" << std::hex << std::setw(8) << std::setfill('0') 
                  << instructions[i].encoding << std::dec << " " << instructions[i].assembly_text << "\n";
    }
    std::cout << "Total instructions: " << instructions.size() << "\n";
}

void testNeonPatternMatching() {
    std::cout << "\n***** Testing NEON Pattern Recognition *****\n";
    
    // Create instructions that should match the NEON pattern
    std::vector<Instruction> instructions;
    
    // Add some setup instructions
    instructions.push_back(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));  // First .2S mul
    instructions.push_back(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));  // Second .2S mul
    
    printInstructions(instructions, "Original Instructions");
    
    // Test the pattern directly
    auto pattern = PeepholeOptimizer::createNeon128BitFusionPattern();
    
    std::cout << "\n***** Testing Pattern Matcher *****\n";
    MatchResult result = pattern->matches(instructions, 0);
    std::cout << "Pattern matched: " << (result.matched ? "YES" : "NO") << "\n";
    std::cout << "Match length: " << result.length << "\n";
    
    if (result.matched) {
        std::cout << "\n***** Testing Pattern Transformer *****\n";
        std::vector<Instruction> transformed = pattern->transform(instructions, 0);
        printInstructions(transformed, "Transformed Instructions");
        
        std::cout << "\nTransformation summary:\n";
        std::cout << "Original count: " << instructions.size() << "\n";
        std::cout << "Transformed count: " << transformed.size() << "\n";
        std::cout << "Match length reported: " << result.length << "\n";
        
        if (transformed.size() != result.length) {
            std::cout << "*** ERROR: Transformer returned " << transformed.size() 
                      << " instructions but matcher reported length " << result.length << " ***\n";
        }
    }
}

void testWithPeepholeOptimizer() {
    std::cout << "\n***** Testing with Full Peephole Optimizer *****\n";
    
    // Create instruction stream
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add NEON instructions that should be optimized
    stream.add_instruction(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
    stream.add_instruction(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));
    
    auto instructions_before = stream.get_instructions();
    printInstructions(instructions_before, "Before Optimization");
    
    // Create peephole optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Run optimization
    optimizer.optimize(stream, 3);
    
    auto instructions_after = stream.get_instructions();
    printInstructions(instructions_after, "After Optimization");
    
    // Print stats
    const auto& stats = optimizer.getStats();
    std::cout << "\nOptimization Statistics:\n";
    std::cout << "Instructions before: " << stats.total_instructions_before << "\n";
    std::cout << "Instructions after: " << stats.total_instructions_after << "\n";
    std::cout << "Optimizations applied: " << stats.optimizations_applied << "\n";
    
    for (const auto& [pattern, count] : stats.pattern_matches) {
        std::cout << "Pattern '" << pattern << "': " << count << " matches\n";
    }
}

void testPatternWithVariousSpacing() {
    std::cout << "\n***** Testing Pattern with Various Instruction Spacing *****\n";
    
    // Test case 1: Adjacent instructions
    {
        std::vector<Instruction> instructions;
        instructions.push_back(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
        instructions.push_back(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));
        
        auto pattern = PeepholeOptimizer::createNeon128BitFusionPattern();
        MatchResult result = pattern->matches(instructions, 0);
        std::cout << "Adjacent MUL instructions - Matched: " << (result.matched ? "YES" : "NO") 
                  << ", Length: " << result.length << "\n";
    }
    
    // Test case 2: Instructions with gap
    {
        std::vector<Instruction> instructions;
        instructions.push_back(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
        
        // Add a NOp or other instruction between
        Instruction nop;
        nop.encoding = 0xd503201f;  // NOP encoding
        nop.assembly_text = "nop";
        instructions.push_back(nop);
        
        instructions.push_back(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));
        
        auto pattern = PeepholeOptimizer::createNeon128BitFusionPattern();
        MatchResult result = pattern->matches(instructions, 0);
        std::cout << "MUL with NOP between - Matched: " << (result.matched ? "YES" : "NO") 
                  << ", Length: " << result.length << "\n";
    }
    
    // Test case 3: Single MUL instruction (should not match)
    {
        std::vector<Instruction> instructions;
        instructions.push_back(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
        
        auto pattern = PeepholeOptimizer::createNeon128BitFusionPattern();
        MatchResult result = pattern->matches(instructions, 0);
        std::cout << "Single MUL instruction - Matched: " << (result.matched ? "YES" : "NO") 
                  << ", Length: " << result.length << "\n";
    }
}

void analyzeEncodingPattern() {
    std::cout << "\n***** Analyzing NEON MUL Encoding Pattern *****\n";
    
    // Create a .2S MUL instruction and examine its encoding
    Instruction mul_2s = VectorCodeGen::mul_vector_2s("V0", "V0", "V1");
    std::cout << "MUL .2S encoding: 0x" << std::hex << std::setw(8) << std::setfill('0') 
              << mul_2s.encoding << std::dec << "\n";
    std::cout << "Expected: 0x0ea19c00\n";
    std::cout << "Match: " << (mul_2s.encoding == 0x0ea19c00 ? "YES" : "NO") << "\n";
    
    // Show what the .4S version should look like
    uint32_t expected_4s = mul_2s.encoding | 0x40000000;
    std::cout << "Expected .4S encoding: 0x" << std::hex << std::setw(8) << std::setfill('0') 
              << expected_4s << std::dec << "\n";
    
    // Analyze bit patterns
    std::cout << "\nBit analysis:\n";
    std::cout << ".2S: " << std::bitset<32>(mul_2s.encoding) << "\n";
    std::cout << ".4S: " << std::bitset<32>(expected_4s) << "\n";
    std::cout << "Bit 30 change (2S->4S): " << std::bitset<32>(0x40000000) << "\n";
}

int main() {
    std::cout << "=== NEON Peephole Optimization Debug Tool ===\n";
    
    try {
        analyzeEncodingPattern();
        testNeonPatternMatching();
        testPatternWithVariousSpacing();
        testWithPeepholeOptimizer();
        
        std::cout << "\n=== Debug Analysis Complete ===\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error during testing: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
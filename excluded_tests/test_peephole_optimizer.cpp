#include <iostream>
#include <cassert>
#include "../PeepholeOptimizer.h"
#include "../InstructionDecoder.h"
#include "../InstructionStream.h"
#include "../InstructionComparator.h"

// Test function for the redundant move pattern optimization
void testRedundantMovePattern() {
    std::cout << "Testing redundant move pattern optimization..." << std::endl;
    
    // Create a peephole optimizer
    PeepholeOptimizer optimizer;
    
    // Create a simple instruction stream with the pattern we want to optimize
    InstructionStream buffer(LabelManager::instance(), false);
    
    // Case 1: Chain of moves pattern
    // mov x1, x0; mov x2, x1 -> should become mov x2, x0
    buffer.add(Instruction(0, "mov x1, x0"));
    buffer.add(Instruction(0, "mov x2, x1"));
    
    // Optimize the buffer
    optimizer.optimize(buffer);
    
    // Check the result - should be a single instruction
    assert(buffer.size() == 1);
    std::string optimized = buffer.get(0).toString();
    std::cout << "  Original pattern: 'mov x1, x0; mov x2, x1'" << std::endl;
    std::cout << "  Optimized to: '" << optimized << "'" << std::endl;
    
    // Verify the instruction is now "mov x2, x0"
    assert(optimized.find("mov x2, x0") != std::string::npos || 
           optimized.find("MOV x2, x0") != std::string::npos);
    
    // Reset the buffer for next test
    buffer.clear();
    
    // Case 2: Circular moves pattern
    // mov x1, x0; mov x0, x1 -> should keep just the first move
    buffer.add(Instruction(0, "mov x1, x0"));
    buffer.add(Instruction(0, "mov x0, x1"));
    
    // Optimize the buffer
    optimizer.optimize(buffer);
    
    // Check the result - should be a single instruction
    assert(buffer.size() == 1);
    optimized = buffer.get(0).toString();
    std::cout << "  Original pattern: 'mov x1, x0; mov x0, x1'" << std::endl;
    std::cout << "  Optimized to: '" << optimized << "'" << std::endl;
    
    // Verify the instruction is now "mov x1, x0" (first instruction kept)
    assert(optimized.find("mov x1, x0") != std::string::npos ||
           optimized.find("MOV x1, x0") != std::string::npos);
    
    std::cout << "Redundant move pattern tests passed!" << std::endl;
}

// Test function for a pattern that should not be optimized
void testNonOptimizablePattern() {
    std::cout << "Testing non-optimizable pattern..." << std::endl;
    
    // Create a peephole optimizer
    PeepholeOptimizer optimizer;
    
    // Create a simple instruction stream with a pattern that should not be optimized
    InstructionStream buffer(LabelManager::instance(), false);
    
    // This pattern doesn't match our optimization rules
    buffer.add(Instruction(0, "mov x1, x0"));
    buffer.add(Instruction(0, "add x2, x1, #1"));
    
    // Save original instructions for comparison
    std::string instr1 = buffer.get(0).toString();
    std::string instr2 = buffer.get(1).toString();
    
    // Optimize the buffer
    optimizer.optimize(buffer);
    
    // Check the result - should still be two instructions
    assert(buffer.size() == 2);
    std::cout << "  Original: '" << instr1 << "; " << instr2 << "'" << std::endl;
    std::cout << "  After optimization: '" << buffer.get(0).toString() << "; " << buffer.get(1).toString() << "'" << std::endl;
    
    // Verify instructions are unchanged
    assert(buffer.get(0).toString() == instr1);
    assert(buffer.get(1).toString() == instr2);
    
    std::cout << "Non-optimizable pattern test passed!" << std::endl;
}

// Test special register handling
void testSpecialRegisters() {
    std::cout << "Testing special register handling..." << std::endl;
    
    // Create a peephole optimizer
    PeepholeOptimizer optimizer;
    
    // Create a simple instruction stream with special registers
    InstructionStream buffer(LabelManager::instance(), false);
    
    // Case: Using SP register
    // mov x1, sp; mov x2, x1 -> should become mov x2, sp
    buffer.add(Instruction(0, "mov x1, sp"));
    buffer.add(Instruction(0, "mov x2, x1"));
    
    // Optimize the buffer
    optimizer.optimize(buffer);
    
    // Check the result - should be a single instruction
    assert(buffer.size() == 1);
    std::string optimized = buffer.get(0).toString();
    std::cout << "  Original pattern: 'mov x1, sp; mov x2, x1'" << std::endl;
    std::cout << "  Optimized to: '" << optimized << "'" << std::endl;
    
    // Verify the instruction is now "mov x2, sp"
    assert(optimized.find("mov x2, sp") != std::string::npos || 
           optimized.find("MOV x2, sp") != std::string::npos);
    
    std::cout << "Special register tests passed!" << std::endl;
}

int main() {
    std::cout << "Running PeepholeOptimizer tests..." << std::endl;
    
    testRedundantMovePattern();
    testNonOptimizablePattern();
    testSpecialRegisters();
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

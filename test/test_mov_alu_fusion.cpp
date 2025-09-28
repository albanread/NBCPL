#include "../PeepholeOptimizer.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include "../InstructionDecoder.h"
#include "../EncoderExtended.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>

/**
 * @brief Test the MOV-ALU Fusion pattern
 * 
 * This test verifies that the PeepholeOptimizer correctly fuses
 * MOV instructions with subsequent ALU operations that use the
 * MOVed value.
 */
void test_mov_alu_fusion() {
    std::cout << "=== Testing MOV-ALU Fusion Optimization ===" << std::endl;
    
    // Create instruction streams for testing different patterns
    InstructionStream mov_add_stream;
    InstructionStream mov_sub_stream;
    
    // Test case 1: MOVZ x1, #100; ADD x0, x2, x1 -> ADD x0, x2, #100
    mov_add_stream.add_instruction(Encoder::create_movz_imm("x1", 100));
    
    Instruction add_instr;
    add_instr.assembly_text = "add x0, x2, x1";
    
    // Manually encode ADD x0, x2, x1
    uint32_t sf_bit = 1 << 31;  // 64-bit operation
    uint32_t op = 0 << 30;      // ADD operation
    uint32_t s_bit = 0 << 29;   // Don't set flags
    uint32_t rd = 0;            // x0
    uint32_t rn = 2;            // x2
    uint32_t rm = 1;            // x1
    add_instr.encoding = sf_bit | op | s_bit | (0x0B << 24) | (rm << 16) | (0 << 10) | (rn << 5) | rd;
    
    mov_add_stream.add_instruction(add_instr);
    
    // Test case 2: MOVZ x1, #50; SUB x0, x2, x1 -> SUB x0, x2, #50
    mov_sub_stream.add_instruction(Encoder::create_movz_imm("x1", 50));
    
    Instruction sub_instr;
    sub_instr.assembly_text = "sub x0, x2, x1";
    
    // Manually encode SUB x0, x2, x1
    op = 1 << 30;      // SUB operation
    sub_instr.encoding = sf_bit | op | s_bit | (0x0B << 24) | (rm << 16) | (0 << 10) | (rn << 5) | rd;
    
    mov_sub_stream.add_instruction(sub_instr);
    
    // Create the peephole optimizer
    PeepholeOptimizer optimizer(true);  // Enable tracing
    
    // Apply optimizations to each test case
    optimizer.optimize(mov_add_stream);
    optimizer.optimize(mov_sub_stream);
    
    // Verify results for Test Case 1: MOVZ x1, #100; ADD x0, x2, x1 -> ADD x0, x2, #100
    {
        std::vector<Instruction> instructions = mov_add_stream.get_instructions();
        
        // Should have one instruction after optimization
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("add x0, x2, #100") != std::string::npos);
        std::cout << "✓ MOVZ x1, #100; ADD x0, x2, x1 -> ADD x0, x2, #100: PASS" << std::endl;
    }
    
    // Verify results for Test Case 2: MOVZ x1, #50; SUB x0, x2, x1 -> SUB x0, x2, #50
    {
        std::vector<Instruction> instructions = mov_sub_stream.get_instructions();
        
        // Should have one instruction after optimization
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("sub x0, x2, #50") != std::string::npos);
        std::cout << "✓ MOVZ x1, #50; SUB x0, x2, x1 -> SUB x0, x2, #50: PASS" << std::endl;
    }
    
    std::cout << "All MOV-ALU fusion tests PASSED!" << std::endl;
}

// Test for more complex cases, including cases where the MOV register is the first operand
void test_mov_alu_fusion_advanced() {
    std::cout << "=== Testing Advanced MOV-ALU Fusion Patterns ===" << std::endl;
    
    // Test case 3: MOVZ x1, #42; AND x0, x2, x1 -> AND x0, x2, #42
    InstructionStream mov_and_stream;
    mov_and_stream.add_instruction(Encoder::create_movz_imm("x1", 42));
    
    Instruction and_instr;
    and_instr.assembly_text = "and x0, x2, x1";
    
    // Manually encode AND x0, x2, x1 (simplified)
    uint32_t sf_bit = 1 << 31;  // 64-bit operation
    and_instr.encoding = sf_bit | (0x0A << 24) | (1 << 16) | (0 << 10) | (2 << 5) | 0;
    
    mov_and_stream.add_instruction(and_instr);
    
    // Test case 4: MOVZ x1, #25; ADD x0, x1, x2 -> ADD x0, x2, #25
    // This is more complex because the MOV register is the first operand
    InstructionStream mov_add_first_stream;
    mov_add_first_stream.add_instruction(Encoder::create_movz_imm("x1", 25));
    
    Instruction add_first_instr;
    add_first_instr.assembly_text = "add x0, x1, x2";
    
    // Manually encode ADD x0, x1, x2
    add_first_instr.encoding = sf_bit | (0 << 30) | (0 << 29) | (0x0B << 24) | (2 << 16) | (0 << 10) | (1 << 5) | 0;
    
    mov_add_first_stream.add_instruction(add_first_instr);
    
    // Create the peephole optimizer
    PeepholeOptimizer optimizer(true);  // Enable tracing
    
    // Apply optimizations to each test case
    optimizer.optimize(mov_and_stream);
    optimizer.optimize(mov_add_first_stream);
    
    // Verify results for Test Case 3: MOVZ x1, #42; AND x0, x2, x1 -> AND x0, x2, #42
    {
        std::vector<Instruction> instructions = mov_and_stream.get_instructions();
        
        // Should have one instruction after optimization
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("and") != std::string::npos);
        assert(assembly_text.find("#42") != std::string::npos);
        std::cout << "✓ MOVZ x1, #42; AND x0, x2, x1 -> AND x0, x2, #42: PASS" << std::endl;
    }
    
    // Verify results for Test Case 4: MOVZ x1, #25; ADD x0, x1, x2 -> ADD x0, x2, #25
    {
        std::vector<Instruction> instructions = mov_add_first_stream.get_instructions();
        
        // Should have one instruction after optimization
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        // The order might be flipped in the optimized instruction
        bool is_add_with_imm = assembly_text.find("add") != std::string::npos && 
                               assembly_text.find("#25") != std::string::npos;
        
        assert(is_add_with_imm);
        std::cout << "✓ MOVZ x1, #25; ADD x0, x1, x2 -> ADD x0, x2, #25: PASS" << std::endl;
    }
    
    std::cout << "All advanced MOV-ALU fusion tests PASSED!" << std::endl;
}

// Test case where optimization should NOT occur - when register is used elsewhere
void test_mov_alu_fusion_safety() {
    std::cout << "=== Testing MOV-ALU Fusion Safety Checks ===" << std::endl;
    
    InstructionStream safety_stream;
    
    // Set up: MOVZ x1, #100; ADD x0, x2, x1; ADD x3, x1, #5
    // The optimization should NOT occur because x1 is used later
    
    safety_stream.add_instruction(Encoder::create_movz_imm("x1", 100));
    
    Instruction add_instr1;
    add_instr1.assembly_text = "add x0, x2, x1";
    uint32_t sf_bit = 1 << 31;
    add_instr1.encoding = sf_bit | (0 << 30) | (0 << 29) | (0x0B << 24) | (1 << 16) | (0 << 10) | (2 << 5) | 0;
    safety_stream.add_instruction(add_instr1);
    
    Instruction add_instr2;
    add_instr2.assembly_text = "add x3, x1, #5";
    add_instr2.encoding = sf_bit | (0 << 30) | (0 << 29) | (0x11 << 24) | (5 << 10) | (1 << 5) | 3;
    safety_stream.add_instruction(add_instr2);
    
    // Create the peephole optimizer
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations
    optimizer.optimize(safety_stream);
    
    // Verify that no optimization occurred (still 3 instructions)
    {
        std::vector<Instruction> instructions = safety_stream.get_instructions();
        
        // Should still have all 3 instructions
        assert(instructions.size() == 3);
        
        std::cout << "✓ Safety check passed - no optimization when register used later: PASS" << std::endl;
    }
    
    std::cout << "All safety check tests PASSED!" << std::endl;
}

int main() {
    test_mov_alu_fusion();
    test_mov_alu_fusion_advanced();
    test_mov_alu_fusion_safety();
    return 0;
}

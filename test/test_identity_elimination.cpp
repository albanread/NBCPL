#include "../PeepholeOptimizer.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include "../InstructionComparator.h"
#include <iostream>
#include <vector>
#include <cassert>

/**
 * @brief Test the Identity Operation Elimination pattern
 * 
 * This test verifies that the PeepholeOptimizer correctly eliminates
 * identity operations like adding zero, multiplying by one, etc.
 */
void test_identity_operation_elimination() {
    std::cout << "=== Testing Identity Operation Elimination ===" << std::endl;
    
    // Create instruction streams for testing different patterns
    InstructionStream add_zero_stream;
    InstructionStream mul_one_stream;
    InstructionStream sub_self_stream;
    InstructionStream mul_neg_one_stream;
    
    // Test case 1: ADD x0, x1, #0 -> MOV x0, x1
    add_zero_stream.add_instruction(Encoder::create_add_imm("x0", "x1", 0));
    
    // Test case 2: MUL x0, x1, w2 where w2 = 1
    mul_one_stream.add_instruction(Encoder::create_movz_imm("w2", 1));
    mul_one_stream.add_instruction(Encoder::create_mul_reg("x0", "x1", "w2"));
    
    // Test case 3: SUB x0, x1, x1 -> MOV x0, #0
    sub_self_stream.add_instruction(Encoder::create_sub_reg("x0", "x1", "x1"));
    
    // Test case 4: MUL x0, x1, #-1 -> NEG x0, x1
    Instruction mul_neg_one;
    mul_neg_one.assembly_text = "mul x0, x1, #-1";
    // Encode a MUL with -1 (this is a simplification for the test)
    mul_neg_one.encoding = 0x9B017C20;  // Approximate encoding
    mul_neg_one_stream.add_instruction(mul_neg_one);
    
    // Create the peephole optimizer
    PeepholeOptimizer optimizer(true);  // Enable tracing
    
    // Apply optimizations to each test case
    optimizer.optimize(add_zero_stream);
    optimizer.optimize(mul_one_stream);
    optimizer.optimize(sub_self_stream);
    optimizer.optimize(mul_neg_one_stream);
    
    // Verify results for Test Case 1: ADD x0, x1, #0 -> MOV x0, x1
    {
        std::vector<Instruction> instructions = add_zero_stream.get_instructions();
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("mov x0, x1") != std::string::npos);
        std::cout << "✓ ADD x0, x1, #0 -> MOV x0, x1: PASS" << std::endl;
    }
    
    // Verify results for Test Case 2: MUL with register holding 1 -> MOV
    {
        std::vector<Instruction> instructions = mul_one_stream.get_instructions();
        
        // Should have two instructions: the MOVZ and the transformed MUL -> MOV
        assert(instructions.size() == 2);
        
        std::string assembly_text = instructions[1].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("mov x0, x1") != std::string::npos);
        std::cout << "✓ MUL x0, x1, w2 (where w2=1) -> MOV x0, x1: PASS" << std::endl;
    }
    
    // Verify results for Test Case 3: SUB x0, x1, x1 -> MOV x0, #0
    {
        std::vector<Instruction> instructions = sub_self_stream.get_instructions();
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("movz x0, #0") != std::string::npos || 
               assembly_text.find("mov x0, #0") != std::string::npos);
        std::cout << "✓ SUB x0, x1, x1 -> MOV x0, #0: PASS" << std::endl;
    }
    
    // Verify results for Test Case 4: MUL x0, x1, #-1 -> NEG x0, x1
    {
        std::vector<Instruction> instructions = mul_neg_one_stream.get_instructions();
        assert(instructions.size() == 1);
        
        std::string assembly_text = instructions[0].assembly_text;
        std::transform(assembly_text.begin(), assembly_text.end(), assembly_text.begin(), ::tolower);
        
        assert(assembly_text.find("neg x0, x1") != std::string::npos);
        std::cout << "✓ MUL x0, x1, #-1 -> NEG x0, x1: PASS" << std::endl;
    }
    
    std::cout << "All identity operation elimination tests PASSED!" << std::endl;
}

int main() {
    test_identity_operation_elimination();
    return 0;
}

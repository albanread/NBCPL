#include "../PeepholeOptimizer.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>
#include <vector>
#include <cassert>

// Helper function to verify the pattern works correctly
bool testRedundantLoadElimination(const std::vector<std::string>& input_asm,
                                const std::vector<std::string>& expected_asm) {
    // Create instruction stream from input assembly
    InstructionStream stream(LabelManager::instance(), false);
    for (const auto& instr : input_asm) {
        Instruction i;
        i.assembly_text = instr;
        stream.add_instruction(i);
    }
    
    // Create peephole optimizer with only our pattern
    PeepholeOptimizer optimizer(true); // Enable tracing
    optimizer.addPattern(PeepholeOptimizer::createRedundantLoadEliminationPattern());
    
    // Run the optimization
    optimizer.optimize(stream, 1); // Single pass
    
    // Get the optimized instructions
    const auto& optimized = stream.get_instructions();
    
    // Check if the output matches expected
    if (optimized.size() != expected_asm.size()) {
        std::cerr << "Output size mismatch! Got " << optimized.size() 
                  << ", expected " << expected_asm.size() << std::endl;
        
        std::cerr << "Actual output:" << std::endl;
        for (const auto& instr : optimized) {
            std::cerr << "  " << instr.assembly_text << std::endl;
        }
        
        return false;
    }
    
    for (size_t i = 0; i < optimized.size(); ++i) {
        if (optimized[i].assembly_text != expected_asm[i]) {
            std::cerr << "Instruction mismatch at position " << i << ":" << std::endl;
            std::cerr << "  Expected: " << expected_asm[i] << std::endl;
            std::cerr << "  Got:      " << optimized[i].assembly_text << std::endl;
            return false;
        }
    }
    
    return true;
}

// Test cases
int main() {
    int tests_passed = 0;
    int tests_failed = 0;
    
    std::cout << "Testing Redundant Load Elimination Pattern..." << std::endl;

    // Test case 1: Basic redundant load elimination
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "sub x2, x3, #10",
            "ldr x1, [sp, #16]"
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "sub x2, x3, #10",
            "mov x1, x0"
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 1: Basic redundant load elimination passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 1: Basic redundant load elimination failed" << std::endl;
            tests_failed++;
        }
    }
    
    // Test case 2: Multiple redundant loads
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "ldr x1, [sp, #16]",
            "mul x5, x6, x7",
            "ldr x2, [sp, #16]"
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "mov x1, x0",
            "mul x5, x6, x7",
            "mov x2, x0"
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 2: Multiple redundant loads passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 2: Multiple redundant loads failed" << std::endl;
            tests_failed++;
        }
    }

    // Test case 3: Redundant load elimination with different offset
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #8]",
            "add x3, x4, x5",
            "ldr x1, [sp, #16]", // Different offset, should not be eliminated
            "ldr x2, [sp, #8]"   // Same offset as first load, should be eliminated
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #8]",
            "add x3, x4, x5",
            "ldr x1, [sp, #16]", // Should remain unchanged
            "mov x2, x0"         // Should be converted to MOV
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 3: Different offset loads passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 3: Different offset loads failed" << std::endl;
            tests_failed++;
        }
    }

    // Test case 4: Store invalidates optimization
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "str x7, [sp, #16]", // Store to same location
            "ldr x1, [sp, #16]"  // Should not be eliminated due to the store
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "str x7, [sp, #16]",
            "ldr x1, [sp, #16]"  // Should remain unchanged
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 4: Store invalidates optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 4: Store invalidates optimization failed" << std::endl;
            tests_failed++;
        }
    }

    // Test case 5: Base register modification invalidates optimization
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "add sp, sp, #32", // Modifies the base register
            "ldr x1, [sp, #16]" // Should not be eliminated due to modified base register
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "add sp, sp, #32",
            "ldr x1, [sp, #16]" // Should remain unchanged
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 5: Base register modification passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 5: Base register modification failed" << std::endl;
            tests_failed++;
        }
    }

    // Test case 6: Modification of source register invalidates optimization
    {
        std::vector<std::string> input = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "mov x0, #42",     // Modifies the register holding the loaded value
            "ldr x1, [sp, #16]" // Should not be eliminated since x0 was modified
        };
        
        std::vector<std::string> expected = {
            "ldr x0, [sp, #16]",
            "add x3, x4, x5",
            "mov x0, #42",
            "ldr x1, [sp, #16]" // Should remain unchanged
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 6: Source register modification passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 6: Source register modification failed" << std::endl;
            tests_failed++;
        }
    }

    // Test case 7: Mixing 32-bit (w) and 64-bit (x) registers
    {
        std::vector<std::string> input = {
            "ldr w0, [sp, #16]",
            "add x3, x4, x5",
            "ldr x1, [sp, #16]" // Load into x register from same location
        };
        
        std::vector<std::string> expected = {
            "ldr w0, [sp, #16]",
            "add x3, x4, x5",
            "mov x1, w0, uxtw" // Should use proper extension from w to x
        };
        
        bool passed = testRedundantLoadElimination(input, expected);
        if (passed) {
            std::cout << "✓ Test 7: Mixed register types passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test 7: Mixed register types failed" << std::endl;
            tests_failed++;
        }
    }

    // Summary
    std::cout << "\nRedundant Load Elimination Test Results:" << std::endl;
    std::cout << "Total tests: " << (tests_passed + tests_failed) << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    
    return tests_failed > 0 ? 1 : 0;
}

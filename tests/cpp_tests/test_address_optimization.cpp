#include "../PeepholeOptimizer.h"
#include "../InstructionStream.h"
#include "../Encoder.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <memory>

// Test helper to verify pattern matching and transformation
bool testPattern(const std::vector<std::string>& input, const std::vector<std::string>& expected_output) {
    // Create instruction stream from input assembly
    InstructionStream stream(LabelManager::instance(), false);
    for (const auto& instr : input) {
        stream.add_instruction(instr);
    }
    
    // Create peephole optimizer with just the pattern we want to test
    PeepholeOptimizer optimizer(true); // Enable tracing
    optimizer.addPattern(PeepholeOptimizer::createAddressGenerationPattern());
    
    // Run optimization
    optimizer.optimize(stream);
    
    // Check if output matches expected
    const auto& optimized = stream.get_instructions();
    if (optimized.size() != expected_output.size()) {
        std::cerr << "Output size mismatch: got " << optimized.size() 
                  << ", expected " << expected_output.size() << std::endl;
        
        std::cerr << "Actual output:" << std::endl;
        for (const auto& instr : optimized) {
            std::cerr << "  " << instr.assembly_text << std::endl;
        }
        
        return false;
    }
    
    for (size_t i = 0; i < optimized.size(); ++i) {
        if (optimized[i].assembly_text != expected_output[i]) {
            std::cerr << "Mismatch at instruction " << i << ":" << std::endl;
            std::cerr << "  Expected: " << expected_output[i] << std::endl;
            std::cerr << "  Got:      " << optimized[i].assembly_text << std::endl;
            return false;
        }
    }
    
    return true;
}

// Test cases for address generation optimization
int main() {
    int tests_passed = 0;
    int tests_failed = 0;
    
    std::cout << "Testing Address Generation Optimization Pattern..." << std::endl;

    // Pattern 1: ADD Xd, Xn, #imm; ADD Xd, Xd, Xm -> ADD Xd, Xn, Xm; ADD Xd, Xd, #imm
    {
        std::vector<std::string> input = {
            "add x0, x1, #100",
            "add x0, x0, x2"
        };
        
        std::vector<std::string> expected = {
            "add x0, x1, x2",
            "add x0, x0, #100"
        };
        
        bool passed = testPattern(input, expected);
        if (passed) {
            std::cout << "✓ Test case 1: ADD-ADD optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test case 1: ADD-ADD optimization failed" << std::endl;
            tests_failed++;
        }
    }

    // Pattern 2: ADD Xd, Xn, #imm; ADD Xd, Xd, Xm, LSL #imm2 -> ADD Xd, Xn, Xm, LSL #imm2; ADD Xd, Xd, #imm
    {
        std::vector<std::string> input = {
            "add x0, x1, #16",
            "add x0, x0, x2, lsl #3"
        };
        
        std::vector<std::string> expected = {
            "add x0, x1, x2, lsl #3",
            "add x0, x0, #16"
        };
        
        bool passed = testPattern(input, expected);
        if (passed) {
            std::cout << "✓ Test case 2: ADD-ADD-shift optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test case 2: ADD-ADD-shift optimization failed" << std::endl;
            tests_failed++;
        }
    }

    // Pattern 3: ADD Xd, Xn, Xm; LDR Xt, [Xd, #0] -> LDR Xt, [Xn, Xm]
    {
        std::vector<std::string> input = {
            "add x0, x1, x2",
            "ldr x3, [x0]"
        };
        
        std::vector<std::string> expected = {
            "ldr x3, [x1, x2]"
        };
        
        bool passed = testPattern(input, expected);
        if (passed) {
            std::cout << "✓ Test case 3: ADD-LDR optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test case 3: ADD-LDR optimization failed" << std::endl;
            tests_failed++;
        }
    }

    // Pattern 4: ADD Xd, Xn, #imm1; LDR Xt, [Xd, #imm2] -> LDR Xt, [Xn, #(imm1+imm2)]
    {
        std::vector<std::string> input = {
            "add x0, x1, #16",
            "ldr x3, [x0, #8]"
        };
        
        std::vector<std::string> expected = {
            "ldr x3, [x1, #24]"
        };
        
        bool passed = testPattern(input, expected);
        if (passed) {
            std::cout << "✓ Test case 4: ADD-LDR with offset optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test case 4: ADD-LDR with offset optimization failed" << std::endl;
            tests_failed++;
        }
    }

    // Pattern 4 with STR: ADD Xd, Xn, #imm1; STR Xt, [Xd, #imm2] -> STR Xt, [Xn, #(imm1+imm2)]
    {
        std::vector<std::string> input = {
            "add x0, x1, #16",
            "str x3, [x0, #8]"
        };
        
        std::vector<std::string> expected = {
            "str x3, [x1, #24]"
        };
        
        bool passed = testPattern(input, expected);
        if (passed) {
            std::cout << "✓ Test case 5: ADD-STR with offset optimization passed" << std::endl;
            tests_passed++;
        } else {
            std::cerr << "✗ Test case 5: ADD-STR with offset optimization failed" << std::endl;
            tests_failed++;
        }
    }
    
    // Summary
    std::cout << "\nAddress Generation Optimization Test Results:" << std::endl;
    std::cout << "Total tests: " << (tests_passed + tests_failed) << std::endl;
    std::cout << "Passed: " << tests_passed << std::endl;
    std::cout << "Failed: " << tests_failed << std::endl;
    
    return tests_failed > 0 ? 1 : 0;
}

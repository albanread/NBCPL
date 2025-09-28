#include "PeepholeOptimizer.h"
#include "EncoderExtended.h"
#include "InstructionStream.h"
#include "InstructionDecoder.h"
#include <memory>
#include <iostream>
#include <iomanip>
#include <cassert>

// Utility function to print instructions with addresses
void printInstructions(const std::vector<Instruction>& instructions, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << std::setw(4) << i << ": " << instructions[i].assembly_text << "\n";
    }
    std::cout << "Total instructions: " << instructions.size() << "\n";
}

// Utility function to print optimization stats
void printStats(const PeepholeOptimizer::OptimizationStats& stats) {
    std::cout << "\n=== Optimization Statistics ===\n";
    std::cout << "Instructions before: " << stats.total_instructions_before << "\n";
    std::cout << "Instructions after: " << stats.total_instructions_after << "\n";
    std::cout << "Optimizations applied: " << stats.optimizations_applied << "\n";
    
    std::cout << "Pattern matches:\n";
    for (const auto& [pattern, count] : stats.pattern_matches) {
        std::cout << "  - " << pattern << ": " << count << "\n";
    }
}

// Test 1: Multiplication by Power of Two
void testMultiplyByPowerOfTwo() {
    std::cout << "\n***** Testing Multiply by Power of Two *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: Multiply by 8 (should be converted to shift left by 3)
    instructions.push_back(Encoder::create_movz_imm("x1", 8, 0));  // Load constant 8 into x1
    instructions.push_back(Encoder::create_mul_reg("x0", "x2", "x1"));  // x0 = x2 * x1 (x2 * 8)
    
    // Example 2: Multiply by 16 (should be converted to shift left by 4)
    instructions.push_back(Encoder::create_movz_imm("x3", 16, 0));  // Load constant 16 into x3
    instructions.push_back(Encoder::create_mul_reg("x4", "x5", "x3"));  // x4 = x5 * x3 (x5 * 16)
    
    // Example 3: Multiply by non-power of two (should remain unchanged)
    instructions.push_back(Encoder::create_movz_imm("x7", 6, 0));  // Load constant 6 into x7
    instructions.push_back(Encoder::create_mul_reg("x6", "x8", "x7"));  // x6 = x8 * x7 (x8 * 6)
    
    // Add some more instructions to make the stream more realistic
    instructions.push_back(Encoder::create_add_reg("x9", "x0", "x4"));  // x9 = x0 + x4
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create instruction stream for optimizer with the singleton LabelManager
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the instructions to the stream
    for (const auto& instr : instructions) {
        stream.add(instr);
    }
    
    // Create optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations with 5 passes to fully exercise the optimizer
    optimizer.optimize(stream, 5);
    
    // Print the optimized instructions
    printInstructions(stream.get_instructions(), "Optimized Instructions");
    
    // Print stats
    printStats(optimizer.getStats());
    
    // Verify the optimizations
    const auto& optimized = stream.get_instructions();
    
    // Check for LSL instructions that replaced MULs
    bool found_lsl_8 = false;
    bool found_lsl_16 = false;
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text.find("lsl x0, x2, #3") != std::string::npos) {
            found_lsl_8 = true;
        }
        if (instr.assembly_text.find("lsl x4, x5, #4") != std::string::npos) {
            found_lsl_16 = true;
        }
    }
    
    std::cout << "Test results:\n";
    std::cout << "  MUL by 8 -> LSL by 3: " << (found_lsl_8 ? "PASS" : "FAIL") << "\n";
    std::cout << "  MUL by 16 -> LSL by 4: " << (found_lsl_16 ? "PASS" : "FAIL") << "\n";
}

// Test 2: Division by Power of Two
void testDivideByPowerOfTwo() {
    std::cout << "\n***** Testing Division by Power of Two *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: Divide by 4 (should be converted to arithmetic shift right by 2)
    instructions.push_back(Encoder::create_movz_imm("x1", 4, 0));  // Load constant 4 into x1
    instructions.push_back(Encoder::create_sdiv_reg("x0", "x2", "x1"));  // x0 = x2 / x1 (x2 / 4)
    
    // Example 2: Divide by 8 (should be converted to arithmetic shift right by 3)
    instructions.push_back(Encoder::create_movz_imm("x3", 8, 0));  // Load constant 8 into x3
    instructions.push_back(Encoder::create_sdiv_reg("x4", "x5", "x3"));  // x4 = x5 / x3 (x5 / 8)
    
    // Example 3: Divide by non-power of two (should remain unchanged)
    instructions.push_back(Encoder::create_movz_imm("x7", 6, 0));  // Load constant 6 into x7
    instructions.push_back(Encoder::create_sdiv_reg("x6", "x8", "x7"));  // x6 = x8 / x7 (x8 / 6)
    
    // Add some more instructions to make the stream more realistic
    instructions.push_back(Encoder::create_add_reg("x9", "x0", "x4"));  // x9 = x0 + x4
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create instruction stream for optimizer with the singleton LabelManager
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the instructions to the stream
    for (const auto& instr : instructions) {
        stream.add(instr);
    }
    
    // Create optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations with 5 passes to fully exercise the optimizer
    optimizer.optimize(stream, 5);
    
    // Print the optimized instructions
    printInstructions(stream.get_instructions(), "Optimized Instructions");
    
    // Print stats
    printStats(optimizer.getStats());
    
    // Verify the optimizations
    const auto& optimized = stream.get_instructions();
    
    // Check for ASR instructions that replaced SDIVs
    bool found_asr_4 = false;
    bool found_asr_8 = false;
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text.find("asr x0, x2, #2") != std::string::npos) {
            found_asr_4 = true;
        }
        if (instr.assembly_text.find("asr x4, x5, #3") != std::string::npos) {
            found_asr_8 = true;
        }
    }
    
    std::cout << "Test results:\n";
    std::cout << "  SDIV by 4 -> ASR by 2: " << (found_asr_4 ? "PASS" : "FAIL") << "\n";
    std::cout << "  SDIV by 8 -> ASR by 3: " << (found_asr_8 ? "PASS" : "FAIL") << "\n";
}

// Test 3: Compare Zero and Branch
void testCompareZeroBranch() {
    std::cout << "\n***** Testing Compare Zero and Branch *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: CMP + B.EQ (should be converted to CBZ)
    instructions.push_back(Encoder::create_cmp_imm("x0", 0));  // Compare x0 with 0
    instructions.push_back(Encoder::create_branch_conditional("eq", "label1"));  // Branch if equal
    
    // Example 2: CMP + B.NE (should be converted to CBNZ)
    instructions.push_back(Encoder::create_cmp_imm("w3", 0));  // Compare w3 with 0
    instructions.push_back(Encoder::create_branch_conditional("ne", "label2"));  // Branch if not equal
    
    // Example 3: CMP with non-zero (should remain unchanged)
    instructions.push_back(Encoder::create_cmp_imm("x5", 5));  // Compare x5 with 5
    instructions.push_back(Encoder::create_branch_conditional("eq", "label3"));  // Branch if equal
    
    // Add label definitions
    Instruction label1;
    label1.is_label_definition = true;
    label1.target_label = "label1";
    label1.assembly_text = "label1:";
    instructions.push_back(label1);
    
    Instruction label2;
    label2.is_label_definition = true;
    label2.target_label = "label2";
    label2.assembly_text = "label2:";
    instructions.push_back(label2);
    
    Instruction label3;
    label3.is_label_definition = true;
    label3.target_label = "label3";
    label3.assembly_text = "label3:";
    instructions.push_back(label3);
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create instruction stream for optimizer with the singleton LabelManager
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the instructions to the stream
    for (const auto& instr : instructions) {
        stream.add(instr);
    }
    
    // Create optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations with 5 passes to fully exercise the optimizer
    optimizer.optimize(stream, 5);
    
    // Print the optimized instructions
    printInstructions(stream.get_instructions(), "Optimized Instructions");
    
    // Print stats
    printStats(optimizer.getStats());
    
    // Verify the optimizations
    const auto& optimized = stream.get_instructions();
    
    // Check for CBZ and CBNZ instructions
    bool found_cbz = false;
    bool found_cbnz = false;
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text.find("cbz x0, label1") != std::string::npos) {
            found_cbz = true;
        }
        if (instr.assembly_text.find("cbnz w3, label2") != std::string::npos) {
            found_cbnz = true;
        }
    }
    
    std::cout << "Test results:\n";
    std::cout << "  CMP + B.EQ -> CBZ: " << (found_cbz ? "PASS" : "FAIL") << "\n";
    std::cout << "  CMP + B.NE -> CBNZ: " << (found_cbnz ? "PASS" : "FAIL") << "\n";
}

// Test 4: ALU Operation Fusion
void testFuseAluOperations() {
    std::cout << "\n***** Testing ALU Operation Fusion *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: ADD Xd, Xn, #imm1 ; ADD Xd, Xd, #imm2
    instructions.push_back(Encoder::create_add_imm("x0", "x1", 100));   // ADD x0, x1, #100
    instructions.push_back(Encoder::create_add_imm("x0", "x0", 200));   // ADD x0, x0, #200
    
    // Example 2: Another similar pattern with different registers
    instructions.push_back(Encoder::create_add_imm("x2", "x3", 50));    // ADD x2, x3, #50
    instructions.push_back(Encoder::create_add_imm("x2", "x2", 70));    // ADD x2, x2, #70
    
    // Example 3: Should NOT be combined (different dest registers)
    instructions.push_back(Encoder::create_add_imm("x4", "x5", 10));    // ADD x4, x5, #10
    instructions.push_back(Encoder::create_add_imm("x6", "x4", 20));    // ADD x6, x4, #20
    
    // Example 4: Should NOT be combined (exceed 12-bit immediate)
    instructions.push_back(Encoder::create_add_imm("x7", "x8", 3000));  // ADD x7, x8, #3000
    instructions.push_back(Encoder::create_add_imm("x7", "x7", 2000));  // ADD x7, x7, #2000
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create instruction stream for optimizer with the singleton LabelManager
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the instructions to the stream
    for (const auto& instr : instructions) {
        stream.add(instr);
    }
    
    // Create optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations with 5 passes to fully exercise the optimizer
    optimizer.optimize(stream, 5);
    
    // Print the optimized instructions
    printInstructions(stream.get_instructions(), "Optimized Instructions");
    
    // Print stats
    printStats(optimizer.getStats());
    
    // Verify the optimizations
    const auto& optimized = stream.get_instructions();
    
    // Check for fused ADD instructions
    bool found_fused_add1 = false;
    bool found_fused_add2 = false;
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text.find("add x0, x1, #300") != std::string::npos) {
            found_fused_add1 = true;
        }
        if (instr.assembly_text.find("add x2, x3, #120") != std::string::npos) {
            found_fused_add2 = true;
        }
    }
    
    std::cout << "Test results:\n";
    std::cout << "  Fused ADD x0, x1, #100; ADD x0, x0, #200 -> ADD x0, x1, #300: " << (found_fused_add1 ? "PASS" : "FAIL") << "\n";
    std::cout << "  Fused ADD x2, x3, #50; ADD x2, x2, #70 -> ADD x2, x3, #120: " << (found_fused_add2 ? "PASS" : "FAIL") << "\n";
}

// Test 5: Load-Store Forwarding
void testLoadStoreForwarding() {
    std::cout << "\n***** Testing Load-Store Forwarding *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: Simple store followed by load
    instructions.push_back(Encoder::create_movz_imm("x0", 42, 0));             // Set x0 = 42
    instructions.push_back(Encoder::create_str_imm("x0", "sp", 16));           // Store x0 to [sp, #16]
    instructions.push_back(Encoder::create_add_imm("x1", "x2", 5));            // Some intermediate instruction
    instructions.push_back(Encoder::create_ldr_imm("x3", "sp", 16));           // Load [sp, #16] into x3
    
    // Example 2: Store followed by load with many instructions in between
    instructions.push_back(Encoder::create_movz_imm("x4", 100, 0));            // Set x4 = 100
    instructions.push_back(Encoder::create_str_imm("x4", "x29", 32));          // Store x4 to [x29, #32]
    instructions.push_back(Encoder::create_add_imm("x5", "x6", 10));           // Intermediate instruction 1
    instructions.push_back(Encoder::create_sub_imm("x7", "x8", 5));            // Intermediate instruction 2
    instructions.push_back(Encoder::create_mov_reg("x9", "x10"));              // Intermediate instruction 3
    instructions.push_back(Encoder::create_ldr_imm("x11", "x29", 32));         // Load [x29, #32] into x11
    
    // Example 3: Store followed by another store to same address, then load (should not optimize)
    instructions.push_back(Encoder::create_movz_imm("x12", 200, 0));           // Set x12 = 200
    instructions.push_back(Encoder::create_str_imm("x12", "x28", 48));         // Store x12 to [x28, #48]
    instructions.push_back(Encoder::create_movz_imm("x13", 300, 0));           // Set x13 = 300
    instructions.push_back(Encoder::create_str_imm("x13", "x28", 48));         // Store x13 to [x28, #48] (overwrites)
    instructions.push_back(Encoder::create_ldr_imm("x14", "x28", 48));         // Load [x28, #48] into x14
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create instruction stream for optimizer with the singleton LabelManager
    InstructionStream stream(LabelManager::instance(), false);
    
    // Add the instructions to the stream
    for (const auto& instr : instructions) {
        stream.add(instr);
    }
    
    // Create optimizer with tracing enabled
    PeepholeOptimizer optimizer(true);
    
    // Apply optimizations with 5 passes to fully exercise the optimizer
    optimizer.optimize(stream, 5);
    
    // Print the optimized instructions
    printInstructions(stream.get_instructions(), "Optimized Instructions");
    
    // Print stats
    printStats(optimizer.getStats());
    
    // Verify the optimizations
    const auto& optimized = stream.get_instructions();
    
    // Check for MOV instructions that replaced LDRs
    bool found_mov_x3_x0 = false;
    bool found_mov_x11_x4 = false;
    bool found_original_ldr_x14 = true; // This one should NOT be optimized since there was an intervening store
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text.find("mov x3, x0") != std::string::npos) {
            found_mov_x3_x0 = true;
        }
        if (instr.assembly_text.find("mov x11, x4") != std::string::npos) {
            found_mov_x11_x4 = true;
        }
        // Third case should remain as an LDR instruction
        if (instr.assembly_text.find("ldr x14, [x28, #48]") == std::string::npos &&
            instr.assembly_text.find("mov x14, x13") != std::string::npos) {
            found_original_ldr_x14 = false;
        }
    }
    
    std::cout << "Test results:\n";
    std::cout << "  STR x0, [sp, #16] + LDR x3, [sp, #16] -> MOV x3, x0: " << (found_mov_x3_x0 ? "PASS" : "FAIL") << "\n";
    std::cout << "  STR x4, [x29, #32] + LDR x11, [x29, #32] -> MOV x11, x4: " << (found_mov_x11_x4 ? "PASS" : "FAIL") << "\n";
    std::cout << "  Intervening store (no optimization): " << (found_original_ldr_x14 ? "PASS" : "FAIL") << "\n";
}

int main(int argc, char* argv[]) {
    // Set default test mode
    bool run_all = true;
    bool detailed_output = false;
    int selected_test = 0;

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            detailed_output = true;
        } else if (arg == "--test" && i + 1 < argc) {
            selected_test = std::stoi(argv[++i]);
            run_all = false;
        } else if (arg == "--help" || arg == "-h") {
            std::cout << "Peephole Optimizer Test Usage:\n"
                      << "  --verbose, -v : Enable detailed output\n"
                      << "  --test N      : Run only test number N\n"
                      << "                  1: Multiply by Power of Two\n"
                      << "                  2: Divide by Power of Two\n"
                      << "                  3: Compare Zero Branch\n"
                      << "                  4: ALU Operation Fusion\n"
                      << "                  5: Load-Store Forwarding\n"
                      << "  --help, -h    : Show this help message\n";
            return 0;
        }
    }
    
    std::cout << "=== Peephole Optimization Tests ===\n";
    
    // Run selected test or all tests
    if (!run_all) {
        switch (selected_test) {
            case 1: testMultiplyByPowerOfTwo(); break;
            case 2: testDivideByPowerOfTwo(); break;
            case 3: testCompareZeroBranch(); break;
            case 4: testFuseAluOperations(); break;
            case 5: testLoadStoreForwarding(); break;
            default:
                std::cout << "Invalid test number: " << selected_test << "\n";
                return 1;
        }
    } else {
        // Run all tests
        testMultiplyByPowerOfTwo();
        testDivideByPowerOfTwo();
        testCompareZeroBranch();
        testFuseAluOperations();
        testLoadStoreForwarding();
    }
    
    std::cout << "\n=== All tests complete ===\n";
    return 0;
}

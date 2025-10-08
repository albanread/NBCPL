// testing/encoder_validation/EncoderTester.cpp
#include "EncoderTester.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>

#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <functional>
#include <map>
#include <set>
#include "TestableEncoders.h"

bool EncoderTester::run_all_tests() {
    std::cout << "\n=== NewBCPL Encoder Validation Framework ===" << std::endl;
    std::cout << "Testing NEON encoder functions against Clang output...\n" << std::endl;

    ensureValidationDirectory();
    initialize_test_map();

    tests_run = 0;
    tests_passed = 0;

    for (const auto& test_pair : encoder_test_map) {
        const std::string& test_name = test_pair.first;
        std::cout << "Testing " << test_name << "..." << std::endl;
        bool result = test_pair.second();
        if (result) {
            std::cout << "  ✅ " << test_name << " [PASS]" << std::endl;
        } else {
            std::cout << "  ❌ " << test_name << " [FAIL]" << std::endl;
        }
    }

    // Print final summary
    std::cout << "\n=== Test Results Summary ===" << std::endl;
    std::cout << "Tests run: " << tests_run << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;

    if (tests_passed == tests_run) {
        std::cout << "✅ ALL TESTS PASSED" << std::endl;
        return true;
    } else {
        std::cout << "❌ SOME TESTS FAILED" << std::endl;
        return false;
    }
}

bool EncoderTester::test_gen_neon_fminp_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fminp_4s(mock_node);
    return runValidation("gen_neon_fminp_4s", instruction);
}

bool EncoderTester::test_gen_neon_fminp_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fminp_2s(mock_node);
    return runValidation("gen_neon_fminp_2s", instruction);
}

bool EncoderTester::test_gen_neon_fminp_4h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fminp_4h(mock_node);
    return runValidation("gen_neon_fminp_4h", instruction);
}

bool EncoderTester::test_gen_neon_fmaxp_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmaxp_4s(mock_node);
    return runValidation("gen_neon_fmaxp_4s", instruction);
}

bool EncoderTester::test_gen_neon_fmaxp_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmaxp_2s(mock_node);
    return runValidation("gen_neon_fmaxp_2s", instruction);
}

bool EncoderTester::test_gen_neon_fmaxp_4h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmaxp_4h(mock_node);
    return runValidation("gen_neon_fmaxp_4h", instruction);
}

bool EncoderTester::test_gen_neon_faddp_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_faddp_4s(mock_node);
    return runValidation("gen_neon_faddp_4s", instruction);
}

bool EncoderTester::test_gen_neon_faddp_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_faddp_2s(mock_node);
    return runValidation("gen_neon_faddp_2s", instruction);
}

bool EncoderTester::test_gen_neon_faddp_4h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_faddp_4h(mock_node);
    return runValidation("gen_neon_faddp_4h", instruction);
}

bool EncoderTester::test_gen_neon_addp_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_addp_4s(mock_node);
    return runValidation("gen_neon_addp_4s", instruction);
}

bool EncoderTester::test_gen_neon_addp_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_addp_2s(mock_node);
    return runValidation("gen_neon_addp_2s", instruction);
}

bool EncoderTester::test_gen_neon_sminp_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sminp_4s(mock_node);
    return runValidation("gen_neon_sminp_4s", instruction);
}

bool EncoderTester::test_gen_neon_sminp_8h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sminp_8h(mock_node);
    return runValidation("gen_neon_sminp_8h", instruction);
}

bool EncoderTester::test_gen_neon_sminp_16b() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sminp_16b(mock_node);
    return runValidation("gen_neon_sminp_16b", instruction);
}

// ===== LEGACY SCALAR ENCODER TESTS =====

bool EncoderTester::test_gen_scalar_nop() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_nop(mock_node);
    return runValidation("gen_scalar_nop", instruction);
}

bool EncoderTester::test_gen_scalar_add_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_add_reg(mock_node);
    return runValidation("gen_scalar_add_reg", instruction);
}

bool EncoderTester::test_gen_scalar_sub_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_sub_reg(mock_node);
    return runValidation("gen_scalar_sub_reg", instruction);
}

bool EncoderTester::test_gen_scalar_mul_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_mul_reg(mock_node);
    return runValidation("gen_scalar_mul_reg", instruction);
}

bool EncoderTester::test_gen_scalar_mov_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_mov_reg(mock_node);
    return runValidation("gen_scalar_mov_reg", instruction);
}

// ===== LEGACY FLOATING POINT ENCODER TESTS =====

bool EncoderTester::test_gen_fp_fadd_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fadd_reg(mock_node);
    return runValidation("gen_fp_fadd_reg", instruction);
}

bool EncoderTester::test_gen_fp_fsub_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fsub_reg(mock_node);
    return runValidation("gen_fp_fsub_reg", instruction);
}

bool EncoderTester::test_gen_fp_fmul_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fmul_reg(mock_node);
    return runValidation("gen_fp_fmul_reg", instruction);
}

bool EncoderTester::test_gen_fp_fdiv_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fdiv_reg(mock_node);
    return runValidation("gen_fp_fdiv_reg", instruction);
}

// ===== LEGACY VECTOR FLOATING POINT ENCODER TESTS =====

bool EncoderTester::test_gen_vec_fadd_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fadd_4s(mock_node);
    return runValidation("gen_vec_fadd_4s", instruction);
}

bool EncoderTester::test_gen_vec_fadd_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fadd_2s(mock_node);
    return runValidation("gen_vec_fadd_2s", instruction);
}

bool EncoderTester::test_gen_vec_fadd_2d() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fadd_2d(mock_node);
    return runValidation("gen_vec_fadd_2d", instruction);
}

bool EncoderTester::test_gen_vec_fsub_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fsub_4s(mock_node);
    return runValidation("gen_vec_fsub_4s", instruction);
}

bool EncoderTester::test_gen_vec_fsub_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fsub_2s(mock_node);
    return runValidation("gen_vec_fsub_2s", instruction);
}

bool EncoderTester::test_gen_vec_fmul_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fmul_4s(mock_node);
    return runValidation("gen_vec_fmul_4s", instruction);
}

bool EncoderTester::test_gen_vec_fmul_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_vec_fmul_2s(mock_node);
    return runValidation("gen_vec_fmul_2s", instruction);
}

// ===== LEGACY LOAD/STORE ENCODER TESTS =====

bool EncoderTester::test_gen_mem_ldr_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_mem_ldr_imm(mock_node);
    return runValidation("gen_mem_ldr_imm", instruction);
}

bool EncoderTester::test_gen_mem_str_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_mem_str_imm(mock_node);
    return runValidation("gen_mem_str_imm", instruction);
}

bool EncoderTester::test_gen_mem_ldp_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_mem_ldp_imm(mock_node);
    return runValidation("gen_mem_ldp_imm", instruction);
}

bool EncoderTester::test_gen_mem_stp_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_mem_stp_imm(mock_node);
    return runValidation("gen_mem_stp_imm", instruction);
}

// ===== LEGACY BRANCH ENCODER TESTS =====

bool EncoderTester::test_gen_scalar_add_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_add_imm(mock_node);
    return runValidation("gen_scalar_add_imm", instruction);
}

bool EncoderTester::test_gen_scalar_sub_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_sub_imm(mock_node);
    return runValidation("gen_scalar_sub_imm", instruction);
}

bool EncoderTester::test_gen_scalar_cmp_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_cmp_reg(mock_node);
    return runValidation("gen_scalar_cmp_reg", instruction);
}

bool EncoderTester::test_gen_scalar_cmp_imm() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_scalar_cmp_imm(mock_node);
    return runValidation("gen_scalar_cmp_imm", instruction);
}

bool EncoderTester::test_gen_fp_fcmp_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fcmp_reg(mock_node);
    return runValidation("gen_fp_fcmp_reg", instruction);
}

bool EncoderTester::test_gen_fp_fsqrt_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fsqrt_reg(mock_node);
    return runValidation("gen_fp_fsqrt_reg", instruction);
}

bool EncoderTester::test_gen_fp_fneg_reg() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_fp_fneg_reg(mock_node);
    return runValidation("gen_fp_fneg_reg", instruction);
}

bool EncoderTester::test_gen_neon_smax_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_smax_4s(mock_node);
    return runValidation("gen_neon_smax_4s", instruction);
}

bool EncoderTester::test_gen_neon_smax_8h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_smax_8h(mock_node);
    return runValidation("gen_neon_smax_8h", instruction);
}

bool EncoderTester::test_gen_neon_smax_16b() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_smax_16b(mock_node);
    return runValidation("gen_neon_smax_16b", instruction);
}

bool EncoderTester::test_gen_neon_add_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_add_4s(mock_node);
    return runValidation("gen_neon_add_4s", instruction);
}

bool EncoderTester::test_gen_neon_add_8h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_add_8h(mock_node);
    return runValidation("gen_neon_add_8h", instruction);
}

bool EncoderTester::test_gen_neon_add_16b() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_add_16b(mock_node);
    return runValidation("gen_neon_add_16b", instruction);
}

bool EncoderTester::test_gen_neon_sub_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sub_4s(mock_node);
    return runValidation("gen_neon_sub_4s", instruction);
}

bool EncoderTester::test_gen_neon_sub_8h() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sub_8h(mock_node);
    return runValidation("gen_neon_sub_8h", instruction);
}

bool EncoderTester::test_gen_neon_sub_16b() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_sub_16b(mock_node);
    return runValidation("gen_neon_sub_16b", instruction);
}

bool EncoderTester::test_gen_neon_fmin_4s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmin_4s(mock_node);
    return runValidation("gen_neon_fmin_4s", instruction);
}

bool EncoderTester::test_gen_neon_fmin_2s() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmin_2s(mock_node);
    return runValidation("gen_neon_fmin_2s", instruction);
}

bool EncoderTester::test_gen_neon_fmin_2d() {
    PairwiseReductionLoopStatement mock_node = createMockReductionStatement();
    Instruction instruction = gen_neon_fmin_2d(mock_node);
    return runValidation("gen_neon_fmin_2d", instruction);
}

bool EncoderTester::runValidation(const std::string& test_name, const Instruction& instruction) {
    tests_run++;
    
    // Step 1: Check if we have valid assembly
    if (instruction.assembly_text.empty()) {
        reportResult(test_name, false, "", "No assembly generated");
        return false;
    }
    
    // Step 2: Write assembly to file and try to compile with clang
    std::string asm_file = validation_dir + "/" + test_name + ".s";
    std::string obj_file = validation_dir + "/" + test_name + ".o";
    
    if (!validateAssembly(instruction.assembly_text, obj_file)) {
        reportResult(test_name, false, "", "Assembly failed to compile with clang");
        return false;
    }
    
    // Step 3: Extract binary encoding from clang-compiled object
    std::string clang_hex = extractClangBinaryEncoding(obj_file);
    if (clang_hex.empty()) {
        reportResult(test_name, false, "", "Failed to extract clang binary encoding");
        return false;
    }
    
    // Step 4: Compare encodings
    std::string our_hex = encodingToHexString(instruction.encoding);
    
    bool passed = false;
    
    // Special handling for relocatable instructions (branches, calls, etc.)
    if (instruction.relocation != RelocationType::NONE) {
        // For relocatable instructions, we need to mask out the immediate field
        // since our encoder generates position-independent code (immediate=0)
        // while Clang generates position-dependent code with calculated offsets
        
        uint32_t clang_encoding = 0;
        uint32_t our_encoding = instruction.encoding;
        
        // Parse clang hex to uint32_t
        if (clang_hex.length() == 8) {
            clang_encoding = static_cast<uint32_t>(std::stoul(clang_hex, nullptr, 16));
        }
        
        if (instruction.relocation == RelocationType::PC_RELATIVE_19_BIT_OFFSET) {
            // For conditional branches: mask out bits 23-5 (19-bit immediate)
            uint32_t mask = ~(0x7FFFF << 5); // Clear bits 23-5
            uint32_t clang_masked = clang_encoding & mask;
            uint32_t our_masked = our_encoding & mask;
            passed = (clang_masked == our_masked);
            
            if (passed) {
                reportResult(test_name, true, clang_hex, our_hex + " (relocatable - immediate fields masked)");
            } else {
                reportResult(test_name, false, clang_hex, our_hex + " (relocatable - masked comparison failed)");
            }
        } else {
            // For other relocation types, fall back to exact comparison for now
            passed = (clang_hex == our_hex);
            reportResult(test_name, passed, clang_hex, our_hex + " (relocatable - exact comparison)");
        }
    } else {
        // Standard exact comparison for non-relocatable instructions
        passed = (clang_hex == our_hex);
        reportResult(test_name, passed, clang_hex, our_hex);
    }
    
    if (passed) {
        tests_passed++;
    }
    
    return passed;
}

void EncoderTester::ensureValidationDirectory() {
    // Create validation directory using system call
    std::string mkdir_cmd = "mkdir -p " + validation_dir;
    system(mkdir_cmd.c_str());
}

bool EncoderTester::validateAssembly(const std::string& assembly_text, const std::string& output_file) {
    // Write assembly to temporary file
    std::string asm_file = output_file.substr(0, output_file.find_last_of('.')) + ".s";
    
    std::ofstream asm_out(asm_file);
    if (!asm_out) {
        return false;
    }
    
    // Write a minimal assembly file with our instruction
    asm_out << ".text\n";
    asm_out << ".global _start\n";
    asm_out << "_start:\n";
    asm_out << "    " << assembly_text << "\n";
    asm_out << "    nop\n";  // Add a NOP to ensure we have something after our instruction
    asm_out << "\n";
    // Add common labels that branch instructions might reference
    asm_out << "test_label:\n";
    asm_out << "    nop\n";
    asm_out << "\n";
    asm_out << "test_function:\n";
    asm_out << "    nop\n";
    asm_out << "    ret\n";
    asm_out.close();
    
    // Try to assemble with clang
    std::string clang_cmd = "clang -c -target arm64-apple-macos11 " + asm_file + " -o " + output_file + " 2>/dev/null";
    
    int result = std::system(clang_cmd.c_str());
    
    if (result != 0) {
        // Keep assembly file for debugging when assembly fails
        return false;
    }
    
    // Keep assembly file for debugging - don't clean up
    // std::filesystem::remove(asm_file);
    
    return (result == 0);
}

std::string EncoderTester::extractClangBinaryEncoding(const std::string& object_file) {
    // Use objdump to extract the binary encoding
    std::string objdump_cmd = "objdump -d " + object_file + " 2>/dev/null";
    
    FILE* pipe = popen(objdump_cmd.c_str(), "r");
    if (!pipe) {
        return "";
    }
    
    std::string result;
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), pipe)) {
        result += buffer;
    }
    pclose(pipe);
    
    // Parse objdump output to find our instruction
    std::istringstream iss(result);
    std::string line;
    while (std::getline(iss, line)) {
        // Look for lines with hex bytes (format: "address: bytes instruction")
        if (line.find(':') != std::string::npos && line.find_first_of("0123456789abcdef") != std::string::npos) {
            // Extract the hex bytes between the colon and the instruction mnemonic
            size_t colon_pos = line.find(':');
            size_t tab_pos = line.find('\t', colon_pos);
            if (colon_pos != std::string::npos && tab_pos != std::string::npos) {
                std::string hex_part = line.substr(colon_pos + 1, tab_pos - colon_pos - 1);
                
                // Remove spaces and extract the first 4 bytes (8 hex chars) for our instruction
                std::string clean_hex;
                for (char c : hex_part) {
                    if (std::isxdigit(c)) {
                        clean_hex += c;
                    }
                }
                
                // Return first 8 hex characters (32-bit instruction)
                if (clean_hex.length() >= 8) {
                    return clean_hex.substr(0, 8);
                }
            }
        }
    }
    
    return "";
}

PairwiseReductionLoopStatement EncoderTester::createMockReductionStatement() {
    // Create a minimal mock AST node for testing purposes
    PairwiseReductionLoopStatement node("test_vec_a", "test_vec_b", "test_result", "test_intrinsic", 0);
    
    return node;
}

std::string EncoderTester::encodingToHexString(uint32_t encoding) {
    std::ostringstream oss;
    oss << std::hex << std::setfill('0') << std::setw(8) << encoding;
    return oss.str();
}

void EncoderTester::reportResult(const std::string& test_name, bool passed, 
                                const std::string& expected_hex, 
                                const std::string& actual_hex) {
    if (passed) {
        std::cout << "  ✅ " << test_name << " [PASS]" << std::endl;
    } else {
        std::cout << "  ❌ " << test_name << " [FAIL]" << std::endl;
        if (!expected_hex.empty() && !actual_hex.empty()) {
            std::cout << "     Expected (Clang): " << expected_hex << std::endl;
            std::cout << "     Actual (Ours):    " << actual_hex << std::endl;
        } else if (!actual_hex.empty()) {
            std::cout << "     Error: " << actual_hex << std::endl;
        }
    }
}

bool EncoderTester::run_single_test(const std::string& encoder_name) {
    std::cout << "\n=== Testing Individual Encoder: " << encoder_name << " ===" << std::endl;
    
    ensureValidationDirectory();
    initialize_test_map();
    
    tests_run = 0;
    tests_passed = 0;
    
    // Check if encoder exists
    auto it = encoder_test_map.find(encoder_name);
    if (it == encoder_test_map.end()) {
        std::cout << "❌ ERROR: Unknown encoder '" << encoder_name << "'" << std::endl;
        suggest_similar_encoders(encoder_name);
        return false;
    }
    
    // Run the specific test
    bool result = it->second();
    
    // Print summary
    std::cout << "\n=== Test Result ===" << std::endl;
    std::cout << "Encoder: " << encoder_name << std::endl;
    std::cout << "Result: " << (result ? "✅ PASS" : "❌ FAIL") << std::endl;
    
    return result;
}

bool EncoderTester::run_pattern_tests(const std::string& pattern) {
    std::cout << "\n=== Testing Encoders Matching Pattern: " << pattern << " ===" << std::endl;
    
    ensureValidationDirectory();
    initialize_test_map();
    
    // Find matching encoders
    std::vector<std::string> matching_encoders = find_matching_encoders(pattern);
    
    if (matching_encoders.empty()) {
        std::cout << "❌ ERROR: No encoders match pattern '" << pattern << "'" << std::endl;
        std::cout << "Available encoders:" << std::endl;
        for (const std::string& encoder_name : list_available_encoders()) {
            std::cout << "  " << encoder_name << std::endl;
        }
        return false;
    }
    
    std::cout << "Found " << matching_encoders.size() << " matching encoder(s):" << std::endl;
    for (const std::string& name : matching_encoders) {
        std::cout << "  " << name << std::endl;
    }
    std::cout << std::endl;
    
    tests_run = 0;
    tests_passed = 0;
    bool all_passed = true;
    
    // Run tests for matching encoders
    for (size_t i = 0; i < matching_encoders.size(); i++) {
        const std::string& encoder_name = matching_encoders[i];
        std::cout << "[" << (i + 1) << "/" << matching_encoders.size() << "] Testing " << encoder_name << "..." << std::endl;
        
        auto it = encoder_test_map.find(encoder_name);
        if (it != encoder_test_map.end()) {
            bool result = it->second();
            if (!result) {
                all_passed = false;
            }
        }
    }
    
    // Print summary
    std::cout << "\n=== Pattern Test Results ===" << std::endl;
    std::cout << "Pattern: " << pattern << std::endl;
    std::cout << "Tests run: " << tests_run << std::endl;
    std::cout << "Tests passed: " << tests_passed << std::endl;
    std::cout << "Tests failed: " << (tests_run - tests_passed) << std::endl;
    
    if (all_passed) {
        std::cout << "✅ ALL PATTERN TESTS PASSED" << std::endl;
    } else {
        std::cout << "❌ SOME PATTERN TESTS FAILED" << std::endl;
    }
    
    return all_passed;
}

std::vector<std::string> EncoderTester::list_available_encoders() {
    initialize_test_map();
    
    std::vector<std::string> encoder_names;
    for (const auto& pair : encoder_test_map) {
        encoder_names.push_back(pair.first);
    }
    
    // Sort alphabetically for better presentation
    std::sort(encoder_names.begin(), encoder_names.end());
    
    return encoder_names;
}

std::vector<std::string> EncoderTester::find_matching_encoders(const std::string& pattern) {
    std::vector<std::string> all_encoders = list_available_encoders();
    std::vector<std::string> matching_encoders;
    
    for (const std::string& encoder_name : all_encoders) {
        if (matches_pattern(encoder_name, pattern)) {
            matching_encoders.push_back(encoder_name);
        }
    }
    
    return matching_encoders;
}

void EncoderTester::initialize_test_map() {
    if (!encoder_test_map.empty()) {
        return; // Already initialized
    }
    
    // Map encoder names to their test functions
    encoder_test_map["fminp_4s"] = [this]() { return test_gen_neon_fminp_4s(); };
    encoder_test_map["fminp_2s"] = [this]() { return test_gen_neon_fminp_2s(); };
    encoder_test_map["fminp_4h"] = [this]() { return test_gen_neon_fminp_4h(); };
    
    encoder_test_map["fmaxp_4s"] = [this]() { return test_gen_neon_fmaxp_4s(); };
    encoder_test_map["fmaxp_2s"] = [this]() { return test_gen_neon_fmaxp_2s(); };
    encoder_test_map["fmaxp_4h"] = [this]() { return test_gen_neon_fmaxp_4h(); };
    
    encoder_test_map["faddp_4s"] = [this]() { return test_gen_neon_faddp_4s(); };
    encoder_test_map["faddp_2s"] = [this]() { return test_gen_neon_faddp_2s(); };
    encoder_test_map["faddp_4h"] = [this]() { return test_gen_neon_faddp_4h(); };
    
    encoder_test_map["addp_4s"] = [this]() { return test_gen_neon_addp_4s(); };
    encoder_test_map["addp_2s"] = [this]() { return test_gen_neon_addp_2s(); };
    
    encoder_test_map["sminp_4s"] = [this]() { return test_gen_neon_sminp_4s(); };
    encoder_test_map["sminp_8h"] = [this]() { return test_gen_neon_sminp_8h(); };
    encoder_test_map["sminp_16b"] = [this]() { return test_gen_neon_sminp_16b(); };
    
    // Legacy scalar encoders
    encoder_test_map["scalar_nop"] = [this]() { return test_gen_scalar_nop(); };
    encoder_test_map["scalar_add_reg"] = [this]() { return test_gen_scalar_add_reg(); };
    encoder_test_map["scalar_sub_reg"] = [this]() { return test_gen_scalar_sub_reg(); };
    encoder_test_map["scalar_mul_reg"] = [this]() { return test_gen_scalar_mul_reg(); };
    encoder_test_map["scalar_mov_reg"] = [this]() { return test_gen_scalar_mov_reg(); };
    
    // Legacy floating point encoders
    encoder_test_map["fp_fadd_reg"] = [this]() { return test_gen_fp_fadd_reg(); };
    encoder_test_map["fp_fsub_reg"] = [this]() { return test_gen_fp_fsub_reg(); };
    encoder_test_map["fp_fmul_reg"] = [this]() { return test_gen_fp_fmul_reg(); };
    encoder_test_map["fp_fdiv_reg"] = [this]() { return test_gen_fp_fdiv_reg(); };
    
    // Legacy vector floating point encoders
    encoder_test_map["vec_fadd_4s"] = [this]() { return test_gen_vec_fadd_4s(); };
    encoder_test_map["vec_fadd_2s"] = [this]() { return test_gen_vec_fadd_2s(); };
    encoder_test_map["vec_fadd_2d"] = [this]() { return test_gen_vec_fadd_2d(); };
    encoder_test_map["vec_fsub_4s"] = [this]() { return test_gen_vec_fsub_4s(); };
    encoder_test_map["vec_fsub_2s"] = [this]() { return test_gen_vec_fsub_2s(); };
    encoder_test_map["vec_fmul_4s"] = [this]() { return test_gen_vec_fmul_4s(); };
    encoder_test_map["vec_fmul_2s"] = [this]() { return test_gen_vec_fmul_2s(); };
    
    // Legacy load/store encoders
    encoder_test_map["mem_ldr_imm"] = [this]() { return test_gen_mem_ldr_imm(); };
    encoder_test_map["mem_str_imm"] = [this]() { return test_gen_mem_str_imm(); };
    encoder_test_map["mem_ldp_imm"] = [this]() { return test_gen_mem_ldp_imm(); };
    encoder_test_map["mem_stp_imm"] = [this]() { return test_gen_mem_stp_imm(); };
    
    // Legacy arithmetic encoders
    encoder_test_map["scalar_add_imm"] = [this]() { return test_gen_scalar_add_imm(); };
    encoder_test_map["scalar_sub_imm"] = [this]() { return test_gen_scalar_sub_imm(); };
    encoder_test_map["scalar_cmp_reg"] = [this]() { return test_gen_scalar_cmp_reg(); };
    encoder_test_map["scalar_cmp_imm"] = [this]() { return test_gen_scalar_cmp_imm(); };
    
    // Legacy floating point encoders
    encoder_test_map["fp_fcmp_reg"] = [this]() { return test_gen_fp_fcmp_reg(); };
    encoder_test_map["fp_fsqrt_reg"] = [this]() { return test_gen_fp_fsqrt_reg(); };
    encoder_test_map["fp_fneg_reg"] = [this]() { return test_gen_fp_fneg_reg(); };
    
    encoder_test_map["smax_4s"] = [this]() { return test_gen_neon_smax_4s(); };
    encoder_test_map["smax_8h"] = [this]() { return test_gen_neon_smax_8h(); };
    encoder_test_map["smax_16b"] = [this]() { return test_gen_neon_smax_16b(); };
    
    encoder_test_map["add_4s"] = [this]() { return test_gen_neon_add_4s(); };
    encoder_test_map["add_8h"] = [this]() { return test_gen_neon_add_8h(); };
    encoder_test_map["add_16b"] = [this]() { return test_gen_neon_add_16b(); };
    
    encoder_test_map["sub_4s"] = [this]() { return test_gen_neon_sub_4s(); };
    encoder_test_map["sub_8h"] = [this]() { return test_gen_neon_sub_8h(); };
    encoder_test_map["sub_16b"] = [this]() { return test_gen_neon_sub_16b(); };
    
    encoder_test_map["fmin_4s"] = [this]() { return test_gen_neon_fmin_4s(); };
    encoder_test_map["fmin_2s"] = [this]() { return test_gen_neon_fmin_2s(); };
    encoder_test_map["fmin_2d"] = [this]() { return test_gen_neon_fmin_2d(); };
    // Wrapper test for Encoder::create_add_reg
    encoder_test_map["create_add_reg"] = [this]() { return this->test_create_add_reg(); };
    // Wrapper test for Encoder::create_and_reg
    encoder_test_map["create_and_reg"] = [this]() { return this->test_create_and_reg(); };
    // Wrapper test for Encoder::create_cmp_imm
    encoder_test_map["create_cmp_imm"] = [this]() { return this->test_create_cmp_imm(); };
    // Wrapper test for Encoder::create_cmp_reg
    encoder_test_map["create_cmp_reg"] = [this]() { return this->test_create_cmp_reg(); };
    // Wrapper test for Encoder::create_eor_reg
    encoder_test_map["create_eor_reg"] = [this]() { return this->test_create_eor_reg(); };
    // Wrapper test for Encoder::create_lsl_imm
    encoder_test_map["create_lsl_imm"] = [this]() { return this->test_create_lsl_imm(); };
    // Wrapper test for Encoder::create_lsl_reg
    encoder_test_map["create_lsl_reg"] = [this]() { return this->test_create_lsl_reg(); };
    // Wrapper test for Encoder::create_lsr_reg
    encoder_test_map["create_lsr_reg"] = [this]() { return this->test_create_lsr_reg(); };
    // Wrapper test for Encoder::create_mov_reg
    encoder_test_map["create_mov_reg"] = [this]() { return this->test_create_mov_reg(); };
    // Wrapper test for Encoder::create_movk_imm
    encoder_test_map["create_movk_imm"] = [this]() { return this->test_create_movk_imm(); };
    // Wrapper test for Encoder::create_movz_imm
    encoder_test_map["create_movz_imm"] = [this]() { return this->test_create_movz_imm(); };
    // Wrapper test for Encoder::create_mul_reg
    encoder_test_map["create_mul_reg"] = [this]() { return this->test_create_mul_reg(); };
    // Wrapper test for Encoder::create_orr_reg
    encoder_test_map["create_orr_reg"] = [this]() { return this->test_create_orr_reg(); };
    // Wrapper test for Encoder::create_sdiv_reg
    encoder_test_map["create_sdiv_reg"] = [this]() { return this->test_create_sdiv_reg(); };
    // Wrapper test for Encoder::create_sub_imm
    encoder_test_map["create_sub_imm"] = [this]() { return this->test_create_sub_imm(); };
    // Wrapper test for Encoder::create_sub_reg
    encoder_test_map["create_sub_reg"] = [this]() { return this->test_create_sub_reg(); };
    // Wrapper test for Encoder::create_brk
    encoder_test_map["create_brk"] = [this]() { return this->test_create_brk(); };
    // Wrapper test for Encoder::create_cset_eq
    encoder_test_map["create_cset_eq"] = [this]() { return this->test_create_cset_eq(); };
    // Wrapper test for Encoder::create_cset
    encoder_test_map["create_cset"] = [this]() { return this->test_create_cset(); };
    // Wrapper test for Encoder::create_csetm_eq
    encoder_test_map["create_csetm_eq"] = [this]() { return this->test_create_csetm_eq(); };







    encoder_test_map["create_directive"] = [this]() { return this->test_create_directive(); };
    encoder_test_map["create_dmb"] = [this]() { return this->test_create_dmb(); };
    encoder_test_map["enc_create_dup_scalar"] = [this]() { return this->test_enc_create_dup_scalar(); };
    encoder_test_map["create_fadd_reg"] = [this]() { return this->test_create_fadd_reg(); };
    encoder_test_map["create_fadd_vector_reg"] = [this]() { return this->test_create_fadd_vector_reg(); };
    encoder_test_map["create_faddp_vector_reg"] = [this]() { return this->test_create_faddp_vector_reg(); };
    encoder_test_map["create_fcmp_reg"] = [this]() { return this->test_create_fcmp_reg(); };

    encoder_test_map["create_fcvtms_reg"] = [this]() { return this->test_create_fcvtms_reg(); };
    encoder_test_map["create_fcvtzs_reg"] = [this]() { return this->test_create_fcvtzs_reg(); };
    encoder_test_map["create_fdiv_reg"] = [this]() { return this->test_create_fdiv_reg(); };
    encoder_test_map["enc_create_fdiv_vector_reg"] = [this]() { return this->test_enc_create_fdiv_vector_reg(); };
    encoder_test_map["create_fmax_vector_reg"] = [this]() { return this->test_create_fmax_vector_reg(); };
    encoder_test_map["enc_create_add_imm"] = [this]() { return this->test_enc_create_add_imm(); };
    //    encoder_test_map["enc_create_add_literal"] = [this]() { return this->test_enc_create_add_literal(); };
    encoder_test_map["enc_create_add_vector_reg"] = [this]() { return this->test_enc_create_add_vector_reg(); };
    encoder_test_map["enc_create_addp_vector_reg"] = [this]() { return this->test_enc_create_addp_vector_reg(); };
    encoder_test_map["enc_create_csetm_ne"] = [this]() { return this->test_enc_create_csetm_ne(); };
    encoder_test_map["enc_create_csetm"] = [this]() { return this->test_enc_create_csetm(); };
    encoder_test_map["enc_create_fadd_reg"] = [this]() { return this->test_enc_create_fadd_reg(); };
    encoder_test_map["enc_create_fadd_vector_reg"] = [this]() { return this->test_enc_create_fadd_vector_reg(); };
    encoder_test_map["enc_create_faddp_vector_reg"] = [this]() { return this->test_enc_create_faddp_vector_reg(); };
    encoder_test_map["enc_create_fcmp_reg"] = [this]() { return this->test_enc_create_fcmp_reg(); };
    encoder_test_map["enc_create_fcvt_d_to_s"] = [this]() { return this->test_enc_create_fcvt_d_to_s(); };
    encoder_test_map["enc_create_fcvtms_reg"] = [this]() { return this->test_enc_create_fcvtms_reg(); };
    encoder_test_map["enc_create_fcvtzs_reg"] = [this]() { return this->test_enc_create_fcvtzs_reg(); };
    encoder_test_map["enc_create_fdiv_reg"] = [this]() { return this->test_enc_create_fdiv_reg(); };
    encoder_test_map["enc_create_fmax_vector_reg"] = [this]() { return this->test_enc_create_fmax_vector_reg(); };
    encoder_test_map["enc_create_fmaxp_vector_reg"] = [this]() { return this->test_enc_create_fmaxp_vector_reg(); };
    encoder_test_map["enc_create_fmin_vector_reg"] = [this]() { return this->test_enc_create_fmin_vector_reg(); };
    encoder_test_map["enc_create_fminp_vector_reg"] = [this]() { return this->test_enc_create_fminp_vector_reg(); };
    encoder_test_map["enc_create_fmla_vector_reg"] = [this]() { return this->test_enc_create_fmla_vector_reg(); };
    encoder_test_map["enc_create_fmov_d_to_x"] = [this]() { return this->test_enc_create_fmov_d_to_x(); };
    encoder_test_map["enc_create_fmov_reg"] = [this]() { return this->test_enc_create_fmov_reg(); };
    encoder_test_map["enc_create_fmov_s_to_w"] = [this]() { return this->test_enc_create_fmov_s_to_w(); };
    encoder_test_map["enc_create_fmov_w_to_s"] = [this]() { return this->test_enc_create_fmov_w_to_s(); };
    encoder_test_map["enc_create_fmov_x_to_d"] = [this]() { return this->test_enc_create_fmov_x_to_d(); };
    encoder_test_map["enc_create_fmul_reg"] = [this]() { return this->test_enc_create_fmul_reg(); };
    encoder_test_map["enc_create_fmul_vector_reg"] = [this]() { return this->test_enc_create_fmul_vector_reg(); };
    encoder_test_map["enc_create_fneg_reg"] = [this]() { return this->test_enc_create_fneg_reg(); };
    encoder_test_map["enc_create_fsqrt_reg"] = [this]() { return this->test_enc_create_fsqrt_reg(); };
    encoder_test_map["enc_create_fsub_reg"] = [this]() { return this->test_enc_create_fsub_reg(); };
    encoder_test_map["enc_create_fsub_vector_reg"] = [this]() { return this->test_enc_create_fsub_vector_reg(); };
    encoder_test_map["enc_create_ld1_vector_reg"] = [this]() { return this->test_enc_create_ld1_vector_reg(); };
    encoder_test_map["enc_create_ldp_imm"] = [this]() { return this->test_enc_create_ldp_imm(); };
    encoder_test_map["enc_create_ldp_post_imm"] = [this]() { return this->test_enc_create_ldp_post_imm(); };
    encoder_test_map["enc_create_ldp_pre_imm"] = [this]() { return this->test_enc_create_ldp_pre_imm(); };
    encoder_test_map["enc_create_ldr_fp_imm"] = [this]() { return this->test_enc_create_ldr_fp_imm(); };
    encoder_test_map["enc_create_ldr_imm"] = [this]() { return this->test_enc_create_ldr_imm(); };
    encoder_test_map["enc_create_ldr_scaled_reg_64bit"] = [this]() { return this->test_enc_create_ldr_scaled_reg_64bit(); };
    encoder_test_map["enc_create_ldr_vec_imm"] = [this]() { return this->test_enc_create_ldr_vec_imm(); };
    encoder_test_map["enc_create_ldr_word_imm"] = [this]() { return this->test_enc_create_ldr_word_imm(); };
    encoder_test_map["enc_create_ldrb_imm"] = [this]() { return this->test_enc_create_ldrb_imm(); };
    encoder_test_map["enc_create_mov_fp_sp"] = [this]() { return this->test_enc_create_mov_fp_sp(); };
    encoder_test_map["enc_create_mul_vector_reg"] = [this]() { return this->test_enc_create_mul_vector_reg(); };
    encoder_test_map["enc_create_mvn_reg"] = [this]() { return this->test_enc_create_mvn_reg(); };
    encoder_test_map["enc_create_nop"] = [this]() { return this->test_enc_create_nop(); };
    encoder_test_map["enc_create_scvtf_reg"] = [this]() { return this->test_enc_create_scvtf_reg(); };
    encoder_test_map["enc_create_smax_vector_reg"] = [this]() { return this->test_enc_create_smax_vector_reg(); };
    encoder_test_map["enc_create_smin_vector_reg"] = [this]() { return this->test_enc_create_smin_vector_reg(); };
    encoder_test_map["enc_create_sminp_vector_reg"] = [this]() { return this->test_enc_create_sminp_vector_reg(); };
    encoder_test_map["enc_create_st1_vector_reg"] = [this]() { return this->test_enc_create_st1_vector_reg(); };
    encoder_test_map["enc_create_stp_imm"] = [this]() { return this->test_enc_create_stp_imm(); };
    encoder_test_map["enc_create_stp_pre_imm"] = [this]() { return this->test_enc_create_stp_pre_imm(); };
    encoder_test_map["enc_create_str_fp_imm"] = [this]() { return this->test_enc_create_str_fp_imm(); };
    encoder_test_map["enc_create_str_imm"] = [this]() { return this->test_enc_create_str_imm(); };
    encoder_test_map["enc_create_str_vec_imm"] = [this]() { return this->test_enc_create_str_vec_imm(); };
    encoder_test_map["enc_create_str_word_imm"] = [this]() { return this->test_enc_create_str_word_imm(); };
    encoder_test_map["enc_create_sub_vector_reg"] = [this]() { return this->test_enc_create_sub_vector_reg(); };
    encoder_test_map["enc_create_svc_imm"] = [this]() { return this->test_enc_create_svc_imm(); };
    encoder_test_map["enc_create_uzp1_vector_reg"] = [this]() { return this->test_enc_create_uzp1_vector_reg(); };
}

bool EncoderTester::test_create_add_reg() {
    Instruction instr = ::test_create_add_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_add_reg", instr);
}

bool EncoderTester::test_create_and_reg() {
    Instruction instr = ::test_create_and_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_and_reg", instr);
}

bool EncoderTester::test_create_cmp_imm() {
    Instruction instr = ::test_create_cmp_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_cmp_imm", instr);
}

bool EncoderTester::test_create_cmp_reg() {
    Instruction instr = ::test_create_cmp_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_cmp_reg", instr);
}

bool EncoderTester::test_create_eor_reg() {
    Instruction instr = ::test_create_eor_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_eor_reg", instr);
}

bool EncoderTester::test_create_lsl_imm() {
    Instruction instr = ::test_create_lsl_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_lsl_imm", instr);
}

bool EncoderTester::test_create_lsl_reg() {
    Instruction instr = ::test_create_lsl_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_lsl_reg", instr);
}

bool EncoderTester::test_create_lsr_reg() {
    Instruction instr = ::test_create_lsr_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_lsr_reg", instr);
}

bool EncoderTester::test_create_mov_reg() {
    Instruction instr = ::test_create_mov_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_mov_reg", instr);
}

bool EncoderTester::test_create_movk_imm() {
    Instruction instr = ::test_create_movk_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_movk_imm", instr);
}

bool EncoderTester::test_create_movz_imm() {
    Instruction instr = ::test_create_movz_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_movz_imm", instr);
}

bool EncoderTester::test_create_mul_reg() {
    Instruction instr = ::test_create_mul_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_mul_reg", instr);
}

bool EncoderTester::test_create_orr_reg() {
    Instruction instr = ::test_create_orr_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_orr_reg", instr);
}

bool EncoderTester::test_create_sdiv_reg() {
    Instruction instr = ::test_create_sdiv_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_sdiv_reg", instr);
}

bool EncoderTester::test_create_sub_imm() {
    Instruction instr = ::test_create_sub_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_sub_imm", instr);
}

bool EncoderTester::test_create_sub_reg() {
    Instruction instr = ::test_create_sub_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_sub_reg", instr);
}

bool EncoderTester::test_create_brk() {
    Instruction instr = ::test_create_brk(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_brk", instr);
}

bool EncoderTester::test_create_cset_eq() {
    Instruction instr = ::test_create_cset_eq(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_cset_eq", instr);
}

bool EncoderTester::test_create_cset() {
    Instruction instr = ::test_create_cset(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_cset", instr);
}

bool EncoderTester::test_create_csetm_eq() {
    Instruction instr = ::test_create_csetm_eq(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("create_csetm_eq", instr);
}

bool EncoderTester::test_enc_create_add_imm() {
    Instruction instr = ::test_enc_create_add_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_add_imm", instr);
}

// bool EncoderTester::test_enc_create_add_literal() {
//     Instruction instr = ::test_enc_create_add_literal(); // Calls wrapper in TestableEncoders.cpp
//     return runValidation("enc_create_add_literal", instr);
// }

bool EncoderTester::test_enc_create_add_vector_reg() {
    Instruction instr = ::test_enc_create_add_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_add_vector_reg", instr);
}

bool EncoderTester::test_enc_create_addp_vector_reg() {
    Instruction instr = ::test_enc_create_addp_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_addp_vector_reg", instr);
}

bool EncoderTester::test_enc_create_csetm_ne() {
    Instruction instr = ::test_enc_create_csetm_ne(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_csetm_ne", instr);
}

bool EncoderTester::test_enc_create_csetm() {
    Instruction instr = ::test_enc_create_csetm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_csetm", instr);
}

bool EncoderTester::test_enc_create_fadd_reg() {
    Instruction instr = ::test_enc_create_fadd_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fadd_reg", instr);
}

bool EncoderTester::test_enc_create_fadd_vector_reg() {
    Instruction instr = ::test_enc_create_fadd_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fadd_vector_reg", instr);
}

bool EncoderTester::test_enc_create_faddp_vector_reg() {
    Instruction instr = ::test_enc_create_faddp_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_faddp_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fcmp_reg() {
    Instruction instr = ::test_enc_create_fcmp_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fcmp_reg", instr);
}

bool EncoderTester::test_enc_create_fcvt_d_to_s() {
    Instruction instr = ::test_enc_create_fcvt_d_to_s(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fcvt_d_to_s", instr);
}

bool EncoderTester::test_enc_create_fcvtms_reg() {
    Instruction instr = ::test_enc_create_fcvtms_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fcvtms_reg", instr);
}

bool EncoderTester::test_enc_create_fcvtzs_reg() {
    Instruction instr = ::test_enc_create_fcvtzs_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fcvtzs_reg", instr);
}

bool EncoderTester::test_enc_create_fdiv_reg() {
    Instruction instr = ::test_enc_create_fdiv_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fdiv_reg", instr);
}

bool EncoderTester::test_enc_create_fmax_vector_reg() {
    Instruction instr = ::test_enc_create_fmax_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmax_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fmaxp_vector_reg() {
    Instruction instr = ::test_enc_create_fmaxp_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmaxp_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fmin_vector_reg() {
    Instruction instr = ::test_enc_create_fmin_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmin_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fminp_vector_reg() {
    Instruction instr = ::test_enc_create_fminp_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fminp_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fmla_vector_reg() {
    Instruction instr = ::test_enc_create_fmla_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmla_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fmov_d_to_x() {
    Instruction instr = ::test_enc_create_fmov_d_to_x(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmov_d_to_x", instr);
}

bool EncoderTester::test_enc_create_fmov_reg() {
    Instruction instr = ::test_enc_create_fmov_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmov_reg", instr);
}

bool EncoderTester::test_enc_create_fmov_s_to_w() {
    Instruction instr = ::test_enc_create_fmov_s_to_w(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmov_s_to_w", instr);
}

bool EncoderTester::test_enc_create_fmov_w_to_s() {
    Instruction instr = ::test_enc_create_fmov_w_to_s(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmov_w_to_s", instr);
}

bool EncoderTester::test_enc_create_fmov_x_to_d() {
    Instruction instr = ::test_enc_create_fmov_x_to_d(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmov_x_to_d", instr);
}

bool EncoderTester::test_enc_create_fmul_reg() {
    Instruction instr = ::test_enc_create_fmul_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmul_reg", instr);
}

bool EncoderTester::test_enc_create_fmul_vector_reg() {
    Instruction instr = ::test_enc_create_fmul_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fmul_vector_reg", instr);
}

bool EncoderTester::test_enc_create_fneg_reg() {
    Instruction instr = ::test_enc_create_fneg_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fneg_reg", instr);
}

bool EncoderTester::test_enc_create_fsqrt_reg() {
    Instruction instr = ::test_enc_create_fsqrt_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fsqrt_reg", instr);
}

bool EncoderTester::test_enc_create_fsub_reg() {
    Instruction instr = ::test_enc_create_fsub_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fsub_reg", instr);
}

bool EncoderTester::test_enc_create_fsub_vector_reg() {
    Instruction instr = ::test_enc_create_fsub_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_fsub_vector_reg", instr);
}

bool EncoderTester::test_enc_create_ld1_vector_reg() {
    Instruction instr = ::test_enc_create_ld1_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ld1_vector_reg", instr);
}

bool EncoderTester::test_enc_create_ldp_imm() {
    Instruction instr = ::test_enc_create_ldp_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldp_imm", instr);
}

bool EncoderTester::test_enc_create_ldp_post_imm() {
    Instruction instr = ::test_enc_create_ldp_post_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldp_post_imm", instr);
}

bool EncoderTester::test_enc_create_ldp_pre_imm() {
    Instruction instr = ::test_enc_create_ldp_pre_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldp_pre_imm", instr);
}

bool EncoderTester::test_enc_create_ldr_fp_imm() {
    Instruction instr = ::test_enc_create_ldr_fp_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldr_fp_imm", instr);
}

bool EncoderTester::test_enc_create_ldr_imm() {
    Instruction instr = ::test_enc_create_ldr_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldr_imm", instr);
}

bool EncoderTester::test_enc_create_ldr_scaled_reg_64bit() {
    Instruction instr = ::test_enc_create_ldr_scaled_reg_64bit(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldr_scaled_reg_64bit", instr);
}

bool EncoderTester::test_enc_create_ldr_vec_imm() {
    Instruction instr = ::test_enc_create_ldr_vec_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldr_vec_imm", instr);
}

bool EncoderTester::test_enc_create_ldr_word_imm() {
    Instruction instr = ::test_enc_create_ldr_word_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldr_word_imm", instr);
}

bool EncoderTester::test_enc_create_ldrb_imm() {
    Instruction instr = ::test_enc_create_ldrb_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_ldrb_imm", instr);
}

bool EncoderTester::test_enc_create_mov_fp_sp() {
    Instruction instr = ::test_enc_create_mov_fp_sp(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_mov_fp_sp", instr);
}

bool EncoderTester::test_enc_create_mul_vector_reg() {
    Instruction instr = ::test_enc_create_mul_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_mul_vector_reg", instr);
}

bool EncoderTester::test_enc_create_mvn_reg() {
    Instruction instr = ::test_enc_create_mvn_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_mvn_reg", instr);
}

bool EncoderTester::test_enc_create_nop() {
    Instruction instr = ::test_enc_create_nop(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_nop", instr);
}

bool EncoderTester::test_enc_create_scvtf_reg() {
    Instruction instr = ::test_enc_create_scvtf_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_scvtf_reg", instr);
}

bool EncoderTester::test_enc_create_smax_vector_reg() {
    Instruction instr = ::test_enc_create_smax_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_smax_vector_reg", instr);
}

bool EncoderTester::test_enc_create_smin_vector_reg() {
    Instruction instr = ::test_enc_create_smin_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_smin_vector_reg", instr);
}

bool EncoderTester::test_enc_create_sminp_vector_reg() {
    Instruction instr = ::test_enc_create_sminp_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_sminp_vector_reg", instr);
}

bool EncoderTester::test_enc_create_st1_vector_reg() {
    Instruction instr = ::test_enc_create_st1_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_st1_vector_reg", instr);
}

bool EncoderTester::test_enc_create_stp_imm() {
    Instruction instr = ::test_enc_create_stp_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_stp_imm", instr);
}

bool EncoderTester::test_enc_create_stp_pre_imm() {
    Instruction instr = ::test_enc_create_stp_pre_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_stp_pre_imm", instr);
}

bool EncoderTester::test_enc_create_str_fp_imm() {
    Instruction instr = ::test_enc_create_str_fp_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_str_fp_imm", instr);
}

bool EncoderTester::test_enc_create_str_imm() {
    Instruction instr = ::test_enc_create_str_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_str_imm", instr);
}

bool EncoderTester::test_enc_create_str_vec_imm() {
    Instruction instr = ::test_enc_create_str_vec_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_str_vec_imm", instr);
}

bool EncoderTester::test_enc_create_str_word_imm() {
    Instruction instr = ::test_enc_create_str_word_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_str_word_imm", instr);
}

bool EncoderTester::test_enc_create_sub_vector_reg() {
    Instruction instr = ::test_enc_create_sub_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_sub_vector_reg", instr);
}

bool EncoderTester::test_enc_create_svc_imm() {
    Instruction instr = ::test_enc_create_svc_imm(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_svc_imm", instr);
}

bool EncoderTester::test_enc_create_uzp1_vector_reg() {
    Instruction instr = ::test_enc_create_uzp1_vector_reg(); // Calls wrapper in TestableEncoders.cpp
    return runValidation("enc_create_uzp1_vector_reg", instr);
}

bool EncoderTester::test_create_add_literal() {
    Instruction instr = ::test_create_add_literal(); // Calls wrapper
    return runValidation("create_add_literal", instr);
}

bool EncoderTester::test_create_adrp() {
    Instruction instr = ::test_create_adrp(); // Calls wrapper
    return runValidation("create_adrp", instr);
}

bool EncoderTester::test_create_br_reg() {
    Instruction instr = ::test_create_br_reg(); // Calls wrapper
    return runValidation("create_br_reg", instr);
}

bool EncoderTester::test_create_branch_conditional() {
    Instruction instr = ::test_create_branch_conditional(); // Calls wrapper
    return runValidation("create_branch_conditional", instr);
}

bool EncoderTester::test_create_branch_unconditional() {
    Instruction instr = ::test_create_branch_unconditional(); // Calls wrapper
    return runValidation("create_branch_unconditional", instr);
}

bool EncoderTester::test_create_branch_with_link_register() {
    Instruction instr = ::test_create_branch_with_link_register(); // Calls wrapper
    return runValidation("create_branch_with_link_register", instr);
}

bool EncoderTester::test_create_branch_with_link() {
    Instruction instr = ::test_create_branch_with_link(); // Calls wrapper
    return runValidation("create_branch_with_link", instr);
}

bool EncoderTester::test_create_directive() {
    Instruction instr = ::test_create_directive(); // Calls wrapper
    return runValidation("create_directive", instr);
}

bool EncoderTester::test_create_dmb() {
    Instruction instr = ::test_create_dmb(); // Calls wrapper
    return runValidation("create_dmb", instr);
}

bool EncoderTester::test_enc_create_dup_scalar() {
    Instruction instr = ::test_enc_create_dup_scalar(); // Calls wrapper
    return runValidation("enc_create_dup_scalar", instr);
}

bool EncoderTester::test_create_fadd_reg() {
    Instruction instr = ::test_create_fadd_reg(); // Calls wrapper
    return runValidation("create_fadd_reg", instr);
}

bool EncoderTester::test_create_fadd_vector_reg() {
    Instruction instr = ::test_create_fadd_vector_reg(); // Calls wrapper
    return runValidation("create_fadd_vector_reg", instr);
}

bool EncoderTester::test_create_faddp_vector_reg() {
    Instruction instr = ::test_create_faddp_vector_reg(); // Calls wrapper
    return runValidation("create_faddp_vector_reg", instr);
}

bool EncoderTester::test_create_fcmp_reg() {
    Instruction instr = ::test_create_fcmp_reg(); // Calls wrapper
    return runValidation("create_fcmp_reg", instr);
}

bool EncoderTester::test_create_fcvt_d_to_s() {
    Instruction instr = ::test_create_fcvt_d_to_s(); // Calls wrapper
    return runValidation("create_fcvt_d_to_s", instr);
}

bool EncoderTester::test_create_fcvtms_reg() {
    Instruction instr = ::test_create_fcvtms_reg(); // Calls wrapper
    return runValidation("create_fcvtms_reg", instr);
}

bool EncoderTester::test_create_fcvtzs_reg() {
    Instruction instr = ::test_create_fcvtzs_reg(); // Calls wrapper
    return runValidation("create_fcvtzs_reg", instr);
}

bool EncoderTester::test_create_fdiv_reg() {
    Instruction instr = ::test_create_fdiv_reg(); // Calls wrapper
    return runValidation("create_fdiv_reg", instr);
}

bool EncoderTester::test_enc_create_fdiv_vector_reg() {
    Instruction instr = ::test_enc_create_fdiv_vector_reg(); // Calls wrapper
    return runValidation("enc_create_fdiv_vector_reg", instr);
}

bool EncoderTester::test_create_fmax_vector_reg() {
    Instruction instr = ::test_create_fmax_vector_reg(); // Calls wrapper
    return runValidation("create_fmax_vector_reg", instr);
}

bool EncoderTester::matches_pattern(const std::string& name, const std::string& pattern) {
    // Simple wildcard matching supporting * (matches any characters)
    if (pattern == "*") {
        return true;
    }
    
    // If no wildcard, exact match
    if (pattern.find('*') == std::string::npos) {
        return name == pattern;
    }
    
    // Handle patterns with * wildcards
    size_t pattern_pos = 0;
    size_t name_pos = 0;
    
    while (pattern_pos < pattern.size() && name_pos < name.size()) {
        if (pattern[pattern_pos] == '*') {
            // Skip the *
            pattern_pos++;
            
            // If * is at the end, match everything remaining
            if (pattern_pos == pattern.size()) {
                return true;
            }
            
            // Find the next non-* character in pattern
            char next_char = pattern[pattern_pos];
            
            // Find that character in the name
            while (name_pos < name.size() && name[name_pos] != next_char) {
                name_pos++;
            }
            
            // If we didn't find it, no match
            if (name_pos == name.size()) {
                return false;
            }
        } else {
            // Regular character matching
            if (name[name_pos] != pattern[pattern_pos]) {
                return false;
            }
            pattern_pos++;
            name_pos++;
        }
    }
    
    // Handle remaining * at end of pattern
    while (pattern_pos < pattern.size() && pattern[pattern_pos] == '*') {
        pattern_pos++;
    }
    
    // Both should be at end for a complete match
    return pattern_pos == pattern.size() && name_pos == name.size();
}

void EncoderTester::suggest_similar_encoders(const std::string& invalid_name) {
    std::vector<std::string> all_encoders = list_available_encoders();
    std::vector<std::string> suggestions;
    
    // Find encoders with similar names (simple substring matching)
    for (const std::string& encoder_name : all_encoders) {
        // Check if the invalid name is a substring of a valid encoder
        if (encoder_name.find(invalid_name) != std::string::npos) {
            suggestions.push_back(encoder_name);
        }
        // Check if a valid encoder is a substring of the invalid name
        else if (invalid_name.find(encoder_name) != std::string::npos) {
            suggestions.push_back(encoder_name);
        }
    }
    
    if (!suggestions.empty()) {
        std::cout << "Did you mean:" << std::endl;
        for (const std::string& suggestion : suggestions) {
            std::cout << "  " << suggestion << std::endl;
        }
    } else {
        std::cout << "Available encoders:" << std::endl;
        for (const std::string& encoder_name : all_encoders) {
            std::cout << "  " << encoder_name << std::endl;
        }
    }
}
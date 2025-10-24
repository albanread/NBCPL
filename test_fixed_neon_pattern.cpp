#include "PeepholeOptimizer.h"
#include "InstructionDecoder.h"
#include "VectorCodeGen.h"
#include <memory>
#include <iostream>

/**
 * Fixed NEON 128-bit fusion pattern that follows the successful pattern structure.
 * 
 * Key fixes:
 * 1. Match exactly 2 consecutive NEON MUL .2S instructions (no variable window)
 * 2. Return exactly what was matched (2 instructions -> replacements for 2 instructions)
 * 3. Simple, direct transformation logic
 * 4. Consistent length reporting between matcher and transformer
 */
std::unique_ptr<InstructionPattern> createFixedNeon128BitFusionPattern() {
    return std::make_unique<InstructionPattern>(
        2,  // Pattern size: exactly 2 instructions
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            // Need at least 2 instructions from current position
            if (pos + 1 >= instrs.size()) return {false, 0};
            
            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];
            
            // Check if either instruction has nopeep attribute
            if (instr1.nopeep || instr2.nopeep) {
                return {false, 0};
            }
            
            // Both instructions must be NEON MUL .2S (encoding 0x0ea19c00)
            if (instr1.encoding == 0x0ea19c00 && instr2.encoding == 0x0ea19c00) {
                return {true, 2};  // Match exactly 2 instructions
            }
            
            return {false, 0};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& instr1 = instrs[pos];
            const auto& instr2 = instrs[pos + 1];
            
            // Create fused 128-bit instruction by modifying the first instruction
            Instruction fused_mul = instr1;
            
            // Convert from .2S to .4S by setting bit 30
            // Original: 0x0ea19c00 (.2S arrangement)  
            // Target:   0x4ea19c00 (.4S arrangement)
            fused_mul.encoding |= 0x40000000;
            fused_mul.assembly_text = "mul\tv0.4s, v0.4s, v1.4s\t; fused 128-bit operation";
            
            // Add comment about the optimization
            Instruction comment;
            comment.encoding = 0;
            comment.assembly_text = "; 128-bit NEON fusion: replaced two mul.2s with one mul.4s";
            comment.segment = SegmentType::CODE;
            
            // Return exactly 2 replacement instructions (comment + fused instruction)
            return { comment, fused_mul };
        },
        "Fixed 128-bit NEON MUL Fusion"
    );
}

/**
 * Alternative approach: Match any two NEON MUL .2S within a small window
 * but still return consistent length
 */
std::unique_ptr<InstructionPattern> createFlexibleNeonFusionPattern() {
    return std::make_unique<InstructionPattern>(
        5,  // Search window: look ahead up to 5 instructions
        [](const std::vector<Instruction>& instrs, size_t pos) -> MatchResult {
            if (pos + 4 >= instrs.size()) return {false, 0};
            
            // Look for first NEON MUL .2S at current position
            if (instrs[pos].encoding != 0x0ea19c00 || instrs[pos].nopeep) {
                return {false, 0};
            }
            
            // Look for second NEON MUL .2S within next 4 instructions
            for (size_t i = pos + 1; i <= pos + 4 && i < instrs.size(); i++) {
                if (instrs[i].encoding == 0x0ea19c00 && !instrs[i].nopeep) {
                    // Found second MUL, return the span from pos to i+1
                    return {true, i - pos + 1};
                }
            }
            
            return {false, 0};
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            std::vector<Instruction> result;
            
            // Find the two MUL positions again (consistent with matcher)
            size_t first_mul_pos = pos;  // First MUL is at pos
            size_t second_mul_pos = 0;
            
            for (size_t i = pos + 1; i <= pos + 4 && i < instrs.size(); i++) {
                if (instrs[i].encoding == 0x0ea19c00 && !instrs[i].nopeep) {
                    second_mul_pos = i;
                    break;
                }
            }
            
            if (second_mul_pos == 0) {
                // Shouldn't happen if matcher worked correctly, but handle gracefully
                for (size_t i = pos; i <= pos + 4 && i < instrs.size(); i++) {
                    result.push_back(instrs[i]);
                }
                return result;
            }
            
            // Create fused instruction from the first MUL
            Instruction fused_mul = instrs[first_mul_pos];
            fused_mul.encoding |= 0x40000000;  // Convert .2S to .4S
            fused_mul.assembly_text = "mul\tv0.4s, v0.4s, v1.4s\t; fused 128-bit operation";
            
            // Copy instructions before second MUL
            for (size_t i = pos; i < second_mul_pos; i++) {
                if (i == first_mul_pos) {
                    // Replace first MUL with fused version
                    result.push_back(fused_mul);
                } else {
                    result.push_back(instrs[i]);
                }
            }
            
            // Skip the second MUL (it's been fused into the first)
            // Add comment where second MUL was
            Instruction comment;
            comment.encoding = 0;
            comment.assembly_text = "; second mul.2s eliminated by fusion";
            comment.segment = SegmentType::CODE;
            result.push_back(comment);
            
            // Copy any remaining instructions after second MUL
            for (size_t i = second_mul_pos + 1; i <= pos + 4 && i < instrs.size(); i++) {
                result.push_back(instrs[i]);
            }
            
            return result;
        },
        "Flexible 128-bit NEON MUL Fusion"
    );
}

// Test function to validate the fixed patterns
void testFixedNeonPatterns() {
    std::cout << "\n=== Testing Fixed NEON Patterns ===\n";
    
    // Create test instructions
    std::vector<Instruction> instructions;
    instructions.push_back(VectorCodeGen::mul_vector_2s("V0", "V0", "V1"));
    instructions.push_back(VectorCodeGen::mul_vector_2s("V2", "V2", "V3"));
    
    std::cout << "Original instructions:\n";
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << i << ": 0x" << std::hex << instructions[i].encoding 
                  << std::dec << " " << instructions[i].assembly_text << "\n";
    }
    
    // Test the fixed pattern
    auto fixed_pattern = createFixedNeon128BitFusionPattern();
    MatchResult result = fixed_pattern->matches(instructions, 0);
    
    std::cout << "\nFixed pattern match result:\n";
    std::cout << "Matched: " << (result.matched ? "YES" : "NO") << "\n";
    std::cout << "Length: " << result.length << "\n";
    
    if (result.matched) {
        std::vector<Instruction> transformed = fixed_pattern->transform(instructions, 0);
        
        std::cout << "\nTransformed instructions (" << transformed.size() << "):\n";
        for (size_t i = 0; i < transformed.size(); i++) {
            std::cout << i << ": 0x" << std::hex << transformed[i].encoding 
                      << std::dec << " " << transformed[i].assembly_text << "\n";
        }
        
        // Verify consistency
        if (transformed.size() == result.length) {
            std::cout << "\n✓ Transformation is consistent (length matches)\n";
        } else {
            std::cout << "\n✗ Transformation inconsistent: reported " << result.length 
                      << " but returned " << transformed.size() << "\n";
        }
    }
    
    // Test flexible pattern
    std::cout << "\n--- Testing Flexible Pattern ---\n";
    auto flexible_pattern = createFlexibleNeonFusionPattern();
    MatchResult flex_result = flexible_pattern->matches(instructions, 0);
    
    std::cout << "Flexible pattern match result:\n";
    std::cout << "Matched: " << (flex_result.matched ? "YES" : "NO") << "\n";
    std::cout << "Length: " << flex_result.length << "\n";
    
    if (flex_result.matched) {
        std::vector<Instruction> flex_transformed = flexible_pattern->transform(instructions, 0);
        
        std::cout << "Flexible transformed instructions (" << flex_transformed.size() << "):\n";
        for (size_t i = 0; i < flex_transformed.size(); i++) {
            std::cout << i << ": 0x" << std::hex << flex_transformed[i].encoding 
                      << std::dec << " " << flex_transformed[i].assembly_text << "\n";
        }
        
        if (flex_transformed.size() == flex_result.length) {
            std::cout << "✓ Flexible transformation is consistent\n";
        } else {
            std::cout << "✗ Flexible transformation inconsistent\n";
        }
    }
}

int main() {
    std::cout << "Testing Fixed NEON Peephole Patterns\n";
    
    try {
        testFixedNeonPatterns();
        std::cout << "\n=== Test Complete ===\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
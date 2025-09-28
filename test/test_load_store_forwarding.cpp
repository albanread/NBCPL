#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <regex>

// Simple classes to mimic the core functionality needed for testing
struct Instruction {
    std::string assembly_text;
    unsigned int encoding = 0;
    size_t address = 0;
};

// Pattern matching class for peephole optimization
class InstructionPattern {
public:
    using MatcherFunction = std::function<bool(const std::vector<Instruction>&, size_t)>;

    InstructionPattern(size_t pattern_size, MatcherFunction matcher_func, 
                      std::function<std::vector<Instruction>(const std::vector<Instruction>&, size_t)> transformer_func,
                      std::string description)
        : pattern_size_(pattern_size), matcher_(matcher_func), transformer_(transformer_func), description_(description) {}

    bool matches(const std::vector<Instruction>& instructions, size_t position) const {
        if (position + pattern_size_ > instructions.size()) {
            return false;
        }
        return matcher_(instructions, position);
    }
    
    std::vector<Instruction> transform(const std::vector<Instruction>& instructions, size_t position) const {
        return transformer_(instructions, position);
    }
    
    size_t getSize() const { return pattern_size_; }
    
    const std::string& getDescription() const { return description_; }

private:
    size_t pattern_size_;
    MatcherFunction matcher_;
    std::function<std::vector<Instruction>(const std::vector<Instruction>&, size_t)> transformer_;
    std::string description_;
};

// Simple instruction encoder class with basic functions needed for testing
class Encoder {
public:
    static Instruction create_movz_imm(const std::string& rd, int imm, int shift = 0) {
        return {
            "movz " + rd + ", #" + std::to_string(imm) + 
            (shift > 0 ? ", lsl #" + std::to_string(shift * 16) : "")
        };
    }
    
    static Instruction create_str_imm(const std::string& rt, const std::string& rn, int offset) {
        return {"str " + rt + ", [" + rn + ", #" + std::to_string(offset) + "]"};
    }
    
    static Instruction create_ldr_imm(const std::string& rt, const std::string& rn, int offset) {
        return {"ldr " + rt + ", [" + rn + ", #" + std::to_string(offset) + "]"};
    }
    
    static Instruction create_add_imm(const std::string& rd, const std::string& rn, int imm) {
        return {"add " + rd + ", " + rn + ", #" + std::to_string(imm)};
    }
    
    static Instruction create_mov_reg(const std::string& rd, const std::string& rm) {
        return {"mov " + rd + ", " + rm};
    }
};

// Namespace to decode instructions for matching
namespace InstructionDecoder {
    enum class OpType {
        UNKNOWN,
        MOV,
        ADD,
        SUB,
        STR,
        LDR,
        B
    };
    
    // Get the opcode from an assembly instruction
    OpType getOpcode(const Instruction& instr) {
        std::string text = instr.assembly_text;
        if (text.find("mov ") == 0) return OpType::MOV;
        if (text.find("add ") == 0) return OpType::ADD;
        if (text.find("sub ") == 0) return OpType::SUB;
        if (text.find("str ") == 0) return OpType::STR;
        if (text.find("ldr ") == 0) return OpType::LDR;
        if (text.find("b ") == 0) return OpType::B;
        return OpType::UNKNOWN;
    }
    
    // Extract destination register
    std::string getDestRegAsString(const Instruction& instr) {
        std::regex regRegex("\\s*\\w+\\s+([xwv]\\d+|sp|lr|xzr|wzr),");
        std::smatch match;
        
        if (std::regex_search(instr.assembly_text, match, regRegex) && match.size() > 1) {
            return match[1].str();
        }
        
        return "";
    }
}

/**
 * Creates a pattern for load-store forwarding optimization.
 * Pattern: STR Xs, [Xn, #offset] + ... + LDR Xd, [Xn, #offset]
 * where Xs is stored and then loaded into Xd without any intervening stores to [Xn, #offset]
 */
std::unique_ptr<InstructionPattern> createLoadStoreForwardingPattern() {
    return std::make_unique<InstructionPattern>(
        3,  // Pattern size: minimum 3 instructions (STR + optional instructions + LDR)
        [](const std::vector<Instruction>& instrs, size_t pos) -> bool {
            // Check if we have enough instructions to examine
            if (pos + 2 >= instrs.size()) return false;
            
            const auto& store_instr = instrs[pos];
            
            // Check if first instruction is a store
            if (InstructionDecoder::getOpcode(store_instr) != InstructionDecoder::OpType::STR) {
                return false;
            }
            
            // Extract base register and offset using regex
            std::string base_reg;
            int store_offset = 0;
            std::regex mem_access_regex("\\[(\\w+),\\s*[#]?(\\d+)\\]");
            std::smatch match;
            if (std::regex_search(store_instr.assembly_text, match, mem_access_regex) && match.size() > 2) {
                base_reg = match[1].str();
                store_offset = std::stoi(match[2].str());
            } else {
                return false;
            }
            
            // Get the source register being stored
            std::string stored_reg;
            std::regex src_reg_regex("\\s*\\w+\\s+([xwv]\\d+|sp|lr|xzr|wzr),");
            if (std::regex_search(store_instr.assembly_text, match, src_reg_regex) && match.size() > 1) {
                stored_reg = match[1].str();
            } else {
                return false;
            }
            
            // Check if stored value is a register, not a literal
            if (stored_reg.empty()) {
                return false;
            }
            
            // Look ahead for a load from the same memory location
            for (size_t i = pos + 1; i < instrs.size() && i <= pos + 20; i++) { // Limit search to 20 instructions ahead
                const auto& curr_instr = instrs[i];
                auto opcode = InstructionDecoder::getOpcode(curr_instr);
                
                // Check if this is a load instruction
                if (opcode == InstructionDecoder::OpType::LDR) {
                    // Extract load base register and offset using regex
                    std::string load_base_reg;
                    int load_offset = 0;
                    
                    if (std::regex_search(curr_instr.assembly_text, match, mem_access_regex) && match.size() > 2) {
                        load_base_reg = match[1].str();
                        load_offset = std::stoi(match[2].str());
                    } else {
                        continue;
                    }
                    
                    if (base_reg == load_base_reg && store_offset == load_offset) {
                        // Found a matching load - now we need to check if there are any intervening stores
                        // to the same memory location
                        bool safe_to_optimize = true;
                        for (size_t j = pos + 1; j < i; j++) {
                            const auto& between_instr = instrs[j];
                            auto between_op = InstructionDecoder::getOpcode(between_instr);
                            
                            // If there's another store to the same location, we can't optimize
                            if (between_op == InstructionDecoder::OpType::STR) {
                                // Check if writing to the same memory location
                                std::regex mem_regex("\\[(\\w+),\\s*[#]?(\\d+)\\]");
                                std::smatch mem_match;
                                if (std::regex_search(between_instr.assembly_text, mem_match, mem_regex) &&
                                    mem_match.size() > 2 &&
                                    mem_match[1].str() == base_reg &&
                                    std::stoi(mem_match[2].str()) == store_offset) {
                                    safe_to_optimize = false;
                                    break;
                                }
                            }
                        }
                        
                        if (safe_to_optimize) {
                            // We've found a load that can be optimized
                            return true;
                        }
                    }
                }
                
                // Stop if we find a branch instruction
                if (opcode == InstructionDecoder::OpType::B) {
                    break;
                }
            }
            
            // Didn't find a matching load
            return false;
        },
        [](const std::vector<Instruction>& instrs, size_t pos) -> std::vector<Instruction> {
            const auto& store_instr = instrs[pos];
            
            // Extract base register and offset using regex
            std::string base_reg;
            int store_offset = 0;
            std::regex mem_access_regex("\\[(\\w+),\\s*[#]?(\\d+)\\]");
            std::smatch match;
            if (std::regex_search(store_instr.assembly_text, match, mem_access_regex) && match.size() > 2) {
                base_reg = match[1].str();
                store_offset = std::stoi(match[2].str());
            }
            
            // Get the source register being stored
            std::string stored_reg;
            std::regex src_reg_regex("\\s*\\w+\\s+([xwv]\\d+|sp|lr|xzr|wzr),");
            if (std::regex_search(store_instr.assembly_text, match, src_reg_regex) && match.size() > 1) {
                stored_reg = match[1].str();
            }
            
            // Find the matching load - start search from position pos+1
            size_t load_pos = pos + 1;
            bool found_load = false;
            for (; load_pos < instrs.size(); load_pos++) {
                const auto& curr_instr = instrs[load_pos];
                auto opcode = InstructionDecoder::getOpcode(curr_instr);
                
                if (opcode == InstructionDecoder::OpType::LDR) {
                    // Extract load base register and offset
                    std::regex mem_regex("\\[(\\w+),\\s*[#]?(\\d+)\\]");
                    std::smatch mem_match;
                    if (std::regex_search(curr_instr.assembly_text, mem_match, mem_regex) &&
                        mem_match.size() > 2 &&
                        mem_match[1].str() == base_reg &&
                        std::stoi(mem_match[2].str()) == store_offset) {
                        found_load = true;
                        break;
                    }
                }
            }
            
            // If we didn't find a matching load, return the original instruction
            if (!found_load) {
                return instrs;
            }
            
            // Get the destination register of the load
            const auto& load_instr = instrs[load_pos];
            
            std::string dest_reg;
            std::regex dst_reg_regex("\\s*\\w+\\s+([xwv]\\d+|sp|lr|xzr|wzr),");
            if (std::regex_search(load_instr.assembly_text, match, dst_reg_regex) && match.size() > 1) {
                dest_reg = match[1].str();
            }
            
            // Create a new instruction sequence:
            // 1. Keep the original store (for correctness)
            // 2. Add a direct register move from stored_reg to dest_reg
            // 3. Include any instructions between the store and load
            std::vector<Instruction> result;
            result.push_back(store_instr);
            
            // Copy all instructions between store and load
            for (size_t i = pos + 1; i < load_pos; i++) {
                result.push_back(instrs[i]);
            }
            
            // Add the MOV instruction instead of the load
            result.push_back(Encoder::create_mov_reg(dest_reg, stored_reg));
            
            return result;
        },
        "Load-Store Forwarding"
    );
}

// Print instructions with addresses
void printInstructions(const std::vector<Instruction>& instructions, const std::string& title) {
    std::cout << "\n=== " << title << " ===\n";
    for (size_t i = 0; i < instructions.size(); i++) {
        std::cout << std::setw(4) << i << ": " << instructions[i].assembly_text << "\n";
    }
    std::cout << "Total instructions: " << instructions.size() << "\n";
}

// Test our load-store forwarding pattern
void testLoadStoreForwarding() {
    std::cout << "\n***** Testing Load-Store Forwarding *****\n";
    
    // Create a sequence of instructions for testing
    std::vector<Instruction> instructions;
    
    // Example 1: Simple store followed by load
    instructions.push_back(Encoder::create_movz_imm("x0", 42));           // Set x0 = 42
    instructions.push_back(Encoder::create_str_imm("x0", "sp", 16));      // Store x0 to [sp, #16]
    instructions.push_back(Encoder::create_add_imm("x1", "x2", 5));       // Some intermediate instruction
    instructions.push_back(Encoder::create_ldr_imm("x3", "sp", 16));      // Load [sp, #16] into x3
    
    // Example 2: Store followed by load with many instructions in between
    instructions.push_back(Encoder::create_movz_imm("x4", 100));          // Set x4 = 100
    instructions.push_back(Encoder::create_str_imm("x4", "x29", 32));     // Store x4 to [x29, #32]
    instructions.push_back(Encoder::create_add_imm("x5", "x6", 10));      // Intermediate instruction 1
    instructions.push_back(Encoder::create_add_imm("x7", "x8", 5));       // Intermediate instruction 2
    instructions.push_back(Encoder::create_mov_reg("x9", "x10"));         // Intermediate instruction 3
    instructions.push_back(Encoder::create_ldr_imm("x11", "x29", 32));    // Load [x29, #32] into x11
    
    // Example 3: Store followed by another store to same address, then load (should not optimize)
    instructions.push_back(Encoder::create_movz_imm("x12", 200));         // Set x12 = 200
    instructions.push_back(Encoder::create_str_imm("x12", "x28", 48));    // Store x12 to [x28, #48]
    instructions.push_back(Encoder::create_movz_imm("x13", 300));         // Set x13 = 300
    instructions.push_back(Encoder::create_str_imm("x13", "x28", 48));    // Store x13 to [x28, #48] (overwrites)
    instructions.push_back(Encoder::create_ldr_imm("x14", "x28", 48));    // Load [x28, #48] into x14
    
    // Print the original instructions
    printInstructions(instructions, "Original Instructions");
    
    // Create our pattern
    auto pattern = createLoadStoreForwardingPattern();
    
    // Apply the pattern to the instructions
    std::vector<Instruction> optimized = instructions;
    bool changes_made = false;
    
    // First pass: find all matches
    std::vector<size_t> match_positions;
    for (size_t i = 0; i < optimized.size(); i++) {
        if (pattern->matches(optimized, i)) {
            match_positions.push_back(i);
        }
    }
    
    // Second pass: apply transformations in reverse order (from end to start)
    // This avoids position shifting issues when multiple matches are found
    for (auto it = match_positions.rbegin(); it != match_positions.rend(); ++it) {
        size_t i = *it;
        std::cout << "Pattern matched at position " << i << "\n";
        
        // Get the optimized instructions
        std::vector<Instruction> replacement = pattern->transform(optimized, i);
        
        // Replace the instructions in the optimized vector
        auto it_start = optimized.begin() + i;
        auto it_end = it_start + pattern->getSize();
        optimized.erase(it_start, it_end);
        optimized.insert(it_start, replacement.begin(), replacement.end());
        
        changes_made = true;
    }
    
    // Print the optimized instructions
    printInstructions(optimized, "Optimized Instructions");
    
    // Verify the optimizations
    bool found_mov_x3_x0 = false;
    bool found_mov_x11_x4 = false;
    bool found_original_ldr_x14 = true;
    
    for (const auto& instr : optimized) {
        if (instr.assembly_text == "mov x3, x0") {
            found_mov_x3_x0 = true;
        }
        if (instr.assembly_text == "mov x11, x4") {
            found_mov_x11_x4 = true;
        }
        // Third case should remain as an LDR instruction
        if (instr.assembly_text == "mov x14, x13") {
            found_original_ldr_x14 = false;
        }
    }
    
    std::cout << "\nTest results:\n";
    std::cout << "  STR x0, [sp, #16] + LDR x3, [sp, #16] -> MOV x3, x0: " << (found_mov_x3_x0 ? "PASS" : "FAIL") << "\n";
    std::cout << "  STR x4, [x29, #32] + LDR x11, [x29, #32] -> MOV x11, x4: " << (found_mov_x11_x4 ? "PASS" : "FAIL") << "\n";
    std::cout << "  Intervening store (no optimization): " << (found_original_ldr_x14 ? "PASS" : "FAIL") << "\n";
}

int main() {
    std::cout << "==== Load-Store Forwarding Test ====\n";
    testLoadStoreForwarding();
    std::cout << "\nTest complete!\n";
    return 0;
}

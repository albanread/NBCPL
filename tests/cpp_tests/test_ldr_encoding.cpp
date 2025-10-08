#include <iostream>
#include <iomanip>
#include <cstdint>

// Test LDR Q encoding vs clang
uint32_t get_reg_encoding_simple(const std::string& reg) {
    if (reg[0] == 'Q' || reg[0] == 'q') {
        return std::stoi(reg.substr(1));
    }
    if (reg[0] == 'X' || reg[0] == 'x') {
        return std::stoi(reg.substr(1));
    }
    return 0;
}

uint32_t create_ldr_vec_encoding(const std::string& qt, const std::string& xn, int immediate) {
    uint32_t qt_encoding = get_reg_encoding_simple(qt);
    uint32_t xn_encoding = get_reg_encoding_simple(xn);
    
    uint32_t scaled_imm = immediate / 16;
    uint32_t encoding = 0x3DC00000;
    
    encoding |= (scaled_imm << 10);
    encoding |= (xn_encoding << 5);
    encoding |= qt_encoding;
    
    return encoding;
}

int main() {
    std::cout << "Testing LDR Q encodings vs clang:\n\n";
    
    struct TestCase {
        std::string qt, xn;
        int imm;
        uint32_t expected;
        std::string description;
    };
    
    TestCase tests[] = {
        {"Q0", "X26", 0, 0x3dc00340, "ldr q0, [x26]"},
        {"Q1", "X25", 0, 0x3dc00321, "ldr q1, [x25]"},
        {"Q3", "X26", 16, 0x3dc00743, "ldr q3, [x26, #0x10]"},
        {"Q4", "X25", 16, 0x3dc00724, "ldr q4, [x25, #0x10]"}
    };
    
    for (auto& test : tests) {
        uint32_t our_encoding = create_ldr_vec_encoding(test.qt, test.xn, test.imm);
        
        std::cout << test.description << ":\n";
        std::cout << "  Expected: 0x" << std::hex << std::setw(8) << std::setfill('0') << test.expected << "\n";
        std::cout << "  Our code: 0x" << std::hex << std::setw(8) << std::setfill('0') << our_encoding << "\n";
        std::cout << "  Match: " << (our_encoding == test.expected ? "YES" : "NO") << "\n\n";
    }
    
    return 0;
}
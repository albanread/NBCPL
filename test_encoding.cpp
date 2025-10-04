#include <iostream>
#include <iomanip>
#include <cstdint>

// Simplified encoder test - just the ADD vector logic
uint32_t get_reg_encoding(const std::string& reg) {
    if (reg[0] == 'V' || reg[0] == 'D') {
        return std::stoi(reg.substr(1));
    }
    return 0;
}

uint32_t create_add_vector_encoding(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    uint32_t encoding;
    
    if (arrangement == "4S") {
        encoding = 0x4ea08400;
    } else if (arrangement == "2S") {
        encoding = 0x0ea08400;
    } else {
        encoding = 0x0e208400; // fallback
    }

    encoding |= (rm << 16) | (rn << 5) | rd;
    return encoding;
}

int main() {
    std::cout << "Testing NEON ADD encodings vs clang results:\n\n";
    
    // Test cases from clang .lst file
    struct TestCase {
        std::string vd, vn, vm, arrangement;
        uint32_t expected;
        std::string description;
    };
    
    TestCase tests[] = {
        {"V2", "V0", "V1", "4S", 0x4ea18402, "ADD V2.4S, V0.4S, V1.4S"},
        {"V5", "V3", "V4", "4S", 0x4ea48465, "ADD V5.4S, V3.4S, V4.4S"},
        {"V16", "V6", "V7", "4S", 0x4ea784d0, "ADD V16.4S, V6.4S, V7.4S"},
        {"V19", "V17", "V18", "4S", 0x4eb28633, "ADD V19.4S, V17.4S, V18.4S"}
    };
    
    for (auto& test : tests) {
        uint32_t our_encoding = create_add_vector_encoding(test.vd, test.vn, test.vm, test.arrangement);
        
        std::cout << test.description << ":\n";
        std::cout << "  Expected: 0x" << std::hex << std::setw(8) << std::setfill('0') << test.expected << "\n";
        std::cout << "  Our code: 0x" << std::hex << std::setw(8) << std::setfill('0') << our_encoding << "\n";
        std::cout << "  Match: " << (our_encoding == test.expected ? "YES" : "NO") << "\n";
        
        if (our_encoding != test.expected) {
            uint32_t diff = our_encoding ^ test.expected;
            std::cout << "  Diff:     0x" << std::hex << std::setw(8) << std::setfill('0') << diff << "\n";
        }
        std::cout << "\n";
    }
    
    return 0;
}
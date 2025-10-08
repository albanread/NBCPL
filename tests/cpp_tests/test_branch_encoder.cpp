#include "Encoder.h"
#include <iostream>

int main() {
    std::cout << "Testing branch conditional encoder..." << std::endl;
    
    try {
        // Test the encoder directly
        Instruction instr = Encoder::create_branch_conditional("eq", "test_label");
        
        std::cout << "Assembly text: '" << instr.assembly_text << "'" << std::endl;
        std::cout << "Encoding: 0x" << std::hex << instr.encoding << std::dec << std::endl;
        std::cout << "Relocation type: " << static_cast<int>(instr.relocation) << std::endl;
        std::cout << "Target label: '" << instr.target_label << "'" << std::endl;
        
    } catch (const std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
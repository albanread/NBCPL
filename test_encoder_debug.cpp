#include "Encoder.h"
#include <iostream>
#include <iomanip>

int main() {
    std::cout << "Testing NEON ADD .4S instruction encoding..." << std::endl;
    
    // Test the encoding we generate vs what clang produces
    auto instr = Encoder::create_add_vector_reg("V2", "V0", "V1", "4S");
    
    std::cout << "Our encoding: 0x" << std::hex << std::setfill('0') << std::setw(8) 
              << instr.encoding << std::endl;
    std::cout << "Clang encoding: 0x4ea18402" << std::endl;
    std::cout << "Assembly: " << instr.assembly << std::endl;
    
    // Break down the bits
    uint32_t encoding = instr.encoding;
    std::cout << "\nBit breakdown:" << std::endl;
    std::cout << "Q bit (30): " << ((encoding >> 30) & 1) << std::endl;
    std::cout << "U bit (29): " << ((encoding >> 29) & 1) << std::endl;
    std::cout << "Size (23:22): " << ((encoding >> 22) & 3) << std::endl;
    std::cout << "Opcode (15:10): 0x" << std::hex << ((encoding >> 10) & 0x3F) << std::endl;
    std::cout << "Rm (20:16): " << std::dec << ((encoding >> 16) & 0x1F) << std::endl;
    std::cout << "Rn (9:5): " << ((encoding >> 5) & 0x1F) << std::endl;
    std::cout << "Rd (4:0): " << (encoding & 0x1F) << std::endl;
    
    // Expected clang encoding breakdown
    uint32_t clang_encoding = 0x4ea18402;
    std::cout << "\nClang bit breakdown:" << std::endl;
    std::cout << "Q bit (30): " << ((clang_encoding >> 30) & 1) << std::endl;
    std::cout << "U bit (29): " << ((clang_encoding >> 29) & 1) << std::endl;
    std::cout << "Size (23:22): " << ((clang_encoding >> 22) & 3) << std::endl;
    std::cout << "Opcode (15:10): 0x" << std::hex << ((clang_encoding >> 10) & 0x3F) << std::endl;
    std::cout << "Rm (20:16): " << std::dec << ((clang_encoding >> 16) & 0x1F) << std::endl;
    std::cout << "Rn (9:5): " << ((clang_encoding >> 5) & 0x1F) << std::endl;
    std::cout << "Rd (4:0): " << (clang_encoding & 0x1F) << std::endl;
    
    return 0;
}
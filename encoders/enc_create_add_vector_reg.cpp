// Status: PASS - All ADD vector variations tested by NewBCPL --test-encoders
#include "Encoder.h"
#include <sstream>
#include <stdexcept>

Instruction Encoder::create_add_vector_reg(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    uint32_t rd = get_reg_encoding(vd);
    uint32_t rn = get_reg_encoding(vn);
    uint32_t rm = get_reg_encoding(vm);

    uint32_t encoding;

    // Hard-coded correct encodings based on clang output
    // Clang produces 4ea18402 for ADD V2.4S, V0.4S, V1.4S
    // This means: base = 4ea08400, then add registers
    
    if (arrangement == "2S") {
        // 64-bit version: 0ea08400 base
        encoding = 0x0ea08400;
    } else if (arrangement == "4S") {
        // 128-bit version: 4ea08400 base (from clang reverse engineering)
        encoding = 0x4ea08400;
    } else if (arrangement == "8B") {
        // 64-bit 8-byte: 0e208400 base  
        encoding = 0x0e208400;
    } else if (arrangement == "16B") {
        // 128-bit 16-byte: 4e208400 base
        encoding = 0x4e208400;
    } else if (arrangement == "4H") {
        // 64-bit 4-halfword: 0e608400 base
        encoding = 0x0e608400;
    } else if (arrangement == "8H") {
        // 128-bit 8-halfword: 4e608400 base
        encoding = 0x4e608400;
    } else if (arrangement == "2D") {
        // 128-bit 2-doubleword: 4ee08400 base
        encoding = 0x4ee08400;
    } else {
        throw std::runtime_error("Unsupported arrangement for ADD vector: " + arrangement);
    }

    // Add register fields: Rm[20:16], Rn[9:5], Rd[4:0]
    encoding |= (rm << 16) | (rn << 5) | rd;

    // Debug output to compare with clang
    printf("NEON ADD encoding debug:\n");
    printf("  vd=%s rd=%d, vn=%s rn=%d, vm=%s rm=%d, arrangement=%s\n", 
           vd.c_str(), rd, vn.c_str(), rn, vm.c_str(), rm, arrangement.c_str());
    printf("  Our encoding: 0x%08x\n", encoding);
    if (arrangement == "4S" && rd == 2 && rn == 0 && rm == 1) {
        printf("  Clang reference: 0x4ea18402 for ADD V2.4S, V0.4S, V1.4S\n");
    }
    fflush(stdout);

    std::stringstream ss;
    ss << "ADD " << vd << "." << arrangement << ", " << vn << "." << arrangement << ", " << vm << "." << arrangement;
    Instruction instr(encoding, ss.str());
    instr.opcode = InstructionDecoder::OpType::ADD_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}
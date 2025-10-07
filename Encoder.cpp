#include "BitPatcher.h"
#include "Encoder.h"
#include <iostream>

// --- OOP helpers for codegen ---

Instruction Encoder::create_mov_reg_comment(const std::string &xd, const std::string &xs, const std::string &comment) {
    // ARM64 MOV: Xd = Xs
    uint32_t xd_val = Encoder::get_reg_encoding(xd);
    uint32_t xs_val = Encoder::get_reg_encoding(xs);

    // Base opcode for MOV (alias for ORR): 0xAA0003E0
    BitPatcher patcher(0xAA0003E0);
    patcher.patch(xs_val, 16, 5); // Rm (source)
    patcher.patch(xd_val, 0, 5);  // Rd (dest)

    std::string assembly = "MOV " + xd + ", " + xs;
    if (!comment.empty()) {
        assembly += "    // " + comment;
    }
    Instruction instr(patcher.get_value(), assembly);
    instr.opcode = InstructionDecoder::OpType::MOV;
    instr.dest_reg = xd_val;
    instr.src_reg1 = xs_val;
    return instr;
}

Instruction Encoder::create_stp_fp_pre_imm(const std::string &dt1, const std::string &dt2, const std::string &dn, int immediate) {
    // ARM64: STP Dt1, Dt2, [Dn, #imm]!
    // Encoding follows "Load/Store Pair (pre-index)" format.
    // Key fields for 64-bit FP registers (D):
    // - opc (bits 31-30): 01
    // - V   (bit 26)    : 1 (indicates FP/SIMD registers)
    // - L   (bit 22)    : 0 (for Store)

    uint32_t rt1_val = get_reg_encoding(dt1);
    uint32_t rt2_val = get_reg_encoding(dt2);
    uint32_t rn_val = get_reg_encoding(dn);

    if (immediate % 8 != 0 || immediate < -512 || immediate > 504) {
        throw std::invalid_argument("STP (FP) immediate must be a multiple of 8 in range [-512, 504].");
    }

    // Scale the immediate for the 7-bit signed offset field (imm7).
    int imm7 = immediate / 8;

    // Base opcode for STP (FP, 64-bit, pre-indexed) is 0x69800000.
    BitPatcher patcher(0x69800000);
    patcher.patch(imm7, 15, 7);
    patcher.patch(rt2_val, 10, 5);
    patcher.patch(rn_val, 5, 5);
    patcher.patch(rt1_val, 0, 5);

    std::string assembly = "STP " + dt1 + ", " + dt2 + ", [" + dn + ", #" + std::to_string(immediate) + "]!";
    Instruction instr(patcher.get_value(), assembly);
    instr.opcode = InstructionDecoder::OpType::STP; // Reuse existing OpType
    instr.src_reg1 = rt1_val;
    instr.src_reg2 = rt2_val;
    instr.base_reg = rn_val;
    instr.immediate = immediate;
    instr.uses_immediate = true;
    instr.is_mem_op = true;
    return instr;
}

Instruction Encoder::create_ldp_fp_post_imm(const std::string &dt1, const std::string &dt2, const std::string &dn, int immediate) {
    // ARM64: LDP Dt1, Dt2, [Dn], #imm
    // Encoding follows "Load/Store Pair (post-index)" format.
    // Key fields for 64-bit FP registers (D):
    // - opc (bits 31-30): 01
    // - V   (bit 26)    : 1 (indicates FP/SIMD registers)
    // - L   (bit 22)    : 1 (for Load)

    uint32_t rt1_val = get_reg_encoding(dt1);
    uint32_t rt2_val = get_reg_encoding(dt2);
    uint32_t rn_val = get_reg_encoding(dn);

    if (immediate % 8 != 0 || immediate < -512 || immediate > 504) {
        throw std::invalid_argument("LDP (FP) immediate must be a multiple of 8 in range [-512, 504].");
    }

    // Scale the immediate for the 7-bit signed offset field (imm7).
    int imm7 = immediate / 8;

    // Base opcode for LDP (FP, 64-bit, post-indexed) is 0x68C00000.
    BitPatcher patcher(0x68C00000);
    patcher.patch(imm7, 15, 7);
    patcher.patch(rt2_val, 10, 5);
    patcher.patch(rn_val, 5, 5);
    patcher.patch(rt1_val, 0, 5);

    std::string assembly = "LDP " + dt1 + ", " + dt2 + ", [" + dn + "], #" + std::to_string(immediate);
    Instruction instr(patcher.get_value(), assembly);
    instr.opcode = InstructionDecoder::OpType::LDP; // Reuse existing OpType
    instr.dest_reg = rt1_val;
    // instr.src_reg1 is used for the second destination register in LDP
    instr.src_reg1 = rt2_val;
    instr.base_reg = rn_val;
    instr.immediate = immediate;
    instr.uses_immediate = true;
    instr.is_mem_op = true;
    return instr;
}






#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <string>

/**
 * @brief Encodes the ARM64 'BIC' (Bit Clear) instruction.
 * @details
 * This function generates the 32-bit machine code for a BIC instruction,
 * which performs a bitwise AND of the first source register with the
 * bitwise NOT of the second source register. The result is stored in the
 * destination register. The operation is: `Xd = Xn & ~Xm`.
 *
 * The BIC instruction is an alias for `AND` with an inverted second operand.
 * The encoding follows the "Data-processing (register)" format for AND
 * (shifted register) with the 'N' bit (bit 21) set to 1 to invert Rm.
 *
 * The encoding layout is:
 * - **sf (bit 31)**: 1 for 64-bit, 0 for 32-bit.
 * - **opc, S, Family**: Fixed bits identifying the instruction.
 * - **N (bit 21)**: `1` to invert the second source operand.
 * - **Rm (bits 20-16)**: The second source register `xm`.
 * - **Rn (bits 9-5)**: The first source register `xn`.
 * - **Rd (bits 4-0)**: The destination register `xd`.
 *
 * @param xd The destination register (e.g., "x0", "w1").
 * @param xn The first source register (e.g., "x1", "sp").
 * @param xm The second source register to be inverted (e.g., "x2", "wzr").
 * @return An `Instruction` object containing the encoding and assembly text.
 * @throw std::invalid_argument if register names are invalid or if sizes are mixed.
 */
Instruction Encoder::create_bic_reg(const std::string& xd, const std::string& xn, const std::string& xm) {
    // (A) Perform self-checking by parsing and validating all register arguments first.
    uint32_t rd_num = get_reg_encoding(xd);
    uint32_t rn_num = get_reg_encoding(xn);
    uint32_t rm_num = get_reg_encoding(xm);

    bool rd_is_64 = (xd[0] == 'x' || xd[0] == 'X');
    bool rn_is_64 = (xn[0] == 'x' || xn[0] == 'X');
    bool rm_is_64 = (xm[0] == 'x' || xm[0] == 'X');

    if (!(rd_is_64 == rn_is_64 && rn_is_64 == rm_is_64)) {
        throw std::invalid_argument("Mismatched register sizes. All operands for BIC (register) must be simultaneously 32-bit (W) or 64-bit (X).");
    }

    // (B) Use the BitPatcher to construct the instruction word.
    // Base opcode for 32-bit BIC (register) is 0x0A200000.
    // This is the AND (shifted register) opcode with the 'N' bit (21) already set.
    BitPatcher patcher(0x0A200000);

    if (rd_is_64) {
        patcher.patch(1, 31, 1); // Set the sf bit for 64-bit operation.
    }

    patcher.patch(rd_num, 0, 5);  // Rd
    patcher.patch(rn_num, 5, 5);  // Rn
    patcher.patch(rm_num, 16, 5); // Rm

    // (C) Format the assembly string for the Instruction object.
    std::string assembly_text = "BIC " + xd + ", " + xn + ", " + xm;

    // (D) Return the completed Instruction object. No relocation is needed.
    Instruction instr(patcher.get_value(), assembly_text);
    instr.opcode = InstructionDecoder::OpType::BIC;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);
    instr.src_reg2 = Encoder::get_reg_encoding(xm);
    return instr;
}

/**
 * @brief Encodes the ARM64 'BFI' (Bitfield Insert) instruction.
 * @details
 * This function generates the 32-bit machine code to insert a bitfield from a
 * source register into a destination register, leaving other bits unchanged.
 * BFI is an alias for the BFM (Bitfield Move) instruction.
 * The operation is `BFI <Xd|Wd>, <Xn|Wn>, #lsb, #width`.
 *
 * The encoding follows the "Bitfield" format for BFM:
 * - **sf (bit 31)**: 1 for 64-bit, 0 for 32-bit.
 * - **opc (bits 30-29)**: `01` for BFM (BFI alias).
 * - **Family (bits 28-23)**: `0b100110`.
 * - **N (bit 22)**: Must match `sf`.
 * - **immr (bits 21-16)**: The right-rotate amount, calculated as `(datasize - lsb) % datasize`.
 * - **imms (bits 15-10)**: The most significant bit of the source field, which is `width - 1`.
 * - **Rn (bits 9-5)**: The source register `xn`.
 * - **Rd (bits 4-0)**: The destination register `xd`.
 *
 * @param xd The destination register to be modified (e.g., "x0", "w1").
 * @param xn The source register containing the bits to insert.
 * @param lsb The least significant bit (start position) in the destination (0-63).
 * @param width The width of the bitfield to insert (1-64).
 * @return An `Instruction` object.
 * @throw std::invalid_argument for invalid registers or bitfield parameters.
 */
Instruction Encoder::opt_create_bfi(const std::string& xd, const std::string& xn, int lsb, int width) {
    // 1. Validate register names and determine size
    uint32_t rd_num = get_reg_encoding(xd);
    uint32_t rn_num = get_reg_encoding(xn);
    bool is_64bit = (xd[0] == 'x' || xd[0] == 'X');

    if (is_64bit != (xn[0] == 'x' || xn[0] == 'X')) {
        throw std::invalid_argument("Mismatched register sizes for BFI.");
    }

    // 2. Validate bitfield parameters
    int datasize = is_64bit ? 64 : 32;
    if (lsb < 0 || lsb >= datasize) {
        throw std::invalid_argument("BFI lsb is out of range for the register size.");
    }
    if (width < 1 || width > datasize) {
        throw std::invalid_argument("BFI width is out of range for the register size.");
    }
    if ((lsb + width) > datasize) {
        throw std::invalid_argument("BFI bitfield (lsb + width) exceeds register size.");
    }

    // 3. Calculate encoding fields for the BFM alias
    uint32_t n_val = is_64bit ? 1 : 0;
    uint32_t immr_val = static_cast<uint32_t>((datasize - lsb) % datasize);
    uint32_t imms_val = static_cast<uint32_t>(width - 1);

    // 4. Use BitPatcher to construct the instruction word.
    // Base opcode for BFM (BFI alias) is 0x33000000.
    BitPatcher patcher(0x33000000);

    if (is_64bit) {
        patcher.patch(1, 31, 1); // sf bit
    }

    patcher.patch(n_val, 22, 1);       // N bit
    patcher.patch(immr_val, 16, 6);    // immr (rotate)
    patcher.patch(imms_val, 10, 6);    // imms (msb)
    patcher.patch(rn_num, 5, 5);       // Rn
    patcher.patch(rd_num, 0, 5);       // Rd

    // 5. Format the assembly string and return the Instruction object.
    std::string assembly_text = "BFI " + xd + ", " + xn + ", #" + std::to_string(lsb) + ", #" + std::to_string(width);
    Instruction instr(patcher.get_value(), assembly_text);
    instr.opcode = InstructionDecoder::OpType::BFI;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);
    instr.immediate = lsb; // Store lsb in immediate field
    instr.uses_immediate = true;
    return instr;
}

/**
 * @brief Encodes the ARM64 'BFXIL' (Bitfield Extract and Insert at Low bits) instruction.
 * @details
 * This function generates the 32-bit machine code to extract a bitfield from a
 * source register and insert it at bit position 0 in the destination register,
 * clearing the upper bits of the destination.
 * BFXIL is an alias for the BFM (Bitfield Move) instruction.
 * The operation is `BFXIL <Xd|Wd>, <Xn|Wn>, #lsb, #width`.
 *
 * The encoding follows the "Bitfield" format for BFM:
 * - **sf (bit 31)**: 1 for 64-bit, 0 for 32-bit.
 * - **opc (bits 30-29)**: `01` for BFM (BFXIL alias).
 * - **N (bit 22)**: 1 for 64-bit, 0 for 32-bit.
 * - **immr (bits 21-16)**: Source extraction start bit (lsb).
 * - **imms (bits 15-10)**: Width - 1 of the bitfield.
 * - **Rn (bits 9-5)**: Source register number.
 * - **Rd (bits 4-0)**: Destination register number.
 */
Instruction Encoder::opt_create_bfxil(const std::string& xd, const std::string& xn, int lsb, int width) {
    // 1. Validate register names and determine size
    uint32_t rd_num = get_reg_encoding(xd);
    uint32_t rn_num = get_reg_encoding(xn);
    bool is_64bit = (xd[0] == 'x' || xd[0] == 'X');

    if (is_64bit != (xn[0] == 'x' || xn[0] == 'X')) {
        throw std::invalid_argument("Mismatched register sizes for BFXIL.");
    }

    // 2. Validate bitfield parameters
    int datasize = is_64bit ? 64 : 32;
    if (lsb < 0 || lsb >= datasize) {
        throw std::invalid_argument("BFXIL lsb is out of range for the register size.");
    }
    if (width < 1 || width > datasize) {
        throw std::invalid_argument("BFXIL width is out of range for the register size.");
    }
    if ((lsb + width) > datasize) {
        throw std::invalid_argument("BFXIL bitfield (lsb + width) exceeds register size.");
    }

    // 3. Calculate encoding fields for the BFM alias (BFXIL)
    // For BFXIL: extract from source[lsb:lsb+width-1] and insert at dest[0:width-1]
    uint32_t n_val = is_64bit ? 1 : 0;
    uint32_t immr_val = static_cast<uint32_t>(lsb);  // Source extraction start bit
    uint32_t imms_val = static_cast<uint32_t>(width - 1);  // Width - 1

    // 4. Use BitPatcher to construct the instruction word.
    // Base opcode for BFM (BFXIL alias) is 0x33000000.
    BitPatcher patcher(0x33000000);

    if (is_64bit) {
        patcher.patch(1, 31, 1); // sf bit
    }

    patcher.patch(n_val, 22, 1);       // N bit
    patcher.patch(immr_val, 16, 6);    // immr (source extraction start)
    patcher.patch(imms_val, 10, 6);    // imms (width - 1)
    patcher.patch(rn_num, 5, 5);       // Rn
    patcher.patch(rd_num, 0, 5);       // Rd

    // 5. Format the assembly string and return the Instruction object.
    std::string assembly_text = "BFXIL " + xd + ", " + xn + ", #" + std::to_string(lsb) + ", #" + std::to_string(width);
    Instruction instr(patcher.get_value(), assembly_text);
    instr.opcode = InstructionDecoder::OpType::BFXIL;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);
    instr.immediate = lsb; // Store lsb in immediate field
    instr.uses_immediate = true;
    return instr;
}

/**
 * @brief Creates an ADR instruction. Loads the address of a label into a register.
 * @param xd The destination register.
 * @param label_name The target label.
 * @return A complete Instruction object with relocation info.
 *
 * Note: This is a stub implementation for use by the peephole optimizer's ADR fusion pattern.
 * It emits a pseudo-instruction with relocation info; actual encoding/linking is handled later.
 */

Instruction Encoder::create_adr(const std::string &xd, const std::string &label_name) {
    // For now, emit a dummy encoding and mark with relocation.
    // Real encoding should be handled by the linker/relocator.
    std::string assembly_text = "ADR " + xd + ", " + label_name;
    Instruction instr(0x10000000, assembly_text, RelocationType::PAGE_21_BIT_PC_RELATIVE, label_name, false);
    instr.opcode = InstructionDecoder::OpType::ADR;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.target_label = label_name;
    return instr;
}


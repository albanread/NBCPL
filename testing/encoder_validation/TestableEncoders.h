#pragma once

#include "../../Encoder.h"
#include "../../AST.h"

// Wrapper functions for Encoder::create_* for direct testing.
Instruction test_create_add_reg();
Instruction test_create_and_reg();
Instruction test_create_cmp_imm();
Instruction test_create_cmp_reg();
Instruction test_create_eor_reg();
Instruction test_create_lsl_imm();
Instruction test_create_lsl_reg();
Instruction test_create_lsr_reg();
Instruction test_create_mov_reg();
Instruction test_create_movk_imm();
Instruction test_create_movz_imm();
Instruction test_create_mul_reg();
Instruction test_create_orr_reg();
Instruction test_create_sdiv_reg();
Instruction test_create_sub_imm();
Instruction test_create_sub_reg();
Instruction test_create_brk();
Instruction test_create_cset_eq();
Instruction test_create_cset();
Instruction test_create_csetm_eq();

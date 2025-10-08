# Vector Instruction Encoders Inventory - NewBCPL

This document provides a comprehensive inventory of all vector instruction encoder functions in the NewBCPL codebase, organized by location and functionality. This survey was conducted to identify cleanup opportunities and prepare for FQUAD vector type implementation.

## 1. Main ARM64 NEON Encoders (`NewBCPL/encoders/`)

### Arithmetic Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_add_vector_reg.cpp` | `Encoder::create_add_vector_reg()` | Vector integer addition (ADD Vd.T, Vn.T, Vm.T) |
| `enc_create_sub_vector_reg.cpp` | `Encoder::create_sub_vector_reg()` | Vector integer subtraction (SUB Vd.T, Vn.T, Vm.T) |
| `enc_create_mul_vector_reg.cpp` | `Encoder::create_mul_vector_reg()` | Vector integer multiplication (MUL Vd.T, Vn.T, Vm.T) |
| `enc_create_addp_vector_reg.cpp` | `Encoder::create_addp_vector_reg()` | Vector pairwise addition (ADDP Vd.T, Vn.T, Vm.T) |

### Floating-Point Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_fadd_vector_reg.cpp` | `Encoder::create_fadd_vector_reg()` | Vector float addition (FADD Vd.T, Vn.T, Vm.T) |
| `enc_create_fsub_vector_reg.cpp` | `Encoder::enc_create_fsub_vector_reg()` | Vector float subtraction (FSUB Vd.T, Vn.T, Vm.T) |
| `enc_create_fmul_vector_reg.cpp` | `Encoder::create_fmul_vector_reg()` | Vector float multiplication (FMUL Vd.T, Vn.T, Vm.T) |
| `enc_create_fdiv_vector_reg.cpp` | `Encoder::enc_create_fdiv_vector_reg()` | Vector float division (FDIV Vd.T, Vn.T, Vm.T) |
| `enc_create_faddp_vector_reg.cpp` | `Encoder::create_faddp_vector_reg()` | Vector float pairwise addition (FADDP Vd.T, Vn.T, Vm.T) |

### Min/Max Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_smin_vector_reg.cpp` | `Encoder::create_smin_vector_reg()` | Vector signed minimum (SMIN Vd.T, Vn.T, Vm.T) |
| `enc_create_fmin_vector_reg.cpp` | `Encoder::create_fmin_vector_reg()` | Vector float minimum (FMIN Vd.T, Vn.T, Vm.T) |
| `enc_create_fminp_vector_reg.cpp` | `Encoder::create_fminp_vector_reg()` | Vector float pairwise minimum (FMINP Vd.T, Vn.T, Vm.T) |
| `enc_create_fmaxp_vector_reg.cpp` | `Encoder::create_fmaxp_vector_reg()` | Vector float pairwise maximum (FMAXP Vd.T, Vn.T, Vm.T) |

### Advanced Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_fmla_vector_reg.cpp` | `Encoder::create_fmla_vector_reg()` | Vector fused multiply-add (FMLA Vd.T, Vn.T, Vm.T) |
| `enc_create_uzp1_vector_reg.cpp` | `Encoder::create_uzp1_vector_reg()` | Unzip even elements (UZP1 Vd.T, Vn.T, Vm.T) |
| `enc_create_uzp2_vector_reg.cpp` | `Encoder::create_uzp2_vector_reg()` | Unzip odd elements (UZP2 Vd.T, Vn.T, Vm.T) |

### Memory Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_ld1_vector_reg.cpp` | `Encoder::create_ld1_vector_reg()` | Load single structure to vector (LD1 {Vt.T}, [Xn]) |
| `enc_create_st1_vector_reg.cpp` | `Encoder::create_st1_vector_reg()` | Store single structure from vector (ST1 {Vt.T}, [Xn]) |
| `enc_create_ldr_vec_imm.cpp` | `Encoder::create_ldr_vec_imm()` | Load Q register with immediate offset |
| `enc_create_str_vec_imm.cpp` | `Encoder::create_str_vec_imm()` | Store Q register with immediate offset |

### Scalar-Vector Operations
| File | Function | Description |
|------|----------|-------------|
| `enc_create_dup_scalar.cpp` | `Encoder::enc_create_dup_scalar()` | Duplicate scalar to vector (DUP Vd.T, Rn) |

## 2. VectorCodeGen Class (`NewBCPL/VectorCodeGen.h/.cpp`)

### Public Specialized Encoders for PAIR/FPAIR Operations
| Function | Description | Assembly |
|----------|-------------|----------|
| `sub_vector_2s()` | PAIR subtraction | SUB Vd.2S, Vn.2S, Vm.2S |
| `mul_vector_2s()` | PAIR multiplication | MUL Vd.2S, Vn.2S, Vm.2S |
| `add_vector_2s()` | PAIR addition | ADD Vd.2S, Vn.2S, Vm.2S |
| `fadd_vector_2s()` | FPAIR addition | FADD Vd.2S, Vn.2S, Vm.2S |
| `fsub_vector_2s()` | FPAIR subtraction | FSUB Vd.2S, Vn.2S, Vm.2S |
| `fmul_vector_2s()` | FPAIR multiplication | FMUL Vd.2S, Vn.2S, Vm.2S |
| `fdiv_vector_2s()` | FPAIR division | FDIV Vd.2S, Vn.2S, Vm.2S |

### Private Custom Vector Encoders (vecgen_* methods)
| Function | Description |
|----------|-------------|
| `vecgen_fadd_vector()` | Custom FADD vector encoding |
| `vecgen_fsub_vector()` | Custom FSUB vector encoding |
| `vecgen_fmul_vector()` | Custom FMUL vector encoding |
| `vecgen_fdiv_vector()` | Custom FDIV vector encoding |
| `vecgen_add_vector()` | Custom ADD vector encoding |
| `vecgen_sub_vector()` | Custom SUB vector encoding |
| `vecgen_mul_vector()` | Custom MUL vector encoding |

### Lane Operations
| Function | Description |
|----------|-------------|
| `vecgen_ins_element()` | Insert element from vector lane (INS Vd[i], Vn[j]) |
| `vecgen_ins_general()` | Insert from general register (INS Vd[i], Rn) |
| `vecgen_umov_sized()` | Move vector lane to general register (UMOV Rd, Vn[i]) |
| `vecgen_mov_element()` | Move vector element (MOV Rd, Vn[i]) |
| `vecgen_dup_general()` | Duplicate general register to vector (DUP Vd.T, Rn) |
| `vecgen_dup_scalar()` | Duplicate scalar to vector (DUP Vd.T, Vn[i]) |

### Memory Operations for Q Registers
| Function | Description |
|----------|-------------|
| `vecgen_ldr_q()` | Load Q register (LDR Qt, [Xn, #offset]) |
| `vecgen_str_q()` | Store Q register (STR Qt, [Xn, #offset]) |

### FMOV Operations for Vector Registers
| Function | Description |
|----------|-------------|
| `vecgen_fmov_x_to_d()` | Move X register to D register (FMOV Dd, Xn) |
| `vecgen_fmov_d_to_x()` | Move D register to X register (FMOV Xd, Dn) |
| `vecgen_fmov_w_to_s()` | Move W register to S register (FMOV Sd, Wn) |
| `vecgen_fmov_s_to_w()` | Move S register to W register (FMOV Wd, Sn) |
| `vecgen_fmov_s_lane()` | Move S register from vector lane (FMOV Sd, Vn.S[i]) |
| `vecgen_umov()` | Move vector lane to W register (UMOV Wd, Vn.S[i]) |
| `vecgen_smov()` | Sign-extend move from vector lane (SMOV Wd, Vn.B[i]) |

### Conversion Operations
| Function | Description |
|----------|-------------|
| `vecgen_fcvt_s_to_d()` | Convert single to double precision (FCVT Dd, Sn) |
| `vecgen_fcvt_d_to_s()` | Convert double to single precision (FCVT Sd, Dn) |

## 3. NewCodeGenerator Specialized Functions (`NewBCPL/NewCodeGenerator.cpp`)

### Pairwise Reduction Generators
| Function | Description | Target Type |
|----------|-------------|-------------|
| `generateFPairPairwiseMin()` | FPAIR pairwise minimum using FMINP.2S | FPAIR (2x32-bit float) |
| `generateFQuadPairwiseMin()` | FQUAD component-wise minimum using FMIN.4S | FQUAD (4x32-bit float) |
| `generateFOctPairwiseMin()` | FOCT pairwise minimum using FMINP.4S | FOCT (8x32-bit float) |
| `generateFloatVectorPairwiseMin()` | Float vector pairwise minimum | FVEC |
| `generatePairPairwiseMin()` | PAIR pairwise minimum using SMIN.2S | PAIR (2x32-bit int) |
| `generateQuadPairwiseMin()` | QUAD component-wise minimum using SMIN.4S | QUAD (4x32-bit int) |
| `generateOctPairwiseMin()` | OCT component-wise minimum (dual 4S operations) | OCT (8x32-bit int) |
| `generateIntegerVectorPairwiseMin()` | Integer vector pairwise minimum | VEC |

## 4. Other Vector Instruction Usage

### ReductionCodeGen (`NewBCPL/reductions.cpp`)
| Function | Description | Instructions Used |
|----------|-------------|------------------|
| `generatePairsReduction()` | PAIRS vector reduction | `create_ldr_vec_imm()`, `create_str_vec_imm()` |
| `generateStructuredReductionLoop()` | Structured reduction loops | Vector load/store operations |

### Generator Files (`NewBCPL/generators/`)
| File | Function | Usage |
|------|----------|-------|
| `gen_VectorAccess.cpp` | Vector bounds checking | `create_ldr_imm()` for length checks |
| `gen_UnaryOp.cpp` | Vector/table length operations | `create_ldr_imm()` for length retrieval |

## Issues & Cleanup Opportunities

### 1. Duplicate Functionality
- **Problem**: Two separate sets of vector arithmetic encoders exist
  - Main encoders in `encoders/` directory (standard approach)
  - Custom `vecgen_*` methods in `VectorCodeGen` class (specialized approach)
- **Impact**: Code duplication, maintenance overhead, potential inconsistencies

### 2. Inconsistent Naming Conventions
- **Problem**: Three different naming patterns:
  - `create_*` prefix (standard encoders)
  - `enc_create_*` prefix (some encoders)
  - `vecgen_*` prefix (VectorCodeGen methods)
- **Impact**: Confusion, harder to locate functions, inconsistent API

### 3. Scattered Vector Memory Operations
- **Problem**: Multiple implementations for similar functionality:
  - `create_ldr_vec_imm()` and `create_str_vec_imm()` in encoders/
  - `vecgen_ldr_q()` and `vecgen_str_q()` in VectorCodeGen
  - Standard `create_ldr_imm()` used for some vector operations
- **Impact**: Unclear which to use in different contexts

### 4. Missing FQUAD Support
- **Problem**: New FQUAD (4x16-bit float) type lacks dedicated encoders
- **Required**: FQUAD-specific instruction encoders for arithmetic operations
- **Current Workaround**: Using existing 4S operations, but may need 4H support

### 5. Inconsistent Function Signatures
- **Problem**: Some functions use different parameter orders or naming
- **Example**: `enc_create_fsub_vector_reg()` vs `create_fadd_vector_reg()`

## Recommendations for Cleanup

### Phase 1: Consolidation
1. **Standardize Naming**: Choose one naming convention (recommend `create_*`)
2. **Consolidate Encoders**: Move all vector encoders to `encoders/` directory
3. **Remove Duplicates**: Eliminate duplicate functionality between main encoders and VectorCodeGen

### Phase 2: FQUAD Integration
1. **Add FQUAD Encoders**: Create encoders for 4H (4x16-bit) operations
2. **Extend Existing**: Modify current encoders to handle 4H arrangement
3. **Update VectorCodeGen**: Add FQUAD-specific methods

### Phase 3: Organization
1. **Group by Function**: Organize encoders into logical groups (arithmetic, memory, etc.)
2. **Consistent APIs**: Ensure all vector encoders have similar parameter patterns
3. **Documentation**: Add comprehensive documentation for each encoder

### Phase 4: Testing
1. **Validation Suite**: Create tests for all vector instruction encoders
2. **FQUAD Testing**: Specific test cases for new FQUAD operations
3. **Performance Testing**: Ensure consolidated encoders maintain performance

## FQUAD Implementation Notes

For the current FQUAD implementation, the following new encoders may be needed:
- `create_fadd_vector_4h()` - FADD V.4H (if 16-bit float operations needed)
- `create_fsub_vector_4h()` - FSUB V.4H
- `create_fmul_vector_4h()` - FMUL V.4H
- `create_fdiv_vector_4h()` - FDIV V.4H

Alternatively, continue using 4S operations with proper conversion between 16-bit and 32-bit floats.

---

*Generated: [Current Date]*  
*Purpose: Pre-cleanup inventory for FQUAD vector type implementation*
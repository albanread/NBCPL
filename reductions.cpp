#include "reductions.h"
#include "NewCodeGenerator.h"
#include "analysis/ASTAnalyzer.h"
#include "VectorCodeGen.h"
#include <stdexcept>
#include <iostream>

ReductionCodeGen::ReductionCodeGen(RegisterManager& register_manager,
                                   NewCodeGenerator& code_generator,
                                   ASTAnalyzer& analyzer)
    : register_manager_(register_manager),
      code_generator_(code_generator),
      analyzer_(analyzer) {
}

bool ReductionCodeGen::isReductionOperation(const std::string& func_name) {
    return func_name == "MIN" || 
           func_name == "MAX" || 
           func_name == "SUM";
}

void ReductionCodeGen::generateReduction(FunctionCall& node, const std::string& op_name) {
    if (node.arguments.size() != 2) {
        throw std::runtime_error("Reduction operations require exactly 2 arguments");
    }

    // Analyze argument types to determine dispatch
    VarType left_type = analyzer_.getExpressionType(node.arguments[0].get());
    VarType right_type = analyzer_.getExpressionType(node.arguments[1].get());

    // Verify both arguments are the same type
    if (left_type != right_type) {
        throw std::runtime_error("Reduction operation arguments must be the same type");
    }

    // Dispatch based on type
    if (left_type == VarType::PAIRS) {
        if (op_name == "MIN") {
            generatePairsMin(node.arguments);
        } else if (op_name == "MAX") {
            generatePairsMax(node.arguments);
        } else if (op_name == "SUM") {
            generatePairsSum(node.arguments);
        }
    } else if (left_type == VarType::FPAIRS) {
        if (op_name == "MIN") {
            generateFPairsMin(node.arguments);
        } else if (op_name == "MAX") {
            generateFPairsMax(node.arguments);
        } else if (op_name == "SUM") {
            generateFPairsSum(node.arguments);
        }
    } else if (left_type == VarType::VEC) {
        if (op_name == "MIN") {
            generateVecMin(node.arguments);
        } else if (op_name == "MAX") {
            generateVecMax(node.arguments);
        } else if (op_name == "SUM") {
            generateVecSum(node.arguments);
        }
    } else if (left_type == VarType::FVEC) {
        if (op_name == "MIN") {
            generateFVecMin(node.arguments);
        } else if (op_name == "MAX") {
            generateFVecMax(node.arguments);
        } else if (op_name == "SUM") {
            generateFVecSum(node.arguments);
        }
    } else {
        throw std::runtime_error("Unsupported type for reduction operation: " + std::to_string(static_cast<int>(left_type)));
    }
}

// === PAIRS Reductions (Integer) ===

void ReductionCodeGen::generatePairsMin(const std::vector<ExprPtr>& args) {
    // Get operands (must be variables for PAIRS vectors)
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("PAIRS MIN operation requires variable operands");
    }

    // Get vector size and addresses
    size_t vector_size = getVectorSize(args[0].get());
    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);
    std::string result_addr = allocateResultVector(VarType::PAIRS, vector_size);

    // Process 2 PAIRs at a time using 128-bit Q registers
    for (size_t i = 0; i < vector_size; i += 2) {
        size_t offset = i * 8; // Each PAIR = 8 bytes

        // Acquire Q registers for 128-bit NEON operations
        std::string q_left = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_right = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_result = register_manager_.acquire_q_scratch_reg(code_generator_);

        // Convert Q register names to V register names for encoder
        std::string v_left = q_left.substr(1);   // Q5 -> 5
        std::string v_right = q_right.substr(1); // Q6 -> 6
        std::string v_result = q_result.substr(1); // Q7 -> 7

        // Load 2 PAIRs (16 bytes) from each vector
        emit(Encoder::create_ldr_vec_imm("Q" + v_left, left_addr, offset));
        emit(Encoder::create_ldr_vec_imm("Q" + v_right, right_addr, offset));

        // NEON SMIN operation: process 4 × 32-bit components in parallel
        emit(vecgen_smin_4s("V" + v_result, "V" + v_left, "V" + v_right));

        // Store result (2 PAIRs = 16 bytes)
        emit(Encoder::create_str_vec_imm("Q" + v_result, result_addr, offset));

        // Release Q registers
        register_manager_.release_fp_register(q_left);
        register_manager_.release_fp_register(q_right);
        register_manager_.release_fp_register(q_result);
    }

    // Handle remainder PAIR if vector_size is odd (scalar fallback)
    if (vector_size % 2 == 1) {
        size_t last_index = vector_size - 1;
        size_t offset = last_index * 8;

        std::string left_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);
        std::string right_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);
        std::string result_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);

        // Load, compare, and store scalar PAIR
        emit(Encoder::create_ldr_imm(left_pair_reg, left_addr, offset));
        emit(Encoder::create_ldr_imm(right_pair_reg, right_addr, offset));
        
        // Scalar minimum (simplified - needs proper PAIR field handling)
        emit(Encoder::create_cmp_reg(left_pair_reg, right_pair_reg));
        emit(Encoder::create_csel(result_pair_reg, left_pair_reg, right_pair_reg, "LT"));
        
        emit(Encoder::create_str_imm(result_pair_reg, result_addr, offset));

        register_manager_.release_register(left_pair_reg);
        register_manager_.release_register(right_pair_reg);
        register_manager_.release_register(result_pair_reg);
    }

    // Set result register
    code_generator_.expression_result_reg_ = result_addr;
}

void ReductionCodeGen::generatePairsMax(const std::vector<ExprPtr>& args) {
    // Similar to generatePairsMin but uses SMAX instead of SMIN
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("PAIRS MAX operation requires variable operands");
    }

    size_t vector_size = getVectorSize(args[0].get());
    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);
    std::string result_addr = allocateResultVector(VarType::PAIRS, vector_size);

    // Process 2 PAIRs at a time using NEON SMAX
    for (size_t i = 0; i < vector_size; i += 2) {
        size_t offset = i * 8;

        std::string q_left = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_right = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_result = register_manager_.acquire_q_scratch_reg(code_generator_);

        std::string v_left = q_left.substr(1);
        std::string v_right = q_right.substr(1);
        std::string v_result = q_result.substr(1);

        emit(Encoder::create_ldr_vec_imm("Q" + v_left, left_addr, offset));
        emit(Encoder::create_ldr_vec_imm("Q" + v_right, right_addr, offset));

        // NEON SMAX operation
        emit(vecgen_smax_4s("V" + v_result, "V" + v_left, "V" + v_right));

        emit(Encoder::create_str_vec_imm("Q" + v_result, result_addr, offset));

        register_manager_.release_fp_register(q_left);
        register_manager_.release_fp_register(q_right);
        register_manager_.release_fp_register(q_result);
    }

    // Handle remainder (similar scalar fallback but with GT condition)
    if (vector_size % 2 == 1) {
        size_t last_index = vector_size - 1;
        size_t offset = last_index * 8;

        std::string left_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);
        std::string right_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);
        std::string result_pair_reg = register_manager_.acquire_scratch_reg(code_generator_);

        emit(Encoder::create_ldr_imm(left_pair_reg, left_addr, offset));
        emit(Encoder::create_ldr_imm(right_pair_reg, right_addr, offset));
        
        emit(Encoder::create_cmp_reg(left_pair_reg, right_pair_reg));
        emit(Encoder::create_csel(result_pair_reg, left_pair_reg, right_pair_reg, "GT"));
        
        emit(Encoder::create_str_imm(result_pair_reg, result_addr, offset));

        register_manager_.release_register(left_pair_reg);
        register_manager_.release_register(right_pair_reg);
        register_manager_.release_register(result_pair_reg);
    }

    code_generator_.expression_result_reg_ = result_addr;
}

void ReductionCodeGen::generatePairsSum(const std::vector<ExprPtr>& args) {
    // Reuse existing ADD logic from VectorCodeGen - SUM is alias for ADD
    // Could delegate to VectorCodeGen::generatePairsVectorBinaryOp with Add operation
    
    // For now, implement directly using ADD.4S
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("PAIRS SUM operation requires variable operands");
    }

    size_t vector_size = getVectorSize(args[0].get());
    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);
    std::string result_addr = allocateResultVector(VarType::PAIRS, vector_size);

    // Process using existing ADD vector instruction
    for (size_t i = 0; i < vector_size; i += 2) {
        size_t offset = i * 8;

        std::string q_left = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_right = register_manager_.acquire_q_scratch_reg(code_generator_);
        std::string q_result = register_manager_.acquire_q_scratch_reg(code_generator_);

        std::string v_left = q_left.substr(1);
        std::string v_right = q_right.substr(1);
        std::string v_result = q_result.substr(1);

        emit(Encoder::create_ldr_vec_imm("Q" + v_left, left_addr, offset));
        emit(Encoder::create_ldr_vec_imm("Q" + v_right, right_addr, offset));

        // Use existing ADD vector instruction
        emit(Encoder::create_add_vector_reg("V" + v_result, "V" + v_left, "V" + v_right, "4S"));

        emit(Encoder::create_str_vec_imm("Q" + v_result, result_addr, offset));

        register_manager_.release_fp_register(q_left);
        register_manager_.release_fp_register(q_right);
        register_manager_.release_fp_register(q_result);
    }

    code_generator_.expression_result_reg_ = result_addr;
}

// === NEON Instruction Generators ===

Instruction ReductionCodeGen::vecgen_smin_4s(const std::string& vd, 
                                             const std::string& vn, 
                                             const std::string& vm) {
    // SMIN Vd.4S, Vn.4S, Vm.4S
    // Encoding: 0100 1110 1010 0000 0110 11xx xxxx xxxx
    // Base: 0x4EA00000 | size=00 (32-bit) | opcode=011011 (SMIN)
    uint32_t base_opcode = 0x4EA06C00; // SMIN.4S base encoding
    
    // Extract register numbers (assume format "V12" -> 12)
    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));
    
    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    
    return Instruction(instruction, "SMIN " + vd + ".4S, " + vn + ".4S, " + vm + ".4S");
}

Instruction ReductionCodeGen::vecgen_smax_4s(const std::string& vd, 
                                             const std::string& vn, 
                                             const std::string& vm) {
    // SMAX Vd.4S, Vn.4S, Vm.4S  
    // Encoding: 0100 1110 1010 0000 0110 01xx xxxx xxxx
    // Base: 0x4EA00000 | size=00 (32-bit) | opcode=011001 (SMAX)
    uint32_t base_opcode = 0x4EA06400; // SMAX.4S base encoding
    
    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));
    
    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    
    return Instruction(instruction, "SMAX " + vd + ".4S, " + vn + ".4S, " + vm + ".4S");
}

Instruction ReductionCodeGen::vecgen_fmin_4s(const std::string& vd, 
                                             const std::string& vn, 
                                             const std::string& vm) {
    // FMIN Vd.4S, Vn.4S, Vm.4S
    // Floating-point minimum (to be implemented for FPAIRS)
    uint32_t base_opcode = 0x4EA0F400; // FMIN.4S base encoding
    
    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));
    
    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    
    return Instruction(instruction, "FMIN " + vd + ".4S, " + vn + ".4S, " + vm + ".4S");
}

Instruction ReductionCodeGen::vecgen_fmax_4s(const std::string& vd, 
                                             const std::string& vn, 
                                             const std::string& vm) {
    // FMAX Vd.4S, Vn.4S, Vm.4S
    // Floating-point maximum (to be implemented for FPAIRS)
    uint32_t base_opcode = 0x4E20F400; // FMAX.4S base encoding
    
    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));
    
    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    
    return Instruction(instruction, "FMAX " + vd + ".4S, " + vn + ".4S, " + vm + ".4S");
}

// === Placeholder implementations for other vector types ===

void ReductionCodeGen::generateFPairsMin(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FPAIRS MIN not yet implemented");
}

void ReductionCodeGen::generateFPairsMax(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FPAIRS MAX not yet implemented");
}

void ReductionCodeGen::generateFPairsSum(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FPAIRS SUM not yet implemented");
}

void ReductionCodeGen::generateVecMin(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("VEC MIN not yet implemented");
}

void ReductionCodeGen::generateVecMax(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("VEC MAX not yet implemented");
}

void ReductionCodeGen::generateVecSum(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("VEC SUM not yet implemented");
}

void ReductionCodeGen::generateFVecMin(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FVEC MIN not yet implemented");
}

void ReductionCodeGen::generateFVecMax(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FVEC MAX not yet implemented");
}

void ReductionCodeGen::generateFVecSum(const std::vector<ExprPtr>& args) {
    throw std::runtime_error("FVEC SUM not yet implemented");
}

// === Helper Methods ===

size_t ReductionCodeGen::getVectorSize(Expression* expr) {
    // For now, return fixed size - should query symbol table in real implementation
    return 8; // Placeholder
}

std::string ReductionCodeGen::allocateResultVector(VarType vector_type, size_t vector_size) {
    // Use GETVEC to allocate memory for result vector
    std::string size_reg = register_manager_.acquire_scratch_reg(code_generator_);
    
    // Calculate total words needed based on vector type
    size_t total_words;
    if (vector_type == VarType::PAIRS) {
        total_words = vector_size * 2; // 2 words per PAIR
    } else if (vector_type == VarType::FPAIRS) {
        total_words = vector_size * 2; // 2 words per FPAIR  
    } else {
        total_words = vector_size; // 1 word per element for VEC/FVEC
    }
    
    emit(Encoder::create_movz_imm(size_reg, total_words));
    
    // Call GETVEC to allocate memory
    std::string result_reg = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_mov_reg(result_reg, "X0")); // Save X0
    emit(Encoder::create_mov_reg("X0", size_reg));    // Set up GETVEC argument
    emit(Encoder::create_branch_with_link("GETVEC"));
    emit(Encoder::create_mov_reg(size_reg, "X0"));    // Get result
    emit(Encoder::create_mov_reg("X0", result_reg));  // Restore X0
    
    register_manager_.release_register(result_reg);
    return size_reg; // Return register containing allocated address
}

void ReductionCodeGen::emit(const Instruction& instr) {
    code_generator_.emit(instr);
}
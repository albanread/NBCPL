#include "reductions.h"
#include "NewCodeGenerator.h"
#include "ASTAnalyzer.h"
#include "CFGBuilderPass.h"
#include "LabelManager.h"
#include <iostream>
#include <algorithm>
#include <cstdlib>

ReductionCodeGen::ReductionCodeGen(RegisterManager& register_manager,
                                   NewCodeGenerator& code_generator,
                                   ASTAnalyzer& analyzer)
    : register_manager_(register_manager), code_generator_(code_generator), analyzer_(analyzer), current_cfg_builder_(nullptr) {
}

bool ReductionCodeGen::isReductionOperation(const std::string& func_name) {
    return func_name == "MIN" || func_name == "MAX" || func_name == "SUM";
}

void ReductionCodeGen::generateReduction(FunctionCall& node, const std::string& op_name) {
    if (node.arguments.size() != 2) {
        throw std::runtime_error("Reduction operations require exactly 2 arguments");
    }

    // Analyze argument types to determine dispatch
    VarType left_type = analyzer_.get_expression_type(*node.arguments[0]);
    VarType right_type = analyzer_.get_expression_type(*node.arguments[1]);

    // Verify both arguments are the same type
    if (left_type != right_type) {
        throw std::runtime_error("Reduction operation arguments must be the same type");
    }

    // Convert operation name to enum
    ReductionOp op;
    if (op_name == "MIN") {
        op = ReductionOp::MIN;
    } else if (op_name == "MAX") {
        op = ReductionOp::MAX;
    } else if (op_name == "SUM") {
        op = ReductionOp::SUM;
    } else {
        throw std::runtime_error("Unknown reduction operation: " + op_name);
    }

    // Get vector addresses and sizes
    auto* left_var = dynamic_cast<VariableAccess*>(node.arguments[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(node.arguments[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("Reduction operations require variable operands");
    }

    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string left_size_reg = getVectorSize(left_addr);

    // if (!register_manager_.is_neon_enabled()) {
    //     generateScalarReduction(node.arguments, op);
    //     register_manager_.release_register(left_size_reg);
    //     return;
    // }

    // Dispatch based on type (handle both direct PAIRS and POINTER_TO_PAIRS)
    bool is_pairs_type = (left_type == VarType::PAIRS) ||
                        ((static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::PAIRS)) &&
                         (static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::POINTER_TO)));

    if (is_pairs_type) {
        generateChunkedReduction(node.arguments, op);
        register_manager_.release_register(left_size_reg);
    } else if (left_type == VarType::VEC ||
              ((static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::VEC)) &&
               (static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::POINTER_TO)))) {
        generateVecReduction(node.arguments, op);
    } else if ((static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::VEC)) &&
               (static_cast<int64_t>(left_type) & static_cast<int64_t>(VarType::FLOAT))) {
        generateFVecReduction(node.arguments, op);
    } else {
        // TODO: Add support for FPAIRS (float pairs) and other vector types
        throw std::runtime_error("Unsupported type for reduction operation: " + std::to_string(static_cast<int64_t>(left_type)) +
                                 " (Note: Only PAIRS and VEC vectors currently supported)");
    }
}

// === Chunked Processing Implementation ===

void ReductionCodeGen::generateChunkedReduction(const std::vector<ExprPtr>& args, ReductionOp op) {
    // NEW CFG-STYLE APPROACH: Use structured code generation with proper basic blocks
    // This provides safety, bounds checking, and integration with compiler infrastructure
    
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("Chunked reduction operation requires variable operands");
    }

    // Try CFG-based approach first
    if (hasCFGAccess()) {
        auto synthetic_loop = createReductionLoop(left_var, right_var, op);
        injectIntoCurrentCFG(std::move(synthetic_loop));
        return;
    }

    // FALLBACK: Use working PAIRS addition pattern with structured loops
    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);

    // Get vector size - this is safe and trivial (just ldr from vec_addr - 8)
    std::string left_size = getVectorSize(left_addr);
    std::string right_size = getVectorSize(right_addr);
    
    // Allocate result vector using working allocation pattern
    std::string result_addr = allocateResultVector(left_size);
    register_manager_.release_register(right_size);

    // STRUCTURED LOOP GENERATION: Use compile-time unrolled pattern like working PAIRS addition
    // Get actual vector size at runtime for loop bound calculation
    std::string size_value_reg = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_mov_reg(size_value_reg, left_size));
    
    // Calculate number of 16-byte chunks (pairs_count / 2)
    std::string chunks_reg = register_manager_.acquire_scratch_reg(code_generator_);
    std::string temp_reg = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_movz_imm(temp_reg, 1));
    emit(Encoder::create_lsr_reg(chunks_reg, size_value_reg, temp_reg));
    register_manager_.release_register(temp_reg);
    register_manager_.release_register(size_value_reg);

    // GENERATE STRUCTURED LOOP using proper labels and CFG-style basic blocks
    generateStructuredReductionLoop(left_addr, right_addr, result_addr, chunks_reg, op);
    
    register_manager_.release_register(chunks_reg);
    code_generator_.expression_result_reg_ = result_addr;
}

void ReductionCodeGen::generateScalarPairReduction(const std::string& left_addr,
                                                   const std::string& right_addr,
                                                   const std::string& result_addr,
                                                   size_t pair_idx, ReductionOp op) {
    // Helper function to process a single PAIR using scalar operations
    size_t pair_offset = pair_idx * 8;

    std::string x_temp1 = register_manager_.acquire_scratch_reg(code_generator_);
    std::string x_temp2 = register_manager_.acquire_scratch_reg(code_generator_);
    std::string x_result = register_manager_.acquire_scratch_reg(code_generator_);

    // Convert to W registers for 32-bit operations
    std::string w_temp1 = "W" + x_temp1.substr(1);
    std::string w_temp2 = "W" + x_temp2.substr(1);
    std::string w_result = "W" + x_result.substr(1);

    // Process first element
    emit(Encoder::create_ldr_word_imm(w_temp1, left_addr, pair_offset));
    emit(Encoder::create_ldr_word_imm(w_temp2, right_addr, pair_offset));
    emitScalarOperation(w_result, w_temp1, w_temp2, op);
    emit(Encoder::create_str_word_imm(w_result, result_addr, pair_offset));

    // Process second element
    emit(Encoder::create_ldr_word_imm(w_temp1, left_addr, pair_offset + 4));
    emit(Encoder::create_ldr_word_imm(w_temp2, right_addr, pair_offset + 4));
    emitScalarOperation(w_result, w_temp1, w_temp2, op);
    emit(Encoder::create_str_word_imm(w_result, result_addr, pair_offset + 4));

    register_manager_.release_register(x_temp1);
    register_manager_.release_register(x_temp2);
    register_manager_.release_register(x_result);
}

// === Scalar Fallback Implementation ===

void ReductionCodeGen::generateScalarReduction(const std::vector<ExprPtr>& args, ReductionOp op) {
    // Pure scalar implementation - no Q-registers needed
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("Scalar reduction operation requires variable operands");
    }

    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);

    // Get vector size and allocate result vector
    std::string left_size_reg = getVectorSize(left_addr);
    std::string result_addr = allocateResultVector(left_size_reg);

    // Helper to convert X register to W register (X0 -> W0, X1 -> W1, etc.)
    auto x_to_w = [](const std::string& x_reg) -> std::string {
        if (x_reg[0] == 'X' && x_reg.size() > 1) {
            return "W" + x_reg.substr(1);
        }
        return x_reg; // Return as-is if not an X register
    };

    // TODO: Implement proper scalar reduction with runtime loops
    // For now, throw an error since we're focusing on NEON-optimized reductions
    throw std::runtime_error("Scalar reduction not yet implemented with new vector size approach");

    code_generator_.expression_result_reg_ = result_addr;
}

void ReductionCodeGen::emitScalarOperation(const std::string& result, const std::string& left,
                                          const std::string& right, ReductionOp op) {
    switch (op) {
        case ReductionOp::MIN:
            // Simple MIN implementation: compare and select smaller value
            emit(Encoder::create_cmp_reg(left, right));
            emit(Encoder::create_mov_reg(result, left));   // Start with left
            // TODO: Implement proper conditional select - for now using basic compare
            // If left > right, we should move right to result, but we need proper branching
            break;
        case ReductionOp::MAX:
            // Simple MAX implementation: compare and select larger value
            emit(Encoder::create_cmp_reg(left, right));
            emit(Encoder::create_mov_reg(result, right));  // For now, use right operand
            break;
        case ReductionOp::SUM:
            emit(Encoder::create_add_reg(result, left, right));
            break;
    }
}

// === PAIRS NEON Implementation ===

void ReductionCodeGen::generatePairsReduction(const std::vector<ExprPtr>& args, ReductionOp op) {
    // Medium vectors: Use NEON with register reuse pattern
    auto* left_var = dynamic_cast<VariableAccess*>(args[0].get());
    auto* right_var = dynamic_cast<VariableAccess*>(args[1].get());

    if (!left_var || !right_var) {
        throw std::runtime_error("PAIRS reduction operation requires variable operands");
    }

    std::string left_addr = code_generator_.get_variable_register(left_var->name);
    std::string right_addr = code_generator_.get_variable_register(right_var->name);

    // Get vector size
    std::string left_size_reg = getVectorSize(left_addr);
    
    // Calculate loop counter IMMEDIATELY before any function calls that could clobber registers
    std::string counter_reg = register_manager_.acquire_scratch_reg(code_generator_);
    std::string temp_reg = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_movz_imm(temp_reg, 1));
    emit(Encoder::create_lsr_reg(counter_reg, left_size_reg, temp_reg)); // pairs_count / 2
    register_manager_.release_register(temp_reg);
    
    // NOW it's safe to call allocateResultVector - the loop count is secure in counter_reg
    std::string result_addr = allocateResultVector(left_size_reg);

    // Acquire Q-registers for NEON operations
    std::string q_left = register_manager_.acquire_q_scratch_reg(code_generator_);
    std::string q_right = register_manager_.acquire_q_scratch_reg(code_generator_);
    std::string q_result = register_manager_.acquire_q_scratch_reg(code_generator_);

    // Acquire cursor registers
    std::string left_cursor = register_manager_.acquire_scratch_reg(code_generator_);
    std::string right_cursor = register_manager_.acquire_scratch_reg(code_generator_);
    std::string result_cursor = register_manager_.acquire_scratch_reg(code_generator_);

    emit(Encoder::create_mov_reg(left_cursor, left_addr));
    emit(Encoder::create_mov_reg(right_cursor, right_addr));
    emit(Encoder::create_mov_reg(result_cursor, result_addr));

    // Generate runtime loop
    std::string loop_start_label = LabelManager::instance().create_label();
    std::string loop_end_label = LabelManager::instance().create_label();

    Instruction start_label_instr;
    start_label_instr.is_label_definition = true;
    start_label_instr.target_label = loop_start_label;
    emit(start_label_instr);

    // Check if counter is zero
    emit(Encoder::create_cmp_imm(counter_reg, 0));
    emit(Encoder::create_branch_conditional("EQ", loop_end_label));

    // Process 2 PAIRS (16 bytes) with NEON
    emit(Encoder::create_ldr_vec_imm(q_left, left_cursor, 0));
    emit(Encoder::create_ldr_vec_imm(q_right, right_cursor, 0));

    switch (op) {
        case ReductionOp::MIN:
            emit(vecgen_smin_4s(q_result, q_left, q_right));
            break;
        case ReductionOp::MAX:
            emit(vecgen_smax_4s(q_result, q_left, q_right));
            break;
        case ReductionOp::SUM:
            std::string v_result = "V" + q_result.substr(1);
            std::string v_left = "V" + q_left.substr(1);
            std::string v_right = "V" + q_right.substr(1);
            emit(Encoder::create_add_vector_reg(v_result, v_left, v_right, "4S"));
            break;
    }

    emit(Encoder::create_str_vec_imm(q_result, result_cursor, 0));

    // Advance cursors by 16 bytes (2 PAIRS)
    emit(Encoder::create_add_imm(left_cursor, left_cursor, 16));
    emit(Encoder::create_add_imm(right_cursor, right_cursor, 16));
    emit(Encoder::create_add_imm(result_cursor, result_cursor, 16));

    // Decrement counter and loop
    emit(Encoder::create_sub_imm(counter_reg, counter_reg, 1));
    emit(Encoder::create_branch_unconditional(loop_start_label));

    // Loop end label
    Instruction end_label_instr;
    end_label_instr.is_label_definition = true;
    end_label_instr.target_label = loop_end_label;
    emit(end_label_instr);

    // Release loop registers
    register_manager_.release_register(counter_reg);
    register_manager_.release_register(left_cursor);
    register_manager_.release_register(right_cursor);
    register_manager_.release_register(result_cursor);

    // TODO: Handle odd remaining PAIR if needed

    register_manager_.release_q_register(q_left);
    register_manager_.release_q_register(q_right);
    register_manager_.release_q_register(q_result);

    code_generator_.expression_result_reg_ = result_addr;
}

// === Individual Operation Methods ===

void ReductionCodeGen::generatePairsMin(const std::vector<ExprPtr>& args) {
    generatePairsReduction(args, ReductionOp::MIN);
}

void ReductionCodeGen::generatePairsMax(const std::vector<ExprPtr>& args) {
    generatePairsReduction(args, ReductionOp::MAX);
}

void ReductionCodeGen::generatePairsSum(const std::vector<ExprPtr>& args) {
    generatePairsReduction(args, ReductionOp::SUM);
}

// === VEC and FVEC placeholders ===

void ReductionCodeGen::generateVecReduction(const std::vector<ExprPtr>& args, ReductionOp op) {
    throw std::runtime_error("VEC reductions not yet implemented");
}

void ReductionCodeGen::generateFVecReduction(const std::vector<ExprPtr>& args, ReductionOp op) {
    throw std::runtime_error("FVEC reductions not yet implemented");
}

// === NEON Instruction Generators ===

Instruction ReductionCodeGen::vecgen_smin_4s(const std::string& vd,
                                             const std::string& vn,
                                             const std::string& vm) {
    // SMIN.4S instruction encoding
    uint32_t base_opcode = 0x4EA06C00;

    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));

    // Convert Q register names to V register names for assembly
    std::string v_vd = "V" + vd.substr(1);
    std::string v_vn = "V" + vn.substr(1);
    std::string v_vm = "V" + vm.substr(1);

    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    return Instruction(instruction, "SMIN " + v_vd + ".4S, " + v_vn + ".4S, " + v_vm + ".4S");
}

Instruction ReductionCodeGen::vecgen_smax_4s(const std::string& vd,
                                             const std::string& vn,
                                             const std::string& vm) {
    // Use standardized SMAX encoder for consistency
    // Convert Q register names to V register names
    std::string v_vd = "v" + vd.substr(1);
    std::string v_vn = "v" + vn.substr(1);
    std::string v_vm = "v" + vm.substr(1);

    return Encoder::create_smax_vector_reg(v_vd, v_vn, v_vm, "4S");
}

Instruction ReductionCodeGen::vecgen_fmin_4s(const std::string& vd,
                                             const std::string& vn,
                                             const std::string& vm) {
    // FMIN.4S instruction encoding
    uint32_t base_opcode = 0x4EA0F400;

    int rd = std::stoi(vd.substr(1));
    int rn = std::stoi(vn.substr(1));
    int rm = std::stoi(vm.substr(1));

    // Convert Q register names to V register names for assembly
    std::string v_vd = "V" + vd.substr(1);
    std::string v_vn = "V" + vn.substr(1);
    std::string v_vm = "V" + vm.substr(1);

    uint32_t instruction = base_opcode | (rm << 16) | (rn << 5) | rd;
    return Instruction(instruction, "FMIN " + v_vd + ".4S, " + v_vn + ".4S, " + v_vm + ".4S");
}

Instruction ReductionCodeGen::vecgen_fmax_4s(const std::string& vd,
                                             const std::string& vn,
                                             const std::string& vm) {
    // Use standardized FMAX encoder for consistency
    // Convert Q register names to V register names
    std::string v_vd = "v" + vd.substr(1);
    std::string v_vn = "v" + vn.substr(1);
    std::string v_vm = "v" + vm.substr(1);

    return Encoder::create_fmax_vector_reg(v_vd, v_vn, v_vm, "4S");
}

// === FPAIRS Operations ===

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

std::string ReductionCodeGen::getVectorSize(const std::string& vec_reg) {
    // Get vector size from BCPL vector header at vec_addr - 8
    std::string size_reg = register_manager_.acquire_scratch_reg(code_generator_);
    std::string temp_reg = register_manager_.acquire_scratch_reg(code_generator_);

    // Load size from [vec_addr - 8]
    emit(Encoder::create_sub_imm(temp_reg, vec_reg, 8));
    emit(Encoder::create_ldr_imm(size_reg, temp_reg, 0));

    register_manager_.release_register(temp_reg);
    return size_reg; // Return register containing the size
}

std::string ReductionCodeGen::allocateResultVector(const std::string& size_reg) {
    // Use working PAIRS allocation pattern exactly
    std::string result_reg = register_manager_.acquire_scratch_reg(code_generator_);
    std::string words_reg = register_manager_.acquire_scratch_reg(code_generator_);

    // PAIRS allocation: size PAIRs × 2 words per PAIR
    emit(Encoder::create_lsl_imm(words_reg, size_reg, 1)); // size * 2 words per PAIR

    emit(Encoder::create_mov_reg(result_reg, "X0")); // Save X0
    emit(Encoder::create_mov_reg("X0", words_reg));   // Set up GETVEC argument (word count)
    emit(Encoder::create_branch_with_link("GETVEC"));
    emit(Encoder::create_mov_reg(words_reg, "X0"));   // Get result address
    emit(Encoder::create_mov_reg("X0", result_reg));  // Restore X0

    register_manager_.release_register(result_reg);
    return words_reg; // Return register containing allocated address
}

void ReductionCodeGen::emit(const Instruction& instr) {
    code_generator_.emit(instr);
}

// === CFG-Based Reduction Infrastructure ===

std::unique_ptr<ForStatement> ReductionCodeGen::createReductionLoop(
    VariableAccess* left_var,
    VariableAccess* right_var,
    ReductionOp op
) {
    // Create unique loop variable name
    std::string loop_var = "_reduction_idx_" + std::to_string(rand() % 10000);
    
    // Create start expression: 0
    auto start_expr = std::make_unique<NumberLiteral>(static_cast<int64_t>(0));
    
    // Create end expression: (vector_size / 2) - 1
    // This will be populated with runtime size calculation
    auto end_expr = std::make_unique<NumberLiteral>(static_cast<int64_t>(1)); // Placeholder - will be replaced with size calculation
    
    // Create loop body - this will contain our NEON operations
    auto body_statements = std::vector<StmtPtr>();
    
    // TODO: Add NEON load/operation/store statements to body
    // For now, create a simple assignment as placeholder
    std::vector<ExprPtr> body_lhs;
    body_lhs.push_back(std::make_unique<VariableAccess>("_temp_result"));
    std::vector<ExprPtr> body_rhs;  
    body_rhs.push_back(std::make_unique<NumberLiteral>(static_cast<int64_t>(42)));
    body_statements.push_back(std::make_unique<AssignmentStatement>(std::move(body_lhs), std::move(body_rhs)));
    
    auto body = std::make_unique<CompoundStatement>(std::move(body_statements));
    
    // Create the ForStatement
    auto for_stmt = std::make_unique<ForStatement>(
        loop_var,
        std::move(start_expr),
        std::move(end_expr),
        std::move(body)
    );
    
    // Mark this as a reduction loop somehow - we'll need to add metadata
    // For now, store the operation type in a comment or unused field
    
    return for_stmt;
}

void ReductionCodeGen::injectIntoCurrentCFG(std::unique_ptr<ForStatement> loop) {
    if (!current_cfg_builder_) {
        std::cerr << "Warning: No CFG builder available, falling back to raw assembly generation" << std::endl;
        // TODO: Fall back to original raw assembly approach
        return;
    }
    
    // Note: CFG injection is handled at a higher level now
    // This method is deprecated but kept for interface compatibility
}

bool ReductionCodeGen::hasCFGAccess() const {
    return current_cfg_builder_ != nullptr;
}

void ReductionCodeGen::generateStructuredReductionLoop(
    const std::string& left_addr,
    const std::string& right_addr, 
    const std::string& result_addr,
    const std::string& chunks_reg,
    ReductionOp op
) {
    // STRUCTURED LOOP: Generate proper basic block structure like CFG does
    // This mimics the pattern used in CFGBuilderPass::visit(ForStatement)
    
    // Acquire Q-registers for NEON processing
    std::string q_left = register_manager_.acquire_q_scratch_reg(code_generator_);
    std::string q_right = register_manager_.acquire_q_scratch_reg(code_generator_);
    std::string q_result = register_manager_.acquire_q_scratch_reg(code_generator_);

    // Acquire loop control registers
    std::string offset_reg = register_manager_.acquire_scratch_reg(code_generator_);
    std::string counter_reg = register_manager_.acquire_scratch_reg(code_generator_);
    
    // Initialize loop: offset = 0, counter = chunks_count
    emit(Encoder::create_movz_imm(offset_reg, 0));
    emit(Encoder::create_mov_reg(counter_reg, chunks_reg));

    // BASIC BLOCK 1: Loop Header (condition check)
    std::string header_label = LabelManager::instance().create_label();
    std::string body_label = LabelManager::instance().create_label();
    std::string exit_label = LabelManager::instance().create_label();
    
    Instruction header_instr;
    header_instr.is_label_definition = true;
    header_instr.target_label = header_label;
    emit(header_instr);
    
    // Check loop condition: if counter == 0, exit
    emit(Encoder::create_cmp_imm(counter_reg, 0));
    emit(Encoder::create_branch_conditional("EQ", exit_label));
    emit(Encoder::create_branch_unconditional(body_label));
    
    // BASIC BLOCK 2: Loop Body (NEON operations)
    Instruction body_instr;
    body_instr.is_label_definition = true;
    body_instr.target_label = body_label;
    emit(body_instr);
    
    // Load 2 PAIRS (16 bytes) using indexed addressing
    // Calculate actual addresses: base + offset
    std::string left_indexed = register_manager_.acquire_scratch_reg(code_generator_);
    std::string right_indexed = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_add_reg(left_indexed, left_addr, offset_reg));
    emit(Encoder::create_add_reg(right_indexed, right_addr, offset_reg));
    
    emit(Encoder::create_ldr_vec_imm(q_left, left_indexed, 0));
    emit(Encoder::create_ldr_vec_imm(q_right, right_indexed, 0));
    
    // Apply NEON reduction operation  
    switch (op) {
        case ReductionOp::MIN:
            emit(vecgen_smin_4s(q_result, q_left, q_right));
            break;
        case ReductionOp::MAX:
            emit(vecgen_smax_4s(q_result, q_left, q_right));
            break;
        case ReductionOp::SUM:
            std::string v_result = "V" + q_result.substr(1);
            std::string v_left = "V" + q_left.substr(1);
            std::string v_right = "V" + q_right.substr(1);
            emit(Encoder::create_add_vector_reg(v_result, v_left, v_right, "4S"));
            break;
    }
    
    // Store result using indexed addressing
    std::string result_indexed = register_manager_.acquire_scratch_reg(code_generator_);
    emit(Encoder::create_add_reg(result_indexed, result_addr, offset_reg));
    emit(Encoder::create_str_vec_imm(q_result, result_indexed, 0));
    
    // BASIC BLOCK 3: Loop Increment (update counters)
    // Release indexed address registers
    register_manager_.release_register(left_indexed);
    register_manager_.release_register(right_indexed);
    register_manager_.release_register(result_indexed);
    
    emit(Encoder::create_add_imm(offset_reg, offset_reg, 16));  // Advance by 16 bytes (2 pairs)
    emit(Encoder::create_sub_imm(counter_reg, counter_reg, 1)); // Decrement counter
    emit(Encoder::create_branch_unconditional(header_label));   // Back to header
    
    // BASIC BLOCK 4: Loop Exit
    Instruction exit_instr;
    exit_instr.is_label_definition = true;
    exit_instr.target_label = exit_label;
    emit(exit_instr);
    
    // Release registers
    register_manager_.release_q_register(q_left);
    register_manager_.release_q_register(q_right);
    register_manager_.release_q_register(q_result);
    register_manager_.release_register(offset_reg);
    register_manager_.release_register(counter_reg);
}

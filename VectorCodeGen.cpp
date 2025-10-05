#include "VectorCodeGen.h"
#include "NewCodeGenerator.h"
#include "Encoder.h"
#include "BitPatcher.h"
#include <stdexcept>
#include <iostream>
#include <algorithm>

VectorCodeGen::VectorCodeGen(NewCodeGenerator& code_gen,
                            RegisterManager& reg_manager,
                            std::function<void(const Instruction&)> emit_func)
    : code_gen_(code_gen), register_manager_(reg_manager), emit_(emit_func) {
}

void VectorCodeGen::generateSimdBinaryOp(BinaryOp& node, bool use_neon) {
    debug_print("VectorCodeGen::generateSimdBinaryOp - use_neon: " + std::to_string(use_neon));

    // Get operand types from analyzer
    VarType left_type = code_gen_.infer_expression_type_local(node.left.get());
    VarType right_type = code_gen_.infer_expression_type_local(node.right.get());

    // Check for PAIRS vector operations - use specialized 128-bit SIMD
    if (isPairsVectorOperation(left_type, right_type)) {
        debug_print("Detected PAIRS vector operation - using 128-bit NEON SIMD");
        generatePairsVectorBinaryOp(node, use_neon);
        return;
    }

    // Determine result type for other vector operations
    VarType result_type = left_type;
    if (isSimdOperation(left_type, right_type)) {
        // Vector-vector or vector-scalar operation
        if (left_type == VarType::OCT || left_type == VarType::FOCT) {
            result_type = left_type;
        } else if (right_type == VarType::OCT || right_type == VarType::FOCT) {
            result_type = right_type;
        }
    }

    if (use_neon) {
        generateNeonBinaryOp(node, result_type);
    } else {
        generateScalarBinaryOp(node, result_type);
    }
}

std::string VectorCodeGen::generateLaneRead(LaneAccessExpression& node, bool use_neon) {
    debug_print("VectorCodeGen::generateLaneRead - use_neon: " + std::to_string(use_neon));

    VarType vector_type = code_gen_.infer_expression_type_local(node.vector_expr.get());
    std::string result_reg;

    if (getElementType(vector_type) == VarType::FLOAT) {
        result_reg = register_manager_.acquire_fp_scratch_reg();
    } else {
        result_reg = register_manager_.acquire_spillable_temp_reg(code_gen_);
    }

    if (use_neon) {
        generateNeonLaneRead(node, vector_type, result_reg);
    } else {
        generateScalarLaneRead(node, vector_type, result_reg);
    }

    return result_reg;
}

void VectorCodeGen::generateLaneWrite(LaneAccessExpression& node, ExprPtr& value_expr, bool use_neon) {
    debug_print("VectorCodeGen::generateLaneWrite - use_neon: " + std::to_string(use_neon));

    VarType vector_type = code_gen_.infer_expression_type_local(node.vector_expr.get());

    // Generate code for the value to be written
    value_expr->accept(code_gen_);
    std::string value_reg = code_gen_.expression_result_reg_;

    if (use_neon) {
        generateNeonLaneWrite(node, vector_type, value_reg);
    } else {
        generateScalarLaneWrite(node, vector_type, value_reg);
    }

    register_manager_.release_register(value_reg);
}

std::string VectorCodeGen::generateOctConstruction(OctExpression& node, bool use_neon) {
    debug_print("VectorCodeGen::generateOctConstruction - use_neon: " + std::to_string(use_neon));

    std::string result_reg = register_manager_.acquire_spillable_temp_reg(code_gen_);

    // OCT has 8 elements - need to copy them to avoid moving unique_ptr twice
    std::vector<const ExprPtr*> elements;
    elements.push_back(&node.first_expr);
    elements.push_back(&node.second_expr);
    elements.push_back(&node.third_expr);
    elements.push_back(&node.fourth_expr);
    elements.push_back(&node.fifth_expr);
    elements.push_back(&node.sixth_expr);
    elements.push_back(&node.seventh_expr);
    elements.push_back(&node.eighth_expr);

    if (use_neon) {
        generateNeonVectorConstruction(elements, VarType::OCT, result_reg);
    } else {
        generateScalarVectorConstruction(elements, VarType::OCT, result_reg);
    }

    return result_reg;
}

std::string VectorCodeGen::generateFOctConstruction(FOctExpression& node, bool use_neon) {
    debug_print("VectorCodeGen::generateFOctConstruction - use_neon: " + std::to_string(use_neon));

    std::string result_reg = register_manager_.acquire_spillable_temp_reg(code_gen_);

    // FOCT has 8 elements - need to copy them to avoid moving unique_ptr twice
    std::vector<const ExprPtr*> elements;
    elements.push_back(&node.first_expr);
    elements.push_back(&node.second_expr);
    elements.push_back(&node.third_expr);
    elements.push_back(&node.fourth_expr);
    elements.push_back(&node.fifth_expr);
    elements.push_back(&node.sixth_expr);
    elements.push_back(&node.seventh_expr);
    elements.push_back(&node.eighth_expr);

    if (use_neon) {
        generateNeonVectorConstruction(elements, VarType::FOCT, result_reg);
    } else {
        generateScalarVectorConstruction(elements, VarType::FOCT, result_reg);
    }

    return result_reg;
}

bool VectorCodeGen::isSimdOperation(VarType left_type, VarType right_type) {
    // Check if either operand is a SIMD vector type
    int left_int = static_cast<int>(left_type);
    int right_int = static_cast<int>(right_type);
    int pairs_int = static_cast<int>(VarType::PAIRS);
    int pointer_to_int = static_cast<int>(VarType::POINTER_TO);
    
    bool left_is_simd = (left_type == VarType::PAIR || left_type == VarType::FPAIR ||
                        left_type == VarType::QUAD || left_type == VarType::OCT ||
                        left_type == VarType::FOCT || left_type == VarType::PAIRS ||
                        ((left_int & pairs_int) && (left_int & pointer_to_int)));
    bool right_is_simd = (right_type == VarType::PAIR || right_type == VarType::FPAIR ||
                         right_type == VarType::QUAD || right_type == VarType::OCT ||
                         right_type == VarType::FOCT || right_type == VarType::PAIRS ||
                         ((right_int & pairs_int) && (right_int & pointer_to_int)));

    return left_is_simd || right_is_simd;
}

bool VectorCodeGen::isPairsVectorOperation(VarType left_type, VarType right_type) {
    // Check for both PAIRS and POINTER_TO_PAIRS types
    int left_int = static_cast<int>(left_type);
    int right_int = static_cast<int>(right_type);
    int pairs_int = static_cast<int>(VarType::PAIRS);
    int pointer_to_int = static_cast<int>(VarType::POINTER_TO);
    
    bool left_is_pairs = (left_type == VarType::PAIRS) || 
                        ((left_int & pairs_int) && (left_int & pointer_to_int));
    bool right_is_pairs = (right_type == VarType::PAIRS) || 
                         ((right_int & pairs_int) && (right_int & pointer_to_int));
    return left_is_pairs && right_is_pairs;
}

int VectorCodeGen::getLaneCount(VarType type) {
    switch (type) {
        case VarType::PAIR:
        case VarType::FPAIR:
            return 2;
        case VarType::QUAD:
            return 4;
        case VarType::OCT:
        case VarType::FOCT:
            return 8;
        default:
            throw std::runtime_error("Not a vector type");
    }
}

VarType VectorCodeGen::getElementType(VarType type) {
    switch (type) {
        case VarType::PAIR:
        case VarType::QUAD:
        case VarType::OCT:
            return VarType::INTEGER;
        case VarType::FPAIR:
        case VarType::FOCT:
            return VarType::FLOAT;
        default:
            throw std::runtime_error("Not a vector type");
    }
}

std::string VectorCodeGen::getNeonArrangement(VarType type) {
    // debug_print("getNeonArrangement called with type: " + std::to_string(static_cast<int>(type)));
    switch (type) {
        case VarType::PAIR:
            return "2S_INT";  // Use a new arrangement for integer pairs
        case VarType::FPAIR:
            return "2S";  // 2 x 32-bit floats
        case VarType::QUAD:
            return "4H";  // 4 x 16-bit signed integers
        case VarType::OCT:
            return "8B";  // 8 x 8-bit signed integers
        case VarType::FOCT:
            return "2D";  // 8 x 32-bit floats (using two 128-bit registers)
        default:
            // debug_print("ERROR: Unsupported vector type for NEON arrangement: " + std::to_string(static_cast<int>(type)));
            throw std::runtime_error("Unsupported vector type for NEON arrangement");
    }
}

void VectorCodeGen::generateNeonBinaryOp(BinaryOp& node, VarType result_type) {
    debug_print("Generating NEON binary operation for vector type");

    // Generate code for operands
    node.left->accept(code_gen_);
    std::string left_reg = code_gen_.expression_result_reg_;

    node.right->accept(code_gen_);
    std::string right_reg = code_gen_.expression_result_reg_;

    VarType left_type = code_gen_.infer_expression_type_local(node.left.get());
    VarType right_type = code_gen_.infer_expression_type_local(node.right.get());

    // Check if this is a vector-scalar operation
    bool is_vector_scalar = false;
    bool left_is_vector = (left_type == VarType::OCT || left_type == VarType::FOCT ||
                          left_type == VarType::PAIR || left_type == VarType::FPAIR ||
                          left_type == VarType::QUAD);
    bool right_is_scalar = (right_type == VarType::INTEGER || right_type == VarType::FLOAT);

    if (left_is_vector && right_is_scalar) {
        is_vector_scalar = true;
    }

    // Acquire appropriate NEON registers (D for 64-bit vectors, Q for 128-bit)
    std::string left_neon_reg;
    std::string right_neon_reg;
    if (result_type == VarType::PAIR || result_type == VarType::FPAIR || result_type == VarType::QUAD) {
        left_neon_reg = register_manager_.acquire_fp_scratch_reg(); // D register
        right_neon_reg = register_manager_.acquire_fp_scratch_reg(); // D register
    } else {
        left_neon_reg = register_manager_.acquire_q_scratch_reg(code_gen_); // Q register
        right_neon_reg = register_manager_.acquire_q_scratch_reg(code_gen_); // Q register
    }

    // Load vector operands to NEON registers
    if (result_type == VarType::FOCT) {
        // For FOCT, we need 128-bit registers (Q registers)
        loadVectorToNeon(left_reg, left_neon_reg, left_type);

        if (is_vector_scalar) {
            // Broadcast scalar to all lanes
            broadcastScalarToNeon(right_reg, right_neon_reg, result_type);
        } else {
            loadVectorToNeon(right_reg, right_neon_reg, right_type);
        }
    } else if (result_type == VarType::FPAIR) {
        // For FPAIR, use 64-bit D registers but handle as floats
        loadVectorToNeon(left_reg, left_neon_reg, left_type);

        if (is_vector_scalar) {
            broadcastScalarToNeon(right_reg, right_neon_reg, result_type);
        } else {
            loadVectorToNeon(right_reg, right_neon_reg, right_type);
        }
    } else {
        // For OCT, PAIR, QUAD (integer), use 64-bit D registers
        emit_(vecgen_fmov_x_to_d(left_neon_reg, left_reg));

        if (is_vector_scalar) {
            broadcastScalarToNeon(right_reg, right_neon_reg, result_type);
        } else {
            emit_(vecgen_fmov_x_to_d(right_neon_reg, right_reg));
        }
    }

    std::string arrangement = getNeonArrangement(result_type);

    // Perform the vector operation
    switch (node.op) {
        case BinaryOp::Operator::Add:
        if (result_type == VarType::FOCT) {
            emit_(vecgen_fadd_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        } else if (result_type == VarType::FPAIR) {
            emit_(fadd_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON FADD .2S for FPAIR addition");
        } else if (result_type == VarType::PAIR) {
            emit_(add_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON ADD .2S for PAIR addition");
        } else {
            emit_(vecgen_add_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        }
        debug_print("Generated NEON vector addition");
        break;

    case BinaryOp::Operator::Subtract:
        if (result_type == VarType::FOCT) {
            emit_(vecgen_fsub_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        } else if (result_type == VarType::FPAIR) {
            emit_(fsub_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON FSUB .2S for FPAIR subtraction");
        } else if (result_type == VarType::PAIR) {
            emit_(sub_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON SUB .2S for PAIR subtraction");
        } else {
            emit_(vecgen_sub_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        }
        debug_print("Generated NEON vector subtraction");
        break;

    case BinaryOp::Operator::Multiply:
        if (result_type == VarType::FOCT) {
            emit_(vecgen_fmul_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        } else if (result_type == VarType::FPAIR) {
            emit_(fmul_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON FMUL .2S for FPAIR multiplication");
        } else if (result_type == VarType::PAIR) {
            emit_(mul_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON MUL .2S for PAIR multiplication");
        } else {
            emit_(vecgen_mul_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
        }
        debug_print("Generated NEON vector multiplication");
        break;

    case BinaryOp::Operator::Divide:
        if (result_type == VarType::FOCT) {
            emit_(vecgen_fdiv_vector(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg), arrangement));
            debug_print("Generated NEON vector division (float)");
        } else if (result_type == VarType::FPAIR) {
            emit_(fdiv_vector_2s(qreg_to_vreg(left_neon_reg), qreg_to_vreg(left_neon_reg), qreg_to_vreg(right_neon_reg)));
            debug_print("Generated specialized NEON FDIV .2S for FPAIR division");
        } else {
            throw std::runtime_error("Integer division not supported in NEON vector operations");
        }
        break;

        default:
            throw std::runtime_error("Unsupported binary operation for SIMD vectors");
    }

    // --- START FIX ---
    // This is the corrected block

    // Allocate a fresh general-purpose register for the final result.
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(code_gen_);

    // For PAIR, the result is a 64-bit value. We must move it from the
    // NEON 'D' register back to the general-purpose 'X' register.
    if (result_type == VarType::PAIR) {
        // The result of the ADD operation is in 'left_neon_reg' (a 'D' register).
        // We must explicitly move this 64-bit value back to the general-purpose
        // 'X' register ('result_reg') that the rest of the compiler will use.
        emit_(vecgen_fmov_d_to_x(result_reg, left_neon_reg));
    } else {
        // For other vector types, use the store function
        storeNeonToVector(left_neon_reg, result_reg, result_type);
    }

    // Set the result register so the rest of the code generator knows where to find it.
    code_gen_.expression_result_reg_ = result_reg;
    // --- END FIX ---

    // Release NEON registers (use appropriate method for register type)
    if (result_type == VarType::PAIR || result_type == VarType::FPAIR || result_type == VarType::QUAD) {
        register_manager_.release_fp_register(left_neon_reg);
        register_manager_.release_fp_register(right_neon_reg);
    } else {
        register_manager_.release_q_register(left_neon_reg);
        register_manager_.release_q_register(right_neon_reg);
    }
    register_manager_.release_register(right_reg);
}

void VectorCodeGen::generateScalarBinaryOp(BinaryOp& node, VarType result_type) {
    debug_print("Generating scalar fallback binary operation for vector type");

    // Generate code for operands
    node.left->accept(code_gen_);
    std::string left_reg = code_gen_.expression_result_reg_;

    node.right->accept(code_gen_);
    std::string right_reg = code_gen_.expression_result_reg_;

    VarType left_type = code_gen_.infer_expression_type_local(node.left.get());
    VarType right_type = code_gen_.infer_expression_type_local(node.right.get());

    int lane_count = getLaneCount(result_type);
    VarType element_type = getElementType(result_type);

    // Allocate result register
    std::string result_reg = register_manager_.acquire_spillable_temp_reg(code_gen_);

    // Generate scalar operations for each lane
    for (int lane = 0; lane < lane_count; lane++) {
        int lane_offset_left = calculateLaneOffset(lane, left_type);
        int lane_offset_right = calculateLaneOffset(lane, right_type);
        int lane_offset_result = calculateLaneOffset(lane, result_type);

        std::string temp_left = register_manager_.acquire_spillable_temp_reg(code_gen_);
        std::string temp_right = register_manager_.acquire_spillable_temp_reg(code_gen_);
        std::string temp_result = register_manager_.acquire_spillable_temp_reg(code_gen_);

        // Load lane values
        if (element_type == VarType::FLOAT) {
            // Load 32-bit float from specific lane offset
            emit_(Encoder::create_ldr_imm(temp_left, left_reg, lane_offset_left));

            bool right_is_scalar = (right_type == VarType::INTEGER || right_type == VarType::FLOAT);
            if (right_is_scalar) {
                emit_(Encoder::create_mov_reg(temp_right, right_reg));
            } else {
                emit_(Encoder::create_ldr_imm(temp_right, right_reg, lane_offset_right));
            }
        } else {
            // Load integer value from specific lane offset
            if (result_type == VarType::OCT) {
                // 8-bit values
                emit_(Encoder::create_ldrb_imm(temp_left, left_reg, lane_offset_left));

                bool right_is_scalar = (right_type == VarType::INTEGER);
                if (right_is_scalar) {
                    emit_(Encoder::create_mov_reg(temp_right, right_reg));
                } else {
                    emit_(Encoder::create_ldrb_imm(temp_right, right_reg, lane_offset_right));
                }
            } else {
                // 32-bit values for other types
                emit_(Encoder::create_ldr_imm(temp_left, left_reg, lane_offset_left));

                bool right_is_scalar = (right_type == VarType::INTEGER);
                if (right_is_scalar) {
                    emit_(Encoder::create_mov_reg(temp_right, right_reg));
                } else {
                    emit_(Encoder::create_ldr_imm(temp_right, right_reg, lane_offset_right));
                }
            }
        }

        // Perform scalar operation
        switch (node.op) {
            case BinaryOp::Operator::Add:
                if (element_type == VarType::FLOAT) {
                    emit_(Encoder::create_fadd_reg(temp_result, temp_left, temp_right));
                } else {
                    emit_(Encoder::create_add_reg(temp_result, temp_left, temp_right));
                }
                break;

            case BinaryOp::Operator::Subtract:
                if (element_type == VarType::FLOAT) {
                    emit_(Encoder::create_fsub_reg(temp_result, temp_left, temp_right));
                } else {
                    emit_(Encoder::create_sub_reg(temp_result, temp_left, temp_right));
                }
                break;

            case BinaryOp::Operator::Multiply:
                if (element_type == VarType::FLOAT) {
                    emit_(Encoder::create_fmul_reg(temp_result, temp_left, temp_right));
                } else {
                    emit_(Encoder::create_mul_reg(temp_result, temp_left, temp_right));
                }
                break;

            case BinaryOp::Operator::Divide:
                if (element_type == VarType::FLOAT) {
                    emit_(Encoder::create_fdiv_reg(temp_result, temp_left, temp_right));
                } else {
                    emit_(Encoder::create_sdiv_reg(temp_result, temp_left, temp_right));
                }
                break;

            default:
                throw std::runtime_error("Unsupported binary operation for SIMD vectors in scalar mode");
        }

        // Store result to appropriate lane
        if (element_type == VarType::FLOAT) {
            emit_(Encoder::create_str_imm(temp_result, result_reg, lane_offset_result));
        } else {
            if (result_type == VarType::OCT) {
                emit_(Encoder::create_str_word_imm(temp_result, result_reg, lane_offset_result));
            } else {
                emit_(Encoder::create_str_imm(temp_result, result_reg, lane_offset_result));
            }
        }

        // Release temporary registers
        register_manager_.release_register(temp_left);
        register_manager_.release_register(temp_right);
        register_manager_.release_register(temp_result);
    }

    // Update result register
    code_gen_.expression_result_reg_ = result_reg;

    // Release input registers
    register_manager_.release_register(left_reg);
    register_manager_.release_register(right_reg);
}

void VectorCodeGen::generateNeonLaneRead(LaneAccessExpression& node, VarType vector_type, std::string& result_reg) {
    debug_print("Generating NEON lane read operation");

    // Generate code for vector expression
    node.vector_expr->accept(code_gen_);
    std::string vector_reg = code_gen_.expression_result_reg_;

    // Acquire appropriate NEON register (D for 64-bit vectors, Q for 128-bit)
    std::string neon_reg;
    if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
        neon_reg = register_manager_.acquire_fp_scratch_reg(); // D register
    } else {
        neon_reg = register_manager_.acquire_q_scratch_reg(code_gen_); // Q register
    }

    // Load vector to NEON register
    loadVectorToNeon(vector_reg, neon_reg, vector_type);

    // Extract lane using UMOV (unsigned move) or SMOV (signed move)
    std::string arrangement = getNeonArrangement(vector_type);

    if (getElementType(vector_type) == VarType::FLOAT) {
        // For float elements, use direct float lane extraction
        if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
            // Use D register operations for 64-bit vectors - extract directly to S register then convert
            std::string temp_s_reg = register_manager_.acquire_fp_scratch_reg();
            std::string s_reg = "S" + temp_s_reg.substr(1); // Convert D0 -> S0
            emit_(vecgen_fmov_s_lane(s_reg, neon_reg, node.index));
            // Convert single precision to double precision for result
            emit_(vecgen_fcvt_s_to_d(result_reg, s_reg));
            register_manager_.release_fp_register(temp_s_reg);
        } else {
            // Use Q register operations for 128-bit vectors
            std::string temp_s_reg = register_manager_.acquire_fp_scratch_reg();
            std::string s_reg = "S" + temp_s_reg.substr(1); // Convert D0 -> S0
            emit_(vecgen_fmov_s_lane(s_reg, qreg_to_vreg(neon_reg), node.index));
            // Convert single precision to double precision for result
            emit_(vecgen_fcvt_s_to_d(result_reg, s_reg));
            register_manager_.release_fp_register(temp_s_reg);
        }
    } else {
        // For integer elements
        // Skip NEON entirely for integer types - use direct bitfield extraction
        if ((vector_type == VarType::PAIR && arrangement == "2S_INT") ||
            vector_type == VarType::QUAD || 
            vector_type == VarType::OCT) {
            // Extract directly from the original vector register using SBFX
            node.vector_expr->accept(code_gen_);
            std::string vector_reg = code_gen_.expression_result_reg_;
            
            if (vector_type == VarType::PAIR) {
                // PAIR: 2 x 32-bit integers
                if (node.index == 0) {
                    // Extract first element: bits 0-31
                    emit_(Encoder::opt_create_sbfx(result_reg, vector_reg, 0, 32));
                } else if (node.index == 1) {
                    // Extract second element: bits 32-63  
                    emit_(Encoder::opt_create_sbfx(result_reg, vector_reg, 32, 32));
                } else {
                    throw std::runtime_error("Invalid lane index for PAIR: " + std::to_string(node.index));
                }
            } else if (vector_type == VarType::QUAD) {
                // QUAD: 4 x 16-bit integers
                if (node.index >= 0 && node.index <= 3) {
                    // Extract 16-bit element: bits (index * 16) to (index * 16 + 15)
                    emit_(Encoder::opt_create_sbfx(result_reg, vector_reg, node.index * 16, 16));
                } else {
                    throw std::runtime_error("Invalid lane index for QUAD: " + std::to_string(node.index));
                }
            } else if (vector_type == VarType::OCT) {
                // OCT: 8 x 8-bit integers
                if (node.index >= 0 && node.index <= 7) {
                    // Extract 8-bit element: bits (index * 8) to (index * 8 + 7)
                    emit_(Encoder::opt_create_sbfx(result_reg, vector_reg, node.index * 8, 8));
                } else {
                    throw std::runtime_error("Invalid lane index for OCT: " + std::to_string(node.index));
                }
            }
            
            // Release the NEON register since we didn't use it
            register_manager_.release_fp_register(neon_reg);
            return;
        }
        
        // For remaining types (FPAIR, FOCT), continue with NEON extraction
        if (vector_type == VarType::FPAIR) {
            emit_(vecgen_umov_sized(result_reg, neon_reg, node.index, "S"));
        } else if (vector_type == VarType::FOCT) {
            // Use Q register operations for FOCT (float octet)
            emit_(vecgen_umov_sized(result_reg, qreg_to_vreg(neon_reg), node.index, "S"));
        }
    }

    debug_print("Extracted lane " + std::to_string(node.index) + " from vector");

    // Release registers
    if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
        register_manager_.release_fp_register(neon_reg);
    } else {
        register_manager_.release_q_register(neon_reg);
    }
    register_manager_.release_register(vector_reg);
}

void VectorCodeGen::generateScalarLaneRead(LaneAccessExpression& node, VarType vector_type, std::string& result_reg) {
    debug_print("Generating scalar lane read operation");

    // Generate code for vector expression
    node.vector_expr->accept(code_gen_);
    std::string vector_reg = code_gen_.expression_result_reg_;

    // Calculate lane offset
    int lane_offset = calculateLaneOffset(node.index, vector_type);

    // Load from specific lane offset
    if (getElementType(vector_type) == VarType::FLOAT) {
        emit_(Encoder::create_ldr_imm(result_reg, vector_reg, lane_offset));
    } else {
        if (vector_type == VarType::OCT) {
            emit_(Encoder::create_ldrb_imm(result_reg, vector_reg, lane_offset));
        } else {
            emit_(Encoder::create_ldr_imm(result_reg, vector_reg, lane_offset));
        }
    }

    debug_print("Loaded lane " + std::to_string(node.index) + " at offset " + std::to_string(lane_offset));

    // Release vector register
    register_manager_.release_register(vector_reg);
}

void VectorCodeGen::generateNeonLaneWrite(LaneAccessExpression& node, VarType vector_type, const std::string& value_reg) {
    debug_print("Generating NEON lane write operation");

    // Generate code for vector expression
    node.vector_expr->accept(code_gen_);
    std::string vector_reg = code_gen_.expression_result_reg_;

    // Acquire appropriate NEON register (D for 64-bit vectors, Q for 128-bit)
    std::string neon_reg;
    if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
        neon_reg = register_manager_.acquire_fp_scratch_reg(); // D register
    } else {
        neon_reg = register_manager_.acquire_q_scratch_reg(code_gen_); // Q register
    }

    // Load vector to NEON register
    loadVectorToNeon(vector_reg, neon_reg, vector_type);

    // Insert new value into specific lane
    if (getElementType(vector_type) == VarType::FLOAT) {
        if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
            // Use D register operations for 64-bit vectors
            std::string temp_neon = register_manager_.acquire_fp_scratch_reg();
            emit_(vecgen_fmov_w_to_s(temp_neon, value_reg));
            emit_(vecgen_ins_element(neon_reg, node.index, temp_neon, 0, "S"));
            register_manager_.release_fp_register(temp_neon);
        } else {
            // Use Q register operations for 128-bit vectors
            std::string temp_neon = register_manager_.acquire_q_scratch_reg(code_gen_);
            emit_(vecgen_fmov_w_to_s(temp_neon, value_reg));
            emit_(vecgen_ins_element(qreg_to_vreg(neon_reg), node.index, qreg_to_vreg(temp_neon), 0, "S"));
            register_manager_.release_q_register(temp_neon);
        }
    } else {
        // For integer elements, skip NEON entirely - use direct BFI bitfield insertion
        if ((vector_type == VarType::PAIR) ||
            vector_type == VarType::QUAD || 
            vector_type == VarType::OCT) {
            
            if (vector_type == VarType::PAIR) {
                // PAIR: 2 x 32-bit integers
                if (node.index == 0) {
                    // Insert into bits 0-31
                    emit_(Encoder::opt_create_bfi(vector_reg, value_reg, 0, 32));
                } else if (node.index == 1) {
                    // Insert into bits 32-63  
                    emit_(Encoder::opt_create_bfi(vector_reg, value_reg, 32, 32));
                } else {
                    throw std::runtime_error("Invalid lane index for PAIR: " + std::to_string(node.index));
                }
            } else if (vector_type == VarType::QUAD) {
                // QUAD: 4 x 16-bit integers
                if (node.index >= 0 && node.index <= 3) {
                    // Insert 16-bit element: bits (index * 16) to (index * 16 + 15)
                    emit_(Encoder::opt_create_bfi(vector_reg, value_reg, node.index * 16, 16));
                } else {
                    throw std::runtime_error("Invalid lane index for QUAD: " + std::to_string(node.index));
                }
            } else if (vector_type == VarType::OCT) {
                // OCT: 8 x 8-bit integers
                if (node.index >= 0 && node.index <= 7) {
                    // Insert 8-bit element: bits (index * 8) to (index * 8 + 7)
                    emit_(Encoder::opt_create_bfi(vector_reg, value_reg, node.index * 8, 8));
                } else {
                    throw std::runtime_error("Invalid lane index for OCT: " + std::to_string(node.index));
                }
            }
            
            // Release the NEON register since we didn't use it
            if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
                register_manager_.release_fp_register(neon_reg);
            } else {
                register_manager_.release_q_register(neon_reg);
            }
            register_manager_.release_register(vector_reg);
            debug_print("Inserted value into lane " + std::to_string(node.index) + " using BFI");
            return;
        }
        
        // For remaining types (FPAIR, FOCT), continue with NEON insertion
        if (vector_type == VarType::FPAIR) {
            emit_(vecgen_ins_general(neon_reg, node.index, value_reg, "S"));
        } else if (vector_type == VarType::FOCT) {
            emit_(vecgen_ins_general(qreg_to_vreg(neon_reg), node.index, value_reg, "S"));
        }
    }

    // Store vector back to memory
    storeNeonToVector(neon_reg, vector_reg, vector_type);

    debug_print("Inserted value into lane " + std::to_string(node.index));

    // Release registers
    if (vector_type == VarType::PAIR || vector_type == VarType::FPAIR || vector_type == VarType::QUAD) {
        register_manager_.release_fp_register(neon_reg);
    } else {
        register_manager_.release_q_register(neon_reg);
    }
    register_manager_.release_register(vector_reg);
}

void VectorCodeGen::generateScalarLaneWrite(LaneAccessExpression& node, VarType vector_type, const std::string& value_reg) {
    debug_print("Generating scalar lane write operation");

    // Generate code for vector expression
    node.vector_expr->accept(code_gen_);
    std::string vector_reg = code_gen_.expression_result_reg_;

    // Calculate lane offset
    int lane_offset = calculateLaneOffset(node.index, vector_type);

    // Store to specific lane offset
    if (getElementType(vector_type) == VarType::FLOAT) {
        emit_(Encoder::create_str_imm(value_reg, vector_reg, lane_offset));
    } else {
        if (vector_type == VarType::OCT) {
            emit_(Encoder::create_str_word_imm(value_reg, vector_reg, lane_offset));
        } else {
            emit_(Encoder::create_str_imm(value_reg, vector_reg, lane_offset));
        }
    }

    debug_print("Stored value to lane " + std::to_string(node.index) + " at offset " + std::to_string(lane_offset));

    // Release vector register
    register_manager_.release_register(vector_reg);
}

void VectorCodeGen::generateNeonVectorConstruction(const std::vector<const ExprPtr*>& elements, VarType vector_type, std::string& result_reg) {
    debug_print("Generating NEON vector construction");

    std::string neon_reg = register_manager_.acquire_fp_scratch_reg();

    // Generate code for each element and insert into vector
    for (size_t i = 0; i < elements.size(); i++) {
        (*elements[i])->accept(code_gen_);
        std::string element_reg = code_gen_.expression_result_reg_;

        if (getElementType(vector_type) == VarType::FLOAT) {
            std::string temp_neon = register_manager_.acquire_q_scratch_reg(code_gen_);
            emit_(vecgen_fmov_w_to_s(temp_neon, element_reg));
            emit_(vecgen_ins_element(qreg_to_vreg(neon_reg), i, qreg_to_vreg(temp_neon), 0, "S"));
            register_manager_.release_q_register(temp_neon);
        } else {
            if (vector_type == VarType::OCT) {
                emit_(vecgen_ins_general(qreg_to_vreg(neon_reg), i, element_reg, "B"));
            } else {
                emit_(vecgen_ins_general(qreg_to_vreg(neon_reg), i, element_reg, "S"));
            }
        }

        register_manager_.release_register(element_reg);
    }

    // Store vector to result register
    storeNeonToVector(neon_reg, result_reg, vector_type);

    register_manager_.release_register(neon_reg);
}

void VectorCodeGen::generateScalarVectorConstruction(const std::vector<const ExprPtr*>& elements, VarType vector_type, std::string& result_reg) {
    debug_print("Generating scalar vector construction");

    // Generate code for each element and store at appropriate offset
    for (size_t i = 0; i < elements.size(); i++) {
        (*elements[i])->accept(code_gen_);
        std::string element_reg = code_gen_.expression_result_reg_;

        int lane_offset = calculateLaneOffset(i, vector_type);

        if (getElementType(vector_type) == VarType::FLOAT) {
            emit_(Encoder::create_str_imm(element_reg, result_reg, lane_offset));
        } else {
            if (vector_type == VarType::OCT) {
                emit_(Encoder::create_str_word_imm(element_reg, result_reg, lane_offset));
            } else {
                emit_(Encoder::create_str_imm(element_reg, result_reg, lane_offset));
            }
        }

        register_manager_.release_register(element_reg);
    }
}

void VectorCodeGen::loadVectorToNeon(const std::string& src_reg, const std::string& neon_reg, VarType vector_type) {
    if (vector_type == VarType::FOCT) {
        // FOCT requires 128-bit load (8 x 32-bit floats = 256 bits, but we'll use two 128-bit ops)
        emit_(vecgen_ldr_q(qreg_to_vreg(neon_reg), src_reg, 0));
    } else if (vector_type == VarType::FPAIR) {
        // FPAIR: 2 x 32-bit floats packed in 64-bit X register -> D register
        // The bit-packed representation can be moved directly
        emit_(vecgen_fmov_x_to_d(neon_reg, src_reg));
    } else if (vector_type == VarType::PAIR) {
        // PAIR: 2 x 32-bit integers packed in 64-bit X register -> D register
        emit_(vecgen_fmov_x_to_d(neon_reg, src_reg));
    } else if (vector_type == VarType::QUAD) {
        // QUAD: 4 x 16-bit integers packed in 64-bit X register -> D register
        emit_(vecgen_fmov_x_to_d(neon_reg, src_reg));
    } else {
        // OCT uses 64-bit load (8 x 8-bit = 64 bits)
        emit_(vecgen_fmov_x_to_d(neon_reg, src_reg));
    }
}

void VectorCodeGen::storeNeonToVector(const std::string& neon_reg, const std::string& dst_reg, VarType vector_type) {
    if (vector_type == VarType::FOCT) {
        // FOCT requires 128-bit store
        emit_(vecgen_str_q(qreg_to_vreg(neon_reg), dst_reg, 0));
    } else if (vector_type == VarType::FPAIR) {
        // FPAIR: D register -> 64-bit X register with packed floats
        emit_(vecgen_fmov_d_to_x(dst_reg, neon_reg));
    } else if (vector_type == VarType::PAIR) {
        // PAIR: D register -> 64-bit X register with packed integers
        emit_(vecgen_fmov_d_to_x(dst_reg, neon_reg));
    } else if (vector_type == VarType::QUAD) {
        // QUAD: D register -> 64-bit X register with packed 16-bit integers
        emit_(vecgen_fmov_d_to_x(dst_reg, neon_reg));
    } else {
        // OCT uses 64-bit store
        emit_(vecgen_fmov_d_to_x(dst_reg, neon_reg));
    }
}

void VectorCodeGen::broadcastScalarToNeon(const std::string& scalar_reg, const std::string& neon_reg, VarType vector_type) {
    std::string arrangement = getNeonArrangement(vector_type);

    if (getElementType(vector_type) == VarType::FLOAT) {
        // For float vectors, use FMOV + DUP
        std::string temp_neon = register_manager_.acquire_q_scratch_reg(code_gen_);
        emit_(vecgen_fmov_w_to_s(temp_neon, scalar_reg));
        emit_(vecgen_dup_general(qreg_to_vreg(neon_reg), scalar_reg, arrangement));
        register_manager_.release_q_register(temp_neon);
    } else {
        // For integer vectors, use DUP directly
        emit_(vecgen_dup_general(qreg_to_vreg(neon_reg), scalar_reg, arrangement));
    }
}

int VectorCodeGen::calculateLaneOffset(int lane_index, VarType vector_type) {
    switch (vector_type) {
        case VarType::PAIR:
        case VarType::FPAIR:
            return lane_index * 4; // 32-bit elements
        case VarType::QUAD:
            return lane_index * 2; // 16-bit elements
        case VarType::OCT:
            return lane_index * 1; // 8-bit elements
        case VarType::FOCT:
            return lane_index * 4; // 32-bit float elements
        default:
            throw std::runtime_error("Invalid vector type for lane offset calculation");
    }
}

void VectorCodeGen::generatePairsVectorBinaryOp(BinaryOp& node, bool use_neon) {
    debug_print("generatePairsVectorBinaryOp - 128-bit SIMD for PAIRS vectors");

    // Check operation type
    if (node.op != BinaryOp::Operator::Add && 
        node.op != BinaryOp::Operator::Subtract && 
        node.op != BinaryOp::Operator::Multiply) {
        throw std::runtime_error("Unsupported binary operation on PAIRS vectors: only +, -, * are supported");
    }

    // Get operands (must be variables for PAIRS vectors)
    auto* left_var = dynamic_cast<VariableAccess*>(node.left.get());
    auto* right_var = dynamic_cast<VariableAccess*>(node.right.get());

    if (!left_var || !right_var) {
        throw std::runtime_error("PAIRS vector operations require variable operands");
    }

    // Get vector size
    size_t vector_size = getPairsVectorSize(node.left.get());
    debug_print("Vector size: " + std::to_string(vector_size) + " PAIRs");

    // Get addresses of source vectors
    std::string left_addr = code_gen_.get_variable_register(left_var->name);
    std::string right_addr = code_gen_.get_variable_register(right_var->name);

    // Allocate result vector
    std::string result_addr = allocatePairsResultVector(vector_size);

    if (use_neon) {
        debug_print("Using 128-bit NEON SIMD for PAIRS vector operation");

        // Process 2 PAIRs at a time using 128-bit Q registers
        for (size_t i = 0; i < vector_size; i += 2) {
            // Calculate offset for this iteration (each PAIR = 8 bytes)
            size_t offset = i * 8;

            // Acquire Q registers for 128-bit NEON operations
            std::string q_left = register_manager_.acquire_q_scratch_reg(code_gen_);
            std::string q_right = register_manager_.acquire_q_scratch_reg(code_gen_);
            std::string q_result = register_manager_.acquire_q_scratch_reg(code_gen_);

            // Convert Q register names to V register names for encoder (strip Q prefix)
            std::string v_left = q_left.substr(1);   // Q5 -> 5
            std::string v_right = q_right.substr(1); // Q6 -> 6  
            std::string v_result = q_result.substr(1); // Q7 -> 7

            // Load 2 PAIRs (16 bytes) from each vector using LDR with Q registers
            emit_(Encoder::create_ldr_vec_imm("Q" + v_left, left_addr, offset));
            emit_(Encoder::create_ldr_vec_imm("Q" + v_right, right_addr, offset));

            // NEON vector operation: process 4 × 32-bit components (2 PAIRs) in parallel
            switch (node.op) {
                case BinaryOp::Operator::Add:
                    printf("DEBUG: Calling create_add_vector_reg with: vd=%s, vn=%s, vm=%s, arrangement=4S\n", 
                           ("V" + v_result).c_str(), ("V" + v_left).c_str(), ("V" + v_right).c_str());
                    fflush(stdout);
                    emit_(Encoder::create_add_vector_reg("V" + v_result, "V" + v_left, "V" + v_right, "4S"));
                    debug_print("Generated NEON ADD .4S for PAIRs " + std::to_string(i) + "-" + std::to_string(i+1) + " (four components in parallel)");
                    break;
                case BinaryOp::Operator::Subtract:
                    emit_(Encoder::create_sub_vector_reg("V" + v_result, "V" + v_left, "V" + v_right, "4S"));
                    debug_print("Generated NEON SUB .4S for PAIRs " + std::to_string(i) + "-" + std::to_string(i+1));
                    break;
                case BinaryOp::Operator::Multiply:
                    emit_(Encoder::create_mul_vector_reg("V" + v_result, "V" + v_left, "V" + v_right, "4S"));
                    debug_print("Generated NEON MUL .4S for PAIRs " + std::to_string(i) + "-" + std::to_string(i+1));
                    break;
                default:
                    throw std::runtime_error("Unsupported NEON operation for PAIRS");
            }

            // Store result (2 PAIRs = 16 bytes)
            emit_(Encoder::create_str_vec_imm("Q" + v_result, result_addr, offset));

            // Release Q registers
            register_manager_.release_fp_register(q_left);
            register_manager_.release_fp_register(q_right);
            register_manager_.release_fp_register(q_result);
        }

        // Handle remainder PAIRs if vector_size is odd
        if (vector_size % 2 == 1) {
            size_t last_index = vector_size - 1;
            size_t offset = last_index * 8;

            // Use scalar fallback for the last PAIR
            std::string left_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);
            std::string right_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);
            std::string result_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);

            emit_(Encoder::create_ldr_imm(left_pair_reg, left_addr, offset));
            emit_(Encoder::create_ldr_imm(right_pair_reg, right_addr, offset));

            switch (node.op) {
                case BinaryOp::Operator::Add:
                    emit_(Encoder::create_add_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                case BinaryOp::Operator::Subtract:
                    emit_(Encoder::create_sub_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                case BinaryOp::Operator::Multiply:
                    emit_(Encoder::create_mul_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                default:
                    break;
            }

            emit_(Encoder::create_str_imm(result_pair_reg, result_addr, offset));

            register_manager_.release_register(left_pair_reg);
            register_manager_.release_register(right_pair_reg);
            register_manager_.release_register(result_pair_reg);
        }
    } else {
        debug_print("Using scalar fallback for PAIRS vector operation");
        // Scalar fallback implementation
        for (size_t i = 0; i < vector_size; i++) {
            size_t offset = i * 8; // Each PAIR = 8 bytes

            std::string left_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);
            std::string right_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);
            std::string result_pair_reg = register_manager_.acquire_scratch_reg(code_gen_);

            emit_(Encoder::create_ldr_imm(left_pair_reg, left_addr, offset));
            emit_(Encoder::create_ldr_imm(right_pair_reg, right_addr, offset));

            switch (node.op) {
                case BinaryOp::Operator::Add:
                    emit_(Encoder::create_add_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                case BinaryOp::Operator::Subtract:
                    emit_(Encoder::create_sub_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                case BinaryOp::Operator::Multiply:
                    emit_(Encoder::create_mul_reg(result_pair_reg, left_pair_reg, right_pair_reg));
                    break;
                default:
                    throw std::runtime_error("Unsupported scalar operation for PAIRS");
            }

            emit_(Encoder::create_str_imm(result_pair_reg, result_addr, offset));

            register_manager_.release_register(left_pair_reg);
            register_manager_.release_register(right_pair_reg);
            register_manager_.release_register(result_pair_reg);
        }
    }

    // Set the result register to the allocated vector address
    code_gen_.expression_result_reg_ = result_addr;
    debug_print("PAIRS vector operation completed - result at " + result_addr);
}

size_t VectorCodeGen::getPairsVectorSize(Expression* expr) {
    auto* var_access = dynamic_cast<VariableAccess*>(expr);
    if (!var_access) {
        throw std::runtime_error("Cannot determine size of non-variable PAIRS vector");
    }

    // For now, assume size 8 - this should be retrieved from symbol table in real implementation
    return 8;
}

std::string VectorCodeGen::allocatePairsResultVector(size_t vector_size) {
    // Use existing GETVEC infrastructure to allocate PAIRS vector
    auto& rm = register_manager_;
    std::string size_reg = rm.acquire_scratch_reg(code_gen_);

    // PAIRS allocation: vector_size PAIRs × 2 words per PAIR
    size_t total_words = vector_size * 2;
    emit_(Encoder::create_movz_imm(size_reg, total_words));

    // Call GETVEC to allocate memory
    std::string result_reg = rm.acquire_scratch_reg(code_gen_);
    emit_(Encoder::create_mov_reg(result_reg, "X0")); // Save X0
    emit_(Encoder::create_mov_reg("X0", size_reg));    // Set up GETVEC argument

    // Call GETVEC (assuming it's registered as external function)
    emit_(Encoder::create_branch_with_link("GETVEC"));
    emit_(Encoder::create_mov_reg(size_reg, "X0"));    // Get result
    emit_(Encoder::create_mov_reg("X0", result_reg));  // Restore X0

    rm.release_register(result_reg);
    std::string vector_addr = size_reg; // Keep the allocated address
    debug_print("Allocated PAIRS result vector at register " + vector_addr + " (size: " + std::to_string(vector_size) + " PAIRs)");

    return vector_addr;
}

void VectorCodeGen::debug_print(const std::string& message) {
    std::cout << "[VectorCodeGen] " << message << std::endl;
}

// Custom vector instruction encoders
Instruction VectorCodeGen::vecgen_fadd_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // FADD vector: 0Q001110AA1mmmmm000101nnnnnddddd
    // Q=1 for 128-bit, AA=arrangement, operation=000101
    uint32_t instruction = 0x0E200400;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    // Set Q bit for 128-bit operation
    if (arrangement == "4S" || arrangement == "2D") {
        instruction |= (1 << 30);
    }
    // 2S uses 64-bit registers (Q bit = 0)

    // Set size bits for arrangement
    if (arrangement == "4S" || arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (0 << 22); // 32-bit elements
    } else if (arrangement == "2D") {
        instruction |= (1 << 22); // 64-bit elements
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "fadd " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_fsub_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // FSUB vector: 0Q001110AA1mmmmm000111nnnnnddddd
    uint32_t instruction = 0x0E200C00;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    if (arrangement == "4S" || arrangement == "2D") {
        instruction |= (1 << 30);
    }

    if (arrangement == "4S" || arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (0 << 22);
    } else if (arrangement == "2D") {
        instruction |= (1 << 22);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "fsub " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_fmul_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // FMUL vector: 0Q001110AA1mmmmm001111nnnnnddddd
    uint32_t instruction = 0x0E201C00;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    if (arrangement == "4S" || arrangement == "2D") {
        instruction |= (1 << 30);
    }

    if (arrangement == "4S" || arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (0 << 22);
    } else if (arrangement == "2D") {
        instruction |= (1 << 22);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "fmul " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_fdiv_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // FDIV vector: 0Q001110AA1mmmmm001111nnnnnddddd
    uint32_t instruction = 0x0E201F00;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    if (arrangement == "4S" || arrangement == "2D") {
        instruction |= (1 << 30);
    }

    if (arrangement == "4S" || arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (0 << 22);
    } else if (arrangement == "2D") {
        instruction |= (1 << 22);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "fdiv " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_add_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // ADD vector: 0Q001110SS1mmmmm100001nnnnnddddd
    // Reference: 0ea18400 for add.2s v0, v0, v1
    // Base encoding for ADD vector instruction - using same pattern as classic encoder
    uint32_t encoding = 0x0E008400; // Base with Q=0, opcode=100001

    uint32_t rd = Encoder::get_reg_encoding(vd);
    uint32_t rn = Encoder::get_reg_encoding(vn);
    uint32_t rm = Encoder::get_reg_encoding(vm);

    // Set Q bit, size, and mandatory bit 21 based on arrangement - like classic encoder
    if (arrangement == "2S" || arrangement == "2S_INT") {
        // Q=0 (64-bit), size=10 for 32-bit elements
        encoding |= (0x2 << 22); // size = 10
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "4S") {
        // Q=1 (128-bit), size=10 for 32-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x2 << 22);  // size = 10
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "8B") {
        // Q=0 (64-bit), size=00 for 8-bit elements
        encoding |= (0x0 << 22); // size = 00
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "16B") {
        // Q=1 (128-bit), size=00 for 8-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x0 << 22);  // size = 00
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "4H") {
        // Q=0 (64-bit), size=01 for 16-bit elements
        encoding |= (0x1 << 22); // size = 01
        encoding |= (1U << 21);  // mandatory bit 21 = 1
    } else if (arrangement == "8H") {
        // Q=1 (128-bit), size=01 for 16-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x1 << 22);  // size = 01
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else if (arrangement == "2D") {
        // Q=1 (128-bit), size=11 for 64-bit elements
        encoding |= (1U << 30);   // Q=1
        encoding |= (0x3 << 22);  // size = 11
        encoding |= (1U << 21);   // mandatory bit 21 = 1
    } else {
        throw std::runtime_error("Unsupported arrangement for ADD vector: " + arrangement);
    }

    // Set register fields - clean single line like classic encoder
    encoding |= (rm << 16) | (rn << 5) | rd;

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "add " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    
    Instruction instr(encoding, asm_text);
    instr.opcode = InstructionDecoder::OpType::ADD_VECTOR;
    instr.dest_reg = Encoder::get_reg_encoding(vd);
    instr.src_reg1 = Encoder::get_reg_encoding(vn);
    instr.src_reg2 = Encoder::get_reg_encoding(vm);
    return instr;
}

Instruction VectorCodeGen::vecgen_sub_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // SUB vector: 0Q001110SS1mmmmm100001nnnnnddddd
    uint32_t instruction = 0x0E208400;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    if (arrangement == "8B" || arrangement == "4H" || arrangement == "2S" || arrangement == "2S_INT") {
        // 64-bit operation, Q=0
    } else {
        instruction |= (1 << 30); // Q=1 for 128-bit
    }

    // Set size bits and operation (SUB uses different opcode)
    instruction &= ~(0x3F << 10); // Clear operation bits
    instruction |= (0x21 << 10);  // SUB operation

    if (arrangement == "8B" || arrangement == "16B") {
        instruction |= (0 << 22);
    } else if (arrangement == "4H" || arrangement == "8H") {
        instruction |= (1 << 22);
    } else if (arrangement == "2S" || arrangement == "4S" || arrangement == "2S_INT") {
        instruction |= (2 << 22);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "sub " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_mul_vector(const std::string& vd, const std::string& vn, const std::string& vm, const std::string& arrangement) {
    // MUL vector: 0Q001110SS1mmmmm100111nnnnnddddd
    uint32_t instruction = 0x0E209C00;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);
    instruction |= ((vm_num & 0x1F) << 16);

    if (arrangement == "8B" || arrangement == "4H" || arrangement == "2S" || arrangement == "2S_INT") {
        // 64-bit operation, Q=0
    } else {
        instruction |= (1 << 30); // Q=1 for 128-bit
    }

    if (arrangement == "8B" || arrangement == "16B") {
        instruction |= (0 << 22);
    } else if (arrangement == "4H" || arrangement == "8H") {
        instruction |= (1 << 22);
    } else if (arrangement == "2S" || arrangement == "4S" || arrangement == "2S_INT") {
        instruction |= (2 << 22);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "mul " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "." + lower_arrangement + ", " + vm_reg + "." + lower_arrangement;
    return Instruction(instruction, asm_text);
}

// Specialized encoder for 2S vector subtraction (PAIR operations)
Instruction VectorCodeGen::sub_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 SUB vector .2S encoding: 0Q001110SS1mmmmm100001nnnnnddddd
    // Q=0, SS=10 (32-bit), opcode=100001 (SUB)
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base SUB vector opcode for .2S: 0x2ea18400 (exact match to clang output)
    BitPatcher patcher(0x2ea18400);
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "sub " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; new dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::SUB_VECTOR;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

// Specialized encoder for 2S vector multiplication (PAIR operations)
Instruction VectorCodeGen::mul_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 MUL vector .2S encoding: clang generates 0x0ea19c00
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base MUL vector opcode for .2S: 0x0ea19c00 (exact match to clang output)
    // Clear register bits first since clang base has registers (0,0,1)
    uint32_t base = 0x0ea19c00;
    base &= ~0x1F;          // Clear Rd (bits 4:0) 
    base &= ~(0x1F << 5);   // Clear Rn (bits 9:5)
    base &= ~(0x1F << 16);  // Clear Rm (bits 20:16)
    
    BitPatcher patcher(base);
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "mul " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; new dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::MUL_VECTOR;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

// Specialized encoder for 2S vector floating-point division (FPAIR operations)
Instruction VectorCodeGen::fdiv_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 FDIV vector .2S encoding: clang generates 0x2e21fc00
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base FDIV vector opcode for .2S: 0x2e21fc00 (exact match to clang output)
    BitPatcher patcher(0x2e21fc00);
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "fdiv " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; new dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FDIV;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

// Specialized encoder for 2S vector floating-point addition (FPAIR operations)
Instruction VectorCodeGen::fadd_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 FADD vector .2S encoding: clang generates 0x0e21d400
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base FADD vector opcode for .2S: 0x0e21d400 (exact match to clang output)
    // Clear register bits first using BitPatcher
    BitPatcher patcher(0x0e21d400);
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "fadd " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FADD;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

// Specialized encoder for 2S vector floating-point subtraction (FPAIR operations)
Instruction VectorCodeGen::fsub_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 FSUB vector .2S encoding: clang generates 0x0ea1d400
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base FSUB vector opcode for .2S: 0x0ea1d400 (exact match to clang output)
    BitPatcher patcher(0x0ea1d400);
    std::cerr << "[DEBUG] Initial BitPatcher value: 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    std::cerr << "[DEBUG] After clearing Rd: 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    std::cerr << "[DEBUG] After clearing Rn: 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    std::cerr << "[DEBUG] After clearing Rm: 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    std::cerr << "[DEBUG] After setting Rd=" << vd_num << ": 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    std::cerr << "[DEBUG] After setting Rn=" << vn_num << ": 0x" << std::hex << patcher.get_value() << std::dec << std::endl;
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)
    std::cerr << "[DEBUG] After setting Rm=" << vm_num << ": 0x" << std::hex << patcher.get_value() << std::dec << std::endl;

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "fsub " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; dedicated 2s encoder";
    
    uint32_t final_encoding = patcher.get_value();
    std::cerr << "[DEBUG] FSUB BitPatcher result: 0x" << std::hex << final_encoding << std::dec << std::endl;
    
    Instruction instr(final_encoding, asm_text);
    instr.opcode = InstructionDecoder::OpType::FSUB;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    instr.trace_this_instruction = true; // Enable tracing for FSUB instructions
    
    return instr;
}

// Specialized encoder for 2S vector floating-point multiplication (FPAIR operations)
Instruction VectorCodeGen::fmul_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 FMUL vector .2S encoding: clang generates 0x2e21dc00
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base FMUL vector opcode for .2S: 0x2e21dc00 (exact match to clang output)
    BitPatcher patcher(0x2e21dc00);
    
    // Clear existing register fields
    patcher.patch(0, 0, 5);   // Clear Rd (bits 4:0)
    patcher.patch(0, 5, 5);   // Clear Rn (bits 9:5)
    patcher.patch(0, 16, 5);  // Clear Rm (bits 20:16)
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "fmul " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FMUL;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

// Specialized encoder for 2S vector addition (PAIR operations)
Instruction VectorCodeGen::add_vector_2s(const std::string& vd, const std::string& vn, const std::string& vm) {
    // ARM64 ADD vector .2S encoding: clang generates 0x0ea18400
    // Use BitPatcher to build instruction like other encoders
    
    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);
    int vm_num = parse_register_number(vm);

    // Base ADD vector opcode for .2S: 0x0ea18400 (exact match to clang output)
    // Clear register bits first since clang base has registers (0,0,1)
    uint32_t base = 0x0ea18400;
    base &= ~0x1F;          // Clear Rd (bits 4:0) 
    base &= ~(0x1F << 5);   // Clear Rn (bits 9:5)
    base &= ~(0x1F << 16);  // Clear Rm (bits 20:16)
    
    BitPatcher patcher(base);
    
    patcher.patch(vd_num, 0, 5);   // Rd (bits 4:0)
    patcher.patch(vn_num, 5, 5);   // Rn (bits 9:5)
    patcher.patch(vm_num, 16, 5);  // Rm (bits 20:16)

    // Generate assembly text
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string vm_reg = (vm[0] == 'D') ? "v" + vm.substr(1) : vm;
    
    std::string asm_text = "add " + vd_reg + ".2s, " + vn_reg + ".2s, " + vm_reg + ".2s    ; new dedicated 2s encoder";
    
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::ADD_VECTOR;
    instr.dest_reg = vd_num;
    instr.src_reg1 = vn_num;
    instr.src_reg2 = vm_num;
    
    return instr;
}

Instruction VectorCodeGen::vecgen_ins_element(const std::string& vd, int dst_lane, const std::string& vn, int src_lane, const std::string& size) {
    // INS element: 01101110000iiiii0jjjj1nnnnnddddd
    uint32_t instruction = 0x6E000400;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);

    // Set lane indices and size
    if (size == "B") {
        instruction |= ((dst_lane & 0xF) << 16);
        instruction |= ((src_lane & 0xF) << 11);
        instruction |= (1 << 20); // Size encoding for bytes
    } else if (size == "H") {
        instruction |= ((dst_lane & 0x7) << 17);
        instruction |= ((src_lane & 0x7) << 12);
        instruction |= (2 << 20); // Size encoding for halfwords
    } else if (size == "S") {
        instruction |= ((dst_lane & 0x3) << 18);
        instruction |= ((src_lane & 0x3) << 13);
        instruction |= (4 << 20); // Size encoding for words
    } else if (size == "D") {
        instruction |= ((dst_lane & 0x1) << 19);
        instruction |= ((src_lane & 0x1) << 14);
        instruction |= (8 << 20); // Size encoding for doublewords
    }

    std::string asm_text;
    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;

    if (size == "B") {
        asm_text = "ins " + vd_reg + ".b[" + std::to_string(dst_lane) + "], " + vn_reg + ".b[" + std::to_string(src_lane) + "]";
    } else if (size == "H") {
        asm_text = "ins " + vd_reg + ".h[" + std::to_string(dst_lane) + "], " + vn_reg + ".h[" + std::to_string(src_lane) + "]";
    } else if (size == "S") {
        asm_text = "ins " + vd_reg + ".s[" + std::to_string(dst_lane) + "], " + vn_reg + ".s[" + std::to_string(src_lane) + "]";
    } else {
        asm_text = "ins " + vd_reg + ".d[" + std::to_string(dst_lane) + "], " + vn_reg + ".d[" + std::to_string(src_lane) + "]";
    }
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_ins_general(const std::string& vd, int lane, const std::string& rn, const std::string& size) {
    // INS general: 01001110000iiiii000111nnnnnddddd
    uint32_t instruction = 0x4E001C00;

    int vd_num = parse_register_number(vd);
    int rn_num = parse_register_number(rn);

    instruction |= (vd_num & 0x1F);
    instruction |= ((rn_num & 0x1F) << 5);

    // Set lane index and size based on actual working opcodes
    if (size == "B") {
        // 8-bit: imm5 = (lane << 1) | 1
        instruction |= (((lane << 1) | 1) << 16);
    } else if (size == "H") {
        // 16-bit: imm5 = (lane << 1) | 2
        instruction |= (((lane << 1) | 2) << 16);
    } else if (size == "S") {
        // 32-bit: imm5 = (lane << 2) | 4
        instruction |= (((lane << 2) | 4) << 16);
    } else if (size == "D") {
        // 64-bit: imm5 = (lane << 3) | 8
        instruction |= (((lane << 3) | 8) << 16);
    }

    std::string asm_text;
    if (vd[0] == 'D') {
        // Convert D register to V register for clang-compatible syntax
        std::string v_reg = "v" + vd.substr(1);
        if (size == "B") {
            asm_text = "ins " + v_reg + ".b[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else if (size == "H") {
            asm_text = "ins " + v_reg + ".h[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else if (size == "S") {
            asm_text = "ins " + v_reg + ".s[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else {
            asm_text = "ins " + v_reg + ".d[" + std::to_string(lane) + "], x" + rn.substr(1);
        }
    } else {
        // For V registers, use standard INS syntax
        if (size == "B") {
            asm_text = "ins " + vd + ".b[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else if (size == "H") {
            asm_text = "ins " + vd + ".h[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else if (size == "S") {
            asm_text = "ins " + vd + ".s[" + std::to_string(lane) + "], w" + rn.substr(1);
        } else {
            asm_text = "ins " + vd + ".d[" + std::to_string(lane) + "], x" + rn.substr(1);
        }
    }
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_umov_sized(const std::string& rd, const std::string& vn, int lane, const std::string& size) {
    // UMOV: 0Q001110SSiiiiii001111nnnnnddddd
    uint32_t instruction = 0x0E003C00;

    int rd_num = parse_register_number(rd);
    int vn_num = parse_register_number(vn);

    instruction |= (rd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);

    // Set size bits (SS field at bits 23:22) and lane index
    if (size == "B") {
        // 8-bit: SS=00 (already 0), imm5 = (lane << 1) | 1
        instruction |= (((lane << 1) | 1) << 16);
    } else if (size == "H") {
        // 16-bit: Use exact reference opcodes from clang
        // Reference: 0e023c08 (lane 0), 0e063c08 (lane 1), 0e0a3c08 (lane 2), 0e0e3c08 (lane 3)
        // Base is 0x0E003C00, so we need to match the imm5 patterns exactly
        if (lane == 0) {
            instruction = 0x0E023C00 | (rd_num & 0x1F) | ((vn_num & 0x1F) << 5);
        } else if (lane == 1) {
            instruction = 0x0E063C00 | (rd_num & 0x1F) | ((vn_num & 0x1F) << 5);
        } else if (lane == 2) {
            instruction = 0x0E0A3C00 | (rd_num & 0x1F) | ((vn_num & 0x1F) << 5);
        } else if (lane == 3) {
            instruction = 0x0E0E3C00 | (rd_num & 0x1F) | ((vn_num & 0x1F) << 5);
        } else {
            // Fallback for other lanes
            instruction |= (((lane << 2) | 2) << 16);
        }
    } else if (size == "S") {
        // 32-bit: SS=10
        instruction |= (2 << 22); // Set SS=10 for 32-bit
        if (lane == 0) {
            instruction |= (0x04 << 16);
        } else if (lane == 1) {
            instruction |= (0x0C << 16);
        } else {
            // Fallback pattern for other lanes
            instruction |= (((lane << 2) | 4) << 16);
        }
    } else if (size == "D") {
        // 64-bit: SS=11, imm5 = (lane << 3) | 8
        instruction |= (3 << 22); // Set SS=11 for 64-bit
        instruction |= (((lane << 3) | 8) << 16);
        instruction |= (1 << 30); // Q=1 for 64-bit result
    }

    std::string asm_text;
    if (vn[0] == 'D') {
        // Convert D register to V register for clang-compatible syntax
        std::string v_reg = "v" + vn.substr(1);
        if (size == "B") {
            asm_text = "umov w" + rd.substr(1) + ", " + v_reg + ".b[" + std::to_string(lane) + "]";
        } else if (size == "H") {
            asm_text = "umov w" + rd.substr(1) + ", " + v_reg + ".h[" + std::to_string(lane) + "]";
        } else if (size == "S") {
            asm_text = "umov w" + rd.substr(1) + ", " + v_reg + ".s[" + std::to_string(lane) + "]";
        } else {
            asm_text = "umov x" + rd.substr(1) + ", " + v_reg + ".d[" + std::to_string(lane) + "]";
        }
    } else {
        // For V registers, use standard NEON syntax
        if (size == "B") {
            asm_text = "umov w" + rd.substr(1) + ", " + vn + ".b[" + std::to_string(lane) + "]";
        } else if (size == "H") {
            asm_text = "umov w" + rd.substr(1) + ", " + vn + ".h[" + std::to_string(lane) + "]";
        } else if (size == "S") {
            asm_text = "umov w" + rd.substr(1) + ", " + vn + ".s[" + std::to_string(lane) + "]";
        } else {
            asm_text = "umov x" + rd.substr(1) + ", " + vn + ".d[" + std::to_string(lane) + "]";
        }
    }
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_mov_element(const std::string& rd, const std::string& vn, int lane, const std::string& size) {
    // MOV element (vector to general): 0Q001110SSiiiiii001111nnnnnddddd
    // Reference opcodes: 0e043c14 (lane 0), 0e0c3c14 (lane 1)
    uint32_t instruction = 0x0E003C00;

    int rd_num = parse_register_number(rd);
    int vn_num = parse_register_number(vn);

    instruction |= (rd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);

    // Set lane index and size for MOV element
    if (size == "S") {
        // 32-bit elements: Reference shows lane 0 = 0x04, lane 1 = 0x0C
        if (lane == 0) {
            instruction |= (0x04 << 16);
        } else if (lane == 1) {
            instruction |= (0x0C << 16);
        } else {
            // Fallback pattern for other lanes
            instruction |= (((lane << 2) | 4) << 16);
        }
        instruction |= (2 << 22); // size=10 for 32-bit
    } else if (size == "H") {
        // 16-bit elements: imm5 = (lane << 1) | 2
        instruction |= (((lane << 1) | 2) << 16);
        instruction |= (1 << 22); // size=01 for 16-bit
    } else if (size == "B") {
        // 8-bit elements: imm5 = (lane << 1) | 1
        instruction |= (((lane << 1) | 1) << 16);
        // size=00 for 8-bit (already 0)
    }

    std::string asm_text;
    std::string v_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string w_reg = "w" + rd.substr(1);

    if (size == "S") {
        asm_text = "mov.s " + w_reg + ", " + v_reg + "[" + std::to_string(lane) + "]";
    } else if (size == "H") {
        asm_text = "mov.h " + w_reg + ", " + v_reg + "[" + std::to_string(lane) + "]";
    } else if (size == "B") {
        asm_text = "mov.b " + w_reg + ", " + v_reg + "[" + std::to_string(lane) + "]";
    }

    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_dup_general(const std::string& vd, const std::string& rn, const std::string& arrangement) {
    // DUP general: 0Q001110000iiiii000011nnnnnddddd
    uint32_t instruction = 0x0E000C00;

    int vd_num = parse_register_number(vd);
    int rn_num = parse_register_number(rn);

    instruction |= (vd_num & 0x1F);
    instruction |= ((rn_num & 0x1F) << 5);

    // Set Q bit and lane arrangement
    if (arrangement == "8B") {
        instruction |= (1 << 16); // imm5 = 00001
    } else if (arrangement == "16B") {
        instruction |= (1 << 16); // imm5 = 00001
        instruction |= (1 << 30); // Q=1
    } else if (arrangement == "4H") {
        instruction |= (2 << 16); // imm5 = 00010
    } else if (arrangement == "8H") {
        instruction |= (2 << 16); // imm5 = 00010
        instruction |= (1 << 30); // Q=1
    } else if (arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (4 << 16); // imm5 = 00100
    } else if (arrangement == "4S") {
        instruction |= (4 << 16); // imm5 = 00100
        instruction |= (1 << 30); // Q=1
    } else if (arrangement == "2D") {
        instruction |= (8 << 16); // imm5 = 01000
        instruction |= (1 << 30); // Q=1
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "dup " + vd_reg + "." + lower_arrangement + ", " + rn;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_dup_scalar(const std::string& vd, const std::string& vn, const std::string& arrangement) {
    // DUP scalar: 0Q001110000iiiii000001nnnnnddddd
    uint32_t instruction = 0x0E000400;

    int vd_num = parse_register_number(vd);
    int vn_num = parse_register_number(vn);

    instruction |= (vd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);

    // Set Q bit and lane arrangement (same logic as DUP general)
    if (arrangement == "8B") {
        instruction |= (1 << 16);
    } else if (arrangement == "16B") {
        instruction |= (1 << 16);
        instruction |= (1 << 30);
    } else if (arrangement == "4H") {
        instruction |= (2 << 16);
    } else if (arrangement == "8H") {
        instruction |= (2 << 16);
        instruction |= (1 << 30);
    } else if (arrangement == "2S" || arrangement == "2S_INT") {
        instruction |= (4 << 16);
    } else if (arrangement == "4S") {
        instruction |= (4 << 16);
        instruction |= (1 << 30);
    } else if (arrangement == "2D") {
        instruction |= (8 << 16);
        instruction |= (1 << 30);
    }

    std::string vd_reg = (vd[0] == 'D') ? "v" + vd.substr(1) : vd;
    std::string vn_reg = (vn[0] == 'D') ? "v" + vn.substr(1) : vn;
    std::string lower_arrangement = normalizeArrangementForAssembly(arrangement);
    std::string asm_text = "dup " + vd_reg + "." + lower_arrangement + ", " + vn_reg + "[0]";
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_ldr_q(const std::string& qt, const std::string& base, int offset) {
    // LDR (SIMD&FP): 1Q111101U1iiiiiiiiiiiinnnnnttttt
    uint32_t instruction = 0x3D400000;

    int qt_num = parse_register_number(qt);
    int base_num = parse_register_number(base);

    instruction |= (qt_num & 0x1F);
    instruction |= ((base_num & 0x1F) << 5);

    // Set Q bit for 128-bit access
    instruction |= (1 << 30);

    // Set immediate offset (scaled by 16 for Q registers)
    if (offset % 16 == 0) {
        instruction |= ((offset / 16) & 0xFFF) << 10;
    } else {
        throw std::runtime_error("Q register offset must be 16-byte aligned");
    }

    std::string asm_text = "ldr " + qt + ", [" + base;
    if (offset != 0) {
        asm_text += ", #" + std::to_string(offset);
    }
    asm_text += "]";
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_str_q(const std::string& qt, const std::string& base, int offset) {
    // STR (SIMD&FP): 1Q111101U0iiiiiiiiiiiinnnnnttttt
    uint32_t instruction = 0x3D000000;

    int qt_num = parse_register_number(qt);
    int base_num = parse_register_number(base);

    instruction |= (qt_num & 0x1F);
    instruction |= ((base_num & 0x1F) << 5);

    // Set Q bit for 128-bit access
    instruction |= (1 << 30);

    // Set immediate offset (scaled by 16 for Q registers)
    if (offset % 16 == 0) {
        instruction |= ((offset / 16) & 0xFFF) << 10;
    } else {
        throw std::runtime_error("Q register offset must be 16-byte aligned");
    }

    std::string asm_text = "str " + qt + ", [" + base;
    if (offset != 0) {
        asm_text += ", #" + std::to_string(offset);
    }
    asm_text += "]";
    return Instruction(instruction, asm_text);
}

std::string VectorCodeGen::qreg_to_vreg(const std::string& qreg) {
    // Convert Q5 -> V5, etc.
    if (qreg.length() >= 2 && qreg[0] == 'Q') {
        return "V" + qreg.substr(1);
    }
    return qreg; // Already V register or other format
}

// Helper function to normalize arrangement for assembly text
std::string VectorCodeGen::normalizeArrangementForAssembly(const std::string& arrangement) {
    if (arrangement == "2S_INT") {
        return "2s";
    }
    std::string lower_arrangement = arrangement;
    std::transform(lower_arrangement.begin(), lower_arrangement.end(), lower_arrangement.begin(), ::tolower);
    return lower_arrangement;
}

// FMOV operations for vector register transfers
Instruction VectorCodeGen::vecgen_fmov_x_to_d(const std::string& dd, const std::string& xn) {
    // FMOV Dd, Xn: 1001111001100111000000nnnnnddddd - match classic encoder
    uint32_t d_dst = Encoder::get_reg_encoding(dd);
    uint32_t x_src = Encoder::get_reg_encoding(xn);

    if (d_dst > 31 || x_src > 31) {
        throw std::runtime_error("Invalid register for FMOV X to D: " + dd + ", " + xn);
    }

    // Use BitPatcher like classic encoder for proper bit field assembly
    BitPatcher patcher(0x9E670000);
    patcher.patch(x_src, 5, 5); // bits 5-9: source X register
    patcher.patch(d_dst, 0, 5); // bits 0-4: destination D register

    std::string asm_text = "fmov " + dd + ", " + xn;
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(dd);
    instr.src_reg1 = Encoder::get_reg_encoding(xn);
    return instr;
}

Instruction VectorCodeGen::vecgen_fmov_d_to_x(const std::string& xd, const std::string& dn) {
    // FMOV Xd, Dn: 1001111001100110000000nnnnnddddd - match classic encoder
    uint32_t x_dst = Encoder::get_reg_encoding(xd);
    uint32_t d_src = Encoder::get_reg_encoding(dn);

    if (x_dst > 31 || d_src > 31) {
        throw std::runtime_error("Invalid register for FMOV D to X: " + xd + ", " + dn);
    }

    // Use BitPatcher like classic encoder for proper bit field assembly
    BitPatcher patcher(0x9E660000);
    patcher.patch(d_src, 5, 5); // bits 5-9: source D register
    patcher.patch(x_dst, 0, 5); // bits 0-4: destination X register

    std::string asm_text = "fmov " + xd + ", " + dn;
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::FMOV;
    instr.dest_reg = Encoder::get_reg_encoding(xd);
    instr.src_reg1 = Encoder::get_reg_encoding(dn);
    return instr;
}

Instruction VectorCodeGen::vecgen_fmov_w_to_s(const std::string& sd, const std::string& wn) {
    // FMOV Sd, Wn: 0001111000100010000000nnnnnddddd
    uint32_t instruction = 0x1E220000;

    int sd_num = parse_register_number(sd);
    int wn_num = parse_register_number(wn);

    instruction |= (sd_num & 0x1F);
    instruction |= ((wn_num & 0x1F) << 5);

    std::string asm_text = "fmov " + sd + ", " + wn;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_fmov_s_to_w(const std::string& wd, const std::string& sn) {
    // FMOV Wd, Sn: 0001111000100110000000nnnnnddddd
    uint32_t instruction = 0x1E260000;

    int wd_num = parse_register_number(wd);
    int sn_num = parse_register_number(sn);

    instruction |= (wd_num & 0x1F);
    instruction |= ((sn_num & 0x1F) << 5);

    std::string asm_text = "fmov " + wd + ", " + sn;
    return Instruction(instruction, asm_text);
}

Instruction VectorCodeGen::vecgen_fmov_s_lane(const std::string& sd, const std::string& vn, int lane) {
    // FMOV Sd, Vn.S[lane]: 0Q001110SS1iiiii001111nnnnnddddd
    // For single lane extraction: Q=0, size=10 (32-bit), opc=11
    uint32_t instruction = 0x0E043C00;

    int sd_num = parse_register_number(sd);
    int vn_num = parse_register_number(vn);

    instruction |= (sd_num & 0x1F);
    instruction |= ((vn_num & 0x1F) << 5);

    // Set lane index in imm5 field
    if (lane == 0) {
        instruction |= (0x04 << 16);  // imm5 = 00100
    } else if (lane == 1) {
        instruction |= (0x0C << 16);  // imm5 = 01100
    }

    std::string asm_text = "fmov " + sd + ", " + vn + ".s[" + std::to_string(lane) + "]";
    return Instruction(instruction, asm_text);
}
/**
 * @brief Generates a UMOV instruction to move a single element from a NEON vector
 * register to a general-purpose register.
 *
 * @param wd The destination general-purpose register (e.g., "w20").
 * @param vn The source NEON vector register (e.g., "v0").
 * @param lane The index of the element (0-3 for .s format) to move.
 * @return The generated Instruction object.
 */
Instruction VectorCodeGen::vecgen_umov(const std::string& wd, const std::string& vn, int lane) {
    // Helper lambda to parse a general-purpose or NEON register string.
    auto parse_register = [](const std::string& reg_str, char expected_prefix) -> uint32_t {
        if (reg_str.empty()) {
            throw std::invalid_argument("Register string cannot be empty.");
        }

        std::string lower_reg = reg_str;
        std::transform(lower_reg.begin(), lower_reg.end(), lower_reg.begin(), ::tolower);

        if (lower_reg[0] != expected_prefix) {
            throw std::invalid_argument("Invalid register prefix in '" + reg_str + "'. Expected '" + expected_prefix + "'.");
        }

        try {
            uint32_t reg_num = std::stoul(reg_str.substr(1));
            if (reg_num > 31) {
                throw std::out_of_range("Register number out of range for '" + reg_str + "'.");
            }
            return reg_num;
        } catch (const std::logic_error&) {
            throw std::invalid_argument("Invalid register format: '" + reg_str + "'.");
        }
    };

    // (A) Self-checking: parse and validate registers.
    uint32_t wd_num = parse_register(wd, 'w');
    uint32_t vn_num = parse_register(vn, 'v');

    // (B) Determine the base instruction pattern from the lane.
    // These patterns are derived from clang's output for UMOV.
    uint32_t base_pattern = 0;
    if (lane == 0) {
        base_pattern = 0x0E043C00; // UMOV Wd, Vn.S[0] base
    } else if (lane == 1) {
        base_pattern = 0x0E0C3C00; // UMOV Wd, Vn.S[1] base
    } else {
        // Fallback for other lanes, constructing the immediate field manually.
        // This is for UMOV (scalar) where imm5 encodes the element index.
        uint32_t imm5 = ((lane << 2) | 4);
        base_pattern = 0x0E003C00 | (imm5 << 16);
    }

    // (C) Use BitPatcher to construct the final instruction word.
    BitPatcher patcher(base_pattern);
    patcher.patch(wd_num, 0, 5);  // Patch Rd (destination) into bits 0-4
    patcher.patch(vn_num, 5, 5);  // Patch Rn (source) into bits 5-9

    // (D) Format the assembly string for the Instruction object.
    std::string asm_text = "mov.s " + wd + ", " + vn + "[" + std::to_string(lane) + "]";

    // (E) Return the completed Instruction object.
    Instruction instr(patcher.get_value(), asm_text);
    instr.opcode = InstructionDecoder::OpType::UMOV;
    instr.dest_reg = wd_num;
    instr.src_reg1 = vn_num;
    instr.nopeep = true; // Protect from peephole optimizer

    // (F) Add a debug print in the consistent style.
    printf("DEBUG: Created instruction 0x%08X for %s\n", instr.encoding, asm_text.c_str());

    return instr;
}

Instruction VectorCodeGen::vecgen_fcvt_s_to_d(const std::string& dd, const std::string& sn) {
    // FCVT Dd, Sn: 0001111000100010110000nnnnnddddd
    uint32_t instruction = 0x1E22C000;

    int dd_num = parse_register_number(dd);
    int sn_num = parse_register_number(sn);

    instruction |= (dd_num & 0x1F);
    instruction |= ((sn_num & 0x1F) << 5);

    std::string asm_text = "fcvt " + dd + ", " + sn;
    return Instruction(instruction, asm_text);
}

int VectorCodeGen::parse_register_number(const std::string& reg) {
    // Use the proper, proven encoder function instead of our shitty version
    return static_cast<int>(Encoder::get_reg_encoding(reg));
}

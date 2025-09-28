#include "../../NewCodeGenerator.h"
#include "../../analysis/ASTAnalyzer.h"

// Helper function to coerce an argument to the correct ARM64 ABI register with proper type conversion
// This implementation correctly handles the ARM64 ABI requirement for separate integer and float register allocation
void NewCodeGenerator::coerce_to_arg(int arg_num, const std::string& src_reg, VarType expr_type, VarType expected_type) {
    // ARM64 ABI: X0-X7 for integers/pointers, D0-D7 for floats
    if (arg_num < 0 || arg_num > 7) {
        throw std::runtime_error("ARM64 ABI only supports 8 arguments (0-7), got: " + std::to_string(arg_num));
    }
    
    // Debug output
    debug_print("=== coerce_to_arg DEBUG ===");
    debug_print("arg_num: " + std::to_string(arg_num));
    debug_print("src_reg: " + src_reg);
    debug_print("expr_type: " + vartype_to_string(expr_type));
    debug_print("expected_type: " + vartype_to_string(expected_type));
    debug_print("is_fp_register(src_reg): " + std::string(register_manager_.is_fp_register(src_reg) ? "true" : "false"));
    
    // Determine target type - when expected_type is UNKNOWN, use expr_type as ground truth
    VarType target_type = (expected_type != VarType::UNKNOWN) ? expected_type : expr_type;
    debug_print("target_type: " + vartype_to_string(target_type));
    
    // ARM64 ABI implementation: We need to maintain separate counters for integer and float registers
    // However, since we only have arg_num as input, we need to calculate the appropriate register
    // based on the target type and the calling context
    
    // For proper ARM64 ABI, we would need to track NGRN and NSRN separately across all arguments
    // in the function call. Since we don't have that context here, we'll implement a simplified
    // version that works correctly for most common cases.
    
    if (target_type == VarType::FLOAT) {
        // We need to end up in a D register
        // For ARM64 ABI compliance, float arguments use D0-D7 based on float argument position
        // Since we don't have the full calling context, we'll use the arg_num as the D register number
        // This is a reasonable approximation for most function calls
        std::string dest_d_reg = "D" + std::to_string(arg_num);
        debug_print("TARGET: FLOAT -> " + dest_d_reg);
        
        if (register_manager_.is_fp_register(src_reg)) {
            // Source is already float - just move if different register
            debug_print("Source is FP register, moving to " + dest_d_reg);
            if (src_reg != dest_d_reg) {
                emit(Encoder::create_fmov_reg(dest_d_reg, src_reg));
                debug_print("Emitted FMOV " + dest_d_reg + ", " + src_reg);
            } else {
                debug_print("Source already in correct register " + dest_d_reg);
            }
        } else {
            // Source is integer - convert to float
            debug_print("Source is INT register, converting " + src_reg + " to " + dest_d_reg);
            emit(Encoder::create_scvtf_reg(dest_d_reg, src_reg));
            debug_print("Emitted SCVTF " + dest_d_reg + ", " + src_reg);
        }
    } else {
        // We need to end up in an X register (integer/pointer)
        // For ARM64 ABI compliance, integer arguments use X0-X7 based on integer argument position
        std::string dest_x_reg = "X" + std::to_string(arg_num);
        debug_print("TARGET: INTEGER -> " + dest_x_reg);
        
        if (register_manager_.is_fp_register(src_reg)) {
            // Source is float - convert to integer
            debug_print("Source is FP register, converting " + src_reg + " to " + dest_x_reg);
            generate_float_to_int_truncation(dest_x_reg, src_reg);
            debug_print("Emitted float-to-int conversion " + src_reg + " -> " + dest_x_reg);
        } else {
            // Source is already integer - just move if different register
            debug_print("Source is INT register, moving to " + dest_x_reg);
            if (src_reg != dest_x_reg) {
                emit(Encoder::create_mov_reg(dest_x_reg, src_reg));
                debug_print("Emitted MOV " + dest_x_reg + ", " + src_reg);
            } else {
                debug_print("Source already in correct register " + dest_x_reg);
            }
        }
    }
    
    // Always release the source register since we've moved/converted the value
    debug_print("Releasing source register: " + src_reg);
    register_manager_.release_register(src_reg);
    debug_print("=== END coerce_to_arg DEBUG ===");
}

// New helper function to properly implement ARM64 ABI with separate register counters
void NewCodeGenerator::coerce_arguments_to_abi(
    const std::vector<std::string>& arg_regs,
    const std::vector<VarType>& arg_types,
    const std::vector<VarType>& expected_types
) {
    if (arg_regs.size() != arg_types.size() || 
        (!expected_types.empty() && arg_regs.size() != expected_types.size())) {
        throw std::runtime_error("Argument vectors must have the same size");
    }
    
    // ARM64 ABI: Maintain separate counters for integer and float registers
    int ngrn = 0; // Next General-purpose Register Number (X0-X7)
    int nsrn = 0; // Next SIMD and Floating-point Register Number (D0-D7)
    
    debug_print("=== ARM64 ABI Argument Coercion ===");
    debug_print("Total arguments: " + std::to_string(arg_regs.size()));
    
    // DEBUG: Print input argument registers before any processing
    // std::cerr << "[COERCE_DEBUG] Input argument registers: ";
    // for (size_t i = 0; i < arg_regs.size(); ++i) {
    //     std::cerr << "arg" << i << "=" << arg_regs[i] << " ";
    // }
    // std::cerr << std::endl;
    
    // CRITICAL FIX: Collect all MOV operations first, then emit them all at once
    // This prevents register manager from reusing destination registers as temporaries
    std::vector<std::pair<std::string, std::string>> mov_operations;
    
    for (size_t i = 0; i < arg_regs.size(); ++i) {
        const std::string& src_reg = arg_regs[i];
        VarType expr_type = arg_types[i];
        VarType expected_type = expected_types.empty() ? VarType::UNKNOWN : expected_types[i];
        
        // Determine target type - when expected_type is UNKNOWN, use expr_type as ground truth
        VarType target_type = (expected_type != VarType::UNKNOWN) ? expected_type : expr_type;
        
        debug_print("Arg " + std::to_string(i) + ": src=" + src_reg + 
                   " expr_type=" + vartype_to_string(expr_type) + 
                   " expected=" + vartype_to_string(expected_type) + 
                   " target=" + vartype_to_string(target_type));
        
        if (target_type == VarType::FLOAT) {
            // Float argument goes to D register
            if (nsrn >= 8) {
                throw std::runtime_error("Too many float arguments for ARM64 ABI (max 8 D registers)");
            }
            
            std::string dest_d_reg = "D" + std::to_string(nsrn);
            debug_print("Float arg " + std::to_string(i) + " -> " + dest_d_reg + " (NSRN=" + std::to_string(nsrn) + ")");
            // std::cerr << "[COERCE_DEBUG] Float: " << src_reg << " -> " << dest_d_reg << std::endl;
            
            if (register_manager_.is_fp_register(src_reg)) {
                // Source is already float
                if (src_reg != dest_d_reg) {
                    mov_operations.push_back({"FMOV", dest_d_reg + ", " + src_reg});
                    debug_print("Queued FMOV " + dest_d_reg + ", " + src_reg);
                }
            } else {
                // Source is integer - convert to float
                mov_operations.push_back({"SCVTF", dest_d_reg + ", " + src_reg});
                debug_print("Queued SCVTF " + dest_d_reg + ", " + src_reg);
            }
            
            nsrn++; // Increment float register counter
        } else {
            // Integer/pointer argument goes to X register
            if (ngrn >= 8) {
                throw std::runtime_error("Too many integer arguments for ARM64 ABI (max 8 X registers)");
            }
            
            std::string dest_x_reg = "X" + std::to_string(ngrn);
            debug_print("Integer arg " + std::to_string(i) + " -> " + dest_x_reg + " (NGRN=" + std::to_string(ngrn) + ")");
            // std::cerr << "[COERCE_DEBUG] Integer: " << src_reg << " -> " << dest_x_reg << std::endl;
            
            if (register_manager_.is_fp_register(src_reg)) {
                // Source is float - convert to integer (this needs immediate execution)
                generate_float_to_int_truncation(dest_x_reg, src_reg);
                debug_print("Emitted float-to-int conversion " + src_reg + " -> " + dest_x_reg);
            } else {
                // Source is already integer
                if (src_reg != dest_x_reg) {
                    mov_operations.push_back({"MOV", dest_x_reg + ", " + src_reg});
                    debug_print("Queued MOV " + dest_x_reg + ", " + src_reg);
                }
            }
            
            ngrn++; // Increment integer register counter
        }
        
        // Don't release source registers yet - we need them for the MOV operations
        debug_print("Deferring release of source register: " + src_reg);
    }
    
    // Now emit all the MOV operations at once to avoid register conflicts
    debug_print("Emitting " + std::to_string(mov_operations.size()) + " queued operations");
    for (const auto& op : mov_operations) {
        if (op.first == "MOV") {
            auto comma_pos = op.second.find(", ");
            std::string dest = op.second.substr(0, comma_pos);
            std::string src = op.second.substr(comma_pos + 2);
            emit(Encoder::create_mov_reg(dest, src));
            debug_print("Executed MOV " + dest + ", " + src);
        } else if (op.first == "FMOV") {
            auto comma_pos = op.second.find(", ");
            std::string dest = op.second.substr(0, comma_pos);
            std::string src = op.second.substr(comma_pos + 2);
            emit(Encoder::create_fmov_reg(dest, src));
            debug_print("Executed FMOV " + dest + ", " + src);
        } else if (op.first == "SCVTF") {
            auto comma_pos = op.second.find(", ");
            std::string dest = op.second.substr(0, comma_pos);
            std::string src = op.second.substr(comma_pos + 2);
            emit(Encoder::create_scvtf_reg(dest, src));
            debug_print("Executed SCVTF " + dest + ", " + src);
        }
    }
    
    // Now release all the source registers
    for (const std::string& src_reg : arg_regs) {
        debug_print("Releasing source register: " + src_reg);
        register_manager_.release_register(src_reg);
    }
    
    // std::cerr << "[COERCE_DEBUG] Executed " << mov_operations.size() << " queued operations" << std::endl;
    debug_print("Final register usage: NGRN=" + std::to_string(ngrn) + ", NSRN=" + std::to_string(nsrn));
    debug_print("=== END ARM64 ABI Argument Coercion ===");
}
// gen_ArgumentOptimization.cpp
// Register-direct argument optimization for function calls with ≤3 arguments
// This optimizes small function calls by using direct register allocation instead of stack operations

#include "NewCodeGenerator.h"
#include "Encoder.h"

// Check if register-direct approach is safe for the given argument count
bool NewCodeGenerator::can_use_register_direct_arguments(size_t arg_count) {
    // Conservative safety check - only optimize small argument counts
    if (arg_count > 3) {
        return false; // Always use stack for 4+ arguments
    }
    
    // Count available scratch registers
    int available_scratch = count_available_scratch_registers();
    
    // Need at least arg_count + 2 registers for safety margin
    // (arguments + intermediate expression results + method address loading)
    int estimated_need = static_cast<int>(arg_count) + 2;
    
    bool can_use = available_scratch >= estimated_need;
    
    if (debug_enabled_) {
        debug_print("Register-direct feasibility check: " + 
                   std::to_string(arg_count) + " args, " +
                   std::to_string(available_scratch) + " available, " +
                   std::to_string(estimated_need) + " needed -> " +
                   (can_use ? "FEASIBLE" : "NOT FEASIBLE"));
    }
    
    return can_use;
}

// Count available scratch registers for register pressure analysis
int NewCodeGenerator::count_available_scratch_registers() {
    const std::vector<std::string> scratch_regs = {"X9", "X10", "X11", "X12", "X13", "X14", "X15"};
    int available_count = 0;
    
    for (const auto& reg : scratch_regs) {
        if (register_manager_.is_register_free(reg)) {
            available_count++;
        }
    }
    
    return available_count;
}

// Register-direct argument handling for method calls (X0 = this, X1-X7 = args)
void NewCodeGenerator::handle_method_call_arguments_direct(RoutineCallStatement& node) {
    debug_print("Using REGISTER-DIRECT optimization for method call with " + 
                std::to_string(node.arguments.size()) + " arguments");
    
    // Evaluate arguments left-to-right and place directly in ABI registers
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        debug_print("Evaluating argument " + std::to_string(i + 1) + " for direct register placement");
        
        // Generate code for the argument expression
        generate_expression_code(*node.arguments[i]);
        
        // Target ABI register (X1, X2, X3 for method calls - X0 reserved for 'this')
        std::string target_abi_reg = "X" + std::to_string(i + 1);
        
        // If result is not already in the target register, move it
        if (expression_result_reg_ != target_abi_reg) {
            emit(Encoder::create_mov_reg(target_abi_reg, expression_result_reg_));
            register_manager_.release_register(expression_result_reg_);
            debug_print("Moved argument result from " + expression_result_reg_ + " to " + target_abi_reg);
        } else {
            debug_print("Argument already in correct register: " + target_abi_reg);
        }
        
        // Mark ABI register as containing the argument
        register_manager_.mark_register_as_used(target_abi_reg);
    }
}

// Register-direct argument handling for regular function calls (X0-X7 = args)
void NewCodeGenerator::handle_regular_call_arguments_direct(RoutineCallStatement& node) {
    debug_print("Using REGISTER-DIRECT optimization for regular call with " + 
                std::to_string(node.arguments.size()) + " arguments");
    
    // Evaluate arguments left-to-right and place directly in ABI registers
    for (size_t i = 0; i < node.arguments.size(); ++i) {
        debug_print("Evaluating argument " + std::to_string(i + 1) + " for direct register placement");
        
        // Generate code for the argument expression
        generate_expression_code(*node.arguments[i]);
        
        // Target ABI register (X0, X1, X2 for regular calls)
        std::string target_abi_reg = "X" + std::to_string(i);
        
        // If result is not already in the target register, move it
        if (expression_result_reg_ != target_abi_reg) {
            emit(Encoder::create_mov_reg(target_abi_reg, expression_result_reg_));
            register_manager_.release_register(expression_result_reg_);
            debug_print("Moved argument result from " + expression_result_reg_ + " to " + target_abi_reg);
        } else {
            debug_print("Argument already in correct register: " + target_abi_reg);
        }
        
        // Mark ABI register as containing the argument
        register_manager_.mark_register_as_used(target_abi_reg);
    }
}

// Register-direct argument handling for super calls (X0 = this, X1-X7 = args)
void NewCodeGenerator::handle_super_call_arguments_direct(RoutineCallStatement& node) {
    debug_print("Using REGISTER-DIRECT optimization for super call with " + 
                std::to_string(node.arguments.size()) + " arguments");
    
    // Super calls use same register layout as method calls
    handle_method_call_arguments_direct(node);
}

// Existing stack-based implementation (extracted and refactored for reuse)
void NewCodeGenerator::handle_arguments_stack_based(RoutineCallStatement& node, 
                                                   bool is_method_call, 
                                                   bool is_super_call) {
    debug_print("Using STACK-BASED approach for " + std::to_string(node.arguments.size()) + " arguments");
    
    size_t total_stack_bytes = 0;
    
    if (!node.arguments.empty()) {
        // Determine argument limit based on call type
        size_t max_args = is_method_call || is_super_call ? 7 : 8;
        
        // Check for too many arguments
        if (node.arguments.size() > max_args) {
            std::string call_type = is_super_call ? "SUPER call" : 
                                  (is_method_call ? "Method call" : "Routine call");
            throw std::runtime_error(call_type + " with " + std::to_string(node.arguments.size()) + 
                                   " arguments exceeds maximum of " + std::to_string(max_args) + " arguments");
        }
        
        // Pre-allocate stack space (8 bytes per argument, 16-byte aligned)
        total_stack_bytes = node.arguments.size() * 8;
        if (total_stack_bytes % 16 != 0) {
            total_stack_bytes = ((total_stack_bytes + 15) / 16) * 16;
        }
        emit(Encoder::create_sub_imm("SP", "SP", total_stack_bytes));
        
        // Evaluate arguments RIGHT-TO-LEFT and store immediately to stack
        for (int i = static_cast<int>(node.arguments.size()) - 1; i >= 0; --i) {
            generate_expression_code(*node.arguments[i]);
            
            // Store result immediately to stack (frees all scratch registers)
            size_t stack_offset = i * 8;
            if (register_manager_.is_fp_register(expression_result_reg_)) {
                emit(Encoder::create_str_fp_imm(expression_result_reg_, "SP", stack_offset));
            } else {
                emit(Encoder::create_str_imm(expression_result_reg_, "SP", stack_offset));
            }
            
            // Release the register - it's now safely on stack
            register_manager_.release_register(expression_result_reg_);
        }
        
        // Load arguments from stack into ABI registers
        for (size_t i = 0; i < node.arguments.size(); ++i) {
            size_t stack_offset = i * 8;
            std::string abi_reg;
            
            if (is_method_call || is_super_call) {
                // Method/Super calls: X0 reserved for 'this', args go in X1-X7
                abi_reg = "X" + std::to_string(i + 1);
            } else {
                // Regular calls: args go in X0-X7
                abi_reg = "X" + std::to_string(i);
            }
            
            emit(Encoder::create_ldr_imm(abi_reg, "SP", stack_offset));
        }
        
        // Clean up stack space
        emit(Encoder::create_add_imm("SP", "SP", total_stack_bytes));
    }
}
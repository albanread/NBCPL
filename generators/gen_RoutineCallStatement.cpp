#include "../NewCodeGenerator.h"
#include "../LabelManager.h"
#include "../analysis/ASTAnalyzer.h"
#include "../NameMangler.h"
#include <stdexcept>
#include "CodeGenUtils.h"
#include "../runtime/ListDataTypes.h"

// Format specifier structure for WRITEF validation
struct FormatSpecifier {
    char spec;
    VarType expected_type;
    size_t position;
    
    FormatSpecifier(char s, VarType t, size_t p) : spec(s), expected_type(t), position(p) {}
};

// Parse format string to extract format specifiers
std::vector<FormatSpecifier> parse_format_string(const std::string& format_str) {
    std::vector<FormatSpecifier> specifiers;
    size_t pos = 0;
    

    
    for (size_t i = 0; i < format_str.length(); ++i) {
        if (format_str[i] == '*') {
            // BCPL-style format specifiers: *F, *N, *I, etc.
            if (i + 1 < format_str.length()) {
                char spec = format_str[i + 1];
                VarType expected_type = VarType::UNKNOWN;
                
                switch (spec) {
                    case 'I': case 'O': case 'X': case 'B': case 'U':
                        expected_type = VarType::INTEGER;
                        break;
                    case 'E': case 'F': case 'G':
                        expected_type = VarType::FLOAT;
                        break;
                    case 'S': case 'C':
                        expected_type = VarType::STRING;
                        break;
                    case 'N':
                        // *N is newline - doesn't consume an argument
                        expected_type = VarType::UNKNOWN;
                        break;
                    default:
                        expected_type = VarType::UNKNOWN;
                        break;
                }
                
                if (expected_type != VarType::UNKNOWN) {
                    specifiers.emplace_back(spec, expected_type, pos++);
                }
                i++; // Skip the specifier character
            }
        } else if (format_str[i] == '%') {
            // C-style format specifiers: %f, %d, %s, etc.
            if (i + 1 < format_str.length()) {
                char spec = format_str[i + 1];
                VarType expected_type = VarType::UNKNOWN;
                
                // NOTE: Keep this list in sync with the analyzer's parse_writef_format_string!
                switch (spec) {
                    case 'd': case 'i': case 'o': case 'x': case 'X': case 'u':
                    case 'N': // Alias for integer
                        expected_type = VarType::INTEGER;
                        break;
                    case 'f': case 'F': case 'e': case 'E': case 'g': case 'G':
                        expected_type = VarType::FLOAT;
                        break;
                    case 's': case 'c':
                    case 'S': // Alias for string
                        expected_type = VarType::STRING;
                        break;
                    case 'P':
                        expected_type = VarType::PAIR;
                        break;
                    case 'Q':
                        expected_type = VarType::FPAIR;
                        break;
                    case '%':
                        // %% is literal % - doesn't consume an argument
                        expected_type = VarType::UNKNOWN;
                        break;
                    default:
                        expected_type = VarType::UNKNOWN;
                        break;
                }
                
                if (expected_type != VarType::UNKNOWN) {
                    specifiers.emplace_back(spec, expected_type, pos++);
                }
                i++; // Skip the specifier character
            }
        }
    }
    

    return specifiers;
}

// Check if two types are compatible for WRITEF
// WRITEF is permissive - it accepts any type and handles conversion at runtime
bool types_compatible(VarType actual, VarType expected) {
    // WRITEF accepts any type - let the runtime handle conversion
    return true;
}

// Convert VarType to string for error messages
std::string type_to_string(VarType type) {
    switch (type) {
        case VarType::INTEGER: return "INTEGER";
        case VarType::FLOAT: return "FLOAT";
        case VarType::STRING: return "STRING";
        case VarType::UNKNOWN: return "UNKNOWN";
        default: return "OTHER";
    }
}

// Validate WRITEF format string against provided arguments
void validate_writef_call(const std::vector<ExprPtr>& arguments) {
    if (arguments.empty()) {
        throw std::runtime_error("WRITEF requires at least a format string argument.");
    }
    
    // Get the format string (first argument)
    auto* format_literal = dynamic_cast<StringLiteral*>(arguments[0].get());
    if (!format_literal) {
        // If it's not a literal, we can't validate at compile time
        return;
    }
    
    std::string format_str = format_literal->value;
    std::vector<FormatSpecifier> specifiers = parse_format_string(format_str);
    
    // Check that the number of format specifiers matches the number of value arguments
    size_t value_arg_count = arguments.size() - 1; // Exclude format string
    if (specifiers.size() != value_arg_count) {
        throw std::runtime_error("WRITEF format string expects " + std::to_string(specifiers.size()) + 
                                " arguments but " + std::to_string(value_arg_count) + " were provided.");
    }
    
    // WRITEF validation is now permissive - let runtime handle type conversion
    // We still parse format specifiers for argument count validation, but don't enforce types
    if (specifiers.size() != value_arg_count) {
        // Only validate argument count, not types
        return; // Types are validated at runtime, not compile time
    }
}

void NewCodeGenerator::visit(RoutineCallStatement& node) {
    debug_print("--- Entering NewCodeGenerator::visit(RoutineCallStatement& node) [ARM64 ABI COMPLIANT] ---");

    // Skip codegen for the SETTYPE intrinsic, as it's a compile-time directive.
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
        if (var_access->name == "SETTYPE") {
            debug_print("Skipping code generation for SETTYPE intrinsic.");
            return;
        }
    }

    // Phase 4: Manual spilling removed - LinearScanAllocator ensures call-crossing
    // variables are allocated to callee-saved registers (preserved automatically)

    // --- UNIFIED LOGIC FOR ALL METHOD AND SUPER CALLS ---
    if (auto* member_access = dynamic_cast<MemberAccessExpression*>(node.routine_expr.get())) {
        debug_print("Detected a method call to: " + member_access->member_name);

        // --- Stage 1: Evaluate all arguments into temporary registers ---
        std::vector<std::string> arg_value_regs;
        for (const auto& arg_expr : node.arguments) {
            generate_expression_code(*arg_expr);
            std::string temp_reg;
            if (register_manager_.is_fp_register(expression_result_reg_)) {
                temp_reg = register_manager_.acquire_spillable_fp_temp_reg(*this);
                emit(Encoder::create_fmov_reg(temp_reg, expression_result_reg_));
            } else {
                temp_reg = register_manager_.acquire_spillable_temp_reg(*this);
                emit(Encoder::create_mov_reg(temp_reg, expression_result_reg_));
            }
            register_manager_.release_register(expression_result_reg_);
            arg_value_regs.push_back(temp_reg);
        }

        // --- Stage 2: Evaluate the object pointer ('this') ---
        generate_expression_code(*member_access->object_expr);
        std::string this_ptr_reg = expression_result_reg_;

        // --- Stage 3: Set up 'this' pointer in X0 ---
        emit(Encoder::create_mov_reg("X0", this_ptr_reg));
        register_manager_.release_register(this_ptr_reg);

        // --- Stage 4: Prepare argument type information for ARM64 ABI coercion ---
        std::vector<VarType> arg_types;
        std::vector<VarType> expected_types;
        
        // Look up method signature for parameter types
        std::string class_name = get_class_name_for_expression(member_access->object_expr.get());
        ClassMethodInfo* method_info = class_table_->lookup_class_method(class_name, member_access->member_name);
        Symbol method_symbol;
        bool has_symbol_param_info = false;
        std::string method_qualified_name = class_name + "::" + member_access->member_name;
        if (symbol_table_->lookup(method_qualified_name, method_symbol)) {
            has_symbol_param_info = true;
        }

        for (size_t i = 0; i < arg_value_regs.size(); ++i) {
            // Infer the actual type of this argument expression
            VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
            arg_types.push_back(arg_expr_type);
            
            // Determine expected parameter type based on method signature
            VarType expected_type = VarType::UNKNOWN;
            if (method_info && i < method_info->parameters.size()) {
                expected_type = method_info->parameters[i].type;
            } else if (has_symbol_param_info && i < method_symbol.parameters.size()) {
                expected_type = method_symbol.parameters[i].type;
            }
            expected_types.push_back(expected_type);
        }
        
        // Use ARM64 ABI compliant argument coercion (starting from X1/D0 since X0 is 'this')
        // We need to manually handle the offset since method args start at position 1
        std::vector<VarType> method_expected_types;
        for (size_t i = 0; i < expected_types.size(); ++i) {
            method_expected_types.push_back(expected_types[i]);
        }
        
        // For method calls, we need to manually place arguments starting from X1/D0
        // since coerce_arguments_to_abi assumes arguments start from X0/D0
        int ngrn = 1; // Start from X1 (X0 is 'this')
        int nsrn = 0; // Start from D0
        
        for (size_t i = 0; i < arg_value_regs.size(); ++i) {
            VarType expr_type = arg_types[i];
            VarType expected_type = expected_types[i];
            VarType target_type = (expected_type != VarType::UNKNOWN) ? expected_type : expr_type;
            
            const std::string& src_reg = arg_value_regs[i];
            
            if (target_type == VarType::FLOAT) {
                if (nsrn >= 8) {
                    throw std::runtime_error("Too many float arguments for ARM64 ABI (max 8 D registers)");
                }
                
                std::string dest_d_reg = "D" + std::to_string(nsrn);
                
                if (register_manager_.is_fp_register(src_reg)) {
                    if (src_reg != dest_d_reg) {
                        emit(Encoder::create_fmov_reg(dest_d_reg, src_reg));
                    }
                } else {
                    emit(Encoder::create_scvtf_reg(dest_d_reg, src_reg));
                }
                nsrn++;
            } else {
                if (ngrn >= 8) {
                    throw std::runtime_error("Too many integer arguments for ARM64 ABI (max 8 X registers)");
                }
                
                std::string dest_x_reg = "X" + std::to_string(ngrn);
                
                if (register_manager_.is_fp_register(src_reg)) {
                    generate_float_to_int_truncation(dest_x_reg, src_reg);
                } else {
                    if (src_reg != dest_x_reg) {
                        emit(Encoder::create_mov_reg(dest_x_reg, src_reg));
                    }
                }
                ngrn++;
            }
            
            register_manager_.release_register(src_reg);
        }

        // --- FIX: Direct call for constructors ---
        if (member_access->member_name == "CREATE") {
            debug_print("Detected a direct constructor call. Performing non-virtual call.");

            std::string class_name = get_class_name_for_expression(member_access->object_expr.get());
            if (class_name.empty()) {
                throw std::runtime_error("Constructor call without a known class type.");
            }
            std::string mangled_name = NameMangler::mangleMethod(class_name, member_access->member_name);

            emit(Encoder::create_branch_with_link(mangled_name));
            register_manager_.invalidate_caller_saved_registers();
        } else {
            // --- Stage 5: Decide between direct and virtual call based on method_info ---
            if (method_info->is_virtual && !method_info->is_final) {
                // Virtual call: vtable lookup
                size_t vtable_offset = method_info->vtable_slot * 8;

                std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
                std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);

                emit(Encoder::create_ldr_imm(vtable_ptr_reg, "X0", 0, "Load vtable pointer for method call"));
                emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, "Load method address for " + member_access->member_name));
                register_manager_.release_register(vtable_ptr_reg);

                emit(Encoder::create_branch_with_link_register(method_addr_reg));
                register_manager_.release_register(method_addr_reg);
                register_manager_.invalidate_caller_saved_registers();
            } else {
                // Direct call for non-virtual or final methods
                emit(Encoder::create_branch_with_link(method_info->qualified_name));
                register_manager_.invalidate_caller_saved_registers();
            }
        }

    } else if (auto* super_access = dynamic_cast<SuperMethodAccessExpression*>(node.routine_expr.get())) {
        // --- THIS IS A SUPER CALL (e.g., SUPER.CREATE(...)) ---
        debug_print("Detected a SUPER method call.");

        // --- Stage 1: Evaluate all arguments into temporary registers ---
        std::vector<std::string> arg_value_regs;
        for (const auto& arg_expr : node.arguments) {
            generate_expression_code(*arg_expr);
            std::string temp_reg;
            if (register_manager_.is_fp_register(expression_result_reg_)) {
                temp_reg = register_manager_.acquire_spillable_fp_temp_reg(*this);
                emit(Encoder::create_fmov_reg(temp_reg, expression_result_reg_));
            } else {
                temp_reg = register_manager_.acquire_spillable_temp_reg(*this);
                emit(Encoder::create_mov_reg(temp_reg, expression_result_reg_));
            }
            register_manager_.release_register(expression_result_reg_);
            arg_value_regs.push_back(temp_reg);
        }

        // --- Stage 2: Get the '_this' pointer from its home register ---
        std::string this_ptr_reg = get_variable_register("_this");
        emit(Encoder::create_mov_reg("X0", this_ptr_reg));

        // --- Stage 3: Prepare argument type information for ARM64 ABI coercion ---
        std::vector<VarType> arg_types;
        std::vector<VarType> expected_types;
        
        // Look up parent method signature for parameter types
        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
        ClassMethodInfo* method_info = nullptr;
        Symbol method_symbol;
        bool has_symbol_param_info = false;
        
        if (class_entry && !class_entry->parent_name.empty()) {
            const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
            if (parent_entry) {
                method_info = parent_entry->findMethod(super_access->member_name, false);
                // Also try to get parameter info from symbol table
                std::string method_qualified_name = class_entry->parent_name + "::" + super_access->member_name;
                has_symbol_param_info = symbol_table_->lookup(method_qualified_name, method_symbol);
            }
        }

        for (size_t i = 0; i < arg_value_regs.size(); ++i) {
            // Infer the actual type of this argument expression
            VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
            arg_types.push_back(arg_expr_type);
            
            // Determine expected parameter type based on parent method signature
            VarType expected_type = VarType::UNKNOWN;
            if (method_info && i < method_info->parameters.size()) {
                expected_type = method_info->parameters[i].type;
            } else if (has_symbol_param_info && i < method_symbol.parameters.size()) {
                expected_type = method_symbol.parameters[i].type;
            }
            expected_types.push_back(expected_type);
        }
        
        // Use ARM64 ABI compliant argument coercion (starting from X1/D0 since X0 is 'this')
        int ngrn = 1; // Start from X1 (X0 is 'this')
        int nsrn = 0; // Start from D0
        
        for (size_t i = 0; i < arg_value_regs.size(); ++i) {
            VarType expr_type = arg_types[i];
            VarType expected_type = expected_types[i];
            VarType target_type = (expected_type != VarType::UNKNOWN) ? expected_type : expr_type;
            
            const std::string& src_reg = arg_value_regs[i];
            
            if (target_type == VarType::FLOAT) {
                if (nsrn >= 8) {
                    throw std::runtime_error("Too many float arguments for ARM64 ABI (max 8 D registers)");
                }
                
                std::string dest_d_reg = "D" + std::to_string(nsrn);
                
                if (register_manager_.is_fp_register(src_reg)) {
                    if (src_reg != dest_d_reg) {
                        emit(Encoder::create_fmov_reg(dest_d_reg, src_reg));
                    }
                } else {
                    emit(Encoder::create_scvtf_reg(dest_d_reg, src_reg));
                }
                nsrn++;
            } else {
                if (ngrn >= 8) {
                    throw std::runtime_error("Too many integer arguments for ARM64 ABI (max 8 X registers)");
                }
                
                std::string dest_x_reg = "X" + std::to_string(ngrn);
                
                if (register_manager_.is_fp_register(src_reg)) {
                    generate_float_to_int_truncation(dest_x_reg, src_reg);
                } else {
                    if (src_reg != dest_x_reg) {
                        emit(Encoder::create_mov_reg(dest_x_reg, src_reg));
                    }
                }
                ngrn++;
            }
            
            register_manager_.release_register(src_reg);
        }

        // --- Stage 4: Find the parent method and make a direct call ---
        if (!class_entry || class_entry->parent_name.empty())
            throw std::runtime_error("SUPER call in class with no parent: " + current_class_name_);
        const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
        if (!parent_entry)
            throw std::runtime_error("Parent class not found: " + class_entry->parent_name);

        if (!method_info)
            throw std::runtime_error("SUPER: Parent method '" + super_access->member_name + "' not found.");

        emit(Encoder::create_branch_with_link(method_info->qualified_name));
        register_manager_.invalidate_caller_saved_registers();

    } else {
        // --- THIS IS A REGULAR FUNCTION/ROUTINE CALL (e.g., WRITEN(x)) ---
        debug_print("Detected a regular function/routine call.");

        // Check if this is a special built-in like WRITEF
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
            if (var_access->name == "WRITEF") {
                // Handle WRITEF specially - it uses its own ABI where float arguments 
                // are passed in X registers (not D registers) to carry type information
                if (node.arguments.empty()) {
                    throw std::runtime_error("WRITEF requires at least a format string argument.");
                }
                
                // Validate format string and argument types
                validate_writef_call(node.arguments);
                
                // Determine the actual function to call based on argument count
                std::string actual_func_name;
                if (node.arguments.size() == 1) {
                    actual_func_name = "WRITEF";
                } else {
                    actual_func_name = "WRITEF" + std::to_string(node.arguments.size() - 1);
                }
                
                // Check if the runtime function is registered
                if (!RuntimeManager::instance().is_function_registered(actual_func_name)) {
                    throw std::runtime_error("WRITEF with " + std::to_string(node.arguments.size() - 1) + 
                                            " arguments is not supported (max 7 arguments).");
                }
                
                // Evaluate and move arguments into ABI registers
                for (size_t i = 0; i < node.arguments.size(); ++i) {
                    generate_expression_code(*node.arguments[i]);
                    std::string arg_reg = expression_result_reg_;
                    std::string dest_reg = "X" + std::to_string(i);
                    
                    // Handle float argument passing for WRITEF
                    // Note: WRITEF uses a special ABI where float arguments are passed in X registers
                    // (not D registers) to carry type information in the format string. We preserve
                    // the bit pattern by moving from D register to X register without conversion.
                    if (register_manager_.is_fp_register(arg_reg)) {
                        std::string temp_reg = register_manager_.acquire_scratch_reg(*this);
                        emit(Encoder::create_fmov_d_to_x(temp_reg, arg_reg));
                        register_manager_.release_register(arg_reg);
                        arg_reg = temp_reg;
                    }
                    
                    if (arg_reg != dest_reg) {
                        emit(Encoder::create_mov_reg(dest_reg, arg_reg));
                        register_manager_.release_register(arg_reg);
                    }
                }
                // Check if function exists for the computed function name
                
                // Call the appropriate WRITEF function using veneer system
                if (veneer_manager_.has_veneer(actual_func_name)) {
                    std::string veneer_label = actual_func_name + "_veneer";
                    emit(Encoder::create_branch_with_link(veneer_label));
                } else {
                    // DEBUG: Fallback to old method - X19 table access
                    std::cerr << "[DEBUG FALLBACK] WRITEF variant '" << actual_func_name 
                              << "' not found in veneer system, using X19 fallback" << std::endl;
                    std::cerr << "[DEBUG FALLBACK] Available veneers: ";
                    const auto& veneer_labels = veneer_manager_.get_veneer_labels();
                    for (const auto& pair : veneer_labels) {
                        std::cerr << pair.first << " ";
                    }
                    std::cerr << std::endl;
                    
                    size_t offset = RuntimeManager::instance().get_function_offset(actual_func_name);
                    std::string addr_reg = register_manager_.acquire_scratch_reg(*this);
                    
                    Instruction ldr_instr = Encoder::create_ldr_imm(addr_reg, "X19", offset);
                    ldr_instr.jit_attribute = JITAttribute::JitAddress;
                    emit(ldr_instr);
                    
                    Instruction blr_instr = Encoder::create_branch_with_link_register(addr_reg);
                    blr_instr.jit_attribute = JITAttribute::JitCall;
                    blr_instr.target_label = actual_func_name;
                    emit(blr_instr);
                    register_manager_.release_register(addr_reg);
                }
                register_manager_.invalidate_caller_saved_registers();
                
            } else {
                // Regular function/routine call handling with ARM64 ABI compliant argument coercion
                
                // --- Stage 1: Evaluate all arguments and preserve their result registers ---
                // FIXED: Two-pass strategy to prevent register clobbering
                std::vector<std::string> arg_result_regs;
                for (const auto& arg_expr : node.arguments) {
                    generate_expression_code(*arg_expr);
                    // DO NOT release expression_result_reg_ yet! Save it for later.
                    // This prevents the register manager from reusing it for subsequent arguments.
                    arg_result_regs.push_back(expression_result_reg_);
                }
                
                // --- Stage 2: Collect type information for ARM64 ABI coercion ---
                Symbol routine_symbol;
                bool has_param_info = false;
                if (symbol_table_->lookup(var_access->name, routine_symbol)) {
                    has_param_info = true;
                }
                
                std::vector<VarType> arg_types;
                std::vector<VarType> expected_types;
                
                for (size_t i = 0; i < arg_result_regs.size(); ++i) {
                    // Infer the actual type of this argument expression
                    VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
                    arg_types.push_back(arg_expr_type);
                    
                    // Determine expected parameter type based on routine signature
                    VarType expected_type = VarType::UNKNOWN;
                    if (has_param_info && i < routine_symbol.parameters.size()) {
                        expected_type = routine_symbol.parameters[i].type;
                    }
                    expected_types.push_back(expected_type);
                }
                
                // --- Stage 3: Use ARM64 ABI compliant argument coercion ---
                coerce_arguments_to_abi(arg_result_regs, arg_types, expected_types);
                
                // Note: coerce_arguments_to_abi() will release the source registers internally

                // --- Stage 4: Perform the routine call (BL or BLR) ---
                if (ASTAnalyzer::getInstance().get_function_metrics().count(var_access->name)) {
                    // Internal function - use regular BL
                    emit(Encoder::create_branch_with_link(var_access->name));
                } else if (veneer_manager_.has_veneer(var_access->name)) {
                    // External function with veneer - use standard BL with veneer label
                    std::string veneer_label = var_access->name + "_veneer";
                    emit(Encoder::create_branch_with_link(veneer_label));
                } else if (RuntimeManager::instance().is_function_registered(var_access->name)) {
                    // DEBUG: Fallback to old method (should not happen with single-buffer approach)
                    std::cerr << "[DEBUG FALLBACK] Function '" << var_access->name 
                              << "' registered but not found in veneer system, using X19 fallback" << std::endl;
                    std::cerr << "[DEBUG FALLBACK] Available veneers: ";
                    const auto& veneer_labels = veneer_manager_.get_veneer_labels();
                    for (const auto& pair : veneer_labels) {
                        std::cerr << pair.first << " ";
                    }
                    std::cerr << std::endl;
                    
                    size_t offset = RuntimeManager::instance().get_function_offset(var_access->name);
                    std::string addr_reg = register_manager_.acquire_scratch_reg(*this);

                    Instruction ldr_instr = Encoder::create_ldr_imm(addr_reg, "X19", offset);
                    ldr_instr.jit_attribute = JITAttribute::JitAddress;
                    emit(ldr_instr);

                    Instruction blr_instr = Encoder::create_branch_with_link_register(addr_reg);
                    blr_instr.jit_attribute = JITAttribute::JitCall;
                    blr_instr.target_label = var_access->name;
                    emit(blr_instr);
                    register_manager_.release_register(addr_reg);
                } else {
                    // Check if this is a variable that could be a function pointer
                    Symbol symbol;
                    if (symbol_table_->lookup(var_access->name, symbol)) {
                        debug_print("Found as variable/function pointer");
                        generate_expression_code(*node.routine_expr);
                        emit(Encoder::create_branch_with_link_register(expression_result_reg_));
                        register_manager_.release_register(expression_result_reg_);
                    } else {
                        debug_print("Routine not found anywhere - throwing error");
                        throw std::runtime_error("ERROR: Routine '" + var_access->name + "' is not defined. " +
                            "Check if it's declared, defined, or registered as a runtime function.");
                    }
                }
                register_manager_.invalidate_caller_saved_registers();
            }
        } else {
            // Handle non-variable access routine calls (function pointer calls)
            
            // --- Stage 1: Evaluate all arguments and preserve their result registers ---
            // FIXED: Two-pass strategy to prevent register clobbering
            std::vector<std::string> arg_result_regs;
            for (const auto& arg_expr : node.arguments) {
                generate_expression_code(*arg_expr);
                // DO NOT release expression_result_reg_ yet! Save it for later.
                // This prevents the register manager from reusing it for subsequent arguments.
                arg_result_regs.push_back(expression_result_reg_);
            }
            
            // --- Stage 2: Collect argument types (no expected types for function pointers) ---
            std::vector<VarType> arg_types;
            for (size_t i = 0; i < arg_result_regs.size(); ++i) {
                VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
                arg_types.push_back(arg_expr_type);
            }
            
            // --- Stage 3: Use ARM64 ABI compliant argument coercion (natural types) ---
            coerce_arguments_to_abi(arg_result_regs, arg_types);
            
            // Note: coerce_arguments_to_abi() will release the source registers internally
            
            // --- Stage 4: Call the function pointer ---
            generate_expression_code(*node.routine_expr);
            emit(Encoder::create_branch_with_link_register(expression_result_reg_));
            register_manager_.release_register(expression_result_reg_);
            register_manager_.invalidate_caller_saved_registers();
        }
    }

    // Phase 4: Manual restore removed - callee-saved registers preserved automatically

    // Routines don't have a return value, but the call clobbers X0.
    register_manager_.mark_register_as_used("X0");

    debug_print("--- Exiting NewCodeGenerator::visit(RoutineCallStatement& node) ---");
}
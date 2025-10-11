#include "../NewCodeGenerator.h"
#include "../LabelManager.h"
#include "../analysis/ASTAnalyzer.h"
#include <stdexcept>
#include "CodeGenUtils.h"
#include "../runtime/ListDataTypes.h"

// In generators/gen_FunctionCall.cpp

// emit_push_reg and emit_pop_reg are now defined in CodeGenUtils.h for shared use.

void NewCodeGenerator::visit(FunctionCall& node) {
    debug_print("Visiting FunctionCall node (Refactored Dispatcher).");

    // Phase 4: Manual spilling removed - LinearScanAllocator ensures call-crossing
    // variables are allocated to callee-saved registers (preserved automatically)

    // --- STEP 1: Evaluate All Arguments FIRST ---
    std::vector<std::string> arg_result_regs;
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
        arg_result_regs.push_back(temp_reg);
    }

    // --- STEP 2: Dispatch to the Correct Handler ---
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        const std::string& func_name = var_access->name;
        bool is_method_call = false;
        bool is_transformed_super_call = false;

        // Check if this is a transformed SUPER call (format: "ClassName::MethodName")
        size_t double_colon_pos = func_name.find("::");
        if (double_colon_pos != std::string::npos) {
            std::string class_name = func_name.substr(0, double_colon_pos);
            std::string method_name = func_name.substr(double_colon_pos + 2);
            
            // Verify this is actually a class method
            if (class_table_ && class_table_->lookup_class_method(class_name, method_name)) {
                is_transformed_super_call = true;
                debug_print("Detected transformed SUPER call: " + class_name + "::" + method_name);
            }
        }

        // Check if we are inside a class method context and if the name
        // corresponds to a method in that class.
        if (!current_class_name_.empty() && class_table_) {
            if (class_table_->lookup_class_method(current_class_name_, func_name)) {
                is_method_call = true;
            }
        }

        if (is_transformed_super_call) {
            // Handle transformed SUPER calls using method call logic for argument placement
            debug_print("Routing transformed SUPER call to method handler for proper argument placement");
            handle_method_call_arguments_for_super(node, arg_result_regs, func_name);
        } else if (is_method_call) {
            // Transform it into an explicit MemberAccessExpression on the fly
            // and dispatch to the correct handler.
            debug_print("Implicit 'this' call detected for: " + func_name);
            
            auto this_expr = std::make_unique<VariableAccess>("_this");
            auto member_access = std::make_unique<MemberAccessExpression>(std::move(this_expr), func_name);
            
            // Temporarily replace the node's function expression to reuse the method handler
            auto original_expr = std::move(node.function_expr);
            node.function_expr = std::move(member_access);

            handle_method_call(node, arg_result_regs); // Use the existing, correct method handler

            // Restore the original expression for AST consistency
            node.function_expr = std::move(original_expr);

        } else if (is_special_built_in(func_name)) {
            handle_special_built_in_call(node, arg_result_regs);
        } else {
            handle_regular_call(node, arg_result_regs);
        }
    } else if (node.function_expr->getType() == ASTNode::NodeType::MemberAccessExpr) {
        handle_method_call(node, arg_result_regs);
    } else if (node.function_expr->getType() == ASTNode::NodeType::SuperMethodAccessExpr) {
        handle_super_call(node, arg_result_regs);
    } else {
        // Fallback for complex expressions that resolve to a function pointer
        handle_regular_call(node, arg_result_regs);
    }

    // Phase 4: Manual restore removed - callee-saved registers preserved automatically
}

void NewCodeGenerator::handle_method_call_arguments_for_super(FunctionCall& node, const std::vector<std::string>& arg_result_regs, const std::string& func_name) {
    debug_print("Handling method call arguments for transformed SUPER call: " + func_name);

    if (arg_result_regs.empty()) {
        throw std::runtime_error("Transformed SUPER call missing _this argument");
    }

    // For transformed SUPER calls, we need method call argument placement:
    // X0 = _this (first argument), X1 = second argument, X2 = third argument, etc.
    
    // Set up _this pointer in X0
    std::string this_ptr_reg = arg_result_regs[0];
    emit(Encoder::create_mov_reg_comment("X0", this_ptr_reg, "_this pointer"));
    register_manager_.release_register(this_ptr_reg);

    // Set up remaining arguments starting from X1 (method call convention)
    for (size_t i = 1; i < arg_result_regs.size(); ++i) {
        std::string target_reg = "X" + std::to_string(i);
        std::string param_comment = "Parameter " + std::to_string(i - 1) + " (method call)";
        
        emit(Encoder::create_mov_reg_comment(target_reg, arg_result_regs[i], param_comment));
        register_manager_.release_register(arg_result_regs[i]);
    }

    // Use direct BL for transformed SUPER calls (no vtable lookup needed)
    emit(Encoder::create_branch_with_link(func_name));
    register_manager_.invalidate_caller_saved_registers();

    // Set result register to X0 (assuming integer return type)
    expression_result_reg_ = "X0";

    debug_print("Successfully handled transformed SUPER call arguments");
}

// ============================================================================
// Helper Function Implementations
// ============================================================================

bool NewCodeGenerator::is_special_built_in(const std::string& func_name) {
    static const std::unordered_set<std::string> built_ins = {
        "AS_INT", "AS_FLOAT", "AS_STRING", "AS_LIST",
        "FIND", "MAP", "FILTER",
    };
    return built_ins.count(func_name);
}

void NewCodeGenerator::handle_special_built_in_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    auto* var_access = static_cast<VariableAccess*>(node.function_expr.get());
    const std::string& function_name = var_access->name;
    
    // MIN/MAX/SUM are now handled as regular function calls that get converted to statements during parsing

    if (function_name == "AS_INT" || function_name == "AS_FLOAT" || function_name == "AS_STRING" || function_name == "AS_LIST") {
        if (node.arguments.size() != 1) {
            throw std::runtime_error(function_name + " expects exactly one argument.");
        }
        generate_expression_code(*node.arguments[0]);
        std::string ptr_reg = expression_result_reg_;
        std::string tag_reg = register_manager_.acquire_scratch_reg(*this);
        std::string good_type_label = label_manager_.create_label();
        int64_t expected_tag = 0;
        if (function_name == "AS_INT") expected_tag = ATOM_INT;
        if (function_name == "AS_FLOAT") expected_tag = ATOM_FLOAT;
        if (function_name == "AS_STRING") expected_tag = ATOM_STRING;
        if (function_name == "AS_LIST") expected_tag = ATOM_LIST_POINTER;
        emit(Encoder::create_ldr_imm(tag_reg, ptr_reg, 0, "Load runtime type tag"));
        emit(Encoder::create_cmp_imm(tag_reg, expected_tag));
        register_manager_.release_register(tag_reg);
        emit(Encoder::create_branch_conditional("EQ", good_type_label));
        emit(Encoder::create_brk(1));
        instruction_stream_.define_label(good_type_label);
        if (function_name == "AS_LIST" || function_name == "AS_STRING") {
            std::string dest_x_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(dest_x_reg, ptr_reg, 8));
            if (function_name == "AS_STRING") {
                emit(Encoder::create_add_imm(dest_x_reg, dest_x_reg, 8));
            }
            expression_result_reg_ = dest_x_reg;
        } else if (function_name == "AS_FLOAT") {
            std::string dest_d_reg = register_manager_.acquire_fp_scratch_reg();
            emit(Encoder::create_ldr_fp_imm(dest_d_reg, ptr_reg, 8));
            expression_result_reg_ = dest_d_reg;
        } else {
            std::string dest_x_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(dest_x_reg, ptr_reg, 8));
            expression_result_reg_ = dest_x_reg;
        }
        register_manager_.release_register(ptr_reg);
        return;
    }

    if (function_name == "FIND" && node.arguments.size() == 2) {
        generate_expression_code(*node.arguments[0]);
        emit(Encoder::create_mov_reg("X0", expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
        generate_expression_code(*node.arguments[1]);
        std::string value_reg = expression_result_reg_;
        VarType value_type = infer_expression_type_local(node.arguments[1].get());
        int64_t type_tag = (value_type == VarType::FLOAT) ? ATOM_FLOAT : ATOM_INT;
        if (register_manager_.is_fp_register(value_reg)) {
            emit(Encoder::create_fmov_reg("X1", value_reg));
        } else {
            emit(Encoder::create_mov_reg("X1", value_reg));
        }
        register_manager_.release_register(value_reg);
        emit(Encoder::create_movz_movk_abs64("X2", type_tag, ""));
        emit(Encoder::create_branch_with_link("FIND"));
        expression_result_reg_ = "X0";
        return;
    }

    if (function_name == "MAP" && node.arguments.size() == 2) {
        generate_expression_code(*node.arguments[0]);
        emit(Encoder::create_mov_reg("X0", expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
        if (auto* map_var = dynamic_cast<VariableAccess*>(node.arguments[1].get())) {
            std::string map_name = map_var->name;
            emit(Encoder::create_adrp("X1", map_name));
            emit(Encoder::create_add_literal("X1", "X1", map_name));
        } else {
            throw std::runtime_error("Mapping function for MAP must be a function name.");
        }
        emit(Encoder::create_branch_with_link("MAP"));
        expression_result_reg_ = "X0";
        return;
    }

    if (function_name == "FILTER" && node.arguments.size() == 2) {
        generate_expression_code(*node.arguments[0]);
        emit(Encoder::create_mov_reg("X0", expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
        if (auto* predicate_var = dynamic_cast<VariableAccess*>(node.arguments[1].get())) {
            std::string predicate_name = predicate_var->name;
            emit(Encoder::create_adrp("X1", predicate_name));
            emit(Encoder::create_add_literal("X1", "X1", predicate_name));
        } else {
            throw std::runtime_error("Predicate for FILTER must be a function name.");
        }
        emit(Encoder::create_branch_with_link("FILTER"));
        expression_result_reg_ = "X0";
        return;
    }



    throw std::runtime_error("Unknown special built-in: " + function_name);
}

void NewCodeGenerator::handle_method_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    auto* member_access = static_cast<MemberAccessExpression*>(node.function_expr.get());
    std::string this_ptr_reg;
    std::string class_name;
    std::string method_name;
    generate_expression_code(*member_access->object_expr);
    this_ptr_reg = expression_result_reg_;
    class_name = get_class_name_for_expression(member_access->object_expr.get());
    method_name = member_access->member_name;
    ClassMethodInfo* method_info = class_table_->lookup_class_method(class_name, method_name);
    if (!method_info) {
        throw std::runtime_error("Method '" + method_name + "' not found in class '" + class_name + "'.");
    }
    size_t vtable_offset = method_info->vtable_slot * 8;
    std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_ldr_imm(vtable_ptr_reg, this_ptr_reg, 0, "Load vtable pointer"));
    std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);
    emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, "Load method address"));
    register_manager_.release_register(vtable_ptr_reg);
    emit(Encoder::create_mov_reg("X0", this_ptr_reg));
    for (size_t i = 0; i < arg_result_regs.size(); ++i) {
        std::string target_reg = "X" + std::to_string(i + 1);
        emit(Encoder::create_mov_reg(target_reg, arg_result_regs[i]));
        register_manager_.release_register(arg_result_regs[i]);
    }
    register_manager_.release_register(this_ptr_reg);
    emit(Encoder::create_branch_with_link_register(method_addr_reg));
    register_manager_.invalidate_caller_saved_registers();
    register_manager_.release_register(method_addr_reg);
    
    // Set the result register based on the method's return type.
    std::string mangled_name = class_name + "::" + method_name;
    Symbol method_symbol;
    if (symbol_table_ && symbol_table_->lookup(mangled_name, method_symbol)) {
        if (method_symbol.type == VarType::FLOAT) {
            expression_result_reg_ = "D0";
        } else {
            expression_result_reg_ = "X0";
        }
    } else {
        // Fallback: default to X0
        expression_result_reg_ = "X0";
    }
}

void NewCodeGenerator::handle_super_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    auto* super_access = static_cast<SuperMethodAccessExpression*>(node.function_expr.get());
    std::string this_ptr_reg = get_variable_register("_this");
    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
    if (!class_entry || class_entry->parent_name.empty()) {
        throw std::runtime_error("SUPER call in class with no parent: " + current_class_name_);
    }
    const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
    if (!parent_entry) {
        throw std::runtime_error("Parent class not found: " + class_entry->parent_name);
    }
    ClassMethodInfo* method_info = parent_entry->findMethod(super_access->member_name, false);
    if (!method_info) {
        throw std::runtime_error("SUPER: Parent method '" + super_access->member_name + "' not found.");
    }
    emit(Encoder::create_mov_reg("X0", this_ptr_reg));
    for (size_t i = 0; i < arg_result_regs.size(); ++i) {
        std::string target_reg = "X" + std::to_string(i + 1);
        emit(Encoder::create_mov_reg(target_reg, arg_result_regs[i]));
        register_manager_.release_register(arg_result_regs[i]);
    }
    register_manager_.release_register(this_ptr_reg);
    emit(Encoder::create_branch_with_link(method_info->qualified_name));
    register_manager_.invalidate_caller_saved_registers();
    expression_result_reg_ = "X0";
}

void NewCodeGenerator::handle_regular_call(FunctionCall& node, const std::vector<std::string>& arg_result_regs) {
    std::string function_name;
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        function_name = var_access->name;
    }
    
    // FAIL-FAST: Check if function exists before doing any work
    if (!function_name.empty()) {
        bool function_exists = false;
        
        // Check internal functions
        if (ASTAnalyzer::getInstance().get_function_metrics().count(function_name) > 0) {
            function_exists = true;
        }
        // Check external functions with veneers
        else if (veneer_manager_.has_veneer(function_name)) {
            function_exists = true;
        }
        // Check runtime functions
        else if (RuntimeManager::instance().is_function_registered(function_name)) {
            function_exists = true;
        }
        // Check if it's a variable/function pointer
        else {
            Symbol symbol;
            if (symbol_table_->lookup(function_name, symbol)) {
                function_exists = true;
            }
        }
        
        if (!function_exists) {
            throw std::runtime_error("Compilation Error: Function '" + function_name + "' is not defined.\n" +
                "This function was not found in:\n" +
                "  - User-defined functions\n" +
                "  - Built-in/runtime functions\n" +
                "  - External function veneers\n" +
                "  - Variable/function pointer symbols\n" +
                "Please check your spelling or ensure the function is declared/imported.");
        }
    }
    
    bool is_float_call = is_float_function_call(node);
    
    // Debug output for float function detection
    // std::cerr << "[DEBUG REGULAR_CALL] Function: " << function_name << ", is_float_call: " << is_float_call << std::endl;
    
    // Look up parameter types for this function
    Symbol function_symbol;
    bool has_param_info = false;
    if (!function_name.empty() && symbol_table_->lookup(function_name, function_symbol)) {
        has_param_info = true;
        debug_print("Found function symbol: " + function_name + " with " + std::to_string(function_symbol.parameters.size()) + " parameters");
        for (size_t i = 0; i < function_symbol.parameters.size(); ++i) {
            debug_print("  Parameter " + std::to_string(i) + " type: " + vartype_to_string(function_symbol.parameters[i].type));
        }
    } else {
        debug_print("Function symbol not found for: " + function_name);
    }
    
    // Collect argument types for ARM64 ABI compliant coercion
    std::vector<VarType> arg_types;
    std::vector<VarType> expected_types;
    
    for (size_t i = 0; i < arg_result_regs.size(); ++i) {
        // Infer the actual type of this argument expression
        VarType arg_expr_type = infer_expression_type_local(node.arguments[i].get());
        arg_types.push_back(arg_expr_type);
        
        // Determine expected parameter type based on function signature
        VarType expected_type = VarType::UNKNOWN;
        if (has_param_info && i < function_symbol.parameters.size()) {
            expected_type = function_symbol.parameters[i].type;
        }
        expected_types.push_back(expected_type);
    }
    
    // Use the new ARM64 ABI compliant argument coercion
    coerce_arguments_to_abi(arg_result_regs, arg_types, expected_types);
    
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        if (ASTAnalyzer::getInstance().get_function_metrics().count(var_access->name) > 0) {
            // Internal function - use regular BL
            emit(Encoder::create_branch_with_link(var_access->name));
        } else if (veneer_manager_.has_veneer(var_access->name)) {
            // External function with veneer - use standard BL with veneer label
            std::string veneer_label = var_access->name + "_veneer";
            emit(Encoder::create_branch_with_link(veneer_label));
            
        } else if (RuntimeManager::instance().is_function_registered(var_access->name)) {
            // Fallback to old method (should not happen with single-buffer approach)
            size_t offset = RuntimeManager::instance().get_function_offset(var_access->name);
            std::string addr_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(addr_reg, "X19", offset));
            emit(Encoder::create_branch_with_link_register(addr_reg));
            register_manager_.release_register(addr_reg);
        } else {
            // Check if this is a variable that could be a function pointer
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol)) {
                std::cerr << "DEBUG: Found as variable/function pointer" << std::endl;
                generate_expression_code(*var_access);
                emit(Encoder::create_branch_with_link_register(expression_result_reg_));
                register_manager_.release_register(expression_result_reg_);
            } else {
                // This should never happen now due to fail-fast check above
                std::cerr << "INTERNAL ERROR: Function validation failed - this should have been caught earlier" << std::endl;
                throw std::runtime_error("Internal compiler error: Function '" + var_access->name + "' validation failed.");
            }
        }
    } else {
        generate_expression_code(*node.function_expr);
        emit(Encoder::create_branch_with_link_register(expression_result_reg_));
        register_manager_.release_register(expression_result_reg_);
    }
    
    if (is_float_call) {
        // std::cerr << "[DEBUG REGULAR_CALL] Setting result register to D0 for float function: " << function_name << std::endl;
        expression_result_reg_ = "D0";
    } else {
        // std::cerr << "[DEBUG REGULAR_CALL] Setting result register to X0 for non-float function: " << function_name << std::endl;
        expression_result_reg_ = "X0";
    }
}



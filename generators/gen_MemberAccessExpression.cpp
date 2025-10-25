#include "NewCodeGenerator.h"
#include "AST.h"
#include "analysis/ASTAnalyzer.h"
#include "ClassTable.h"
#include "Encoder.h"
#include <stdexcept>
#include <iostream>

void NewCodeGenerator::visit(MemberAccessExpression& node) {
    debug_print("Visiting MemberAccessExpression for member: " + node.member_name);

    // STEP 1: Get type information for the object.
    Symbol object_symbol;
    std::string class_name;

    if (auto* var_access = dynamic_cast<VariableAccess*>(node.object_expr.get())) {
        debug_print("  Object is VariableAccess: " + var_access->name);
        if (symbol_table_ && symbol_table_->lookup(var_access->name, object_symbol)) {
            class_name = object_symbol.class_name;
            debug_print("  Found symbol with class name: " + class_name);
        } else {
            debug_print("  WARNING: Symbol lookup failed for: " + var_access->name);
        }
    } else {
        debug_print("  Object is not a VariableAccess, using get_class_name_for_expression");
        try {
            class_name = get_class_name_for_expression(node.object_expr.get());
        } catch (const std::exception& e) {
            debug_print("  get_class_name_for_expression failed: " + std::string(e.what()));
        }
    }
    
    if (class_name.empty()) {
        debug_print("  ERROR: Could not determine class name for object in member access");
        debug_print("  Member being accessed: " + node.member_name);
        
        // Set a safe dummy register and return to prevent crashes
        std::string dummy_reg = register_manager_.acquire_scratch_reg(*this);
        expression_result_reg_ = dummy_reg;
        return;
    }

    const ClassTableEntry* class_entry = class_table_ ? class_table_->get_class(class_name) : nullptr;
    if (!class_entry) {
        debug_print("  ERROR: Class '" + class_name + "' not found in ClassTable");
        
        // Set a safe dummy register and return to prevent crashes
        std::string dummy_reg = register_manager_.acquire_scratch_reg(*this);
        expression_result_reg_ = dummy_reg;
        return;
    }
    debug_print("  Successfully found class entry for: " + class_name);

    // STEP 2: Decide if this is a data member or a method.
    auto member_it = class_entry->member_variables.find(node.member_name);
    
    // Use the helper method to lookup the method (handles both simple and qualified names)
    debug_print("  Looking up class method '" + node.member_name + "' in class '" + class_name + "'");
    ClassMethodInfo* method_info_ptr = class_table_->lookup_class_method(class_name, node.member_name);
    
    debug_print("  Method lookup result: " + std::string(method_info_ptr != nullptr ? "FOUND" : "NOT FOUND"));

    if (member_it != class_entry->member_variables.end()) {
        // --- PATH A: DATA MEMBER (e.g., p.x) ---
        debug_print("Member '" + node.member_name + "' is a data member.");
        generate_expression_code(*node.object_expr);
        std::string object_ptr_reg = expression_result_reg_;
        size_t offset = member_it->second.offset;
        
        // Check if the member is a float type
        if (member_it->second.type == VarType::FLOAT) {
            debug_print("Member '" + node.member_name + "' is a FLOAT type.");
            std::string dest_reg = register_manager_.get_free_float_register();
            emit(Instruction(0, "// Load float member " + node.member_name + " with offset " + std::to_string(offset)));
            emit(Encoder::create_ldr_fp_imm(dest_reg, object_ptr_reg, offset));
            expression_result_reg_ = dest_reg;
        } else {
            std::string dest_reg = register_manager_.acquire_scratch_reg(*this);
            emit(Encoder::create_ldr_imm(dest_reg, object_ptr_reg, offset, "Load member " + node.member_name));
            expression_result_reg_ = dest_reg;
        }
        register_manager_.release_register(object_ptr_reg);

    } else if (method_info_ptr != nullptr) {
        // --- PATH B: METHOD (e.g., p.set) --- (ENHANCED VIRTUAL/NON-VIRTUAL LOGIC)
        const auto& method_info = *method_info_ptr;
        debug_print("Member '" + node.member_name + "' is a method.");
        debug_print("Method is_virtual: " + std::string(method_info.is_virtual ? "true" : "false"));

        // 1. Get the object's base address (the 'this' pointer).
        generate_expression_code(*node.object_expr);
        std::string object_ptr_reg = expression_result_reg_;

        // 2. The 'this' pointer must be passed as the first argument (X0).
        emit(Encoder::create_mov_reg("X0", object_ptr_reg));
        register_manager_.release_register(object_ptr_reg);

        if (method_info.is_virtual) {
            debug_print("Generating virtual method dispatch for: " + node.member_name);
            generate_virtual_method_call(method_info);
        } else {
            debug_print("Generating direct method call for: " + node.member_name);
            generate_direct_method_call(method_info);
        }
    } else {
        debug_print("ERROR: Member '" + node.member_name + "' not found in class '" + class_name + "'");
        
        // Set a safe dummy register and return to prevent crashes
        std::string dummy_reg = register_manager_.acquire_scratch_reg(*this);
        expression_result_reg_ = dummy_reg;
        return;
    }
}

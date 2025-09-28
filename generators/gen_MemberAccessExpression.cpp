#include "NewCodeGenerator.h"
#include "AST.h"
#include "analysis/ASTAnalyzer.h"
#include "ClassTable.h"
#include "Encoder.h"
#include <stdexcept>
#include <iostream>

// Emergency debug function for vtable validation
static void debug_validate_vtable_ptr(void* vtable_ptr, int slot_index, void* method_ptr) {
    if (vtable_ptr == method_ptr) {
        std::cerr << "\n!!! CRITICAL VTABLE ERROR DETECTED !!!" << std::endl;
        std::cerr << "Vtable at " << vtable_ptr << " has a self-reference at slot " << slot_index << std::endl;
        std::cerr << "This will cause a crash when calling methods" << std::endl;
    }
}

void NewCodeGenerator::visit(MemberAccessExpression& node) {
    debug_print("Visiting MemberAccessExpression for member: " + node.member_name);

    // STEP 1: Get type information for the object.
    ASTAnalyzer& analyzer = ASTAnalyzer::getInstance();
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
        class_name = get_class_name_for_expression(node.object_expr.get());
    }
    if (class_name.empty()) {
        throw std::runtime_error("Could not determine class for member access on '" + node.member_name + "'.");
    }

    const ClassTableEntry* class_entry = class_table_ ? class_table_->get_class(class_name) : nullptr;
    if (!class_entry) {
        debug_print("  ERROR: Class '" + class_name + "' not found in ClassTable");
        throw std::runtime_error("Class '" + class_name + "' not found in ClassTable.");
    }
    debug_print("  Successfully found class entry for: " + class_name);

    // STEP 2: Decide if this is a data member or a method.
    auto member_it = class_entry->member_variables.find(node.member_name);
    
    // Use the helper method to lookup the method (handles both simple and qualified names)
    debug_print("  Looking up class method '" + node.member_name + "' in class '" + class_name + "'");
    ClassMethodInfo* method_info_ptr = class_table_->lookup_class_method(class_name, node.member_name);
    
    debug_print("  Method lookup result: " + std::string(method_info_ptr != nullptr ? "FOUND" : "NOT FOUND"));
    
    // Print available methods for debugging
    debug_print("  Available member methods in class " + class_name + ":");
    for (const auto& method_pair : class_entry->member_methods) {
        debug_print("    - " + method_pair.first);
    }
    
    debug_print("  Available member variables in class " + class_name + ":");
    for (const auto& var_pair : class_entry->member_variables) {
        debug_print("    - " + var_pair.first + " (offset: " + std::to_string(var_pair.second.offset) + ")");
    }

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
        // --- PATH B: METHOD (e.g., p.set) --- (NEW LOGIC)
        debug_print("Member '" + node.member_name + "' is a method. Performing vtable lookup.");

        // 1. Get the object's base address (the 'this' pointer).
        generate_expression_code(*node.object_expr);
        std::string object_ptr_reg = expression_result_reg_;

        // 2. The 'this' pointer must be passed as the first argument (X0).
        emit(Encoder::create_mov_reg("X0", object_ptr_reg));
        register_manager_.release_register(object_ptr_reg);
        // register_manager_.reserve_register("X0"); // Prevent it from being used before the call

        // 3. Get the method's vtable slot
        const auto& method_info = *method_info_ptr;
        size_t vtable_slot = method_info.vtable_slot;
        size_t vtable_offset = vtable_slot * 8; // Each entry is an 8-byte pointer.

        debug_print("Method '" + node.member_name + "' found with vtable slot " + std::to_string(vtable_slot));
        debug_print("Method vtable offset: " + std::to_string(vtable_offset) + " bytes");
        debug_print("Full method info: name=" + method_info.name + ", qualified_name=" + method_info.qualified_name);
        
        // DETAILED VTABLE DIAGNOSTICS
        debug_print("VTABLE DEBUGGING INFO FOR: " + node.member_name);
        debug_print("  - Class: " + class_name);
        debug_print("  - Vtable blueprint size: " + std::to_string(class_entry->vtable_blueprint.size()));
        debug_print("  - Method's expected location in vtable: slot " + std::to_string(vtable_slot));
        
        // Print the entire vtable blueprint for this class
        debug_print("  - Vtable blueprint contents:");
        for (size_t i = 0; i < class_entry->vtable_blueprint.size(); i++) {
            debug_print("    [" + std::to_string(i) + "]: " + 
                      (class_entry->vtable_blueprint[i].empty() ? "<empty>" : class_entry->vtable_blueprint[i]));
        }

        // 4. Generate code to load the method address from the vtable.
        std::string vtable_ptr_reg = register_manager_.acquire_scratch_reg(*this);
        std::string method_addr_reg = register_manager_.acquire_scratch_reg(*this);

        // LDR vtable_ptr_reg, [X0, #0]  (Load address of vtable)
        emit(Encoder::create_ldr_imm(vtable_ptr_reg, "X0", 0, "Load vtable pointer"));
        // LDR method_addr_reg, [vtable_ptr_reg, #offset] (Load method address from vtable at specified slot)
        emit(Encoder::create_ldr_imm(method_addr_reg, vtable_ptr_reg, vtable_offset, "Load method address for " + node.member_name));

        register_manager_.release_register(vtable_ptr_reg);

        // The result of this expression is the callable address of the method.
        expression_result_reg_ = method_addr_reg;
    } else {
        throw std::runtime_error("Member '" + node.member_name + "' not found in class '" + class_name + 
                               "'. (Neither as a member variable nor as a method)");
    }
}

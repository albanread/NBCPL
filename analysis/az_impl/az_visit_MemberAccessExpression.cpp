#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../ClassTable.h"
#include <iostream>

void ASTAnalyzer::visit(MemberAccessExpression& node) {
    // First, visit the object expression
    if (node.object_expr) {
        node.object_expr->accept(*this);
    }

    // Skip access control if we don't have necessary components
    if (!class_table_ || !symbol_table_) {
        return;
    }

    // Try to determine the class of the object being accessed
    std::string object_class_name;
    
    // Case 1: Direct variable access (e.g., obj.member)
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.object_expr.get())) {
        // Look up the variable to find its type
        Symbol symbol;
        if (symbol_table_->lookup(var_access->name, symbol)) {
            // Allow either OBJECT or POINTER_TO_OBJECT for member access
            if (symbol.type == VarType::OBJECT || symbol.type == VarType::POINTER_TO_OBJECT) {
                // For object types, the class_name should contain the class name
                object_class_name = symbol.class_name;
            }
        }
    }
    
    // Case 2: _this access (method accessing own class members)
    else if (auto* var_access = dynamic_cast<VariableAccess*>(node.object_expr.get())) {
        if (var_access->name == "_this" || var_access->name == "SELF") {
            object_class_name = current_class_name_;
        }
    }

    // If we couldn't determine the object's class, we can't check access
    if (object_class_name.empty()) {
        return;
    }

    // Get the class definition
    ClassTableEntry* object_class = class_table_->get_class(object_class_name);
    if (!object_class) {
        return;
    }

    // Look up the member being accessed
    std::string member_name = node.member_name;
    
    // Check if it's a member variable
    auto member_var_it = object_class->member_variables.find(member_name);
    if (member_var_it != object_class->member_variables.end()) {
        const ClassMemberInfo& member_info = member_var_it->second;
        
        if (!check_member_access(member_info.visibility, object_class_name, current_class_name_)) {
            log_access_violation("variable", member_name, object_class_name, member_info.visibility);
        }
        return;
    }

    // Check if it's a member method
    auto method_it = object_class->simple_name_to_method.find(member_name);
    if (method_it != object_class->simple_name_to_method.end()) {
        const ClassMethodInfo* method_info = method_it->second;
        
        if (!check_member_access(method_info->visibility, object_class_name, current_class_name_)) {
            log_access_violation("method", member_name, object_class_name, method_info->visibility);
        }
        return;
    }

    // Member not found - this should be caught by other semantic analysis
}

bool ASTAnalyzer::check_member_access(Visibility member_visibility, 
                                     const std::string& defining_class, 
                                     const std::string& accessing_class) {
    switch (member_visibility) {
        case Visibility::Public:
            // Public members are always accessible
            return true;
            
        case Visibility::Private:
            // Private members are only accessible from the same class
            return accessing_class == defining_class;
            
        case Visibility::Protected:
            // Protected members are accessible from the same class or subclasses
            if (accessing_class == defining_class) {
                return true;
            }
            // Check if accessing_class is a descendant of defining_class
            return class_table_->is_descendant_of(accessing_class, defining_class);
    }
    
    return false; // Default to deny access
}

void ASTAnalyzer::log_access_violation(const std::string& member_type,
                                      const std::string& member_name,
                                      const std::string& defining_class,
                                      Visibility visibility) {
    std::string visibility_str;
    switch (visibility) {
        case Visibility::Public: visibility_str = "public"; break;
        case Visibility::Private: visibility_str = "private"; break;
        case Visibility::Protected: visibility_str = "protected"; break;
    }
    
    std::string error_msg = "[SEMANTIC ERROR] Cannot access " + visibility_str + " " + 
                           member_type + " '" + member_name + "' from class '" + 
                           defining_class + "'";
    
    if (current_class_name_.empty()) {
        error_msg += " (accessing from global scope)";
    } else {
        error_msg += " from class '" + current_class_name_ + "'";
    }
    
    semantic_errors_.push_back(error_msg);
    
    if (trace_enabled_) {
        std::cerr << error_msg << std::endl;
    }
}
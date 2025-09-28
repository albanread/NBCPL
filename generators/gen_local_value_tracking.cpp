#include "../NewCodeGenerator.h"
#include "../AST.h"
#include <iostream>

// Register that a variable holds the address of a canonical expression
void NewCodeGenerator::register_canonical_address(const std::string& var_name, const std::string& canonical_form) {
    debug_print("Registering canonical address: " + var_name + " = " + canonical_form);
    local_value_map_[var_name] = canonical_form;
}

// Check if a variable has a known address value
bool NewCodeGenerator::has_known_address_value(const std::string& var_name) const {
    return local_value_map_.find(var_name) != local_value_map_.end();
}

// Get the canonical form of the address stored in a variable
std::string NewCodeGenerator::get_canonical_address_form(const std::string& var_name) const {
    auto it = local_value_map_.find(var_name);
    if (it != local_value_map_.end()) {
        return it->second;
    }
    return "";
}

// Invalidate tracking for a specific variable (when it's reassigned)
void NewCodeGenerator::invalidate_variable_tracking(const std::string& var_name) {
    auto it = local_value_map_.find(var_name);
    if (it != local_value_map_.end()) {
        debug_print("Invalidating tracking for variable: " + var_name + " (was " + it->second + ")");
        local_value_map_.erase(it);
    }
}

// Clear all local value tracking (called at function boundaries)
void NewCodeGenerator::clear_local_value_tracking() {
    debug_print("Clearing local value tracking for function: " + current_function_name_);
    local_value_map_.clear();
}

// Generate canonical form for an expression (similar to CSE's expression_to_string)
std::string NewCodeGenerator::get_expression_canonical_form(const Expression* expr) const {
    if (!expr) return "";
    
    switch (expr->getType()) {
        case ASTNode::NodeType::StringLit: {
            auto* str_lit = static_cast<const StringLiteral*>(expr);
            return "STRING_LITERAL:\"" + str_lit->value + "\"";
        }
        case ASTNode::NodeType::NumberLit: {
            auto* num_lit = static_cast<const NumberLiteral*>(expr);
            if (num_lit->literal_type == NumberLiteral::LiteralType::Integer) {
                return "NUMBER_LITERAL:" + std::to_string(num_lit->int_value);
            } else {
                return "NUMBER_LITERAL:" + std::to_string(num_lit->float_value);
            }
        }
        case ASTNode::NodeType::CharLit: {
            auto* char_lit = static_cast<const CharLiteral*>(expr);
            return "CHAR_LITERAL:" + std::to_string(static_cast<int>(char_lit->value));
        }
        case ASTNode::NodeType::BooleanLit: {
            auto* bool_lit = static_cast<const BooleanLiteral*>(expr);
            return "BOOLEAN_LITERAL:" + std::string(bool_lit->value ? "true" : "false");
        }
        case ASTNode::NodeType::VariableAccessExpr: {
            auto* var_access = static_cast<const VariableAccess*>(expr);
            return "VARIABLE:" + var_access->name;
        }
        default:
            // For other expression types, we don't track them for now
            return "";
    }
}
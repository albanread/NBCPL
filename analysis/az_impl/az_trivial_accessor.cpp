#include "../ASTAnalyzer.h"
#include "../../AST.h"
#include "../../ClassTable.h"
#include "../../SymbolTable.h"
#include <algorithm>

/**
 * @brief Returns true if the function is a trivial class accessor method.
 */
bool ASTAnalyzer::is_trivial_accessor_method(const std::string& function_name) const {
    auto it = function_metrics_.find(function_name);
    if (it != function_metrics_.end()) {
        return it->second.is_trivial_accessor;
    }
    return false;
}

/**
 * @brief Analyzes a function body AST to determine if it's a trivial accessor (e.g., RESULTIS SELF.X).
 * @return The name of the accessed member if true, or "_this_ptr" if returning SELF, otherwise an empty string.
 */
std::string ASTAnalyzer::identify_trivial_accessor(const ASTNode* body, const std::string& class_name) {
    if (!body || class_name.empty() || !class_table_) return "";

    const Statement* single_stmt = nullptr;

    // 1. Unwrap VALOF/FLOATVALOF bodies if present
    if (const auto* valof = dynamic_cast<const ValofExpression*>(body)) {
        body = valof->body.get();
    } else if (const auto* fvalof = dynamic_cast<const FloatValofExpression*>(body)) {
        body = fvalof->body.get();
    }

    // 2. Extract the single statement from the body block. Ensure no local declarations.
    if (const auto* block = dynamic_cast<const BlockStatement*>(body)) {
        if (block->statements.size() == 1 && block->declarations.empty()) {
            single_stmt = block->statements[0].get();
        }
    } else if (const auto* compound = dynamic_cast<const CompoundStatement*>(body)) {
         // Assuming CompoundStatement doesn't hold declarations based on context.
         if (compound->statements.size() == 1) {
            single_stmt = compound->statements[0].get();
        }
    }

    if (!single_stmt) return "";

    // 3. Check if the single statement is a RESULTIS or RETURN with an expression.
    const Expression* return_expr = nullptr;
    if (const auto* resultis = dynamic_cast<const ResultisStatement*>(single_stmt)) {
        return_expr = resultis->expression.get();
    }

    if (!return_expr) return "";

    // 4. Check if the returned expression is a member access.

    // Option A: Explicit MemberAccessExpression (e.g., _this.X or SELF.X)
    if (const auto* member_access = dynamic_cast<const MemberAccessExpression*>(return_expr)) {
        if (const auto* obj_var = dynamic_cast<const VariableAccess*>(member_access->object_expr.get())) {
            // Note: ASTAnalyzer typically rewrites SELF to _this, but we check both.
            if (obj_var->name == "_this" || obj_var->name == "SELF") {
                // Mark as safe to inline
                auto it = function_metrics_.find(current_function_scope_);
                if (it != function_metrics_.end()) {
                    it->second.is_safe_to_inline = true;
                }
                return member_access->member_name; // Found pattern match
            }
        }
    }

    // Option B: Implicit VariableAccess (e.g., just X, or just SELF)
    if (const auto* var_access = dynamic_cast<const VariableAccess*>(return_expr)) {

        // Handle the case where the accessor returns 'SELF' or '_this' (Identity Accessor)
        if (var_access->name == "_this" || var_access->name == "SELF") {
             return "_this_ptr"; // Special identifier for returning the object pointer itself.
        }

        // Check if this variable name corresponds to a member variable of the class.
        const ClassTableEntry* class_entry = class_table_->get_class(class_name);

        // Robust check: Ensure this access is NOT a local variable shadowing the member.
        bool is_local = false;
        if (symbol_table_) {
            Symbol symbol;
            // Check the immediate local scope.
            if (symbol_table_->lookup(var_access->name, symbol)) {
                 // If it's found locally and it's a local variable or a parameter.
                 // We already handled the _this/SELF parameter case above.
                 if (symbol.kind == SymbolKind::LOCAL_VAR || symbol.kind == SymbolKind::PARAMETER) {
                    is_local = true;
                 }
            }
        }

        if (!is_local && class_entry && class_entry->member_variables.count(var_access->name) > 0) {
            // Mark as safe to inline
            auto it = function_metrics_.find(current_function_scope_);
            if (it != function_metrics_.end()) {
                it->second.is_safe_to_inline = true;
            }
            return var_access->name; // Found pattern match
        }
    }

    return ""; // Does not match the pattern
}

/**
 * @brief Returns true if the function is a trivial setter method.
 */
bool ASTAnalyzer::is_trivial_setter_method(const std::string& function_name) const {
    auto it = function_metrics_.find(function_name);
    if (it != function_metrics_.end()) {
        return it->second.is_trivial_setter;
    }
    return false;
}

/**
 * @brief Analyzes a routine body AST to determine if it's a trivial setter (e.g., SELF.x := value).
 * @return The name of the member being set if true, otherwise an empty string.
 */
std::string ASTAnalyzer::identify_trivial_setter(const ASTNode* body, const std::string& class_name) {
    if (!body || class_name.empty() || !class_table_) return "";

    const Statement* single_stmt = nullptr;

    // 1. Check for a single AssignmentStatement within a Block or Compound statement.
    if (const auto* block = dynamic_cast<const BlockStatement*>(body)) {
        if (block->statements.size() == 1 && block->declarations.empty()) {
            single_stmt = block->statements[0].get();
        }
    } else if (const auto* compound = dynamic_cast<const CompoundStatement*>(body)) {
        if (compound->statements.size() == 1) {
            single_stmt = compound->statements[0].get();
        }
    }

    if (!single_stmt) return "";

    // 2. The single statement must be an AssignmentStatement.
    const AssignmentStatement* assign_stmt = dynamic_cast<const AssignmentStatement*>(single_stmt);
    if (!assign_stmt || assign_stmt->lhs.size() != 1 || assign_stmt->rhs.size() != 1) return "";

    // 3. The LHS must be a MemberAccessExpression (e.g., SELF.x).
    const MemberAccessExpression* member_access = dynamic_cast<const MemberAccessExpression*>(assign_stmt->lhs[0].get());
    if (!member_access) return "";

    // 4. The RHS must be a VariableAccess.
    const VariableAccess* value_var = dynamic_cast<const VariableAccess*>(assign_stmt->rhs[0].get());
    if (!value_var) return "";

    // 5. The RHS variable name must match a parameter name of the parent routine.
    const std::vector<std::string>& parent_params = current_routine_parameters_;
    if (std::find(parent_params.begin(), parent_params.end(), value_var->name) == parent_params.end()) {
        return "";
    }

    // 6. The object part of the LHS must be the 'this' pointer (or SELF).
    const VariableAccess* object_var = dynamic_cast<const VariableAccess*>(member_access->object_expr.get());
    if (!object_var || (object_var->name != "_this" && object_var->name != "SELF")) {
        return "";
    }

    // If all checks pass, it's a setter. Return the member name.
    // Mark as safe to inline
    auto it = function_metrics_.find(current_function_scope_);
    if (it != function_metrics_.end()) {
        it->second.is_safe_to_inline = true;
    }
    return member_access->member_name;
}

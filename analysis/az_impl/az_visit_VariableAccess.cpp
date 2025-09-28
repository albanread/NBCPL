#include "../ASTAnalyzer.h"
#include <iostream>

void ASTAnalyzer::visit(VariableAccess& node) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Visiting VariableAccess: " << node.name << std::endl;
    
    // DEBUG: Track when variable names become empty
    if (node.name.empty()) {
        std::cout << "[ERROR] VariableAccess node has EMPTY NAME! This indicates corruption." << std::endl;
        std::cout << "[ERROR] Stack trace would be helpful here..." << std::endl;
    }
    
    // Store original name for comparison
    std::string original_name = node.name;

    // Rewrite SELF to _this in class method context
    if (node.name == "SELF" && !current_class_name_.empty()) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] Rewriting 'SELF' to '_this' in method of class '"
                      << current_class_name_ << "'" << std::endl;
        }
        node.name = "_this";
    }

    // Handle FOR loop variable renaming first.
    std::string effective_name = get_effective_variable_name(node.name);
    if (effective_name != node.name) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] Renaming variable: '" << node.name << "' -> '" << effective_name << "'" << std::endl;
        }
        node.name = effective_name;
    }
    
    // DEBUG: Check if renaming corrupted the name
    if (node.name.empty() && !original_name.empty()) {
        std::cout << "[ERROR] Variable renaming corrupted name! Original: '" << original_name << "' -> '" << node.name << "'" << std::endl;
        std::cout << "[ERROR] Effective name was: '" << effective_name << "'" << std::endl;
    }

    // Check if this is a class member variable access within a method
    bool is_class_member = false;
    if (!current_class_name_.empty() && class_table_) {
        // We are in a class method context
        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
        if (class_entry && class_entry->member_variables.count(node.name) > 0) {
            is_class_member = true;
            if (trace_enabled_) {
                std::cout << "[ANALYZER TRACE] Identified '" << node.name << "' as class member of '" 
                          << current_class_name_ << "'" << std::endl;
            }
            // Do NOT add class members to local variables in the method's stack frame
            return;
        }
    }

    // Use the SymbolTable to check if this is a global variable.
    Symbol symbol;
    if (symbol_table_ && symbol_table_->lookup(node.name, symbol)) {
        // If the symbol is a GLOBAL_VAR, it's a global access.
        if (symbol.kind == SymbolKind::GLOBAL_VAR) {
            if (current_function_scope_ != "Global") {
                function_metrics_[current_function_scope_].accesses_globals = true;
                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE]   Marked function '" << current_function_scope_ << "' as accessing global '" << node.name << "'." << std::endl;
                }
            }
        }
    } else if (!is_class_member) {
        // Only add to symbol table if it's not a class member
        // Always add the variable to the SymbolTable if not present in the current scope
        if (symbol_table_) {
            Symbol new_symbol(
                node.name,
                SymbolKind::LOCAL_VAR,
                VarType::UNKNOWN,
                symbol_table_->currentScopeLevel(),
                current_function_scope_
            );
            symbol_table_->addSymbol(new_symbol);
        }
        if (variable_definitions_.find(node.name) == variable_definitions_.end()) {
            // If it's a new, undeclared variable, define it in the current lexical scope.
            variable_definitions_[node.name] = current_lexical_scope_;
            if (current_function_scope_ != "Global") {
                function_metrics_[current_function_scope_].num_variables++;
            }
        }
    }
}

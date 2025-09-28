#include "SymbolDiscoveryPass.h"
#include "../AST.h"
#include <cassert>

void SymbolDiscoveryPass::visit(ClassDeclaration& node) {
    trace("Processing class declaration: " + node.name);

    // Store the current class context
    std::string previous_class_name = current_function_name_;
    
    // Process each member of the class directly from the AST
    for (const auto& member : node.members) {
        if (auto* func_decl = dynamic_cast<FunctionDeclaration*>(member.declaration.get())) {
            // Create mangled name for the method
            std::string mangled_name = node.name + "::" + func_decl->name;
            
            // Create symbol for the function with mangled name
            Symbol func_symbol(
                mangled_name,
                SymbolKind::FUNCTION,
                VarType::INTEGER, // Will be updated by type analysis
                symbol_table_->currentScopeLevel(),
                mangled_name
            );
            if (!symbol_table_->addSymbol(func_symbol)) {
                report_duplicate_symbol(mangled_name);
            } else {
                trace("Added class method function: " + mangled_name);
            }
            
            // Enter a new scope for the method body
            symbol_table_->enterScope();
            current_function_name_ = mangled_name;
            
            // Add implicit _this parameter for methods
            Symbol this_param(
                "_this",
                SymbolKind::PARAMETER,
                VarType::POINTER_TO_OBJECT,
                symbol_table_->currentScopeLevel(),
                mangled_name
            );
            symbol_table_->addSymbol(this_param);
            trace("Added implicit _this parameter for method: " + mangled_name);
            
            // Register method parameters
            for (const std::string& param_name : func_decl->parameters) {
                Symbol param_symbol(
                    param_name,
                    SymbolKind::PARAMETER,
                    VarType::UNKNOWN, // Will be inferred later
                    symbol_table_->currentScopeLevel(),
                    mangled_name
                );
                if (!symbol_table_->addSymbol(param_symbol)) {
                    report_duplicate_symbol(param_name);
                } else {
                    trace("Added parameter: " + param_name + " in method " + mangled_name);
                }
            }
            
            // Process the method body
            if (func_decl->body) {
                func_decl->body->accept(*this);
            }
            
            // Exit the method scope
            symbol_table_->exitScope();
            current_function_name_ = previous_class_name;
            
        } else if (auto* routine_decl = dynamic_cast<RoutineDeclaration*>(member.declaration.get())) {
            // Create mangled name for the method
            std::string mangled_name = node.name + "::" + routine_decl->name;
            
            // Create symbol for the routine with mangled name
            Symbol routine_symbol(
                mangled_name,
                SymbolKind::ROUTINE,
                VarType::INTEGER,
                symbol_table_->currentScopeLevel(),
                mangled_name
            );
            if (!symbol_table_->addSymbol(routine_symbol)) {
                report_duplicate_symbol(mangled_name);
            } else {
                trace("Added class method routine: " + mangled_name);
            }
            
            // Enter a new scope for the method body
            symbol_table_->enterScope();
            current_function_name_ = mangled_name;
            
            // Add implicit _this parameter for methods
            Symbol this_param(
                "_this",
                SymbolKind::PARAMETER,
                VarType::POINTER_TO_OBJECT,
                symbol_table_->currentScopeLevel(),
                mangled_name
            );
            symbol_table_->addSymbol(this_param);
            trace("Added implicit _this parameter for method: " + mangled_name);
            
            // Register method parameters
            for (const std::string& param_name : routine_decl->parameters) {
                Symbol param_symbol(
                    param_name,
                    SymbolKind::PARAMETER,
                    VarType::UNKNOWN, // Will be inferred later
                    symbol_table_->currentScopeLevel(),
                    mangled_name
                );
                if (!symbol_table_->addSymbol(param_symbol)) {
                    report_duplicate_symbol(param_name);
                } else {
                    trace("Added parameter: " + param_name + " in method " + mangled_name);
                }
            }
            
            // Process the method body
            if (routine_decl->body) {
                routine_decl->body->accept(*this);
            }
            
            // Exit the method scope
            symbol_table_->exitScope();
            current_function_name_ = previous_class_name;
        } else if (auto* let_decl = dynamic_cast<LetDeclaration*>(member.declaration.get())) {
            // --- START OF FIX ---
            // This is a member variable declaration.
            trace("Processing member variable declaration in class: " + node.name);
            VarType member_type = let_decl->is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
            
            for (const auto& var_name : let_decl->names) {
                Symbol member_symbol(
                    var_name,
                    SymbolKind::MEMBER_VAR,
                    member_type,
                    symbol_table_->currentScopeLevel(),
                    node.name // The function_name context for a member is its class name
                );

                if (!symbol_table_->addSymbol(member_symbol)) {
                    report_duplicate_symbol(var_name);
                } else {
                    trace("Added member variable: " + var_name + " to class " + node.name);
                }
            }
            // --- END OF FIX ---
        }
        // Note: Member variable declarations are now handled above, and still handled by ClassPass for layout
    }
}

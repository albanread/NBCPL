#include "ASTAnalyzer.h"
#include <iostream>
#include <sstream>

void ASTAnalyzer::visit(BlockStatement& node) {
    // Synchronize SymbolTable scope
    if (symbol_table_) {
        symbol_table_->enterScope();
    }

    // Save the current lexical scope before creating a new one.
    std::string previous_lexical_scope = current_lexical_scope_;

    // Create a new, unique name for this block's lexical scope.
    std::ostringstream block_name_ss;
    block_name_ss << previous_lexical_scope << "_block_" << variable_definitions_.size();
    current_lexical_scope_ = block_name_ss.str();

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Entering block scope: " << current_lexical_scope_
                  << " (Function scope remains: " << current_function_scope_ << ")" << std::endl;
        std::cout << "[ANALYZER TRACE] BlockStatement: Traversing " << node.statements.size() << " statements." << std::endl;
    }

    // Traverse declarations and statements within this new lexical scope.
    for (const auto& decl : node.declarations) if (decl) decl->accept(*this);
    for (size_t i = 0; i < node.statements.size(); ++i) {
        if (node.statements[i]) {
            if (trace_enabled_) {
                std::cout << "[ANALYZER TRACE] BlockStatement: Calling accept on statement " << i << " of type " << static_cast<int>(node.statements[i]->getType()) << std::endl;
            }
            node.statements[i]->accept(*this);
        }
    }

    // --- Memory Cleanup Responsibility: RESOLVED ---
    // DESIGN DECISION: CFGBuilderPass handles all automatic memory cleanup for local variables.
    // This approach is superior because CFGBuilderPass has complete visibility of all control 
    // flow exit paths from a scope, ensuring cleanup occurs on ALL possible exit routes 
    // (normal exit, early returns, breaks, exceptions, etc.).
    // 
    // The code below is permanently disabled to prevent double-free crashes.
    // CFGBuilderPass.cpp contains the authoritative cleanup logic in visit(BlockStatement).
    /*
    if (symbol_table_) {
        // Get all symbols for the current scope level
        auto symbols_in_scope = symbol_table_->getSymbolsInScope(symbol_table_->currentScopeLevel());
        // Iterate in reverse to clean up in the opposite order of declaration
        for (auto it = symbols_in_scope.rbegin(); it != symbols_in_scope.rend(); ++it) {
            Symbol& symbol = *it;
            if (symbol.owns_heap_memory) {
                // Check if this is a list type that needs BCPL_FREE_LIST instead of RELEASE
                bool is_list_type = (symbol.type == VarType::POINTER_TO_INT_LIST ||
                                    symbol.type == VarType::POINTER_TO_FLOAT_LIST ||
                                    symbol.type == VarType::POINTER_TO_STRING_LIST ||
                                    symbol.type == VarType::POINTER_TO_ANY_LIST ||
                                    symbol.type == VarType::CONST_POINTER_TO_INT_LIST ||
                                    symbol.type == VarType::CONST_POINTER_TO_FLOAT_LIST ||
                                    symbol.type == VarType::CONST_POINTER_TO_STRING_LIST ||
                                    symbol.type == VarType::CONST_POINTER_TO_ANY_LIST);
                
                if (is_list_type) {
                    // For lists, call BCPL_FREE_LIST(var)
                    auto var_expr = std::make_unique<VariableAccess>(symbol.name);
                    auto free_list_expr = std::make_unique<VariableAccess>("BCPL_FREE_LIST");
                    std::vector<ExprPtr> args;
                    args.push_back(std::move(var_expr));
                    auto free_call = std::make_unique<RoutineCallStatement>(std::move(free_list_expr), std::move(args));
                    node.statements.push_back(std::move(free_call));
                } else {
                    // For objects, call var.RELEASE()
                    auto var_expr = std::make_unique<VariableAccess>(symbol.name);
                    auto release_expr = std::make_unique<MemberAccessExpression>(std::move(var_expr), "RELEASE");
                    auto release_call = std::make_unique<RoutineCallStatement>(std::move(release_expr), std::vector<ExprPtr>{});
                    node.statements.push_back(std::move(release_call));
                }
                // Disable ownership so parent scopes don't double-release
                symbol.owns_heap_memory = false;
            }
        }
    }
    */

    // Restore the previous lexical scope upon exiting the block.
    current_lexical_scope_ = previous_lexical_scope;
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Exiting block scope, returning to: " << current_lexical_scope_ << std::endl;
    }

    // Synchronize SymbolTable scope
    if (symbol_table_) {
        symbol_table_->exitScope();
    }
}

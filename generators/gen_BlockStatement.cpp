#include "NewCodeGenerator.h"
#include "LabelManager.h"
#include "analysis/ASTAnalyzer.h"
#include <iostream>
#include <stdexcept>
#include <sstream>

void NewCodeGenerator::visit(BlockStatement& node) {
    debug_print("Visiting BlockStatement node.");

    // --- FIX START: Synchronize SymbolTable scope ---
    if (symbol_table_) {
        symbol_table_->enterScope();
    }
    // --- FIX END ---

    std::string previous_scope = current_scope_name_;
    std::ostringstream block_name_ss;
    block_name_ss << previous_scope << "_block_" << block_id_counter_++;
    current_scope_name_ = block_name_ss.str();
    debug_print("Entering codegen block scope: " + current_scope_name_);

    // 1. Enter a new scope (for the generator's internal state).
    enter_scope();

    // 2. Process declarations within this block.
    process_declarations(node.declarations);

    // 3. Generate code for statements within this block.
    for (const auto& stmt : node.statements) {
        if (stmt) {
            generate_statement_code(*stmt);
        }
    }

    // --- Emit cleanup code for heap-owning locals before exiting scope ---
    if (symbol_table_) {
        // Get all symbols for the current scope level
        auto symbols_in_scope = symbol_table_->getSymbolsInScope(symbol_table_->currentScopeLevel());
        // Iterate in reverse to clean up in the opposite order of declaration
        for (auto it = symbols_in_scope.rbegin(); it != symbols_in_scope.rend(); ++it) {
            const Symbol& symbol = *it;
            if (symbol.owns_heap_memory) {
                // Generate code to call the appropriate free function for this symbol.
                generate_cleanup_code_for_symbol(symbol);
            }
        }
    }

    // 4. Exit the scope (for the generator's internal state).
    exit_scope();
    
    // --- FIX START: Synchronize SymbolTable scope ---
    if (symbol_table_) {
        symbol_table_->exitScope();
    }
    // --- FIX END ---

    debug_print("Finished visiting BlockStatement node.");
}

#include "../ASTAnalyzer.h"
#include "../../SymbolTable.h"
#include <iostream>

/**
 * @brief Performs semantic analysis on the given program AST.
 * This method resets the analyzer state, discovers all functions/routines,
 * and traverses the AST to collect metrics and semantic information.
 */
void ASTAnalyzer::analyze(Program& program, SymbolTable* symbol_table, ClassTable* class_table) {
    // Store the symbol table and class table references if provided
    symbol_table_ = symbol_table;
    class_table_ = class_table;

    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Starting analysis..." << std::endl;
    
    // Only reset state if signature analysis hasn't been completed
    // This preserves parameter types populated by SignatureAnalysisVisitor
    if (!signature_analysis_complete_) {
        reset_state();
    } else {
        if (trace_enabled_) std::cout << "[ANALYZER TRACE] Skipping reset - signature analysis already complete." << std::endl;
    }
    
    // Always run function discovery, but it now preserves existing parameter types
    first_pass_discover_functions(program);
    program.accept(*this);
    
    // After initial analysis, propagate heap allocation information through call graph
    propagate_heap_allocation_info();
    
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Analysis complete." << std::endl;
}

// Keep the original method for backward compatibility
// This function is already correctly defined in lines 9-17 above
// So we're removing this redundant definition that doesn't match the declaration

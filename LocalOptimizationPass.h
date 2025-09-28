#pragma once

#include "AST.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"
#include "DataTypes.h"
#include "StringTable.h"

// Forward declaration for AST nodes
class AssignmentStatement;
class Stmt;

// Forward declaration for StringTable
class StringTable;


/**
 * LocalOptimizationPass
 * 
 * Performs local optimizations (such as Common Subexpression Elimination)
 * within each basic block of the Control Flow Graph (CFG).
 * 
 * Usage:
 *   LocalOptimizationPass pass;
 *   pass.run(cfgs); // where cfgs is the map from function name to ControlFlowGraph
 */
class LocalOptimizationPass {
public:
    // Pass a pointer to StringTable so we can lift string literals to global labels
    LocalOptimizationPass(StringTable* string_table);

    // Run local optimizations on the AST program.
    void run(Program& ast,
             SymbolTable& symbol_table,
             ASTAnalyzer& analyzer);

private:
    // Map from canonical expression string to temporary variable name
   std::unordered_map<std::string, std::string> available_expressions_;
   int temp_var_counter_;
   StringTable* string_table_; // For string literal lifting

    // --- NEW: Map from canonical expression string to count for analysis stage ---
    std::unordered_map<std::string, int> expr_counts_;

    // Generate a unique temporary variable name for hoisted expressions
    std::string generate_temp_var_name();

    // Infer the type of an expression (INTEGER or FLOAT)
    VarType infer_expression_type(const Expression* expr);

    // Apply CSE and other local optimizations to a single function/routine
    void optimize_function(FunctionDeclaration* func,
                          SymbolTable& symbol_table,
                          ASTAnalyzer& analyzer);

    void optimize_function(RoutineDeclaration* routine,
                          SymbolTable& symbol_table,
                          ASTAnalyzer& analyzer);

    // CSE logic for statement lists
    void optimize_statement_list(std::vector<StmtPtr>& statements,
                                const std::string& current_function_name,
                                SymbolTable& symbol_table,
                                ASTAnalyzer& analyzer);

    // CSE logic for individual expressions
    void optimize_expression(ExprPtr& expr, std::vector<StmtPtr>& statements, size_t& i,
                            const std::string& current_function_name,
                            SymbolTable& symbol_table,
                            ASTAnalyzer& analyzer);

    // Helper: Invalidate expressions that use a given variable name
    void invalidate_expressions_with_var(const std::string& var_name);

    // Helper: Get the canonical string for an expression (for CSE)
    std::string expression_to_string(const Expression* expr) const;

    // --- NEW: Helper to count subexpressions for analysis stage ---
    void count_subexpressions(ASTNode* node);
};

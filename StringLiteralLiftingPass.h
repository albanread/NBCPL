#pragma once

#include "AST.h"
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"
#include "StringTable.h"
#include <map>
#include <string>
#include <memory>

/**
 * StringLiteralLiftingPass
 *
 * This pass transforms all StringLiteral nodes in the AST into local pointer variables.
 * For each unique string literal in a function, it:
 *   - Registers a global label for the string in the StringTable.
 *   - Creates a temporary local variable (e.g., _cse_temp_N) of type POINTER_TO_STRING.
 *   - Adds the variable to the SymbolTable and updates function metrics in ASTAnalyzer.
 *   - Inserts an assignment: _cse_temp_N := @L_strN before the statement using the string.
 *   - Replaces the StringLiteral node with VariableAccess(_cse_temp_N).
 *
 * This pass must run after symbol discovery/type analysis and before CFG/liveness analysis.
 */
class StringLiteralLiftingPass {
public:
    explicit StringLiteralLiftingPass(StringTable* string_table);

    // Main entry point: transforms all string literals in the program.
    void run(Program& ast, SymbolTable& symbol_table, ASTAnalyzer& analyzer);

private:
    StringTable* string_table_;
    int temp_var_counter_;

    // Helper to generate a unique temp variable name
    std::string generate_temp_var_name();

    // Per-function transformation
    void lift_strings_in_function(FunctionDeclaration& func, SymbolTable& symbol_table, ASTAnalyzer& analyzer);
    void lift_strings_in_routine(RoutineDeclaration& routine, SymbolTable& symbol_table, ASTAnalyzer& analyzer);

    // Recursive AST transformation helpers
    void transform_statement_list(std::vector<StmtPtr>& statements,
                                 std::map<std::string, std::string>& string_to_temp,
                                 std::vector<StmtPtr>& new_assignments,
                                 SymbolTable& symbol_table,
                                 ASTAnalyzer& analyzer,
                                 const std::string& current_function_name);

    void transform_expression(ExprPtr& expr,
                             std::map<std::string, std::string>& string_to_temp,
                             std::vector<StmtPtr>& new_assignments,
                             SymbolTable& symbol_table,
                             ASTAnalyzer& analyzer,
                             const std::string& current_function_name);
};
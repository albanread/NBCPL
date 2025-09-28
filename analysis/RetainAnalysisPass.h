#pragma once
#include "ASTVisitor.h"
#include "AST.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include <set>
#include <string>
#include <memory>

/**
 * RetainAnalysisPass:
 * - Scans function/routine bodies for RETAIN statements and escaping variables (via RESULTIS/RETURN).
 * - Suppresses automatic DEFER for retained/escaping variables.
 */
class RetainAnalysisPass : public ASTVisitor {
public:
    RetainAnalysisPass();

    // Run the pass on the whole program
    void run(Program& program, SymbolTable& symbol_table);

    // Visitor overrides
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(RetainStatement& node) override;
    void visit(RemanageStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(DeferStatement& node) override;

private:
    // Reference to the symbol table for ownership flag updates
    SymbolTable* symbol_table_ = nullptr;

    // Set of variable names retained/escaping in the current function
    std::set<std::string> retained_vars;

    // Set of variable names escaping via RESULTIS/RETURN in the current function
    std::set<std::string> escaping_vars;

    // Helper: Remove DeferStatements for retained/escaping variables from a statement list
    void filter_defer_statements(std::vector<StmtPtr>& stmts);

    // Helper: Extract variable name from a DeferStatement (assumes DEFER var.RELEASE())
    std::string get_deferred_variable(const DeferStatement& node) const;

    // Declare helper/member functions
    void analyze_function_or_routine(Statement& node);
    void analyze_compound(CompoundStatement& compound);
    void collect_retained_vars(ASTNode& node);
};
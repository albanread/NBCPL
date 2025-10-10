#ifndef LOOP_INVARIANT_CODE_MOTION_PASS_H
#define LOOP_INVARIANT_CODE_MOTION_PASS_H

#include "Optimizer.h"
#include "AST.h"
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"
#include "analysis/TemporaryVariableFactory.h"
#include <string>
#include <set>
#include <vector>

// Forward declaration for a visitor used in the implementation file.
class VariableUsageVisitor;

class LoopInvariantCodeMotionPass : public Optimizer {
public:
    /**
     * @brief Constructs the Loop Invariant Code Motion pass.
     * @param manifests A map of compile-time constants.
     * @param symbol_table The symbol table, used for adding new temporary variables.
     * @param analyzer The AST analyzer, used for updating function metrics.
     */
    LoopInvariantCodeMotionPass(
        std::unordered_map<std::string, int64_t>& manifests,
        SymbolTable& symbol_table,
        ASTAnalyzer& analyzer,
        bool enable_tracing = false
    );

    // --- Core Pass Methods ---
    std::string getName() const override { return "Loop Invariant Code Motion Pass"; }
    ProgramPtr apply(ProgramPtr program) override;

    // --- Visitor Overrides for Relevant AST Nodes ---
        void visit(FunctionDeclaration& node) override;
        void visit(RoutineDeclaration& node) override;
        void visit(ForStatement& node) override;
        void visit(WhileStatement& node) override;
        void visit(RepeatStatement& node) override;

    private:
        bool enable_tracing_ = false;

private:
    // Helper to determine if an expression is loop-invariant.
    bool isInvariant(Expression* expr, const std::set<std::string>& loop_defined_vars);
    
    // Traverses a statement to find and hoist invariant expressions.
    void findAndHoistInvariants(StmtPtr& stmt, const std::set<std::string>& loop_defined_vars);

    // ✅ Declaration for loop body optimization helper
    void optimize_loop_body(StmtPtr& body, const std::set<std::string>& loop_defined_vars);

    // --- Pass-Specific State ---
    std::string current_function_name_;
    std::vector<StmtPtr> hoisted_statements_; // Buffer for hoisted code.

    // --- Compiler Component References ---
    SymbolTable& symbol_table_;
    ASTAnalyzer& analyzer_;
    TemporaryVariableFactory temp_var_factory_;
};

#endif // LOOP_INVARIANT_CODE_MOTION_PASS_H
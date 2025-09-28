#ifndef COMMON_SUBEXPRESSION_ELIMINATION_PASS_H
#define COMMON_SUBEXPRESSION_ELIMINATION_PASS_H

#include "Optimizer.h"
#include "AST.h"          // <-- Added missing include for AST node types
#include "DataTypes.h"      // <-- Added missing include for VarType
#include <string>           // <-- Added missing include for std::string
#include <unordered_map>
#include <vector>
#include "analysis/TemporaryVariableFactory.h"
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"

class CommonSubexpressionEliminationPass : public Optimizer {
public:
    CommonSubexpressionEliminationPass(
        std::unordered_map<std::string, int64_t>& manifests,
        SymbolTable& symbol_table,
        ASTAnalyzer& analyzer
    );
    std::string getName() const override { return "Common Subexpression Elimination Pass"; }

    // Override visit methods for nodes that define a new scope or contain expressions

    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(LetDeclaration& node) override;
    void visit(BinaryOp& node) override;
    ExprPtr visit_expr(ExprPtr expr) override;
    void visit(UnaryOp& node) override;
    void visit(FunctionCall& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override {}
    void visit(VecAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(BrkStatement& node) override;
    void visit(GlobalVariableDeclaration& node) override;

private:
    std::unordered_map<std::string, std::string> available_expressions_;
    int temp_var_counter_;
    std::vector<StmtPtr> hoisted_declarations_;
    std::string current_function_name_;

    // New members for refactored temp variable management
    SymbolTable& symbol_table_;
    ASTAnalyzer& analyzer_;
    TemporaryVariableFactory temp_var_factory_;

    VarType infer_expression_type(const Expression* expr);

    // --- FIRST PASS: Expression counting ---
    std::unordered_map<std::string, int> expr_counts_;
    void count_subexpressions(ASTNode* node);

    std::string generate_temp_var_name();
    std::string expression_to_string(Expression* expr);
    std::string expression_to_string_recursive(Expression* expr);
};
#endif


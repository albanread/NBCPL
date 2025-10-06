#pragma once

#include "../../AST.h"
#include "../../ASTVisitor.h"
#include "../../SymbolTable.h"
#include <set>
#include <string>
#include <vector>

/**
 * @class VariableUsageVisitor
 * @brief Traverses an AST subtree to find all used or defined variables.
 *
 * This visitor is a robust tool to replace the manual variable discovery
 * logic previously embedded in individual AST Statement nodes.
 */
class VariableUsageVisitor : public ASTVisitor {
public:
    // Constructor that takes a SymbolTable for variable validation
    VariableUsageVisitor(SymbolTable* symbol_table = nullptr) 
        : symbol_table_(symbol_table) {}
    
    // --- Public Interface ---

    // Returns the set of unique variable names found during traversal.
    const std::set<std::string>& getVariables() const {
        return variables_;
    }

    // Allows non-const insertion of a variable name.
    void addVariable(const std::string& name) {
        variables_.insert(name);
    }

    // Convenience function to get variables as a vector.
    std::vector<std::string> getVariablesAsVector() const {
        return std::vector<std::string>(variables_.begin(), variables_.end());
    }

    // --- Visitor Overrides ---

    // The core of the visitor: when a variable is accessed, record its name.
    void visit(VariableAccess& node) override;

    // --- Traversal Overrides for Expressions ---
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(FunctionCall& node) override;
    void visit(SysCall& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(LetDeclaration& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(NewExpression& node) override;
    void visit(SuperMethodCallExpression& node) override;

    // --- Traversal Overrides for Statements ---
    void visit(AssignmentStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(CaseStatement& node) override;
    void visit(DefaultStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(CompoundStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(FreeStatement& node) override;
    void visit(PairwiseReductionLoopStatement& node) override;

private:
    std::set<std::string> variables_;
    SymbolTable* symbol_table_;
};
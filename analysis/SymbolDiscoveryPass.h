#pragma once
#include <string>
#include <memory>
#include <utility>
#include <unordered_set>
#include "../ASTVisitor.h"
#include "../SymbolTable.h"
#include "../ClassTable.h"

/**
 * @class SymbolDiscoveryPass
 * @brief Traverses the AST to build the SymbolTable and ClassTable.
 *
 * This pass runs immediately after parsing and before semantic analysis.
 * It populates a SymbolTable with all declarations in the program, maintaining
 * proper scoping and storing type information, and also builds the ClassTable
 * with initial class metadata.
 */
class SymbolDiscoveryPass : public ASTVisitor {
public:
    using ASTVisitor::visit; // Bring all visit overloads into scope

    void visit(SuperMethodAccessExpression& node) override;

public:
    /**
     * @brief Constructor
     * @param enable_tracing Whether to print debug information
     */
    SymbolDiscoveryPass(bool enable_tracing = false);

    /**
     * @brief Populate symbol and class tables for the entire program
     * @param program The AST root
     * @param symbol_table Reference to the SymbolTable to populate
     * @param class_table Reference to the ClassTable to populate
     */
    void build_into(Program& program, SymbolTable& symbol_table, ClassTable& class_table);

    // --- ASTVisitor overrides for declarations/statements ---
    void visit(Program& node) override;
    void visit(LetDeclaration& node) override;
    void visit(ManifestDeclaration& node) override;
    void visit(StaticDeclaration& node) override;
    void visit(GlobalDeclaration& node) override;
    void visit(GlobalVariableDeclaration& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(LabelDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ForEachStatement& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(ReductionLoopStatement& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(FVecAllocationExpression& node) override;
    void visit(PairsAllocationExpression& node) override;
    void visit(FunctionCall& node) override;

    // ... other visit methods as needed ...

private:
    SymbolTable* symbol_table_;
    ClassTable* class_table_;
    std::string current_function_name_ = "Global";
    std::string current_class_name_;
    bool enable_tracing_;

    void trace(const std::string& message) const;
    void report_duplicate_symbol(const std::string& name) const;
    
    // Process inherited members for a class from its parent
    void processInheritedMembers(const std::string& class_name, const std::string& parent_name);
    
    // Recursively ensure that a parent class is processed before its children
    bool ensureParentProcessed(const std::string& class_name, std::unordered_set<std::string>& resolution_path);
    

};
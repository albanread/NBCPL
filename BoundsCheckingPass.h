#ifndef BOUNDS_CHECKING_PASS_H
#define BOUNDS_CHECKING_PASS_H

#include "AST.h"
#include "ASTVisitor.h"
#include "SymbolTable.h"
#include <vector>
#include <string>

/**
 * BoundsCheckingPass - Compile-time bounds checking for constant vector indices
 * 
 * This pass analyzes VectorAccess nodes with constant indices and checks them
 * against known vector sizes to catch out-of-bounds accesses at compile time.
 * 
 * Features:
 * - Detects negative indices
 * - Detects indices >= vector size
 * - Works with VEC n allocations
 * - Works with VEC [1,2,3] initializers
 * - Collects and reports all errors found
 * 
 * Usage:
 *   BoundsCheckingPass bounds_checker(symbol_table, enable_checking);
 *   bounds_checker.analyze(ast_root);
 *   if (bounds_checker.has_errors()) {
 *       bounds_checker.print_errors();
 *   }
 */
class BoundsCheckingPass : public ASTVisitor {
public:
    /**
     * Constructor
     * @param symbol_table Pointer to the symbol table for variable lookups
     * @param enabled Whether bounds checking is enabled (controlled by compiler flags)
     */
    BoundsCheckingPass(SymbolTable* symbol_table, bool enabled = true);

    /**
     * Analyze the AST for compile-time bounds violations
     * @param root The root AST node to analyze
     */
    void analyze(ASTNode& root);

    /**
     * Check if any bounds violations were found
     * @return true if errors were detected
     */
    bool has_errors() const { return !errors_.empty(); }

    /**
     * Get the list of error messages
     * @return Vector of error message strings
     */
    const std::vector<std::string>& get_errors() const { return errors_; }

    /**
     * Print all error messages to stderr
     */
    void print_errors() const;

    /**
     * Get the number of bounds violations found
     * @return Count of violations
     */
    size_t error_count() const { return errors_.size(); }

    // ASTVisitor interface - most methods are no-ops, we only care about specific nodes
    void visit(Program& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    void visit(BlockStatement& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(VectorAccess& node) override;
    void visit(LetDeclaration& node) override;
    
    // No-op visitors for nodes we don't care about
    void visit(GlobalDeclaration& node) override {}
    void visit(StaticDeclaration& node) override {}
    void visit(ManifestDeclaration& node) override {}
    void visit(LabelDeclaration& node) override {}
    void visit(ClassDeclaration& node) override {}
    void visit(IfStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(CaseStatement& node) override;
    void visit(DefaultStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(GotoStatement& node) override;
    void visit(LabelTargetStatement& node) override;
    void visit(BreakStatement& node) override;
    void visit(LoopStatement& node) override;
    void visit(FinishStatement& node) override;
    void visit(EndcaseStatement& node) override;
    void visit(BrkStatement& node) override;
    void visit(StringStatement& node) override;
    void visit(FreeStatement& node) override;
    void visit(ConditionalBranchStatement& node) override;
    void visit(CompoundStatement& node) override {}
    void visit(ForEachStatement& node) override;
    void visit(RetainStatement& node) override {}
    void visit(RemanageStatement& node) override {}
    void visit(DeferStatement& node) override {}
    
    // Expression visitors - only some are relevant
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(FunctionCall& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(VariableAccess& node) override {}
    void visit(NumberLiteral& node) override {}
    void visit(StringLiteral& node) override {}
    void visit(CharLiteral& node) override {}
    void visit(BooleanLiteral& node) override {}
    void visit(NullLiteral& node) override {}
    void visit(ListExpression& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(NewExpression& node) override {}
    void visit(SuperMethodAccessExpression& node) override {}
    void visit(SuperMethodCallExpression& node) override {}
    void visit(BitfieldAccessExpression& node) override {}
    void visit(FVecAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(VecInitializerExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(SysCall& node) override {}

private:
    SymbolTable* symbol_table_;
    bool enabled_;
    std::vector<std::string> errors_;
    std::string current_function_; // Track current function for better error messages

    /**
     * Add an error message to the error list
     * @param message The error message to add
     */
    void add_error(const std::string& message);

    /**
     * Check if a vector access with constant index is within bounds
     * @param vector_name Name of the vector variable
     * @param index Constant index value
     * @param location_info Additional context for error messages
     */
    void check_constant_vector_access(const std::string& vector_name, 
                                     int64_t index, 
                                     const std::string& location_info);

    /**
     * Visit all statements in a statement list
     * @param statements Vector of statement pointers to visit
     */
    void visit_statements(const std::vector<std::unique_ptr<Statement>>& statements);

    /**
     * Visit all expressions in an expression list
     * @param expressions Vector of expression pointers to visit
     */
    void visit_expressions(const std::vector<std::unique_ptr<Expression>>& expressions);
};

#endif // BOUNDS_CHECKING_PASS_H
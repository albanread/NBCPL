#pragma once

#include "ASTVisitor.h"
#include "AST.h"
#include "ClassTable.h"
#include <memory>
#include <string>
#include <vector>

/**
 * CreateMethodReorderPass - Reorders statements in CREATE methods to avoid call interval bugs
 * 
 * This pass moves SUPER.CREATE calls below any assignments to _this members in CREATE methods.
 * This prevents register allocation issues where member assignment values are held in 
 * caller-saved registers across SUPER.CREATE calls.
 * 
 * Transformation:
 *   ROUTINE CREATE(...) BE $(
 *       SUPER.CREATE(...)     // Move DOWN
 *       member := value       // _this member assignment
 *       _this.field := value  // Direct _this assignment  
 *       LET local := value    // Local assignment (unchanged)
 *       WRITES(...)           // Side effects (preserve order)
 *   $)
 * 
 * Becomes:
 *   ROUTINE CREATE(...) BE $(
 *       member := value       // _this assignments FIRST
 *       _this.field := value  
 *       SUPER.CREATE(...)     // SUPER call AFTER _this assignments
 *       LET local := value    // Other statements preserve order
 *       WRITES(...)
 *   $)
 * 
 * Pipeline Position: After semantic analysis, before code generation
 */
class CreateMethodReorderPass : public ASTVisitor {
public:
    /**
     * Constructor
     * @param class_table Pointer to the class table for class context
     * @param trace_enabled Whether to enable debug tracing
     */
    CreateMethodReorderPass(ClassTable* class_table, bool trace_enabled = false);

    /**
     * Apply the reordering transformation to the entire program
     * @param program The program AST to transform
     * @return Transformed program (may be the same pointer if no changes needed)
     */
    ProgramPtr transform(ProgramPtr program);

    // --- ASTVisitor overrides ---
    
    // Main transformation target
    void visit(RoutineDeclaration& node) override;
    
    // Context tracking
    void visit(ClassDeclaration& node) override;
    void visit(Program& node) override;
    
    // Traversal methods (visit children)
    void visit(FunctionDeclaration& node) override;
    void visit(CompoundStatement& node) override;
    void visit(BlockStatement& node) override;
    void visit(IfStatement& node) override;
    void visit(UnlessStatement& node) override;
    void visit(TestStatement& node) override;
    void visit(WhileStatement& node) override;
    void visit(UntilStatement& node) override;
    void visit(RepeatStatement& node) override;
    void visit(ForStatement& node) override;
    void visit(ForEachStatement& node) override;
    void visit(SwitchonStatement& node) override;
    void visit(CaseStatement& node) override;
    void visit(DefaultStatement& node) override;
    void visit(ValofExpression& node) override;
    void visit(FloatValofExpression& node) override;
    void visit(AssignmentStatement& node) override;
    void visit(RoutineCallStatement& node) override;
    void visit(ReturnStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
    void visit(FunctionCall& node) override;
    void visit(MemberAccessExpression& node) override;
    void visit(VecAllocationExpression& node) override;
    void visit(StringAllocationExpression& node) override;
    void visit(FVecAllocationExpression& node) override;
    void visit(NewExpression& node) override;
    void visit(TableExpression& node) override;
    void visit(ListExpression& node) override;
    void visit(VecInitializerExpression& node) override;
    void visit(SysCall& node) override;
    void visit(FinishStatement& node) override;
    void visit(FreeStatement& node) override;
    
    // No-op visitors (leaves of the AST)
    void visit(LetDeclaration& node) override {}
    void visit(ManifestDeclaration& node) override {}
    void visit(StaticDeclaration& node) override {}
    void visit(GlobalDeclaration& node) override {}
    void visit(GlobalVariableDeclaration& node) override {}
    void visit(LabelDeclaration& node) override {}
    void visit(NumberLiteral& node) override {}
    void visit(StringLiteral& node) override {}
    void visit(CharLiteral& node) override {}
    void visit(BooleanLiteral& node) override {}
    void visit(NullLiteral& node) override {}
    void visit(VariableAccess& node) override {}
    void visit(SuperMethodAccessExpression& node) override {}
    void visit(SuperMethodCallExpression& node) override {}
    void visit(GotoStatement& node) override {}
    void visit(BreakStatement& node) override {}
    void visit(LoopStatement& node) override {}
    void visit(EndcaseStatement& node) override {}
    void visit(BrkStatement& node) override {}
    void visit(StringStatement& node) override {}
    void visit(LabelTargetStatement& node) override {}
    void visit(ConditionalBranchStatement& node) override {}

private:
    ClassTable* class_table_;
    bool trace_enabled_;
    
    // Context tracking
    std::string current_class_name_;
    bool transformation_occurred_;
    
    /**
     * Reorder statements in a CREATE method
     * @param body The compound statement body of the CREATE method
     * @return true if any reordering occurred
     */
    bool reorder_create_method_body(CompoundStatement* body);
    
    /**
     * Reorder statements in a CREATE method (BlockStatement version)
     * @param body The block statement body of the CREATE method
     * @return true if any reordering occurred
     */
    bool reorder_block_statement_body(BlockStatement* body);
    
    /**
     * Check if a statement is an assignment to a _this member
     * @param stmt The statement to check
     * @return true if it's a _this member assignment
     */
    bool is_this_member_assignment(Statement* stmt);
    
    /**
     * Check if a statement is a SUPER.CREATE call
     * @param stmt The statement to check
     * @return true if it's a SUPER.CREATE call
     */
    bool is_super_create_call(Statement* stmt);
    
    /**
     * Debug print utility
     * @param message The message to print if tracing is enabled
     */
    void debug_print(const std::string& message) const;
};
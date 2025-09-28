#pragma once

#include "ASTVisitor.h"
#include "AST.h"
#include "ClassTable.h"
#include "SymbolTable.h"
#include <memory>
#include <string>

/**
 * SuperCallTransformationPass - Transforms SUPER method calls into regular function calls
 * 
 * This pass runs BEFORE liveness analysis and register allocation to ensure that
 * synthesized SUPER calls are visible to the compiler pipeline. It transforms:
 * 
 *   SUPER.methodName(args...)
 * 
 * Into equivalent regular function calls:
 * 
 *   ParentClass::methodName(_this, args...)
 * 
 * This allows liveness analysis to properly detect function calls and ensure
 * call-crossing variables get allocated to callee-saved registers.
 * 
 * Pipeline Position: After semantic analysis, before liveness analysis
 */
class SuperCallTransformationPass : public ASTVisitor {
public:
    /**
     * Constructor
     * @param class_table Pointer to the class table for parent class lookup
     * @param symbol_table Pointer to the symbol table for method resolution
     * @param trace_enabled Whether to enable debug tracing
     */
    SuperCallTransformationPass(ClassTable* class_table, 
                                SymbolTable* symbol_table, 
                                bool trace_enabled = false);

    /**
     * Apply the transformation to the entire program
     * @param program The program AST to transform
     * @return Transformed program (may be the same pointer if no changes needed)
     */
    ProgramPtr transform(ProgramPtr program);

    // --- ASTVisitor overrides ---
    
    // Main transformation targets
    void visit(FunctionCall& node) override;
    void visit(RoutineCallStatement& node) override;
    
    // Context tracking
    void visit(FunctionDeclaration& node) override;
    void visit(ClassDeclaration& node) override;
    void visit(Program& node) override;
    
    // Traversal methods (visit children)
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
    void visit(ReturnStatement& node) override;
    void visit(ResultisStatement& node) override;
    void visit(BinaryOp& node) override;
    void visit(UnaryOp& node) override;
    void visit(ConditionalExpression& node) override;
    void visit(VectorAccess& node) override;
    void visit(CharIndirection& node) override;
    void visit(FloatVectorIndirection& node) override;
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
    void visit(RoutineDeclaration& node) override;
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
    SymbolTable* symbol_table_;
    bool trace_enabled_;
    
    // Context tracking
    std::string current_class_name_;
    std::string current_method_name_;
    bool transformation_occurred_;
    
    /**
     * Transform a SUPER function call into a regular function call
     * @param node The function call node to potentially transform
     * @return true if transformation occurred
     */
    bool transform_super_function_call(FunctionCall& node);
    
    /**
     * Transform a SUPER routine call into a regular routine call
     * @param node The routine call node to potentially transform
     * @return true if transformation occurred
     */
    bool transform_super_routine_call(RoutineCallStatement& node);
    
    /**
     * Create a regular function call that replaces a SUPER call
     * @param super_access The SuperMethodAccessExpression from the original call
     * @param arguments The arguments from the original call
     * @param parent_class_name The name of the parent class
     * @return A new FunctionCall expression
     */
    ExprPtr create_parent_method_call(SuperMethodAccessExpression* super_access,
                                     const std::vector<ExprPtr>& arguments,
                                     const std::string& parent_class_name);
    
    /**
     * Create a regular routine call that replaces a SUPER call
     * @param super_access The SuperMethodAccessExpression from the original call
     * @param arguments The arguments from the original call
     * @param parent_class_name The name of the parent class
     * @return A new RoutineCallStatement
     */
    StmtPtr create_parent_routine_call(SuperMethodAccessExpression* super_access,
                                      const std::vector<ExprPtr>& arguments,
                                      const std::string& parent_class_name);
    
    /**
     * Get the parent class name for the current class
     * @return Parent class name, or empty string if no parent
     */
    std::string get_parent_class_name() const;
    
    /**
     * Debug print utility
     * @param message The message to print if tracing is enabled
     */
    void debug_print(const std::string& message) const;
};
#pragma once

#include "../AST.h"
#include "../ASTVisitor.h"
#include "../SymbolTable.h"
#include "ASTAnalyzer.h"
#include <map>
#include <string>

/**
 * @class SignatureAnalysisVisitor
 * @brief First-pass visitor that analyzes function signatures and parameter types
 * 
 * This visitor performs a preliminary analysis of function and routine declarations
 * to establish parameter types before any function body analysis begins. This solves
 * the chicken-and-egg problem where liveness analysis needs parameter types but
 * parameter type inference happens during body analysis.
 */
class SignatureAnalysisVisitor : public ASTVisitor {
public:
    SignatureAnalysisVisitor(SymbolTable* symbol_table, ASTAnalyzer& analyzer, bool debug = false);
    
    // Main entry point
    void analyze_signatures(Program& program);
    
    // Visitor methods - only for signature analysis
    void visit(Program& node) override;
    void visit(FunctionDeclaration& node) override;
    void visit(RoutineDeclaration& node) override;
    
    // All other visitor methods are no-ops for signature analysis
    void visit(LetDeclaration& node) override {}
    void visit(ManifestDeclaration& node) override {}
    void visit(StaticDeclaration& node) override {}
    void visit(GlobalDeclaration& node) override {}
    void visit(GlobalVariableDeclaration& node) override {}
    void visit(LabelDeclaration& node) override {}
    void visit(ClassDeclaration& node) override {}
    void visit(NumberLiteral& node) override {}
    void visit(StringLiteral& node) override {}
    void visit(CharLiteral& node) override {}
    void visit(BooleanLiteral& node) override {}
    void visit(VariableAccess& node) override {}
    void visit(BinaryOp& node) override {}
    void visit(UnaryOp& node) override {}
    void visit(VectorAccess& node) override {}
    void visit(CharIndirection& node) override {}
    void visit(FloatVectorIndirection& node) override {}
    void visit(BitfieldAccessExpression& node) override {}
    void visit(FunctionCall& node) override {}
    void visit(ConditionalExpression& node) override {}
    void visit(TableExpression& node) override {}
    void visit(ListExpression& node) override {}
    void visit(ForEachStatement& node) override {}
    void visit(FloatValofExpression& node) override {}
    void visit(ValofExpression& node) override {}
    void visit(VecAllocationExpression& node) override {}
    void visit(VecInitializerExpression& node) override {}
    void visit(StringAllocationExpression& node) override {}
    void visit(AssignmentStatement& node) override {}
    void visit(RoutineCallStatement& node) override {}
    void visit(IfStatement& node) override {}
    void visit(UnlessStatement& node) override {}
    void visit(TestStatement& node) override {}
    void visit(BrkStatement& node) override {}
    void visit(WhileStatement& node) override {}
    void visit(UntilStatement& node) override {}
    void visit(RepeatStatement& node) override {}
    void visit(ForStatement& node) override {}
    void visit(SwitchonStatement& node) override {}
    void visit(CaseStatement& node) override {}
    void visit(DefaultStatement& node) override {}
    void visit(GotoStatement& node) override {}
    void visit(ReturnStatement& node) override {}
    void visit(FinishStatement& node) override {}
    void visit(BreakStatement& node) override {}
    void visit(LoopStatement& node) override {}
    void visit(EndcaseStatement& node) override {}
    void visit(ResultisStatement& node) override {}
    void visit(LabelTargetStatement& node) override {}
    void visit(ConditionalBranchStatement& node) override {}
    void visit(CompoundStatement& node) override {}
    void visit(BlockStatement& node) override {}
    void visit(StringStatement& node) override {}
    void visit(SysCall& node) override {}
    void visit(MemberAccessExpression& node) override {}
    void visit(SuperMethodAccessExpression& node) override {}
    void visit(SuperMethodCallExpression& node) override {}
    void visit(FreeStatement& node) override {}
    void visit(DeferStatement& node) override {}
    void visit(RetainStatement& node) override {}
    void visit(RemanageStatement& node) override {}

private:
    SymbolTable* symbol_table_;
    ASTAnalyzer& analyzer_;
    bool debug_enabled_;
    std::string current_class_name_;
    
    // Helper methods
    void analyze_function_signature(const std::string& function_name, 
                                  const std::vector<std::string>& parameters, 
                                  ASTNode* body);
    void analyze_routine_signature(const std::string& routine_name, 
                                 const std::vector<std::string>& parameters, 
                                 ASTNode* body);
    VarType infer_parameter_type_from_usage(const std::string& param_name, 
                                           const std::string& function_name, 
                                           ASTNode* body);
    void debug_print(const std::string& message) const;
};
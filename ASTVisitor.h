#pragma once

#include "AST.h"

// Forward declarations of AST node classes
class Program;
class LetDeclaration;
class ManifestDeclaration;
class StaticDeclaration;
class GlobalDeclaration;
class GlobalVariableDeclaration;
class FunctionDeclaration;
class RoutineDeclaration;
class LabelDeclaration;
class NumberLiteral;
class StringLiteral;
class CharLiteral;
class BooleanLiteral;
class NullLiteral;
class VariableAccess;
class BinaryOp;
class UnaryOp;
class VectorAccess;
class CharIndirection;
class FloatVectorIndirection;
class BitfieldAccessExpression;
class FunctionCall;
class SysCall;
class ConditionalExpression;
class ValofExpression;
class FloatValofExpression;
class VecAllocationExpression;
class StringAllocationExpression;
class FVecAllocationExpression;
class TableExpression;
class AssignmentStatement;
class RoutineCallStatement;
class IfStatement;
class UnlessStatement;
class TestStatement;
class WhileStatement;
class UntilStatement;
class RepeatStatement;
class ForStatement;
class SwitchonStatement;
class CaseStatement;
class DefaultStatement;
class GotoStatement;
class ReturnStatement;
class FinishStatement;
class BreakStatement;
class LoopStatement;
class EndcaseStatement;
class ResultisStatement;
class CompoundStatement;
class BlockStatement;
class StringStatement;
class FreeStatement;
class BrkStatement;
class LabelTargetStatement;
class ConditionalBranchStatement;
class RemanageStatement;
class MinStatement;
class MaxStatement;
class SumStatement;


// Forward declaration for SUPER node
class SuperMethodCallExpression;

 // Base class for all AST visitors.
// Default implementations provided to allow partial implementation in derived classes.
class ASTVisitor {
public:
    virtual ~ASTVisitor() = default;

    // --- Declarations ---
    virtual void visit(Program& node) {}
    virtual void visit(LetDeclaration& node) {}
    virtual void visit(ManifestDeclaration& node) {}
    virtual void visit(StaticDeclaration& node) {}
    virtual void visit(GlobalDeclaration& node) {}
    virtual void visit(GlobalVariableDeclaration& node) {}
    virtual void visit(FunctionDeclaration& node) {}
    virtual void visit(RoutineDeclaration& node) {}
    virtual void visit(LabelDeclaration& node) {}
    virtual void visit(ClassDeclaration& node) {} // New: for ClassDeclaration

    // --- Expressions ---
    virtual void visit(NumberLiteral& node) {}
    virtual void visit(StringLiteral& node) {}
    virtual void visit(CharLiteral& node) {}
    virtual void visit(BooleanLiteral& node) {}
    virtual void visit(NullLiteral& node) {}
    virtual void visit(VariableAccess& node) {}
    virtual void visit(BinaryOp& node) {}
    virtual void visit(UnaryOp& node) {}
    virtual void visit(VectorAccess& node) {}
    virtual void visit(CharIndirection& node) {}
    virtual void visit(FloatVectorIndirection& node) {}
    virtual void visit(BitfieldAccessExpression& node) {}
    virtual void visit(FunctionCall& node) {}
    virtual void visit(SysCall& node) {}
    virtual void visit(ConditionalExpression& node) {}
    virtual void visit(ValofExpression& node) {}
    virtual void visit(FloatValofExpression& node) {}
    virtual void visit(VecAllocationExpression& node) {}
    virtual void visit(VecInitializerExpression& node) {}
    virtual void visit(FVecAllocationExpression& node) {}
    virtual void visit(PairsAllocationExpression& node) {}
    virtual void visit(StringAllocationExpression& node) {}
    virtual void visit(TableExpression& node) {}
    virtual void visit(ListExpression& node) {}
    virtual void visit(NewExpression& node) {} // New: for NewExpression
    virtual void visit(MemberAccessExpression& node) {} // New: for MemberAccessExpression
    virtual void visit(SuperMethodAccessExpression& node) {} // New: for SUPER.method access
    virtual void visit(SuperMethodCallExpression& node) {} // New: for SUPER.method() calls
    virtual void visit(PairExpression& node) {} // New: for pair(x,y) expressions
    virtual void visit(PairAccessExpression& node) {} // New: for p.first/p.second access
    virtual void visit(FPairExpression& node) {} // New: for fpair(x,y) expressions
    virtual void visit(FPairAccessExpression& node) {} // New: for fp.first/fp.second access
    virtual void visit(QuadExpression& node) {} // New: for quad(a,b,c,d) expressions
    virtual void visit(QuadAccessExpression& node) {} // New: for q.first/q.second/q.third/q.fourth access
    virtual void visit(OctExpression& node) {} // New: for oct(...) 8-lane vector expressions
    virtual void visit(FOctExpression& node) {} // New: for foct(...) 8-lane float vector expressions
    virtual void visit(LaneAccessExpression& node) {} // New: for vector.|n| lane access

    // --- Statements ---
    virtual void visit(AssignmentStatement& node) {}
    virtual void visit(RoutineCallStatement& node) {}
    virtual void visit(IfStatement& node) {}
    virtual void visit(UnlessStatement& node) {}
    virtual void visit(TestStatement& node) {}
    virtual void visit(WhileStatement& node) {}
    virtual void visit(UntilStatement& node) {}
    virtual void visit(RepeatStatement& node) {}
    virtual void visit(ForStatement& node) {}
    virtual void visit(ForEachStatement& node) {}
    virtual void visit(SwitchonStatement& node) {}
    virtual void visit(CaseStatement& node) {}
    virtual void visit(DefaultStatement& node) {}
    virtual void visit(GotoStatement& node) {}
    virtual void visit(ReturnStatement& node) {}
    virtual void visit(FinishStatement& node) {}
    virtual void visit(BreakStatement& node) {}
    virtual void visit(LoopStatement& node) {}
    virtual void visit(EndcaseStatement& node) {}
    virtual void visit(ResultisStatement& node) {}
    virtual void visit(CompoundStatement& node) {}
    virtual void visit(BlockStatement& node) {}
    virtual void visit(StringStatement& node) {}
    virtual void visit(RetainStatement& node) {} // <-- Added for RETAIN support
    virtual void visit(RemanageStatement& node) {} // <-- Added for REMANAGE support
    virtual void visit(FreeStatement& node) {}
    virtual void visit(BrkStatement& node) {}
    virtual void visit(LabelTargetStatement& node) {}
    virtual void visit(ConditionalBranchStatement& node) {}

    // --- DEFER ---
    virtual void visit(DeferStatement& node) {}
    virtual void visit(MinStatement& node) {} // <-- Added for MIN statement
    virtual void visit(MaxStatement& node) {} // <-- Added for MAX statement
    virtual void visit(SumStatement& node) {} // <-- Added for SUM statement
    virtual void visit(ReductionLoopStatement& node) {} // <-- Added for reduction loop metadata
};

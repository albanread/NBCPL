#include "AST.h"
#include "ASTVisitor.h"

// Out-of-line virtual destructors for base classes.
// Defining them out-of-line (even if defaulted) in a single .cpp file ensures
// that the vtable for the class is emitted once, preventing linker errors.
ASTNode::~ASTNode() {}
Program::~Program() {}
Declaration::~Declaration() {}
Expression::~Expression() {}
Statement::~Statement() {}
SuperMethodCallExpression::~SuperMethodCallExpression() {}
SuperMethodAccessExpression::~SuperMethodAccessExpression() {}

// --- AST Visitor Accept Methods ---
// Macro to quickly generate the accept methods for each AST node.
#define ACCEPT_METHOD_IMPL(ClassName) \
    void ClassName::accept(ASTVisitor& visitor) { visitor.visit(*this); }

// Program & Top-Level
ACCEPT_METHOD_IMPL(Program)

// Declarations
ACCEPT_METHOD_IMPL(LetDeclaration)
ACCEPT_METHOD_IMPL(ManifestDeclaration)
ACCEPT_METHOD_IMPL(StaticDeclaration)
ACCEPT_METHOD_IMPL(GlobalDeclaration)
ACCEPT_METHOD_IMPL(GlobalVariableDeclaration)
ACCEPT_METHOD_IMPL(ClassDeclaration)

ACCEPT_METHOD_IMPL(FunctionDeclaration)
ACCEPT_METHOD_IMPL(RoutineDeclaration)
ACCEPT_METHOD_IMPL(LabelDeclaration)


// Expressions
ACCEPT_METHOD_IMPL(NumberLiteral)
ACCEPT_METHOD_IMPL(StringLiteral)
ACCEPT_METHOD_IMPL(CharLiteral)
ACCEPT_METHOD_IMPL(BooleanLiteral)
ACCEPT_METHOD_IMPL(NullLiteral)
ACCEPT_METHOD_IMPL(VariableAccess)
ACCEPT_METHOD_IMPL(BinaryOp)
ACCEPT_METHOD_IMPL(UnaryOp)
ACCEPT_METHOD_IMPL(VectorAccess)
ACCEPT_METHOD_IMPL(CharIndirection)
ACCEPT_METHOD_IMPL(FloatVectorIndirection)
ACCEPT_METHOD_IMPL(FunctionCall)
ACCEPT_METHOD_IMPL(SysCall)
ACCEPT_METHOD_IMPL(BitfieldAccessExpression)
ACCEPT_METHOD_IMPL(ConditionalExpression)
ACCEPT_METHOD_IMPL(ValofExpression)
ACCEPT_METHOD_IMPL(FloatValofExpression)
ACCEPT_METHOD_IMPL(VecAllocationExpression)
ACCEPT_METHOD_IMPL(FVecAllocationExpression)
ACCEPT_METHOD_IMPL(StringAllocationExpression)
ACCEPT_METHOD_IMPL(TableExpression)
ACCEPT_METHOD_IMPL(ListExpression)
ACCEPT_METHOD_IMPL(NewExpression)
ACCEPT_METHOD_IMPL(MemberAccessExpression)
ACCEPT_METHOD_IMPL(SuperMethodCallExpression)
ACCEPT_METHOD_IMPL(SuperMethodAccessExpression)
ACCEPT_METHOD_IMPL(PairExpression)
ACCEPT_METHOD_IMPL(PairAccessExpression)
ACCEPT_METHOD_IMPL(FPairExpression)
ACCEPT_METHOD_IMPL(FPairAccessExpression)
ACCEPT_METHOD_IMPL(QuadExpression)
ACCEPT_METHOD_IMPL(QuadAccessExpression)
ACCEPT_METHOD_IMPL(OctExpression)
ACCEPT_METHOD_IMPL(FOctExpression)
ACCEPT_METHOD_IMPL(LaneAccessExpression)

ACCEPT_METHOD_IMPL(VecInitializerExpression)



// Statements
ACCEPT_METHOD_IMPL(ConditionalBranchStatement)
ACCEPT_METHOD_IMPL(AssignmentStatement)
ACCEPT_METHOD_IMPL(RoutineCallStatement)
ACCEPT_METHOD_IMPL(IfStatement)
ACCEPT_METHOD_IMPL(UnlessStatement)
ACCEPT_METHOD_IMPL(TestStatement)
ACCEPT_METHOD_IMPL(WhileStatement)
ACCEPT_METHOD_IMPL(UntilStatement)
ACCEPT_METHOD_IMPL(RepeatStatement)
ACCEPT_METHOD_IMPL(ForStatement)
ACCEPT_METHOD_IMPL(ForEachStatement)
ACCEPT_METHOD_IMPL(SwitchonStatement)
ACCEPT_METHOD_IMPL(CaseStatement)
ACCEPT_METHOD_IMPL(DefaultStatement)
ACCEPT_METHOD_IMPL(GotoStatement)
ACCEPT_METHOD_IMPL(ReturnStatement)
ACCEPT_METHOD_IMPL(FinishStatement)
ACCEPT_METHOD_IMPL(BreakStatement)
ACCEPT_METHOD_IMPL(LoopStatement)
ACCEPT_METHOD_IMPL(EndcaseStatement)
ACCEPT_METHOD_IMPL(ResultisStatement)
ACCEPT_METHOD_IMPL(CompoundStatement)
ACCEPT_METHOD_IMPL(BlockStatement)
ACCEPT_METHOD_IMPL(StringStatement)
ACCEPT_METHOD_IMPL(LabelTargetStatement)
ACCEPT_METHOD_IMPL(BrkStatement)
ACCEPT_METHOD_IMPL(FreeStatement)
ACCEPT_METHOD_IMPL(DeferStatement)

#undef ACCEPT_METHOD_IMPL

// --- RetainStatement out-of-line virtuals ---
void RetainStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

ASTNodePtr RetainStatement::clone() const {
    return std::make_unique<RetainStatement>(variable_names);
}

void RemanageStatement::accept(ASTVisitor& visitor) {
    visitor.visit(*this);
}

ASTNodePtr RemanageStatement::clone() const {
    return std::make_unique<RemanageStatement>(variable_names);
}

ASTNodePtr PairExpression::clone() const {
    return std::make_unique<PairExpression>(
        first_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(first_expr->clone().release())) : nullptr,
        second_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(second_expr->clone().release())) : nullptr
    );
}

ASTNodePtr PairAccessExpression::clone() const {
    return std::make_unique<PairAccessExpression>(
        pair_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(pair_expr->clone().release())) : nullptr,
        access_type
    );
}

ASTNodePtr FPairExpression::clone() const {
    return std::make_unique<FPairExpression>(
        first_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(first_expr->clone().release())) : nullptr,
        second_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(second_expr->clone().release())) : nullptr
    );
}

ASTNodePtr QuadExpression::clone() const {
    return std::make_unique<QuadExpression>(
        first_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(first_expr->clone().release())) : nullptr,
        second_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(second_expr->clone().release())) : nullptr,
        third_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(third_expr->clone().release())) : nullptr,
        fourth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(fourth_expr->clone().release())) : nullptr
    );
}

ASTNodePtr OctExpression::clone() const {
    return std::make_unique<OctExpression>(
        first_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(first_expr->clone().release())) : nullptr,
        second_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(second_expr->clone().release())) : nullptr,
        third_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(third_expr->clone().release())) : nullptr,
        fourth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(fourth_expr->clone().release())) : nullptr,
        fifth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(fifth_expr->clone().release())) : nullptr,
        sixth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(sixth_expr->clone().release())) : nullptr,
        seventh_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(seventh_expr->clone().release())) : nullptr,
        eighth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(eighth_expr->clone().release())) : nullptr
    );
}

ASTNodePtr FOctExpression::clone() const {
    return std::make_unique<FOctExpression>(
        first_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(first_expr->clone().release())) : nullptr,
        second_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(second_expr->clone().release())) : nullptr,
        third_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(third_expr->clone().release())) : nullptr,
        fourth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(fourth_expr->clone().release())) : nullptr,
        fifth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(fifth_expr->clone().release())) : nullptr,
        sixth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(sixth_expr->clone().release())) : nullptr,
        seventh_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(seventh_expr->clone().release())) : nullptr,
        eighth_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(eighth_expr->clone().release())) : nullptr
    );
}

ASTNodePtr LaneAccessExpression::clone() const {
    return std::make_unique<LaneAccessExpression>(
        vector_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(vector_expr->clone().release())) : nullptr,
        index
    );
}

ASTNodePtr QuadAccessExpression::clone() const {
    return std::make_unique<QuadAccessExpression>(
        quad_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(quad_expr->clone().release())) : nullptr,
        access_type
    );
}

ASTNodePtr FPairAccessExpression::clone() const {
    return std::make_unique<FPairAccessExpression>(
        pair_expr ? std::unique_ptr<Expression>(static_cast<Expression*>(pair_expr->clone().release())) : nullptr,
        access_type
    );
}

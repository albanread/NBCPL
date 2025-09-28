#pragma once

#include "../AST.h"
#include "../ASTVisitor.h"
#include "../analysis/ASTAnalyzer.h"
#include "../AST_Cloner.h"
#include <string>
#include <memory>

class MethodInliningPass : public ASTVisitor {
public:
    MethodInliningPass(ASTAnalyzer& analyzer);

    void run(Program& program);

    void visit(FunctionCall& node) override;
    void visit(RoutineCallStatement& node) override;

    void replace_current_expression(ExprPtr new_expr);
    void replace_current_statement(StmtPtr new_stmt);

    void visit(Program& node) override;
    void visit(BlockStatement& node) override;
    void visit(CompoundStatement& node) override;

private:
    ASTAnalyzer& analyzer_;
    ExprPtr expr_replacement_;
    StmtPtr stmt_replacement_;
};

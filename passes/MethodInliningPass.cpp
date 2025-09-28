// MethodInliningPass.cpp
#include "MethodInliningPass.h"
#include <iostream>
#include <string>

// Utility: Check if a method is safe to inline (trivial accessor/setter)
bool is_method_safe_to_inline(const std::string& method_name, const ASTAnalyzer& analyzer) {
    auto it = analyzer.get_function_metrics().find(method_name);
    return it != analyzer.get_function_metrics().end() && it->second.is_safe_to_inline;
}

// Utility: Get the member name for a trivial accessor/setter
std::string get_inlined_member_name(const std::string& method_name, const ASTAnalyzer& analyzer) {
    auto it = analyzer.get_function_metrics().find(method_name);
    if (it != analyzer.get_function_metrics().end()) {
        return it->second.accessed_member_name;
    }
    return "";
}

MethodInliningPass::MethodInliningPass(ASTAnalyzer& analyzer)
    : analyzer_(analyzer) {}

void MethodInliningPass::run(Program& program) {
    program.accept(*this);
}

void MethodInliningPass::visit(FunctionCall& node) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        const std::string& method_name = var_access->name;
        if (is_method_safe_to_inline(method_name, analyzer_) &&
            analyzer_.is_trivial_accessor_method(method_name)) {
            std::string member_name = get_inlined_member_name(method_name, analyzer_);
            if (!member_name.empty() && node.arguments.size() == 1) {
                ExprPtr object_expr = clone_unique_ptr(node.arguments[0]);
                auto member_access = std::make_unique<MemberAccessExpression>(std::move(object_expr), member_name);
                replace_current_expression(std::move(member_access));
                return;
            }
        }
    }
    ASTVisitor::visit(node);
}

void MethodInliningPass::visit(RoutineCallStatement& node) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
        const std::string& method_name = var_access->name;
        if (is_method_safe_to_inline(method_name, analyzer_) &&
            analyzer_.is_trivial_setter_method(method_name)) {
            std::string member_name = get_inlined_member_name(method_name, analyzer_);
            if (!member_name.empty() && node.arguments.size() == 2) {
                ExprPtr object_expr = clone_unique_ptr(node.arguments[0]);
                ExprPtr value_expr = clone_unique_ptr(node.arguments[1]);
                auto member_access = std::make_unique<MemberAccessExpression>(std::move(object_expr), member_name);
                std::vector<ExprPtr> lhs_vec;
                lhs_vec.push_back(std::move(member_access));
                std::vector<ExprPtr> rhs_vec;
                rhs_vec.push_back(std::move(value_expr));
                auto assignment = std::make_unique<AssignmentStatement>(std::move(lhs_vec), std::move(rhs_vec));
                replace_current_statement(std::move(assignment));
                return;
            }
        }
    }
    ASTVisitor::visit(node);
}

void MethodInliningPass::replace_current_expression(ExprPtr new_expr) {
    expr_replacement_ = std::move(new_expr);
}

void MethodInliningPass::replace_current_statement(StmtPtr new_stmt) {
    stmt_replacement_ = std::move(new_stmt);
}

void MethodInliningPass::visit(Program& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (stmt_replacement_) {
            stmt = std::move(stmt_replacement_);
            stmt_replacement_ = nullptr;
        }
    }
}

void MethodInliningPass::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (stmt_replacement_) {
            stmt = std::move(stmt_replacement_);
            stmt_replacement_ = nullptr;
        }
    }
}

void MethodInliningPass::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        stmt->accept(*this);
        if (stmt_replacement_) {
            stmt = std::move(stmt_replacement_);
            stmt_replacement_ = nullptr;
        }
    }
}

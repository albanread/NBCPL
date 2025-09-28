#include "RetainAnalysisPass.h"
#include "../AST.h"
#include "SymbolTable.h"
#include "Symbol.h"
#include <algorithm>

// --- Stub constructor ---
RetainAnalysisPass::RetainAnalysisPass() {}

// --- Helper Visitor to Collect Retained Variable Names ---
class RetainedVariableCollector : public ASTVisitor {
public:
    std::set<std::string> retained_vars;

    void visit(RetainStatement& node) override {
        for (const auto& name : node.variable_names) {
            retained_vars.insert(name);
        }
    }

    void visit(CompoundStatement& node) override {
        for (auto& stmt : node.statements) if (stmt) stmt->accept(*this);
    }
    void visit(BlockStatement& node) override {
        for (auto& stmt : node.statements) if (stmt) stmt->accept(*this);
    }
    void visit(IfStatement& node) override {
        if (node.then_branch) node.then_branch->accept(*this);
    }
    void visit(TestStatement& node) override {
        if (node.then_branch) node.then_branch->accept(*this);
        if (node.else_branch) node.else_branch->accept(*this);
    }
    void visit(ForStatement& node) override {
        if (node.body) node.body->accept(*this);
    }
    // Add other statement visitors as needed...
};

// Utility: Remove DeferStatements for retained variables from a statement vector
static void remove_defer_for_retained(std::vector<StmtPtr>& stmts, const std::set<std::string>& retained_vars) {
    auto is_retained_defer = [&](const StmtPtr& stmt) {
        if (!stmt) return false;
        if (stmt->getType() != ASTNode::NodeType::DeferStmt) return false;
        auto* defer_stmt = static_cast<DeferStatement*>(stmt.get());
        if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(defer_stmt->deferred_statement.get())) {
            // Case 1: DEFER obj.RELEASE()
            if (auto* member_access = dynamic_cast<MemberAccessExpression*>(routine_call->routine_expr.get())) {
                if (!member_access->member_name.empty() && member_access->member_name == "RELEASE") {
                    if (auto* var_access = dynamic_cast<VariableAccess*>(member_access->object_expr.get())) {
                        return retained_vars.count(var_access->name) > 0;
                    }
                }
            }
            // Case 2: DEFER BCPL_FREE_LIST(list_var) or DEFER FREEVEC(str_or_vec_var)
            else if (auto* func_access = dynamic_cast<VariableAccess*>(routine_call->routine_expr.get())) {
                if ((func_access->name == "BCPL_FREE_LIST" || func_access->name == "FREEVEC") && !routine_call->arguments.empty()) {
                    if (auto* arg_var = dynamic_cast<VariableAccess*>(routine_call->arguments[0].get())) {
                        return retained_vars.count(arg_var->name) > 0;
                    }
                }
            }
        }
        return false;
    };
    stmts.erase(std::remove_if(stmts.begin(), stmts.end(), is_retained_defer), stmts.end());
}

// --- RETAIN analysis logic ---
void RetainAnalysisPass::visit(FunctionDeclaration& node) {
    if (!node.body || !symbol_table_) return;

    RetainedVariableCollector collector;
    node.body->accept(collector);
    const auto& retained_vars = collector.retained_vars;
    if (retained_vars.empty()) return;

    for (const auto& var_name : retained_vars) {
        Symbol symbol;
        // Use the function-context-aware lookup
        if (symbol_table_->lookup(var_name, node.name, symbol)) {
            symbol.owns_heap_memory = false; // Disable auto-cleanup
            symbol_table_->updateSymbol(var_name, symbol);
        }
    }
}

void RetainAnalysisPass::visit(RoutineDeclaration& node) {
    if (!node.body || !symbol_table_) return;

    RetainedVariableCollector collector;
    node.body->accept(collector);
    const auto& retained_vars = collector.retained_vars;
    if (retained_vars.empty()) return;

    for (const auto& var_name : retained_vars) {
        Symbol symbol;
        if (symbol_table_->lookup(var_name, node.name, symbol)) {
            symbol.owns_heap_memory = false; // Disable auto-cleanup
            symbol_table_->updateSymbol(var_name, symbol);
        }
    }
}

// Other visitors remain stubs
void RetainAnalysisPass::visit(RetainStatement& node) {}
void RetainAnalysisPass::visit(RemanageStatement& node) {}
void RetainAnalysisPass::visit(ResultisStatement& node) {}
void RetainAnalysisPass::visit(ReturnStatement& node) {}
void RetainAnalysisPass::visit(CompoundStatement& node) {}
void RetainAnalysisPass::visit(DeferStatement& node) {}

void RetainAnalysisPass::run(Program& program, SymbolTable& symbol_table) {
    symbol_table_ = &symbol_table; // Store the reference
    program.accept(*this);
}


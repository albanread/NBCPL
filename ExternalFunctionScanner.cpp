#include "ExternalFunctionScanner.h"
#include "analysis/ASTAnalyzer.h"
#include <iostream>

// --- Core API ---

std::set<std::string> ExternalFunctionScanner::scan(ASTNode& node) {
    external_functions_.clear();
    node.accept(*this);
    return external_functions_;
}

size_t ExternalFunctionScanner::calculate_veneer_section_size() const {
    return external_functions_.size() * get_veneer_size();
}

// --- Utility ---

bool ExternalFunctionScanner::is_external_function(const std::string& function_name) const {
    // Check if this function is registered in the RuntimeManager
    return RuntimeManager::instance().is_function_registered(function_name);
}

// --- Program Root ---

void ExternalFunctionScanner::visit(Program& node) {
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

// --- Function and Routine Calls (Core Collection Logic) ---

void ExternalFunctionScanner::visit(FunctionCall& node) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        if (is_external_function(var_access->name)) {
            external_functions_.insert(var_access->name);
        }
    }
    if (node.function_expr) node.function_expr->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void ExternalFunctionScanner::visit(RoutineCallStatement& node) {
    if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
        if (is_external_function(var_access->name)) {
            external_functions_.insert(var_access->name);
        }
    }
    if (node.routine_expr) node.routine_expr->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

// --- Declarations ---

void ExternalFunctionScanner::visit(LetDeclaration& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void ExternalFunctionScanner::visit(FunctionDeclaration& node) {
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(RoutineDeclaration& node) {
    if (node.body) node.body->accept(*this);
}

// --- Containers (CRITICAL for finding nested calls) ---

void ExternalFunctionScanner::visit(BlockStatement& node) {
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void ExternalFunctionScanner::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

// --- Control Flow Statements (CRITICAL for finding calls in loop/branch bodies) ---

void ExternalFunctionScanner::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void ExternalFunctionScanner::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void ExternalFunctionScanner::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) node.else_branch->accept(*this);
}

void ExternalFunctionScanner::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(UntilStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(RepeatStatement& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void ExternalFunctionScanner::visit(ForStatement& node) {
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(ForEachStatement& node) {
    if (node.collection_expression) node.collection_expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    for (const auto& case_stmt : node.cases) {
        if (case_stmt && case_stmt->command) case_stmt->command->accept(*this);
    }
    if (node.default_case && node.default_case->command) {
        node.default_case->command->accept(*this);
    }
}

// --- Statements with Expressions (Ensure recursion into expressions) ---

void ExternalFunctionScanner::visit(AssignmentStatement& node) {
    for (auto& lhs_expr : node.lhs) {
        if (lhs_expr) lhs_expr->accept(*this);
    }
    for (auto& rhs_expr : node.rhs) {
        if (rhs_expr) rhs_expr->accept(*this);
    }
}

void ExternalFunctionScanner::visit(ResultisStatement& node) {
    if (node.expression) node.expression->accept(*this);
}

void ExternalFunctionScanner::visit(FreeStatement& node) {
    if (node.list_expr) node.list_expr->accept(*this);
}

// --- Expressions that Contain Calls/Recursion (Ensure traversal continues) ---

void ExternalFunctionScanner::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void ExternalFunctionScanner::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

void ExternalFunctionScanner::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}

void ExternalFunctionScanner::visit(VecAllocationExpression& node) {
    // Vector allocation requires GETVEC runtime function
    external_functions_.insert("GETVEC");
    if (node.size_expr) node.size_expr->accept(*this);
}

void ExternalFunctionScanner::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void ExternalFunctionScanner::visit(FVecAllocationExpression& node) {
    // Float vector allocation requires FGETVEC runtime function
    external_functions_.insert("FGETVEC");
    if (node.size_expr) node.size_expr->accept(*this);
}

void ExternalFunctionScanner::visit(PairsAllocationExpression& node) {
    // PAIRS vector allocation uses existing GETVEC runtime function
    external_functions_.insert("GETVEC");
    if (node.size_expr) node.size_expr->accept(*this);
}

void ExternalFunctionScanner::visit(TableExpression& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void ExternalFunctionScanner::visit(ListExpression& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void ExternalFunctionScanner::visit(VecInitializerExpression& node) {
    // Vector initialization requires BCPL_ALLOC_WORDS runtime function
    external_functions_.insert("BCPL_ALLOC_WORDS");
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void ExternalFunctionScanner::visit(ValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(FloatValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void ExternalFunctionScanner::visit(NewExpression& node) {
    for (auto& arg : node.constructor_arguments) {
        if (arg) arg->accept(*this);
    }
}

void ExternalFunctionScanner::visit(MemberAccessExpression& node) {
    if (node.object_expr) node.object_expr->accept(*this);
}

void ExternalFunctionScanner::visit(SuperMethodCallExpression& node) {
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

// --- Other expressions/statements as needed (add more as your AST grows) ---

void ExternalFunctionScanner::visit(VectorAccess& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}
#include "VariableUsageVisitor.h"
#include "../../AST.h"

// The core logic: when we see a variable, add it to our set.
void VariableUsageVisitor::visit(VariableAccess& node) {
    // Only track actual variables, not function/routine names
    if (symbol_table_) {
        Symbol symbol;
        if (symbol_table_->lookup(node.name, symbol) && symbol.is_variable()) {
            variables_.insert(node.name);
        }
    } else {
        // Fallback to old behavior if no symbol table available
        variables_.insert(node.name);
    }
}

// --- Container Traversal ---
void VariableUsageVisitor::visit(AssignmentStatement& node) {
    for (auto& expr : node.lhs) if (expr) expr->accept(*this);
    for (auto& expr : node.rhs) if (expr) expr->accept(*this);
}
void VariableUsageVisitor::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) if (stmt) stmt->accept(*this);
}
void VariableUsageVisitor::visit(BlockStatement& node) {
    for (auto& decl : node.declarations) if (decl) decl->accept(*this);
    for (auto& stmt : node.statements) if (stmt) stmt->accept(*this);
}
void VariableUsageVisitor::visit(LetDeclaration& node) {
    for (auto& init : node.initializers) if (init) init->accept(*this);
}
void VariableUsageVisitor::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}
void VariableUsageVisitor::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) node.else_branch->accept(*this);
}
void VariableUsageVisitor::visit(ForStatement& node) {
    variables_.insert(node.loop_variable); // The loop variable is used and defined
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    if (node.body) node.body->accept(*this);
}
void VariableUsageVisitor::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}
void VariableUsageVisitor::visit(RoutineCallStatement& node) {
    // Check if the routine being called is actually a variable (function pointer)
    bool is_variable_call = false;
    if (symbol_table_ && node.routine_expr) {
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.routine_expr.get())) {
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol) && symbol.is_variable()) {
                is_variable_call = true;
            }
        }
    }
    
    // Only visit the routine expression if it's a variable (function pointer)
    if (is_variable_call && node.routine_expr) {
        node.routine_expr->accept(*this);
    }
    
    // Always visit arguments
    for (auto& arg : node.arguments) if (arg) arg->accept(*this);
}
void VariableUsageVisitor::visit(FunctionCall& node) {
    // CRITICAL FIX: Always visit the function expression for method calls
    // Method calls like obj.method() are represented as FunctionCall where
    // function_expr is a MemberAccessExpression containing the object
    // This ensures that the object variable gets tracked for liveness analysis
    if (node.function_expr) {
        node.function_expr->accept(*this);
    }
    
    // Always visit arguments
    for (auto& arg : node.arguments) {
        if (arg) {
            arg->accept(*this);
        }
    }
}
void VariableUsageVisitor::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}
void VariableUsageVisitor::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

// --- Object-Oriented Nodes ---
void VariableUsageVisitor::visit(MemberAccessExpression& node) {
    // Visit the object expression to track the base object for liveness
    if (node.object_expr) {
        node.object_expr->accept(*this);
    }
}
void VariableUsageVisitor::visit(NewExpression& node) {
    for (auto& arg : node.constructor_arguments) if (arg) arg->accept(*this);
}
void VariableUsageVisitor::visit(SuperMethodCallExpression& node) {
    // SuperMethodCall implies usage of 'this' object
    if (symbol_table_) {
        Symbol symbol;
        if (symbol_table_->lookup("_this", symbol) && symbol.is_variable()) {
            variables_.insert("_this");
        }
    } else {
        variables_.insert("_this");
    }
    
    for (auto& arg : node.arguments) if (arg) arg->accept(*this);
}

// --- Other Expression/Statement Traversal ---
void VariableUsageVisitor::visit(VectorAccess& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}
void VariableUsageVisitor::visit(CharIndirection& node) {
    if (node.string_expr) node.string_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}
void VariableUsageVisitor::visit(FloatVectorIndirection& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}
void VariableUsageVisitor::visit(SysCall& node) {
    if (node.syscall_number) node.syscall_number->accept(*this);
    for (auto& arg : node.arguments) if (arg) arg->accept(*this);
}
void VariableUsageVisitor::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}
void VariableUsageVisitor::visit(ValofExpression& node) {
    if (node.body) node.body->accept(*this);
}
void VariableUsageVisitor::visit(FloatValofExpression& node) {
    if (node.body) node.body->accept(*this);
}
void VariableUsageVisitor::visit(VecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}
void VariableUsageVisitor::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}
void VariableUsageVisitor::visit(TableExpression& node) {
    for (auto& init : node.initializers) if (init) init->accept(*this);
}
void VariableUsageVisitor::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}
void VariableUsageVisitor::visit(UntilStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}
void VariableUsageVisitor::visit(RepeatStatement& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}
void VariableUsageVisitor::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    for (auto& case_stmt : node.cases) if (case_stmt) case_stmt->accept(*this);
    if (node.default_case) node.default_case->accept(*this);
}
void VariableUsageVisitor::visit(CaseStatement& node) {
    if (node.constant_expr) node.constant_expr->accept(*this);
    if (node.command) node.command->accept(*this);
}
void VariableUsageVisitor::visit(DefaultStatement& node) {
    if (node.command) node.command->accept(*this);
}
void VariableUsageVisitor::visit(ResultisStatement& node) {
    if (node.expression) node.expression->accept(*this);
}
void VariableUsageVisitor::visit(FreeStatement& node) {
    if (node.list_expr) node.list_expr->accept(*this);
}

void VariableUsageVisitor::visit(PairwiseReductionLoopStatement& node) {
    // Record uses of the input vector variables
    if (!node.vector_a_name.empty()) {
        variables_.insert(node.vector_a_name);
    }
    if (!node.vector_b_name.empty()) {
        variables_.insert(node.vector_b_name);
    }
    // Note: We don't record result_vector_name as a "use" since it's being written to
}
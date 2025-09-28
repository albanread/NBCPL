#include "SuperCallTransformationPass.h"
#include <iostream>

SuperCallTransformationPass::SuperCallTransformationPass(ClassTable* class_table, 
                                                         SymbolTable* symbol_table, 
                                                         bool trace_enabled)
    : class_table_(class_table), symbol_table_(symbol_table), trace_enabled_(trace_enabled),
      transformation_occurred_(false) {}

ProgramPtr SuperCallTransformationPass::transform(ProgramPtr program) {
    if (!program) return program;
    
    debug_print("Starting SUPER call transformation pass");
    transformation_occurred_ = false;
    
    // Visit the entire program to transform SUPER calls
    program->accept(*this);
    
    if (transformation_occurred_) {
        debug_print("SUPER call transformations completed - program modified");
    } else {
        debug_print("No SUPER calls found - program unchanged");
    }
    
    return program;
}

void SuperCallTransformationPass::visit(Program& node) {
    debug_print("Visiting Program node");
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

void SuperCallTransformationPass::visit(ClassDeclaration& node) {
    std::string prev_class = current_class_name_;
    current_class_name_ = node.name;
    
    debug_print("Entering class: " + current_class_name_);
    
    // Visit all methods in the class
    for (auto& member : node.members) {
        if (member.declaration) member.declaration->accept(*this);
    }
    
    current_class_name_ = prev_class;
    debug_print("Exiting class: " + node.name);
}

void SuperCallTransformationPass::visit(FunctionDeclaration& node) {
    std::string prev_method = current_method_name_;
    current_method_name_ = node.name;
    
    debug_print("Entering method: " + current_method_name_);
    
    // Visit the function body
    if (node.body) {
        node.body->accept(*this);
    }
    
    current_method_name_ = prev_method;
}

void SuperCallTransformationPass::visit(RoutineDeclaration& node) {
    std::string prev_method = current_method_name_;
    current_method_name_ = node.name;
    
    debug_print("Entering routine: " + current_method_name_);
    
    // Visit the routine body
    if (node.body) {
        node.body->accept(*this);
    }
    
    current_method_name_ = prev_method;
}

void SuperCallTransformationPass::visit(FunctionCall& node) {
    debug_print("Visiting FunctionCall");
    
    // Check if this is a SUPER call
    if (transform_super_function_call(node)) {
        transformation_occurred_ = true;
        return;
    }
    
    // Visit function expression and arguments for nested transformations
    if (node.function_expr) {
        node.function_expr->accept(*this);
    }
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void SuperCallTransformationPass::visit(RoutineCallStatement& node) {
    debug_print("Visiting RoutineCallStatement");
    
    // Check if this is a SUPER call
    if (transform_super_routine_call(node)) {
        transformation_occurred_ = true;
        return;
    }
    
    // Visit routine expression and arguments for nested transformations
    if (node.routine_expr) {
        node.routine_expr->accept(*this);
    }
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

bool SuperCallTransformationPass::transform_super_function_call(FunctionCall& node) {
    // Check if the function expression is a SuperMethodAccessExpression
    auto* super_access = dynamic_cast<SuperMethodAccessExpression*>(node.function_expr.get());
    if (!super_access) {
        return false; // Not a SUPER call
    }
    
    debug_print("Found SUPER function call: " + super_access->member_name);
    
    // Get parent class name
    std::string parent_class_name = get_parent_class_name();
    if (parent_class_name.empty()) {
        std::cerr << "[ERROR] SUPER call in class with no parent: " + current_class_name_ << std::endl;
        return false;
    }
    
    debug_print("Transforming SUPER." + super_access->member_name + " to " + parent_class_name + "::" + super_access->member_name);
    
    // Create new function call to parent method
    ExprPtr new_call = create_parent_method_call(super_access, node.arguments, parent_class_name);
    if (!new_call) {
        return false;
    }
    
    // Replace the function expression with a variable access to the parent method
    std::string parent_method_name = parent_class_name + "::" + super_access->member_name;
    node.function_expr = std::make_unique<VariableAccess>(parent_method_name);
    
    // Insert '_this' as the first argument
    std::vector<ExprPtr> new_arguments;
    new_arguments.push_back(std::make_unique<VariableAccess>("_this"));
    
    // Add the original arguments
    for (auto& arg : node.arguments) {
        new_arguments.push_back(std::move(arg));
    }
    
    node.arguments = std::move(new_arguments);
    
    debug_print("Successfully transformed SUPER function call");
    return true;
}

bool SuperCallTransformationPass::transform_super_routine_call(RoutineCallStatement& node) {
    // Check if the routine expression is a SuperMethodAccessExpression
    auto* super_access = dynamic_cast<SuperMethodAccessExpression*>(node.routine_expr.get());
    if (!super_access) {
        return false; // Not a SUPER call
    }
    
    debug_print("Found SUPER routine call: " + super_access->member_name);
    
    // Get parent class name
    std::string parent_class_name = get_parent_class_name();
    if (parent_class_name.empty()) {
        std::cerr << "[ERROR] SUPER call in class with no parent: " + current_class_name_ << std::endl;
        return false;
    }
    
    debug_print("Transforming SUPER." + super_access->member_name + " to " + parent_class_name + "::" + super_access->member_name);
    
    // Replace the routine expression with a variable access to the parent method
    std::string parent_method_name = parent_class_name + "::" + super_access->member_name;
    node.routine_expr = std::make_unique<VariableAccess>(parent_method_name);
    
    // Insert '_this' as the first argument
    std::vector<ExprPtr> new_arguments;
    new_arguments.push_back(std::make_unique<VariableAccess>("_this"));
    
    // Add the original arguments
    for (auto& arg : node.arguments) {
        new_arguments.push_back(std::move(arg));
    }
    
    node.arguments = std::move(new_arguments);
    
    debug_print("Successfully transformed SUPER routine call");
    return true;
}

ExprPtr SuperCallTransformationPass::create_parent_method_call(SuperMethodAccessExpression* super_access,
                                                              const std::vector<ExprPtr>& arguments,
                                                              const std::string& parent_class_name) {
    // Create a variable access for the parent method
    std::string parent_method_name = parent_class_name + "::" + super_access->member_name;
    auto function_expr = std::make_unique<VariableAccess>(parent_method_name);
    
    // Create arguments list with '_this' as first argument
    std::vector<ExprPtr> new_arguments;
    new_arguments.push_back(std::make_unique<VariableAccess>("_this"));
    
    // Clone the original arguments
    for (const auto& arg : arguments) {
        if (arg) {
            new_arguments.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(arg->clone().release())));
        }
    }
    
    // Create the new function call
    return std::make_unique<FunctionCall>(std::move(function_expr), std::move(new_arguments));
}

StmtPtr SuperCallTransformationPass::create_parent_routine_call(SuperMethodAccessExpression* super_access,
                                                               const std::vector<ExprPtr>& arguments,
                                                               const std::string& parent_class_name) {
    // Create a variable access for the parent method
    std::string parent_method_name = parent_class_name + "::" + super_access->member_name;
    auto routine_expr = std::make_unique<VariableAccess>(parent_method_name);
    
    // Create arguments list with '_this' as first argument
    std::vector<ExprPtr> new_arguments;
    new_arguments.push_back(std::make_unique<VariableAccess>("_this"));
    
    // Clone the original arguments
    for (const auto& arg : arguments) {
        if (arg) {
            new_arguments.push_back(std::unique_ptr<Expression>(static_cast<Expression*>(arg->clone().release())));
        }
    }
    
    // Create the new routine call statement
    return std::make_unique<RoutineCallStatement>(std::move(routine_expr), std::move(new_arguments));
}

std::string SuperCallTransformationPass::get_parent_class_name() const {
    if (current_class_name_.empty() || !class_table_) {
        return "";
    }
    
    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
    if (!class_entry) {
        return "";
    }
    
    return class_entry->parent_name;
}

void SuperCallTransformationPass::debug_print(const std::string& message) const {
    if (trace_enabled_) {
        std::cout << "[SuperCallTransformationPass] " << message << std::endl;
    }
}

// --- Traversal methods that visit children ---

void SuperCallTransformationPass::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void SuperCallTransformationPass::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void SuperCallTransformationPass::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void SuperCallTransformationPass::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void SuperCallTransformationPass::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) node.else_branch->accept(*this);
}

void SuperCallTransformationPass::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(UntilStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(RepeatStatement& node) {
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(ForStatement& node) {
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(ForEachStatement& node) {
    if (node.collection_expression) node.collection_expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    for (auto& case_stmt : node.cases) {
        if (case_stmt) case_stmt->accept(*this);
    }
    if (node.default_case) node.default_case->accept(*this);
}

void SuperCallTransformationPass::visit(CaseStatement& node) {
    if (node.constant_expr) node.constant_expr->accept(*this);
    if (node.command) node.command->accept(*this);
}

void SuperCallTransformationPass::visit(DefaultStatement& node) {
    if (node.command) node.command->accept(*this);
}

void SuperCallTransformationPass::visit(ValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(FloatValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void SuperCallTransformationPass::visit(AssignmentStatement& node) {
    for (auto& lhs : node.lhs) {
        if (lhs) lhs->accept(*this);
    }
    for (auto& rhs : node.rhs) {
        if (rhs) rhs->accept(*this);
    }
}

void SuperCallTransformationPass::visit(ReturnStatement& node) {
    // No children to visit
}

void SuperCallTransformationPass::visit(ResultisStatement& node) {
    if (node.expression) node.expression->accept(*this);
}

void SuperCallTransformationPass::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void SuperCallTransformationPass::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

void SuperCallTransformationPass::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}

void SuperCallTransformationPass::visit(VectorAccess& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void SuperCallTransformationPass::visit(CharIndirection& node) {
    if (node.string_expr) node.string_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void SuperCallTransformationPass::visit(FloatVectorIndirection& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void SuperCallTransformationPass::visit(MemberAccessExpression& node) {
    if (node.object_expr) node.object_expr->accept(*this);
}

void SuperCallTransformationPass::visit(VecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void SuperCallTransformationPass::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void SuperCallTransformationPass::visit(FVecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void SuperCallTransformationPass::visit(NewExpression& node) {
    // No children to visit for constructor calls
}

void SuperCallTransformationPass::visit(TableExpression& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void SuperCallTransformationPass::visit(ListExpression& node) {
    for (auto& elem : node.initializers) {
        if (elem) elem->accept(*this);
    }
}

void SuperCallTransformationPass::visit(VecInitializerExpression& node) {
    for (auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void SuperCallTransformationPass::visit(SysCall& node) {
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void SuperCallTransformationPass::visit(FinishStatement& node) {
    if (node.syscall_number) node.syscall_number->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void SuperCallTransformationPass::visit(FreeStatement& node) {
    if (node.list_expr) node.list_expr->accept(*this);
}
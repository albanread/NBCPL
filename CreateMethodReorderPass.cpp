#include "CreateMethodReorderPass.h"
#include "AST.h"
#include <iostream>
#include <algorithm>

CreateMethodReorderPass::CreateMethodReorderPass(ClassTable* class_table, bool trace_enabled)
    : class_table_(class_table), trace_enabled_(trace_enabled), transformation_occurred_(false) {
}

ProgramPtr CreateMethodReorderPass::transform(ProgramPtr program) {
    if (!program) return program;
    
    transformation_occurred_ = false;
    current_class_name_.clear();
    
    debug_print("Starting CREATE method reordering pass");
    program->accept(*this);
    
    if (transformation_occurred_) {
        debug_print("CREATE method reordering transformations applied");
    } else {
        debug_print("No CREATE method reordering needed");
    }
    
    return program;
}

void CreateMethodReorderPass::visit(Program& node) {
    debug_print("Visiting Program node");
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
}

void CreateMethodReorderPass::visit(ClassDeclaration& node) {
    debug_print("Visiting ClassDeclaration: " + node.name);
    
    std::string previous_class = current_class_name_;
    current_class_name_ = node.name;
    
    // Visit all class members
    for (auto& member : node.members) {
        if (member.declaration) {
            member.declaration->accept(*this);
        }
    }
    
    current_class_name_ = previous_class;
}

void CreateMethodReorderPass::visit(RoutineDeclaration& node) {
    debug_print("Visiting RoutineDeclaration: " + node.name);
    
    // Only transform CREATE methods in class context
    if (node.name == "CREATE" && !current_class_name_.empty()) {
        debug_print("Found CREATE method in class " + current_class_name_);
        
        if (auto* compound_body = dynamic_cast<CompoundStatement*>(node.body.get())) {
            debug_print("Analyzing CREATE method body (CompoundStatement)...");
            if (reorder_create_method_body(compound_body)) {
                debug_print("Reordered CREATE method in class " + current_class_name_);
                transformation_occurred_ = true;
            } else {
                debug_print("No reordering needed for CREATE method in class " + current_class_name_);
            }
        } else if (auto* block_body = dynamic_cast<BlockStatement*>(node.body.get())) {
            debug_print("Analyzing CREATE method body (BlockStatement)...");
            if (reorder_block_statement_body(block_body)) {
                debug_print("Reordered CREATE method in class " + current_class_name_);
                transformation_occurred_ = true;
            } else {
                debug_print("No reordering needed for CREATE method in class " + current_class_name_);
            }
        } else {
            debug_print("CREATE method body is neither CompoundStatement nor BlockStatement");
        }
    }
    
    // Visit the method body for nested constructs
    if (node.body) {
        node.body->accept(*this);
    }
}

bool CreateMethodReorderPass::reorder_create_method_body(CompoundStatement* body) {
    if (!body || body->statements.empty()) {
        return false;
    }
    
    debug_print("Analyzing CREATE method body with " + std::to_string(body->statements.size()) + " statements");
    
    // Collect indices of different statement types
    std::vector<size_t> this_assignment_indices;
    std::vector<size_t> super_create_indices;
    std::vector<size_t> other_indices;
    
    for (size_t i = 0; i < body->statements.size(); ++i) {
        if (is_this_member_assignment(body->statements[i].get())) {
            this_assignment_indices.push_back(i);
            debug_print("Found _this assignment at index " + std::to_string(i));
        } else if (is_super_create_call(body->statements[i].get())) {
            super_create_indices.push_back(i);
            debug_print("Found SUPER.CREATE call at index " + std::to_string(i));
        } else {
            other_indices.push_back(i);
        }
    }
    
    // Only reorder if we have both _this assignments and SUPER.CREATE calls
    if (this_assignment_indices.empty() || super_create_indices.empty()) {
        debug_print("No reordering needed - missing _this assignments or SUPER.CREATE calls");
        return false;
    }
    
    // Check if any SUPER.CREATE call comes before any _this assignment
    bool needs_reordering = false;
    for (size_t super_idx : super_create_indices) {
        for (size_t this_idx : this_assignment_indices) {
            if (super_idx < this_idx) {
                needs_reordering = true;
                break;
            }
        }
        if (needs_reordering) break;
    }
    
    if (!needs_reordering) {
        debug_print("No reordering needed - SUPER.CREATE already after _this assignments");
        return false;
    }
    
    debug_print("Reordering needed - moving SUPER.CREATE after _this assignments");
    
    // Create new statement order:
    // 1. All _this assignments first
    // 2. Then all SUPER.CREATE calls  
    // 3. Then all other statements in their original relative order
    std::vector<StmtPtr> reordered_statements;
    reordered_statements.reserve(body->statements.size());
    
    // Add _this assignments first
    for (size_t idx : this_assignment_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Add SUPER.CREATE calls second
    for (size_t idx : super_create_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Add other statements last, preserving their relative order
    for (size_t idx : other_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Replace the original statements
    body->statements = std::move(reordered_statements);
    
    debug_print("Successfully reordered CREATE method statements");
    return true;
}

bool CreateMethodReorderPass::reorder_block_statement_body(BlockStatement* body) {
    if (!body || body->statements.empty()) {
        return false;
    }
    
    debug_print("Analyzing BlockStatement body with " + std::to_string(body->statements.size()) + " statements");
    
    // Collect indices of different statement types
    std::vector<size_t> this_assignment_indices;
    std::vector<size_t> super_create_indices;
    std::vector<size_t> other_indices;
    
    for (size_t i = 0; i < body->statements.size(); ++i) {
        if (is_this_member_assignment(body->statements[i].get())) {
            this_assignment_indices.push_back(i);
            debug_print("Found _this assignment at index " + std::to_string(i));
        } else if (is_super_create_call(body->statements[i].get())) {
            super_create_indices.push_back(i);
            debug_print("Found SUPER.CREATE call at index " + std::to_string(i));
        } else {
            other_indices.push_back(i);
        }
    }
    
    // Only reorder if we have both _this assignments and SUPER.CREATE calls
    if (this_assignment_indices.empty() || super_create_indices.empty()) {
        debug_print("No reordering needed - missing _this assignments or SUPER.CREATE calls");
        return false;
    }
    
    // Check if any SUPER.CREATE call comes before any _this assignment
    bool needs_reordering = false;
    for (size_t super_idx : super_create_indices) {
        for (size_t this_idx : this_assignment_indices) {
            if (super_idx < this_idx) {
                needs_reordering = true;
                break;
            }
        }
        if (needs_reordering) break;
    }
    
    if (!needs_reordering) {
        debug_print("No reordering needed - SUPER.CREATE already after _this assignments");
        return false;
    }
    
    debug_print("Reordering needed - moving SUPER.CREATE after _this assignments");
    
    // Create new statement order:
    // 1. All _this assignments first
    // 2. Then all SUPER.CREATE calls  
    // 3. Then all other statements in their original relative order
    std::vector<StmtPtr> reordered_statements;
    reordered_statements.reserve(body->statements.size());
    
    // Add _this assignments first
    for (size_t idx : this_assignment_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Add SUPER.CREATE calls second
    for (size_t idx : super_create_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Add other statements last, preserving their relative order
    for (size_t idx : other_indices) {
        reordered_statements.push_back(std::move(body->statements[idx]));
    }
    
    // Replace the original statements
    body->statements = std::move(reordered_statements);
    
    debug_print("Successfully reordered BlockStatement statements");
    return true;
}

bool CreateMethodReorderPass::is_this_member_assignment(Statement* stmt) {
    if (!stmt) return false;
    
    debug_print("Checking if statement is _this member assignment...");
    
    // Check for AssignmentStatement
    if (auto* assignment = dynamic_cast<AssignmentStatement*>(stmt)) {
        debug_print("Found AssignmentStatement");
        // Check each LHS expression
        for (const auto& lhs_expr : assignment->lhs) {
            if (auto* member_access = dynamic_cast<MemberAccessExpression*>(lhs_expr.get())) {
                // Check if it's accessing _this
                if (auto* var_access = dynamic_cast<VariableAccess*>(member_access->object_expr.get())) {
                    if (var_access->name == "_this") {
                        debug_print("Found explicit _this member access: " + member_access->member_name);
                        return true;
                    }
                }
            } else if (auto* var_access = dynamic_cast<VariableAccess*>(lhs_expr.get())) {
                debug_print("Found VariableAccess: " + var_access->name);
                // In class context, bare member access is implicitly _this.member
                if (!current_class_name_.empty() && class_table_) {
                    const ClassTableEntry* entry = class_table_->get_class(current_class_name_);
                    if (entry && entry->member_variables.count(var_access->name)) {
                        debug_print("Variable " + var_access->name + " is a member variable - treating as _this member assignment");
                        return true;
                    } else {
                        debug_print("Variable " + var_access->name + " is NOT a member variable");
                    }
                } else {
                    debug_print("No class context or class table");
                }
            }
        }
    } else {
        debug_print("Statement is not an AssignmentStatement");
    }
    
    return false;
}

bool CreateMethodReorderPass::is_super_create_call(Statement* stmt) {
    if (!stmt) return false;
    
    debug_print("Checking if statement is SUPER.CREATE call...");
    
    // Check for RoutineCallStatement with SuperMethodCallExpression
    if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(stmt)) {
        debug_print("Found RoutineCallStatement");
        if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
            debug_print("Found SuperMethodCallExpression: " + super_call->member_name);
            if (super_call->member_name == "CREATE") {
                debug_print("This is a SUPER.CREATE call!");
                return true;
            }
        } else if (auto* super_access = dynamic_cast<SuperMethodAccessExpression*>(routine_call->routine_expr.get())) {
            debug_print("Found SuperMethodAccessExpression: " + super_access->member_name);
            if (super_access->member_name == "CREATE") {
                debug_print("This is a SUPER.CREATE call!");
                return true;
            }
        } else {
            debug_print("RoutineCall is not a SUPER call");
        }
    } else {
        debug_print("Statement is not a RoutineCallStatement");
    }
    
    return false;
}

void CreateMethodReorderPass::debug_print(const std::string& message) const {
    if (trace_enabled_) {
        std::cout << "[CreateMethodReorderPass] " << message << std::endl;
    }
}

// --- Traversal methods (visit children without transformation) ---

void CreateMethodReorderPass::visit(FunctionDeclaration& node) {
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(CompoundStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void CreateMethodReorderPass::visit(BlockStatement& node) {
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void CreateMethodReorderPass::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void CreateMethodReorderPass::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void CreateMethodReorderPass::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) node.else_branch->accept(*this);
}

void CreateMethodReorderPass::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(UntilStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(RepeatStatement& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void CreateMethodReorderPass::visit(ForStatement& node) {
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(ForEachStatement& node) {
    if (node.collection_expression) node.collection_expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    for (auto& case_stmt : node.cases) {
        if (case_stmt) case_stmt->accept(*this);
    }
    if (node.default_case) node.default_case->accept(*this);
}

void CreateMethodReorderPass::visit(CaseStatement& node) {
    if (node.constant_expr) node.constant_expr->accept(*this);
    if (node.command) node.command->accept(*this);
}

void CreateMethodReorderPass::visit(DefaultStatement& node) {
    if (node.command) node.command->accept(*this);
}

void CreateMethodReorderPass::visit(ValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(FloatValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void CreateMethodReorderPass::visit(AssignmentStatement& node) {
    for (auto& lhs : node.lhs) {
        if (lhs) lhs->accept(*this);
    }
    for (auto& rhs : node.rhs) {
        if (rhs) rhs->accept(*this);
    }
}

void CreateMethodReorderPass::visit(RoutineCallStatement& node) {
    if (node.routine_expr) node.routine_expr->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void CreateMethodReorderPass::visit(ReturnStatement& node) {
    // ReturnStatement has no expression field
}

void CreateMethodReorderPass::visit(ResultisStatement& node) {
    if (node.expression) node.expression->accept(*this);
}

void CreateMethodReorderPass::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void CreateMethodReorderPass::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

void CreateMethodReorderPass::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}

void CreateMethodReorderPass::visit(VectorAccess& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void CreateMethodReorderPass::visit(CharIndirection& node) {
    if (node.string_expr) node.string_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void CreateMethodReorderPass::visit(FloatVectorIndirection& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void CreateMethodReorderPass::visit(FunctionCall& node) {
    if (node.function_expr) node.function_expr->accept(*this);
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void CreateMethodReorderPass::visit(MemberAccessExpression& node) {
    if (node.object_expr) node.object_expr->accept(*this);
}

void CreateMethodReorderPass::visit(VecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void CreateMethodReorderPass::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void CreateMethodReorderPass::visit(FVecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void CreateMethodReorderPass::visit(NewExpression& node) {
    for (auto& arg : node.constructor_arguments) {
        if (arg) arg->accept(*this);
    }
}

void CreateMethodReorderPass::visit(TableExpression& node) {
    for (auto& expr : node.initializers) {
        if (expr) expr->accept(*this);
    }
}

void CreateMethodReorderPass::visit(ListExpression& node) {
    for (auto& expr : node.initializers) {
        if (expr) expr->accept(*this);
    }
}

void CreateMethodReorderPass::visit(VecInitializerExpression& node) {
    for (auto& expr : node.initializers) {
        if (expr) expr->accept(*this);
    }
}

void CreateMethodReorderPass::visit(SysCall& node) {
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void CreateMethodReorderPass::visit(FinishStatement& node) {
    // No children to visit
}

void CreateMethodReorderPass::visit(FreeStatement& node) {
    if (node.list_expr) node.list_expr->accept(*this);
}
#include "BoundsCheckingPass.h"
#include "DataTypes.h"
#include <iostream>

BoundsCheckingPass::BoundsCheckingPass(SymbolTable* symbol_table, bool enabled)
    : symbol_table_(symbol_table), enabled_(enabled), current_function_("") {
}

void BoundsCheckingPass::analyze(ASTNode& root) {
    if (!enabled_) {
        return; // Skip analysis if bounds checking is disabled
    }
    
    errors_.clear(); // Clear any previous errors
    current_function_ = ""; // Reset function context
    root.accept(*this); // Start the visitor pattern
}

void BoundsCheckingPass::add_error(const std::string& message) {
    std::string full_message = message;
    if (!current_function_.empty()) {
        full_message = "In function '" + current_function_ + "': " + message;
    }
    errors_.push_back(full_message);
    std::cerr << "Compile-time Bounds Error: " << full_message << std::endl;
}

void BoundsCheckingPass::print_errors() const {
    if (errors_.empty()) {
        return;
    }
    
    std::cerr << "\n=== Compile-time Bounds Checking Errors ===" << std::endl;
    for (size_t i = 0; i < errors_.size(); ++i) {
        std::cerr << "Error " << (i + 1) << ": " << errors_[i] << std::endl;
    }
    std::cerr << "\nTotal: " << errors_.size() << " bounds violation(s) detected." << std::endl;
    std::cerr << "Fix these errors before proceeding." << std::endl;
}

void BoundsCheckingPass::check_constant_vector_access(const std::string& vector_name, 
                                                     int64_t index, 
                                                     const std::string& location_info) {
    if (!symbol_table_) {
        return; // Can't check without symbol table
    }
    
    Symbol symbol;
    if (!symbol_table_->lookup(vector_name, symbol)) {
        return; // Variable not found - will be caught by other passes
    }
    
    // Check if this is a vector or string type with known size
    bool is_vector_type = (symbol.type == VarType::POINTER_TO_INT_VEC || 
                          symbol.type == VarType::POINTER_TO_FLOAT_VEC ||
                          symbol.type == VarType::POINTER_TO_STRING);
    
    if (!is_vector_type || !symbol.has_size) {
        return; // Not a vector with known size - runtime checking will handle it
    }
    
    size_t vector_size = symbol.size;
    
    // Check for negative index - but allow -1 (vector length)
    if (index < -1) {
        add_error("Vector '" + vector_name + "' accessed with invalid negative index " + 
                 std::to_string(index) + ". Only -1 (length) is allowed. " + location_info);
        return;
    }
    
    // If index is -1, it's accessing the vector length - this is legitimate
    if (index == -1) {
        return;
    }
    
    // Check for index >= size
    if (static_cast<size_t>(index) >= vector_size) {
        add_error("Vector '" + vector_name + "' index " + std::to_string(index) + 
                 " is out of bounds (size: " + std::to_string(vector_size) + 
                 ", valid indices: 0-" + std::to_string(vector_size - 1) + "). " + location_info);
    }
}

// ===== VISITOR IMPLEMENTATIONS =====

void BoundsCheckingPass::visit(Program& node) {
    // Visit all top-level declarations
    for (const auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }
}

void BoundsCheckingPass::visit(FunctionDeclaration& node) {
    std::string old_function = current_function_;
    current_function_ = node.name;
    
    // Visit the function body
    if (node.body) {
        node.body->accept(*this);
    }
    
    current_function_ = old_function;
}

void BoundsCheckingPass::visit(RoutineDeclaration& node) {
    std::string old_function = current_function_;
    current_function_ = node.name;
    
    // Visit the routine body
    if (node.body) {
        node.body->accept(*this);
    }
    
    current_function_ = old_function;
}

void BoundsCheckingPass::visit(BlockStatement& node) {
    // Visit all declarations in this block
    for (const auto& decl : node.declarations) {
        if (decl) {
            decl->accept(*this);
        }
    }
    
    // Visit all statements in this block
    visit_statements(node.statements);
}

void BoundsCheckingPass::visit(AssignmentStatement& node) {
    // Visit RHS expressions (may contain vector accesses)
    visit_expressions(node.rhs);
    
    // Visit LHS expressions (may contain vector accesses)
    visit_expressions(node.lhs);
    
    // Track vector sizes for compile-time bounds checking from assignments
    if (symbol_table_ && node.lhs.size() == 1 && node.rhs.size() == 1) {
        // Check if this is a simple assignment like "v := VEC 3"
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.lhs[0].get())) {
            const std::string& var_name = var_access->name;
            
            Symbol symbol;
            if (symbol_table_->lookup(var_name, symbol)) {
                // Check if RHS is a vector allocation
                if (auto* vec_alloc = dynamic_cast<VecAllocationExpression*>(node.rhs[0].get())) {
                    if (auto* size_literal = dynamic_cast<NumberLiteral*>(vec_alloc->size_expr.get())) {
                        symbol.size = static_cast<size_t>(size_literal->int_value);
                        symbol.has_size = true;
                        symbol_table_->updateSymbol(var_name, symbol);
                    }
                } else if (auto* fvec_alloc = dynamic_cast<FVecAllocationExpression*>(node.rhs[0].get())) {
                    if (auto* size_literal = dynamic_cast<NumberLiteral*>(fvec_alloc->size_expr.get())) {
                        symbol.size = static_cast<size_t>(size_literal->int_value);
                        symbol.has_size = true;
                        symbol_table_->updateSymbol(var_name, symbol);
                    }
                } else if (auto* vec_init = dynamic_cast<VecInitializerExpression*>(node.rhs[0].get())) {
                    // For vector initializers like VEC [1, 2, 3], the size is the number of elements
                    symbol.size = vec_init->initializers.size();
                    symbol.has_size = true;
                    symbol_table_->updateSymbol(var_name, symbol);
                }
            }
        }
    }
}

void BoundsCheckingPass::visit(VectorAccess& node) {
    // This is the key method - check for constant index bounds violations
    
    // First, visit sub-expressions recursively
    if (node.vector_expr) {
        node.vector_expr->accept(*this);
    }
    if (node.index_expr) {
        node.index_expr->accept(*this);
    }
    
    // Check for compile-time bounds violation
    auto* var_access = dynamic_cast<VariableAccess*>(node.vector_expr.get());
    auto* index_literal = dynamic_cast<NumberLiteral*>(node.index_expr.get());
    auto* unary_op = dynamic_cast<UnaryOp*>(node.index_expr.get());
    
    if (var_access && index_literal) {
        // We have a vector access with a constant index
        std::string location_info = "Vector access: " + var_access->name + "!" + std::to_string(index_literal->int_value);
        check_constant_vector_access(var_access->name, index_literal->int_value, location_info);
    } else if (var_access && unary_op && unary_op->op == UnaryOp::Operator::Negate) {
        // Check if it's a negated constant (e.g., v!-1)
        auto* negated_literal = dynamic_cast<NumberLiteral*>(unary_op->operand.get());
        if (negated_literal) {
            int64_t negative_index = -negated_literal->int_value;
            std::string location_info = "Vector access: " + var_access->name + "!" + std::to_string(negative_index);
            check_constant_vector_access(var_access->name, negative_index, location_info);
        }
    }
}

void BoundsCheckingPass::visit(LetDeclaration& node) {
    // First visit the initializers (may contain vector accesses or allocations)
    for (const auto& initializer : node.initializers) {
        if (initializer) {
            initializer->accept(*this);
        }
    }
    
    // Track vector sizes for compile-time bounds checking
    if (symbol_table_ && !node.names.empty() && !node.initializers.empty()) {
        // Process each name-initializer pair
        for (size_t i = 0; i < node.names.size() && i < node.initializers.size(); ++i) {
            const std::string& var_name = node.names[i];
            const auto& initializer = node.initializers[i];
            
            Symbol symbol;
            if (symbol_table_->lookup(var_name, symbol)) {
                // Check if this is a vector allocation with constant size
                if (auto* vec_alloc = dynamic_cast<VecAllocationExpression*>(initializer.get())) {
                    if (auto* size_literal = dynamic_cast<NumberLiteral*>(vec_alloc->size_expr.get())) {
                        symbol.size = static_cast<size_t>(size_literal->int_value);
                        symbol.has_size = true;
                        symbol_table_->updateSymbol(var_name, symbol);
                    }
                } else if (auto* fvec_alloc = dynamic_cast<FVecAllocationExpression*>(initializer.get())) {
                    if (auto* size_literal = dynamic_cast<NumberLiteral*>(fvec_alloc->size_expr.get())) {
                        symbol.size = static_cast<size_t>(size_literal->int_value);
                        symbol.has_size = true;
                        symbol_table_->updateSymbol(var_name, symbol);
                    }
                } else if (auto* vec_init = dynamic_cast<VecInitializerExpression*>(initializer.get())) {
                    // For vector initializers like VEC [1, 2, 3], the size is the number of elements
                    symbol.size = vec_init->initializers.size();
                    symbol.has_size = true;
                    symbol_table_->updateSymbol(var_name, symbol);
                }
            }
        }
    }
}

// Control flow statements - visit their sub-statements
void BoundsCheckingPass::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    // Note: IfStatement only has then_branch, no else_branch in this AST
}

void BoundsCheckingPass::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.body) node.body->accept(*this);
}

void BoundsCheckingPass::visit(ForStatement& node) {
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    if (node.body) node.body->accept(*this);
}

void BoundsCheckingPass::visit(RepeatStatement& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void BoundsCheckingPass::visit(UntilStatement& node) {
    if (node.body) node.body->accept(*this);
    if (node.condition) node.condition->accept(*this);
}

void BoundsCheckingPass::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
}

void BoundsCheckingPass::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.then_branch) node.then_branch->accept(*this);
    if (node.else_branch) node.else_branch->accept(*this);
}

void BoundsCheckingPass::visit(ForEachStatement& node) {
    if (node.collection_expression) node.collection_expression->accept(*this);
    if (node.body) node.body->accept(*this);
}

void BoundsCheckingPass::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    for (const auto& case_stmt : node.cases) {
        if (case_stmt) case_stmt->accept(*this);
    }
}

void BoundsCheckingPass::visit(CaseStatement& node) {
    if (node.command) node.command->accept(*this);
}

void BoundsCheckingPass::visit(DefaultStatement& node) {
    if (node.command) node.command->accept(*this);
}

void BoundsCheckingPass::visit(RoutineCallStatement& node) {
    if (node.routine_expr) node.routine_expr->accept(*this);
    visit_expressions(node.arguments);
}

void BoundsCheckingPass::visit(ReturnStatement& node) {
    // ReturnStatement has no expression in current AST
}

void BoundsCheckingPass::visit(ResultisStatement& node) {
    if (node.expression) node.expression->accept(*this);
}

void BoundsCheckingPass::visit(GotoStatement& node) {}
void BoundsCheckingPass::visit(LabelTargetStatement& node) {}
void BoundsCheckingPass::visit(BreakStatement& node) {}
void BoundsCheckingPass::visit(LoopStatement& node) {}
void BoundsCheckingPass::visit(FinishStatement& node) {}
void BoundsCheckingPass::visit(EndcaseStatement& node) {}
void BoundsCheckingPass::visit(BrkStatement& node) {}
void BoundsCheckingPass::visit(StringStatement& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}
void BoundsCheckingPass::visit(FreeStatement& node) {}
void BoundsCheckingPass::visit(ConditionalBranchStatement& node) {}

// Expression visitors
void BoundsCheckingPass::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void BoundsCheckingPass::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

void BoundsCheckingPass::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}

void BoundsCheckingPass::visit(FunctionCall& node) {
    if (node.function_expr) node.function_expr->accept(*this);
    visit_expressions(node.arguments);
}

void BoundsCheckingPass::visit(MemberAccessExpression& node) {
    if (node.object_expr) node.object_expr->accept(*this);
}

void BoundsCheckingPass::visit(ListExpression& node) {
    visit_expressions(node.initializers);
}

void BoundsCheckingPass::visit(VecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void BoundsCheckingPass::visit(FVecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void BoundsCheckingPass::visit(PairsAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void BoundsCheckingPass::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void BoundsCheckingPass::visit(VecInitializerExpression& node) {
    visit_expressions(node.initializers);
}

void BoundsCheckingPass::visit(TableExpression& node) {
    visit_expressions(node.initializers);
}

void BoundsCheckingPass::visit(ValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void BoundsCheckingPass::visit(FloatValofExpression& node) {
    if (node.body) node.body->accept(*this);
}

void BoundsCheckingPass::visit(CharIndirection& node) {
    if (node.string_expr) node.string_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void BoundsCheckingPass::visit(FloatVectorIndirection& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

// Helper methods
void BoundsCheckingPass::visit_statements(const std::vector<std::unique_ptr<Statement>>& statements) {
    for (const auto& stmt : statements) {
        if (stmt) {
            stmt->accept(*this);
        }
    }
}

void BoundsCheckingPass::visit_expressions(const std::vector<std::unique_ptr<Expression>>& expressions) {
    for (const auto& expr : expressions) {
        if (expr) {
            expr->accept(*this);
        }
    }
}
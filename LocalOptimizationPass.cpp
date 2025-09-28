#include "LocalOptimizationPass.h"
#include "StringTable.h"
#include "BasicBlock.h"
#include "analysis/StatelessTypeInference.h"
#include <vector>
#include <iostream>
#include <unordered_set>
#include "AST_Cloner.h"

// --- Expression Type Inference ---
VarType LocalOptimizationPass::infer_expression_type(const Expression* expr) {
    if (!expr) return VarType::INTEGER; // Default to integer

    if (const auto* bin_op = dynamic_cast<const BinaryOp*>(expr)) {
        // Use StatelessTypeInference instead of ASTAnalyzer to avoid state issues
        return StatelessTypeInference::infer_expression_type(expr);
    }
    if (const auto* un_op = dynamic_cast<const UnaryOp*>(expr)) {
        if (un_op->op == UnaryOp::Operator::FloatConvert) {
            return VarType::FLOAT;
        }
        return StatelessTypeInference::infer_expression_type(expr);
    }
    if (const auto* num_lit = dynamic_cast<const NumberLiteral*>(expr)) {
        return (num_lit->literal_type == NumberLiteral::LiteralType::Float) 
               ? VarType::FLOAT : VarType::INTEGER;
    }
    if (const auto* str_lit = dynamic_cast<const StringLiteral*>(expr)) {
        return VarType::POINTER_TO_STRING;
    }
    if (const auto* call = dynamic_cast<const FunctionCall*>(expr)) {
        return StatelessTypeInference::infer_expression_type(expr);
    }
    
    return VarType::INTEGER; // Default fallback
}

// --- Expression Canonicalization ---
static std::string expression_to_string_recursive(const Expression* expr) {
    if (!expr) return "";
    switch (expr->getType()) {
        case ASTNode::NodeType::BinaryOpExpr: {
            const auto* bin = static_cast<const BinaryOp*>(expr);
            std::string op_str = std::to_string(static_cast<int>(bin->op));
            std::string left_str = expression_to_string_recursive(bin->left.get());
            std::string right_str = expression_to_string_recursive(bin->right.get());

            // Canonicalization for commutative operators
            switch (bin->op) {
                case BinaryOp::Operator::Add:
                case BinaryOp::Operator::Multiply:
                case BinaryOp::Operator::LogicalAnd:
                case BinaryOp::Operator::LogicalOr:
                case BinaryOp::Operator::Equal:
                case BinaryOp::Operator::NotEqual:
                    // Add other commutative float ops as needed
                    if (left_str > right_str) {
                        std::swap(left_str, right_str);
                    }
                    break;
                default:
                    // Not a commutative operator, order matters.
                    break;
            }
            return "(BIN_OP " + op_str + " " + left_str + " " + right_str + ")";
        }
        case ASTNode::NodeType::VariableAccessExpr: {
            const auto* var = static_cast<const VariableAccess*>(expr);
            return "(VAR " + var->name + ")";
        }
        case ASTNode::NodeType::NumberLit: {
            const auto* lit = static_cast<const NumberLiteral*>(expr);
            if (lit->literal_type == NumberLiteral::LiteralType::Integer) {
                return "(INT " + std::to_string(lit->int_value) + ")";
            } else {
                return "(FLOAT " + std::to_string(lit->float_value) + ")";
            }
        }

        case ASTNode::NodeType::CharLit: {
            const auto* lit = static_cast<const CharLiteral*>(expr);
            return "(CHAR " + std::to_string(lit->value) + ")";
        }
        case ASTNode::NodeType::BooleanLit: {
            const auto* lit = static_cast<const BooleanLiteral*>(expr);
            return lit->value ? "(BOOL 1)" : "(BOOL 0)";
        }
        case ASTNode::NodeType::FunctionCallExpr: {
            const auto* call = static_cast<const FunctionCall*>(expr);
            std::string func_str;

            // Try to get the function name if it's a VariableAccessExpr
            if (call->function_expr) {
                if (call->function_expr->getType() == ASTNode::NodeType::VariableAccessExpr) {
                    const auto* var = static_cast<const VariableAccess*>(call->function_expr.get());
                    func_str = var->name;
                } else {
                    // Otherwise, serialize the function expression
                    func_str = expression_to_string_recursive(call->function_expr.get());
                }
            } else {
                func_str = "<null_func>";
            }

            std::string args_str;
            for (const auto& arg : call->arguments) {
                if (!args_str.empty()) args_str += " ";
                args_str += expression_to_string_recursive(arg.get());
            }
            return "(CALL " + func_str + (args_str.empty() ? "" : " " + args_str) + ")";
        }
        // Add more cases as needed for your language
        default:
            return "(EXPR)";
    }
}

std::string LocalOptimizationPass::expression_to_string(const Expression* expr) const {
    if (!expr) return "";
    return expression_to_string_recursive(expr);
}

// --- LocalOptimizationPass Implementation ---

LocalOptimizationPass::LocalOptimizationPass(StringTable* string_table, bool trace_optimizer)
    : temp_var_counter_(0), string_table_(string_table), trace_optimizer_(trace_optimizer)
{}

std::string LocalOptimizationPass::generate_temp_var_name() {
    return "_cse_temp_" + std::to_string(temp_var_counter_++);
}

void LocalOptimizationPass::run(Program& ast,
                                SymbolTable& symbol_table,
                                ASTAnalyzer& analyzer) {
    if (trace_optimizer_) std::cout << "[CSE DEBUG] Starting LocalOptimizationPass::run() on AST\n";
    
    // Process all function and routine declarations in the program
    for (auto& decl : ast.declarations) {
        if (auto* func = dynamic_cast<FunctionDeclaration*>(decl.get())) {
            if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing function: " << func->name << "\n";
            optimize_function(func, symbol_table, analyzer);
        } else if (auto* routine = dynamic_cast<RoutineDeclaration*>(decl.get())) {
            if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing routine: " << routine->name << "\n";
            optimize_function(routine, symbol_table, analyzer);
        }
    }
    
    if (trace_optimizer_) std::cout << "[CSE DEBUG] LocalOptimizationPass::run() completed\n";
}

void LocalOptimizationPass::optimize_function(FunctionDeclaration* func,
                                             SymbolTable& symbol_table,
                                             ASTAnalyzer& analyzer) {
    if (!func->body) return;
    
    // Reset state for each function
    expr_counts_.clear();
    available_expressions_.clear();
    temp_var_counter_ = 0;
    
    // Pass 1: Analysis - count subexpressions
    if (trace_optimizer_) std::cout << "[CSE DEBUG] ANALYSIS: Counting subexpressions in function " << func->name << "\n";
    count_subexpressions(func->body.get());
    
    if (trace_optimizer_) {
        std::cout << "[CSE DEBUG] Expression counts after analysis:\n";
        for (const auto& pair : expr_counts_) {
            std::cout << "[CSE DEBUG]   '" << pair.first << "' appears " << pair.second << " times\n";
        }
    }
    
    // Pass 2: Transformation - optimize statements
    if (trace_optimizer_) std::cout << "[CSE DEBUG] TRANSFORMATION: Optimizing function " << func->name << "\n";
    if (auto* compound = dynamic_cast<CompoundStatement*>(func->body.get())) {
        optimize_statement_list(compound->statements, func->name, symbol_table, analyzer);
    } else if (auto* block = dynamic_cast<BlockStatement*>(func->body.get())) {
        optimize_statement_list(block->statements, func->name, symbol_table, analyzer);
    }
}

void LocalOptimizationPass::optimize_function(RoutineDeclaration* routine,
                                             SymbolTable& symbol_table,
                                             ASTAnalyzer& analyzer) {
    if (!routine->body) return;
    
    // Reset state for each function
    expr_counts_.clear();
    available_expressions_.clear();
    temp_var_counter_ = 0;
    
    // Pass 1: Analysis - count subexpressions
    if (trace_optimizer_) std::cout << "[CSE DEBUG] ANALYSIS: Counting subexpressions in routine " << routine->name << "\n";
    count_subexpressions(routine->body.get());
    
    if (trace_optimizer_) {
        std::cout << "[CSE DEBUG] Expression counts after analysis:\n";
        for (const auto& pair : expr_counts_) {
            std::cout << "[CSE DEBUG]   '" << pair.first << "' appears " << pair.second << " times\n";
        }
    }
    
    // Pass 2: Transformation - optimize statements
    if (trace_optimizer_) std::cout << "[CSE DEBUG] TRANSFORMATION: Optimizing routine " << routine->name << "\n";
    if (auto* compound = dynamic_cast<CompoundStatement*>(routine->body.get())) {
        optimize_statement_list(compound->statements, routine->name, symbol_table, analyzer);
    } else if (auto* block = dynamic_cast<BlockStatement*>(routine->body.get())) {
        optimize_statement_list(block->statements, routine->name, symbol_table, analyzer);
    }
}

void LocalOptimizationPass::optimize_statement_list(std::vector<StmtPtr>& statements,
                                                   const std::string& current_function_name,
                                                   SymbolTable& symbol_table,
                                                   ASTAnalyzer& analyzer) {
    if (trace_optimizer_) std::cout << "[CSE DEBUG] TRANSFORMATION: Starting optimization of statement list with " << statements.size() << " statements\n";
    
    // Use an index-based loop to allow for statement insertion.
    for (size_t i = 0; i < statements.size(); ++i) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] TRANSFORMATION: Processing statement " << i << "\n";
        StmtPtr& stmt_ptr = statements[i];
        if (!stmt_ptr) continue;

        if (stmt_ptr->getType() == ASTNode::NodeType::AssignmentStmt) {
            auto* assign = static_cast<AssignmentStatement*>(stmt_ptr.get());
            // CSE Logic for Right-Hand Side Expressions
            for (size_t j = 0; j < assign->rhs.size(); ++j) {
                ExprPtr& rhs_expr = assign->rhs[j];
                if (!rhs_expr) continue;
                optimize_expression(rhs_expr, statements, i, current_function_name, symbol_table, analyzer);
            }
        } else if (stmt_ptr->getType() == ASTNode::NodeType::RoutineCallStmt) {
            auto* routine_call = static_cast<RoutineCallStatement*>(stmt_ptr.get());
            // CSE Logic for Arguments
            for (size_t j = 0; j < routine_call->arguments.size(); ++j) {
                ExprPtr& arg_expr = routine_call->arguments[j];
                if (!arg_expr) continue;
                optimize_expression(arg_expr, statements, i, current_function_name, symbol_table, analyzer);
            }
        }
        // Add other statement types as needed
    }
    
    if (trace_optimizer_) std::cout << "[CSE DEBUG] TRANSFORMATION: Finished optimization of statement list\n";
}

void LocalOptimizationPass::optimize_expression(ExprPtr& expr, std::vector<StmtPtr>& statements, size_t& i,
                                               const std::string& current_function_name,
                                               SymbolTable& symbol_table,
                                               ASTAnalyzer& analyzer) {
    if (!expr) return;

    std::string canonical_expr_str = expression_to_string(expr.get());
    if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing expression: type=" << static_cast<int>(expr->getType()) 
             << " key='" << canonical_expr_str << "' ptr=" << expr.get() << "\n";

    // Only optimize if it's a common subexpression (count > 1) and not a StringLiteral.
    if ((expr_counts_.count(canonical_expr_str) && expr_counts_.at(canonical_expr_str) > 1) &&
        (expr->getType() == ASTNode::NodeType::BinaryOpExpr)) {

        if (trace_optimizer_) std::cout << "[CSE DEBUG] Creating new temp var for common subexpression (count="
                  << expr_counts_.at(canonical_expr_str) << ")\n";

        // If the expression is already available, just replace it.
        auto it = available_expressions_.find(canonical_expr_str);
        if (it != available_expressions_.end()) {
            if (trace_optimizer_) std::cout << "[CSE DEBUG] Found available expression, replacing with temp var: " << it->second
                      << " (old ptr=" << expr.get() << ")\n";
            expr = std::make_unique<VariableAccess>(it->second);
            if (trace_optimizer_) std::cout << "[CSE DEBUG] Replacement complete (new ptr=" << expr.get() << ")\n";
            return;
        }

        // 1. Generate the temporary variable name and store it.
        std::string temp_var_name = generate_temp_var_name();
        available_expressions_[canonical_expr_str] = temp_var_name;

        // --- Infer the type of the expression being hoisted ---
        VarType inferred_type = infer_expression_type(expr.get());

        // --- Register the new variable in the Symbol Table for the current function scope ---
        Symbol temp_symbol(
            temp_var_name,
            SymbolKind::LOCAL_VAR,
            inferred_type,
            symbol_table.getCurrentScopeLevel(),
            current_function_name
        );
        symbol_table.addSymbol(temp_symbol);

        // --- Increment the local variable count in the ASTAnalyzer's metrics ---
        auto metrics_it = analyzer.get_function_metrics_mut().find(current_function_name);
        if (metrics_it == analyzer.get_function_metrics_mut().end()) {
            std::cerr << "LocalOptimizationPass Error: Function metrics not found for: " << current_function_name << std::endl;
            return;
        }
        auto& metrics = metrics_it->second;
        if (inferred_type == VarType::FLOAT) {
            metrics.num_float_variables++;
        } else {
            metrics.num_variables++;
        }
        metrics.variable_types[temp_var_name] = inferred_type;

        // 2. Clone the expression before moving it
        auto expr_clone = clone_unique_ptr<Expression>(expr);

        // 3. Replace the original expression with a variable access to our new temp.
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Replacing expression (old ptr=" << expr.get() << ")\n";
        expr = std::make_unique<VariableAccess>(temp_var_name);
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Replacement complete (new ptr=" << expr.get() << ")\n";

        // 4. Create assignment statement and insert it before the current statement
        std::vector<ExprPtr> lhs_vec;
        lhs_vec.push_back(std::make_unique<VariableAccess>(temp_var_name));
        std::vector<ExprPtr> rhs_vec;
        rhs_vec.push_back(std::move(expr_clone));
        auto assignment = std::make_unique<AssignmentStatement>(
            std::move(lhs_vec),
            std::move(rhs_vec)
        );
        statements.insert(statements.begin() + i, std::move(assignment));
        ++i; // Advance index for the inserted assignment

        // 5. NOTE: The loop index i will be incremented by the calling loop
        // We don't increment here to avoid double increment
    }
}

void LocalOptimizationPass::invalidate_expressions_with_var(const std::string& var_name) {
    std::string var_str = "(VAR " + var_name + ")";
    for (auto it = available_expressions_.begin(); it != available_expressions_.end(); ) {
        if (it->first.find(var_str) != std::string::npos) {
            it = available_expressions_.erase(it);
        } else {
            ++it;
        }
    }
}

// --- NEW: Helper to recursively count all subexpressions in a statement/expression tree (ANALYSIS STAGE) ---
void LocalOptimizationPass::count_subexpressions(ASTNode* node) {
    if (trace_optimizer_) std::cout << "[CSE DEBUG] count_subexpressions: ENTRY\n";
    if (!node) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] count_subexpressions: node is null\n";
        return;
    }
    
    if (trace_optimizer_) std::cout << "[CSE DEBUG] count_subexpressions: examining node type=" << static_cast<int>(node->getType()) << "\n";

    if (auto* expr = dynamic_cast<Expression*>(node)) {
        std::string key = expression_to_string(expr);
        if (trace_optimizer_) {
            std::cout << "[CSE DEBUG] Found expression: type=" << static_cast<int>(expr->getType()) 
                 << " key='" << key << "' (";
            if (expr->getType() == ASTNode::NodeType::StringLit) {
                std::cout << "StringLit";
            } else if (expr->getType() == ASTNode::NodeType::BinaryOpExpr) {
                std::cout << "BinaryOpExpr";
            } else if (expr->getType() == ASTNode::NodeType::FunctionCallExpr) {
                std::cout << "FunctionCallExpr";
            } else {
                std::cout << "Other";
            }
            std::cout << ")\n";
        }
        // Count expressions that could benefit from CSE
        if (expr->getType() == ASTNode::NodeType::BinaryOpExpr ||
            expr->getType() == ASTNode::NodeType::StringLit ||
            expr->getType() == ASTNode::NodeType::FunctionCallExpr) {
            expr_counts_[key]++;
            if (trace_optimizer_) std::cout << "[CSE DEBUG] COUNTED expression: type=" << static_cast<int>(expr->getType()) 
                     << " key='" << key << "' count=" << expr_counts_[key] << "\n";
        }

        if (auto* bin_op = dynamic_cast<BinaryOp*>(expr)) {
            count_subexpressions(bin_op->left.get());
            count_subexpressions(bin_op->right.get());
        } else if (auto* un_op = dynamic_cast<UnaryOp*>(expr)) {
            count_subexpressions(un_op->operand.get());
        } else if (auto* call = dynamic_cast<FunctionCall*>(expr)) {
            count_subexpressions(call->function_expr.get());
            for (auto& arg : call->arguments) count_subexpressions(arg.get());
        }
    }
    else if (auto* let = dynamic_cast<LetDeclaration*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing LetDeclaration with " << let->initializers.size() << " initializers\n";
        for (auto& init : let->initializers) count_subexpressions(init.get());
    } else if (auto* assign = dynamic_cast<AssignmentStatement*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing AssignmentStatement with " << assign->rhs.size() << " RHS expressions\n";
        for (auto& rhs : assign->rhs) count_subexpressions(rhs.get());
    } else if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing RoutineCallStatement with " << routine_call->arguments.size() << " arguments\n";
        for (size_t i = 0; i < routine_call->arguments.size(); ++i) {
            if (trace_optimizer_) std::cout << "[CSE DEBUG]   Argument " << i << " type: " << static_cast<int>(routine_call->arguments[i]->getType()) << "\n";
            count_subexpressions(routine_call->arguments[i].get());
        }
    } else if (auto* block = dynamic_cast<BlockStatement*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing BlockStatement with " << block->statements.size() << " statements\n";
        for (auto& s : block->statements) count_subexpressions(s.get());
    } else if (auto* comp = dynamic_cast<CompoundStatement*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing CompoundStatement with " << comp->statements.size() << " statements\n";
        for (auto& s : comp->statements) count_subexpressions(s.get());
    } else if (auto* if_stmt = dynamic_cast<IfStatement*>(node)) {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Processing IfStatement\n";
        count_subexpressions(if_stmt->condition.get());
        count_subexpressions(if_stmt->then_branch.get());
    } else {
        if (trace_optimizer_) std::cout << "[CSE DEBUG] Unhandled node type: " << static_cast<int>(node->getType()) << "\n";
    }
    if (trace_optimizer_) std::cout << "[CSE DEBUG] count_subexpressions: EXIT\n";
}
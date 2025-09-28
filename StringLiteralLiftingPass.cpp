#include "StringLiteralLiftingPass.h"
#include "AST.h"
#include "SymbolTable.h"
#include "analysis/ASTAnalyzer.h"
#include "StringTable.h"
#include <map>
#include <memory>
#include <iostream>

// Helper to generate unique temp variable names per pass instance
static std::string generate_temp_var_name(int& counter) {
    return "_strlift_temp_" + std::to_string(counter++);
}

class StringLiteralLifter {
public:
    StringLiteralLifter(
        StringTable* string_table,
        SymbolTable& symbol_table,
        ASTAnalyzer& analyzer,
        const std::string& current_function_name,
        int& temp_var_counter,
        std::map<std::string, std::string>& string_to_temp,
        std::vector<StmtPtr>& statements
    )
        : string_table_(string_table),
          symbol_table_(symbol_table),
          analyzer_(analyzer),
          current_function_name_(current_function_name),
          temp_var_counter_(temp_var_counter),
          string_to_temp_(string_to_temp),
          statements_(statements)
    {}

    // Entry point for a statement list
    void process_statement_list() {
        for (size_t i = 0; i < statements_.size(); ++i) {
            process_statement(statements_[i], i);
        }
    }

private:
    StringTable* string_table_;
    SymbolTable& symbol_table_;
    ASTAnalyzer& analyzer_;
    const std::string& current_function_name_;
    int& temp_var_counter_;
    std::map<std::string, std::string>& string_to_temp_;
    std::vector<StmtPtr>& statements_;

    // Recursively process a statement
    void process_statement(StmtPtr& stmt, size_t stmt_index) {
        if (!stmt) return;
        switch (stmt->getType()) {
            case ASTNode::NodeType::AssignmentStmt: {
                auto* assign = static_cast<AssignmentStatement*>(stmt.get());
                for (auto& rhs_expr : assign->rhs) {
                    process_expression(rhs_expr, stmt_index);
                }
                break;
            }
            case ASTNode::NodeType::RoutineCallStmt: {
                auto* routine_call = static_cast<RoutineCallStatement*>(stmt.get());
                for (auto& arg : routine_call->arguments) {
                    process_expression(arg, stmt_index);
                }
                break;
            }
            // Add more statement types as needed
            default:
                break;
        }
    }

    // Recursively process an expression, replacing StringLiterals as needed
    void process_expression(ExprPtr& expr, size_t stmt_index) {
        if (!expr) return;
        switch (expr->getType()) {
            case ASTNode::NodeType::StringLit: {
                auto* str_lit = static_cast<StringLiteral*>(expr.get());
                const std::string& value = str_lit->value;
                auto it = string_to_temp_.find(value);
                if (it != string_to_temp_.end()) {
                    // Cache hit: replace with VariableAccess to existing temp
                    expr = std::make_unique<VariableAccess>(it->second);
                } else {
                    // Cache miss: create label, temp, assignment, update symbol table/metrics
                    std::string label = string_table_->get_or_create_label(value);
                    std::string temp_var_name = generate_temp_var_name(temp_var_counter_);
                    string_to_temp_[value] = temp_var_name;

                    // Register temp variable in symbol table
                    Symbol temp_symbol(
                        temp_var_name,
                        SymbolKind::LOCAL_VAR,
                        VarType::POINTER_TO_STRING,
                        symbol_table_.getCurrentScopeLevel(),
                        current_function_name_
                    );
                    symbol_table_.addSymbol(temp_symbol);

                    // Update function metrics
                    auto metrics_it = analyzer_.get_function_metrics_mut().find(current_function_name_);
                    if (metrics_it != analyzer_.get_function_metrics_mut().end()) {
                        auto& metrics = metrics_it->second;
                        metrics.num_variables++;
                        metrics.variable_types[temp_var_name] = VarType::POINTER_TO_STRING;
                    } else {
                        std::cerr << "StringLiteralLiftingPass Error: Function metrics not found for: " << current_function_name_ << std::endl;
                    }

                    // Inject assignment: temp_var := @label
                    std::vector<ExprPtr> lhs_vec;
                    lhs_vec.push_back(std::make_unique<VariableAccess>(temp_var_name));
                    std::vector<ExprPtr> rhs_vec;
                    rhs_vec.push_back(std::make_unique<UnaryOp>(
                        UnaryOp::Operator::AddressOf,
                        std::make_unique<VariableAccess>(label)
                    ));
                    auto assignment = std::make_unique<AssignmentStatement>(
                        std::move(lhs_vec),
                        std::move(rhs_vec)
                    );
                    // Insert before the current statement
                    statements_.insert(statements_.begin() + stmt_index, std::move(assignment));
                    ++stmt_index; // Advance index for the inserted assignment

                    // Replace the StringLiteral with VariableAccess(temp_var)
                    expr = std::make_unique<VariableAccess>(temp_var_name);
                }
                break;
            }
            case ASTNode::NodeType::BinaryOpExpr: {
                auto* bin = static_cast<BinaryOp*>(expr.get());
                process_expression(bin->left, stmt_index);
                process_expression(bin->right, stmt_index);
                break;
            }
            case ASTNode::NodeType::UnaryOpExpr: {
                auto* un = static_cast<UnaryOp*>(expr.get());
                process_expression(un->operand, stmt_index);
                break;
            }
            case ASTNode::NodeType::FunctionCallExpr: {
                auto* call = static_cast<FunctionCall*>(expr.get());
                process_expression(call->function_expr, stmt_index);
                for (auto& arg : call->arguments) {
                    process_expression(arg, stmt_index);
                }
                break;
            }
            // Add more expression types as needed
            default:
                break;
        }
    }
};

// Main pass implementation
StringLiteralLiftingPass::StringLiteralLiftingPass(StringTable* string_table)
    : string_table_(string_table)
{}

void StringLiteralLiftingPass::run(Program& ast, SymbolTable& symbol_table, ASTAnalyzer& analyzer) {
    temp_var_counter_ = 0;
    for (auto& decl : ast.declarations) {
        std::map<std::string, std::string> string_to_temp;
        if (auto* func = dynamic_cast<FunctionDeclaration*>(decl.get())) {
            if (auto* compound = dynamic_cast<CompoundStatement*>(func->body.get())) {
                StringLiteralLifter lifter(
                    string_table_,
                    symbol_table,
                    analyzer,
                    func->name,
                    temp_var_counter_,
                    string_to_temp,
                    compound->statements
                );
                lifter.process_statement_list();
            }
        } else if (auto* routine = dynamic_cast<RoutineDeclaration*>(decl.get())) {
            if (auto* compound = dynamic_cast<CompoundStatement*>(routine->body.get())) {
                StringLiteralLifter lifter(
                    string_table_,
                    symbol_table,
                    analyzer,
                    routine->name,
                    temp_var_counter_,
                    string_to_temp,
                    compound->statements
                );
                lifter.process_statement_list();
            }
        }
        // Add more declaration types as needed
    }
}
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

// Helper to sort and select top N string literals with > min_count
static std::vector<std::string> select_top_string_literals(
    const std::map<std::string, int>& counts,
    int max_literals,
    int min_count
) {
    std::vector<std::pair<std::string, int>> sorted(counts.begin(), counts.end());
    std::sort(sorted.begin(), sorted.end(), [](const auto& a, const auto& b) {
        return b.second < a.second ? false : (a.second > b.second ? true : a.first < b.first);
    });
    std::vector<std::string> result;
    for (const auto& pair : sorted) {
        if (pair.second > min_count) {
            result.push_back(pair.first);
            if ((int)result.size() >= max_literals) break;
        }
    }
    return result;
}

class StringLiteralLifter {
public:
    StringLiteralLifter(
        StringTable* string_table,
        SymbolTable& symbol_table,
        ASTAnalyzer& analyzer,
        const std::string& current_function_name,
        int& temp_var_counter,
        std::vector<StmtPtr>& statements
    )
        : string_table_(string_table),
          symbol_table_(symbol_table),
          analyzer_(analyzer),
          current_function_name_(current_function_name),
          temp_var_counter_(temp_var_counter),
          statements_(statements)
    {}

    // Entry point for a statement list
    void process_statement_list() {
        // 1. Count all string literal references in the function
        std::map<std::string, int> literal_counts;
        for (const auto& stmt : statements_) {
            count_string_literals_in_statement(stmt, literal_counts);
        }

        // 2. Select up to two most frequent literals with >3 uses
        std::vector<std::string> to_lift = select_top_string_literals(literal_counts, 2, 3);

        // 3. For each, create temp var and inject assignment at the top
        std::map<std::string, std::string> lifted_string_to_temp;
        std::vector<StmtPtr> injected_assignments;
        for (const auto& value : to_lift) {
            std::string label = string_table_->get_or_create_label(value);
            std::string temp_var_name = generate_temp_var_name(temp_var_counter_);
            lifted_string_to_temp[value] = temp_var_name;

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
            injected_assignments.push_back(std::move(assignment));
        }

        // 4. Replace all uses of those literals with temp variable, leave others as-is
        for (auto& stmt : statements_) {
            replace_lifted_string_literals_in_statement(stmt, lifted_string_to_temp);
        }

        // 5. Inject assignments at the top of the function body
        statements_.insert(statements_.begin(), std::make_move_iterator(injected_assignments.begin()), std::make_move_iterator(injected_assignments.end()));
    }

private:
    StringTable* string_table_;
    SymbolTable& symbol_table_;
    ASTAnalyzer& analyzer_;
    const std::string& current_function_name_;
    int& temp_var_counter_;
    std::vector<StmtPtr>& statements_;

    // Recursively count string literals in a statement
    void count_string_literals_in_statement(const StmtPtr& stmt, std::map<std::string, int>& literal_counts) {
        if (!stmt) return;
        switch (stmt->getType()) {
            case ASTNode::NodeType::AssignmentStmt: {
                auto* assign = static_cast<AssignmentStatement*>(stmt.get());
                for (const auto& rhs_expr : assign->rhs) {
                    count_string_literals_in_expression(rhs_expr, literal_counts);
                }
                break;
            }
            case ASTNode::NodeType::RoutineCallStmt: {
                auto* routine_call = static_cast<RoutineCallStatement*>(stmt.get());
                for (const auto& arg : routine_call->arguments) {
                    count_string_literals_in_expression(arg, literal_counts);
                }
                break;
            }
            // Add more statement types as needed
            default:
                break;
        }
    }

    // Recursively count string literals in an expression
    void count_string_literals_in_expression(const ExprPtr& expr, std::map<std::string, int>& literal_counts) {
        if (!expr) return;
        switch (expr->getType()) {
            case ASTNode::NodeType::StringLit: {
                auto* str_lit = static_cast<StringLiteral*>(expr.get());
                literal_counts[str_lit->value]++;
                break;
            }
            case ASTNode::NodeType::BinaryOpExpr: {
                auto* bin = static_cast<BinaryOp*>(expr.get());
                count_string_literals_in_expression(bin->left, literal_counts);
                count_string_literals_in_expression(bin->right, literal_counts);
                break;
            }
            case ASTNode::NodeType::UnaryOpExpr: {
                auto* un = static_cast<UnaryOp*>(expr.get());
                count_string_literals_in_expression(un->operand, literal_counts);
                break;
            }
            case ASTNode::NodeType::FunctionCallExpr: {
                auto* call = static_cast<FunctionCall*>(expr.get());
                count_string_literals_in_expression(call->function_expr, literal_counts);
                for (const auto& arg : call->arguments) {
                    count_string_literals_in_expression(arg, literal_counts);
                }
                break;
            }
            // Add more expression types as needed
            default:
                break;
        }
    }

    // Recursively replace string literals in a statement with temp variable accesses if lifted
    void replace_lifted_string_literals_in_statement(StmtPtr& stmt, const std::map<std::string, std::string>& lifted_string_to_temp) {
        if (!stmt) return;
        switch (stmt->getType()) {
            case ASTNode::NodeType::AssignmentStmt: {
                auto* assign = static_cast<AssignmentStatement*>(stmt.get());
                for (auto& rhs_expr : assign->rhs) {
                    replace_lifted_string_literals_in_expression(rhs_expr, lifted_string_to_temp);
                }
                break;
            }
            case ASTNode::NodeType::RoutineCallStmt: {
                auto* routine_call = static_cast<RoutineCallStatement*>(stmt.get());
                for (auto& arg : routine_call->arguments) {
                    replace_lifted_string_literals_in_expression(arg, lifted_string_to_temp);
                }
                break;
            }
            // Add more statement types as needed
            default:
                break;
        }
    }

    // Recursively replace string literals in an expression with temp variable accesses if lifted
    void replace_lifted_string_literals_in_expression(ExprPtr& expr, const std::map<std::string, std::string>& lifted_string_to_temp) {
        if (!expr) return;
        switch (expr->getType()) {
            case ASTNode::NodeType::StringLit: {
                auto* str_lit = static_cast<StringLiteral*>(expr.get());
                auto it = lifted_string_to_temp.find(str_lit->value);
                if (it != lifted_string_to_temp.end()) {
                    expr = std::make_unique<VariableAccess>(it->second);
                }
                break;
            }
            case ASTNode::NodeType::BinaryOpExpr: {
                auto* bin = static_cast<BinaryOp*>(expr.get());
                replace_lifted_string_literals_in_expression(bin->left, lifted_string_to_temp);
                replace_lifted_string_literals_in_expression(bin->right, lifted_string_to_temp);
                break;
            }
            case ASTNode::NodeType::UnaryOpExpr: {
                auto* un = static_cast<UnaryOp*>(expr.get());
                replace_lifted_string_literals_in_expression(un->operand, lifted_string_to_temp);
                break;
            }
            case ASTNode::NodeType::FunctionCallExpr: {
                auto* call = static_cast<FunctionCall*>(expr.get());
                replace_lifted_string_literals_in_expression(call->function_expr, lifted_string_to_temp);
                for (auto& arg : call->arguments) {
                    replace_lifted_string_literals_in_expression(arg, lifted_string_to_temp);
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
                    compound->statements
                );
                lifter.process_statement_list();
            }
        }
        // Add more declaration types as needed
    }
}
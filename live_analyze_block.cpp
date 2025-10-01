#include "LivenessAnalysisPass.h"
#include "AST.h"
#include <iostream>
#include <set>

void LivenessAnalysisPass::analyze_block(BasicBlock* block) {
    if (trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Entering analyze_block for block: " 
                  << (block ? block->id : "nullptr") << std::endl;
    }

    if (!block) {
        if (trace_enabled_) {
            std::cerr << "[LivenessAnalysisPass] ERROR: analyze_block called with nullptr block!" << std::endl;
        }
        return;
    }

    current_use_set_.clear();
    current_def_set_.clear();
    current_block_being_analyzed_ = block;

    // INTRA-BLOCK CALL INTERVAL ANALYSIS
    // First pass: Identify variables used after any function call within this block
    std::set<std::string> vars_used_after_calls;
    bool found_call = false;
    
    if (trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Starting intra-block analysis for " << block->id 
                  << " with " << block->statements.size() << " statements" << std::endl;
    }
    
    // Scan statements in reverse order to find variables used after calls
    for (auto it = block->statements.rbegin(); it != block->statements.rend(); ++it) {
        const auto& stmt = *it;
        if (!stmt) continue;
        
        // Check if this statement uses any variables
        std::set<std::string> vars_used_in_stmt;
        if (auto* assignment = dynamic_cast<AssignmentStatement*>(stmt.get())) {
            // Collect variables used in RHS of assignments
            for (const auto& rhs_expr : assignment->rhs) {
                collect_variable_uses(rhs_expr.get(), vars_used_in_stmt);
            }
        } else {
            // For other statements, collect all variable uses
            collect_statement_variable_uses(stmt.get(), vars_used_in_stmt);
        }
        
        if (trace_enabled_ && !vars_used_in_stmt.empty()) {
            std::cout << "[LivenessAnalysisPass] Statement uses variables: ";
            for (const auto& var : vars_used_in_stmt) {
                std::cout << var << " ";
            }
            std::cout << std::endl;
        }
        
        // Check if this statement contains a function call
        bool stmt_contains_call = statement_contains_call(stmt.get());
        
        // SPECIAL CASE: Handle intra-statement call intervals
        // For expressions like "N * FUNC(N-1)", we need to detect that N is used
        // both before and after the function call within the same statement
        if (stmt_contains_call) {
            std::set<std::string> vars_used_across_call;
            if (auto* resultis_stmt = dynamic_cast<ResultisStatement*>(stmt.get())) {
                collect_variables_used_across_calls(resultis_stmt->expression.get(), vars_used_across_call);
            } else if (auto* assignment = dynamic_cast<AssignmentStatement*>(stmt.get())) {
                for (const auto& rhs_expr : assignment->rhs) {
                    collect_variables_used_across_calls(rhs_expr.get(), vars_used_across_call);
                }
            }
            
            if (!vars_used_across_call.empty()) {
                vars_used_after_calls.insert(vars_used_across_call.begin(), vars_used_across_call.end());
                if (trace_enabled_) {
                    std::cout << "[LivenessAnalysisPass] Intra-statement call interval detected - variables live across calls: ";
                    for (const auto& var : vars_used_across_call) {
                        std::cout << var << " ";
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        // If we've seen a call, all variables used in this statement are live across calls
        if (found_call) {
            vars_used_after_calls.insert(vars_used_in_stmt.begin(), vars_used_in_stmt.end());
            if (trace_enabled_ && !vars_used_in_stmt.empty()) {
                std::cout << "[LivenessAnalysisPass] Variables used after call: ";
                for (const auto& var : vars_used_in_stmt) {
                    std::cout << var << " ";
                }
                std::cout << std::endl;
            }
        }
        
        if (stmt_contains_call) {
            found_call = true;
            if (trace_enabled_) {
                std::cout << "[LivenessAnalysisPass] Found call in statement" << std::endl;
            }
        }
    }
    
    // Add variables that are used after calls to the use set (call interval fix)
    if (!vars_used_after_calls.empty() && trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Intra-block call interval detected in " << block->id 
                  << " - variables live across calls: ";
        for (const auto& var : vars_used_after_calls) {
            std::cout << var << " ";
        }
        std::cout << std::endl;
    }
    
    // Add these variables to use set to force them into callee-saved registers
    current_use_set_.insert(vars_used_after_calls.begin(), vars_used_after_calls.end());
    
    // Store the variables used across calls for this block for use in data flow analysis
    if (!vars_used_after_calls.empty()) {
        vars_used_across_calls_per_block_[block] = vars_used_after_calls;
    }

    // Second pass: Normal forward processing for Use/Def computation
    int stmt_idx = 0;
    for (const auto& stmt : block->statements) {
        if (!stmt) {
            if (trace_enabled_) {
                std::cout << "[LivenessAnalysisPass] Warning: Null statement at index " << stmt_idx 
                          << " in block " << block->id << std::endl;
            }
            ++stmt_idx;
            continue;
        }
        try {
            stmt->accept(*this);
        } catch (const std::exception& ex) {
            if (trace_enabled_) {
                std::cerr << "[LivenessAnalysisPass] Exception during stmt->accept in block " 
                          << block->id << " at statement index " << stmt_idx << ": " << ex.what() << std::endl;
            }
        } catch (...) {
            if (trace_enabled_) {
                std::cerr << "[LivenessAnalysisPass] Unknown exception during stmt->accept in block " 
                          << block->id << " at statement index " << stmt_idx << std::endl;
            }
        }
        ++stmt_idx;
    }

    use_sets_[block] = current_use_set_;
    def_sets_[block] = current_def_set_;

    if (trace_enabled_) {
        std::cout << "[LivenessAnalysisPass] Exiting analyze_block for block: " << block->id << std::endl;
    }
}

// Helper method to collect variable uses from an expression
void LivenessAnalysisPass::collect_variable_uses(ASTNode* node, std::set<std::string>& vars) {
    if (!node) return;
    
    if (auto* var_access = dynamic_cast<VariableAccess*>(node)) {
        if (symbol_table_) {
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol) && symbol.is_variable()) {
                vars.insert(var_access->name);
            }
        }
    } else if (auto* binary_op = dynamic_cast<BinaryOp*>(node)) {
        collect_variable_uses(binary_op->left.get(), vars);
        collect_variable_uses(binary_op->right.get(), vars);
    } else if (auto* unary_op = dynamic_cast<UnaryOp*>(node)) {
        collect_variable_uses(unary_op->operand.get(), vars);
    } else if (auto* func_call = dynamic_cast<FunctionCall*>(node)) {
        for (const auto& arg : func_call->arguments) {
            collect_variable_uses(arg.get(), vars);
        }
    } else if (auto* vec_access = dynamic_cast<VectorAccess*>(node)) {
        collect_variable_uses(vec_access->vector_expr.get(), vars);
        collect_variable_uses(vec_access->index_expr.get(), vars);
    } else if (auto* member_access = dynamic_cast<MemberAccessExpression*>(node)) {
        collect_variable_uses(member_access->object_expr.get(), vars);
    } else if (auto* pair_access = dynamic_cast<PairAccessExpression*>(node)) {
        // This is the critical fix: when accessing .first or .second,
        // we need to mark the underlying pair variable as used
        collect_variable_uses(pair_access->pair_expr.get(), vars);
    } else if (auto* pair_expr = dynamic_cast<PairExpression*>(node)) {
        collect_variable_uses(pair_expr->first_expr.get(), vars);
        collect_variable_uses(pair_expr->second_expr.get(), vars);
    } else if (auto* fpair_access = dynamic_cast<FPairAccessExpression*>(node)) {
        // This is the critical fix: when accessing .first or .second on FPAIR,
        // we need to mark the underlying fpair variable as used
        collect_variable_uses(fpair_access->pair_expr.get(), vars);
    } else if (auto* fpair_expr = dynamic_cast<FPairExpression*>(node)) {
        collect_variable_uses(fpair_expr->first_expr.get(), vars);
        collect_variable_uses(fpair_expr->second_expr.get(), vars);
    } else if (auto* conditional = dynamic_cast<ConditionalExpression*>(node)) {
        collect_variable_uses(conditional->condition.get(), vars);
        collect_variable_uses(conditional->true_expr.get(), vars);
        collect_variable_uses(conditional->false_expr.get(), vars);
    }
}

// Helper method to collect variable uses from a statement
void LivenessAnalysisPass::collect_statement_variable_uses(ASTNode* stmt, std::set<std::string>& vars) {
    if (!stmt) return;
    
    if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(stmt)) {
        // Check if this is a SUPER method call
        if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
            // This is a SUPER.METHOD call - collect argument variables
            for (const auto& arg : routine_call->arguments) {
                collect_variable_uses(arg.get(), vars);
            }
        } else {
            // Regular routine call
            for (const auto& arg : routine_call->arguments) {
                collect_variable_uses(arg.get(), vars);
            }
        }
    } else if (auto* assignment = dynamic_cast<AssignmentStatement*>(stmt)) {
        for (const auto& rhs_expr : assignment->rhs) {
            collect_variable_uses(rhs_expr.get(), vars);
        }
    } else if (auto* resultis_stmt = dynamic_cast<ResultisStatement*>(stmt)) {
        // CRITICAL FIX: ResultisStatement contains an expression that may use variables
        // after function calls - we need to analyze it for call interval analysis
        collect_variable_uses(resultis_stmt->expression.get(), vars);
    } else if (auto* return_stmt = dynamic_cast<ReturnStatement*>(stmt)) {
        // ReturnStatement in this AST doesn't have an expression field
        // It's handled separately in other statement types
    }
}

// Helper method to check if a statement contains a function call
bool LivenessAnalysisPass::statement_contains_call(ASTNode* stmt) {
    if (!stmt) return false;
    
    if (auto* routine_call = dynamic_cast<RoutineCallStatement*>(stmt)) {
        if (trace_enabled_) {
            std::cout << "[LivenessAnalysisPass] Found RoutineCallStatement";
            if (auto* super_call = dynamic_cast<SuperMethodCallExpression*>(routine_call->routine_expr.get())) {
                std::cout << " (SUPER." << super_call->member_name << ")";
            }
            std::cout << std::endl;
        }
        return true;
    }
    
    if (dynamic_cast<FunctionCall*>(stmt)) {
        if (trace_enabled_) {
            std::cout << "[LivenessAnalysisPass] Found FunctionCall" << std::endl;
        }
        return true;
    }
    
    // Check for calls within assignment statements
    if (auto* assignment = dynamic_cast<AssignmentStatement*>(stmt)) {
        for (const auto& rhs_expr : assignment->rhs) {
            if (expression_contains_call(rhs_expr.get())) {
                return true;
            }
        }
    }
    
    // ================== FIX STARTS HERE ==================
    // Add this block to check inside ResultisStatement expressions.
    if (auto* resultis_stmt = dynamic_cast<ResultisStatement*>(stmt)) {
        if (expression_contains_call(resultis_stmt->expression.get())) {
            return true;
        }
    }
    // =================== FIX ENDS HERE ===================
    
    return false;
}

// Helper method to collect variables that are used both before and after function calls
// within the same expression (e.g., N in "N * FUNC(N-1)")
void LivenessAnalysisPass::collect_variables_used_across_calls(ASTNode* expr, std::set<std::string>& vars) {
    if (!expr) return;
    
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr)) {
        // For binary operations, check if one side contains a call and the other uses variables
        bool left_has_call = expression_contains_call(binary_op->left.get());
        bool right_has_call = expression_contains_call(binary_op->right.get());
        
        if (left_has_call && !right_has_call) {
            // Left side has call, right side variables are used after call
            collect_variable_uses(binary_op->right.get(), vars);
        } else if (right_has_call && !left_has_call) {
            // Right side has call, left side variables are used after call  
            collect_variable_uses(binary_op->left.get(), vars);
        } else if (left_has_call || right_has_call) {
            // Recursively analyze both sides
            collect_variables_used_across_calls(binary_op->left.get(), vars);
            collect_variables_used_across_calls(binary_op->right.get(), vars);
        }
    } else if (auto* unary_op = dynamic_cast<UnaryOp*>(expr)) {
        collect_variables_used_across_calls(unary_op->operand.get(), vars);
    } else if (auto* conditional = dynamic_cast<ConditionalExpression*>(expr)) {
        collect_variables_used_across_calls(conditional->condition.get(), vars);
        collect_variables_used_across_calls(conditional->true_expr.get(), vars);
        collect_variables_used_across_calls(conditional->false_expr.get(), vars);
    }
}

// Helper method to check if an expression contains a function call
bool LivenessAnalysisPass::expression_contains_call(ASTNode* expr) {
    if (!expr) return false;
    
    if (dynamic_cast<FunctionCall*>(expr)) {
        return true;
    }
    
    if (auto* binary_op = dynamic_cast<BinaryOp*>(expr)) {
        return expression_contains_call(binary_op->left.get()) || 
               expression_contains_call(binary_op->right.get());
    }
    
    if (auto* unary_op = dynamic_cast<UnaryOp*>(expr)) {
        return expression_contains_call(unary_op->operand.get());
    }
    
    if (auto* pair_access = dynamic_cast<PairAccessExpression*>(expr)) {
        return expression_contains_call(pair_access->pair_expr.get());
    }
    
    if (auto* pair_expr = dynamic_cast<PairExpression*>(expr)) {
        return expression_contains_call(pair_expr->first_expr.get()) ||
               expression_contains_call(pair_expr->second_expr.get());
    }
    
    if (auto* fpair_access = dynamic_cast<FPairAccessExpression*>(expr)) {
        return expression_contains_call(fpair_access->pair_expr.get());
    }
    
    if (auto* fpair_expr = dynamic_cast<FPairExpression*>(expr)) {
        return expression_contains_call(fpair_expr->first_expr.get()) ||
               expression_contains_call(fpair_expr->second_expr.get());
    }
    
    if (auto* member_access = dynamic_cast<MemberAccessExpression*>(expr)) {
        return expression_contains_call(member_access->object_expr.get());
    }
    
    if (auto* vec_access = dynamic_cast<VectorAccess*>(expr)) {
        return expression_contains_call(vec_access->vector_expr.get()) ||
               expression_contains_call(vec_access->index_expr.get());
    }
    
    if (auto* conditional = dynamic_cast<ConditionalExpression*>(expr)) {
        return expression_contains_call(conditional->condition.get()) ||
               expression_contains_call(conditional->true_expr.get()) ||
               expression_contains_call(conditional->false_expr.get());
    }
    
    return false;
}

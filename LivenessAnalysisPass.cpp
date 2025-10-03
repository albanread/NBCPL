#include "LivenessAnalysisPass.h"
#include "AST.h"
#include "RegisterManager.h"
#include <iostream>

// Constructor
LivenessAnalysisPass::LivenessAnalysisPass(const CFGMap& cfgs, SymbolTable* symbol_table, bool trace_enabled) 
    : cfgs_(cfgs), symbol_table_(symbol_table), trace_enabled_(trace_enabled) {}

// Phase 1: Compute use and def sets for every basic block in every CFG.
// Implementation moved to separate file: live_compute_use_def_sets.cpp

// Analyze a single basic block to populate its use and def sets.
// Implementation moved to separate file: live_analyze_block.cpp



// Visitor for variable access (a 'use' case).
void LivenessAnalysisPass::visit(VariableAccess& node) {
    // Only track actual variables, not function/routine names
    if (symbol_table_) {
        Symbol symbol;
        if (symbol_table_->lookup(node.name, symbol) && symbol.is_variable()) {
            // CORRECTED LIVENESS ANALYSIS:
            // A variable is in use[B] if it is read in block B before being assigned in B.
            // Since we process statements in forward order, if we haven't seen a def yet,
            // this is a use that should be recorded.
            if (current_def_set_.find(node.name) == current_def_set_.end()) {
                current_use_set_.insert(node.name);
            }
        }
    } else {
        // Fallback to old behavior if no symbol table available
        if (current_def_set_.find(node.name) == current_def_set_.end()) {
            current_use_set_.insert(node.name);
        }
    }
}

// Visitor for assignment (a 'def' case).
void LivenessAnalysisPass::visit(AssignmentStatement& node) {
    // First, visit the RHS to find all 'uses'.
    for (const auto& rhs_expr : node.rhs) {
        rhs_expr->accept(*this);
    }

    // Then, visit the LHS to find all 'defs'.
    for (const auto& lhs_expr : node.lhs) {
        if (auto* var = dynamic_cast<VariableAccess*>(lhs_expr.get())) {
            // CORRECTED LIVENESS ANALYSIS:
            // A variable is in def[B] if it is assigned/written in block B.
            // This is independent of whether it's used elsewhere in the block.
            // Standard data-flow analysis: def[B] = {variables assigned in B}
            current_def_set_.insert(var->name);
        }
    }
}

// --- Recursive ASTVisitor overrides for liveness analysis ---

void LivenessAnalysisPass::visit(BinaryOp& node) {
    if (node.left) node.left->accept(*this);
    if (node.right) node.right->accept(*this);
}

void LivenessAnalysisPass::visit(UnaryOp& node) {
    if (node.operand) node.operand->accept(*this);
}

void LivenessAnalysisPass::visit(FunctionCall& node) {
    // Mark this block as containing a function call for call interval fix
    if (current_block_being_analyzed_) {
        blocks_with_calls_.insert(current_block_being_analyzed_);
        if (trace_enabled_) {
            std::cout << "[LivenessAnalysisPass] Block " << current_block_being_analyzed_->id 
                      << " contains function call - marked for call interval fix" << std::endl;
        }
    }
    
    // Check if the function being called is actually a variable (function pointer)
    bool is_variable_call = false;
    if (symbol_table_ && node.function_expr) {
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
            Symbol symbol;
            if (symbol_table_->lookup(var_access->name, symbol) && symbol.is_variable()) {
                is_variable_call = true;
            }
        }
    }
    
    // Only visit the function expression if it's a variable (function pointer)
    if (is_variable_call && node.function_expr) {
        node.function_expr->accept(*this);
    }
    
    // Always visit arguments
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(VectorAccess& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void LivenessAnalysisPass::visit(CharIndirection& node) {
    if (node.string_expr) node.string_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void LivenessAnalysisPass::visit(FloatVectorIndirection& node) {
    if (node.vector_expr) node.vector_expr->accept(*this);
    if (node.index_expr) node.index_expr->accept(*this);
}

void LivenessAnalysisPass::visit(ConditionalExpression& node) {
    if (node.condition) node.condition->accept(*this);
    if (node.true_expr) node.true_expr->accept(*this);
    if (node.false_expr) node.false_expr->accept(*this);
}

void LivenessAnalysisPass::visit(ValofExpression& node) {
    if (!node.body) return;

    // Enter the VALOF block and analyze its statements
    node.body->accept(*this);

    // Ensure any variables declared or used in the VALOF block are analyzed
    if (trace_enabled_) {
        std::cout << "[DEBUG] Analyzing VALOF block for liveness." << std::endl;
    }
}

void LivenessAnalysisPass::visit(RoutineCallStatement& node) {
    // Mark this block as containing a function call for call interval fix
    if (current_block_being_analyzed_) {
        blocks_with_calls_.insert(current_block_being_analyzed_);
        if (trace_enabled_) {
            std::cout << "[LivenessAnalysisPass] Block " << current_block_being_analyzed_->id 
                      << " contains routine call - marked for call interval fix" << std::endl;
        }
    }
    
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
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(IfStatement& node) {
    if (node.condition) node.condition->accept(*this);
    // then_branch belongs to a different basic block and should not be visited here
}

void LivenessAnalysisPass::visit(UnlessStatement& node) {
    if (node.condition) node.condition->accept(*this);
    // then_branch belongs to a different basic block and should not be visited here
}

void LivenessAnalysisPass::visit(TestStatement& node) {
    if (node.condition) node.condition->accept(*this);
    // then_branch and else_branch belong to different basic blocks and should not be visited here
}

void LivenessAnalysisPass::visit(WhileStatement& node) {
    if (node.condition) node.condition->accept(*this);
}

void LivenessAnalysisPass::visit(UntilStatement& node) {
    if (node.condition) node.condition->accept(*this);
}

void LivenessAnalysisPass::visit(RepeatStatement& node) {
    // RepeatStatement node is placed in the condition block.
    // The body belongs to a different basic block and should not be visited here.
}

void LivenessAnalysisPass::visit(ForStatement& node) {
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr) node.end_expr->accept(*this);
    if (node.step_expr) node.step_expr->accept(*this);
    
    // Add the loop variable to the def set (it gets assigned values during iteration)
    current_def_set_.insert(node.unique_loop_variable_name);
}

void LivenessAnalysisPass::visit(ForEachStatement& node) {
    // First, visit the collection expression to record its usage
    if (node.collection_expression) {
        node.collection_expression->accept(*this);
    }
    
    // Add the loop variable to the def set (it gets assigned values from the collection)
    current_def_set_.insert(node.loop_variable_name);
    
    // If there's a type variable, add it to the def set as well
    if (!node.type_variable_name.empty()) {
        current_def_set_.insert(node.type_variable_name);
    }
}

void LivenessAnalysisPass::visit(SwitchonStatement& node) {
    if (node.expression) node.expression->accept(*this);
    // cases and default_case belong to different basic blocks and should not be visited here
}

void LivenessAnalysisPass::visit(CaseStatement& node) {
    if (node.constant_expr) node.constant_expr->accept(*this);
    if (node.command) node.command->accept(*this);
}

void LivenessAnalysisPass::visit(DefaultStatement& node) {
    if (node.command) node.command->accept(*this);
}

void LivenessAnalysisPass::visit(GotoStatement& node) {
    // No variable use in label jump
}

void LivenessAnalysisPass::visit(ReturnStatement& node) {
    // No return value expression in ReturnStatement
}

void LivenessAnalysisPass::visit(FinishStatement& node) {
    if (node.syscall_number) node.syscall_number->accept(*this);
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(BreakStatement& node) {
    // No variable use
}

void LivenessAnalysisPass::visit(LoopStatement& node) {
    // No body field in LoopStatement
}

void LivenessAnalysisPass::visit(EndcaseStatement& node) {
    // No variable use
}

void LivenessAnalysisPass::visit(FreeStatement& node) {
    if (node.list_expr) node.list_expr->accept(*this);
}

void LivenessAnalysisPass::visit(CompoundStatement& node) {
    for (const auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void LivenessAnalysisPass::visit(BlockStatement& node) {
    for (const auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void LivenessAnalysisPass::visit(StringStatement& node) {
    // No variable use
    }

    void LivenessAnalysisPass::visit(ResultisStatement& node) {
        if (node.expression) node.expression->accept(*this);
    }



void LivenessAnalysisPass::visit(LabelTargetStatement& node) {
    // LabelTargetStatement only has a labelName (no statement/command to visit)
}

void LivenessAnalysisPass::visit(ConditionalBranchStatement& node) {
    if (node.condition_expr) node.condition_expr->accept(*this);
    // node.targetLabel is a string, not an AST node
}

void LivenessAnalysisPass::visit(SysCall& node) {
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(VecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void LivenessAnalysisPass::visit(StringAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

void LivenessAnalysisPass::visit(TableExpression& node) {
    for (const auto& entry : node.initializers) {
        if (entry) entry->accept(*this);
    }
}

void LivenessAnalysisPass::visit(GlobalVariableDeclaration& node) {
    // Global variables are not part of liveness analysis within functions.
    // This method is implemented to satisfy the ASTVisitor interface.
    
    // If needed, we could process initializers here:
    // for (const auto& initializer : node.initializers) {
    //     if (initializer) initializer->accept(*this);
    // }
}

// Phase 2: Run the iterative data-flow algorithm.
// Implementation moved to separate file: live_run_data_flow_analysis.cpp

// Main entry point to run the analysis.
// Implementation moved to separate file: live_run.cpp

// Accessors
// Implementation moved to separate files: live_get_in_set.cpp and live_get_out_set.cpp

// Print results
// Implementation moved to separate file: live_print_results.cpp





// Missing Expression visitor methods

void LivenessAnalysisPass::visit(PairExpression& node) {
    if (node.first_expr) node.first_expr->accept(*this);
    if (node.second_expr) node.second_expr->accept(*this);
}

void LivenessAnalysisPass::visit(PairAccessExpression& node) {
    // This is the critical fix: when accessing .first or .second,
    // we must visit the underlying pair expression to mark it as live
    if (node.pair_expr) node.pair_expr->accept(*this);
}

void LivenessAnalysisPass::visit(FPairExpression& node) {
    if (node.first_expr) node.first_expr->accept(*this);
    if (node.second_expr) node.second_expr->accept(*this);
}

void LivenessAnalysisPass::visit(FPairAccessExpression& node) {
    // This is the critical fix: when accessing .first or .second,
    // we must visit the underlying fpair expression to mark it as live
    if (node.pair_expr) node.pair_expr->accept(*this);
}

void LivenessAnalysisPass::visit(QuadExpression& node) {
    if (node.first_expr) node.first_expr->accept(*this);
    if (node.second_expr) node.second_expr->accept(*this);
    if (node.third_expr) node.third_expr->accept(*this);
    if (node.fourth_expr) node.fourth_expr->accept(*this);
}

void LivenessAnalysisPass::visit(QuadAccessExpression& node) {
    // When accessing .first/.second/.third/.fourth,
    // we must visit the underlying quad expression to mark it as live
    if (node.quad_expr) node.quad_expr->accept(*this);
}

void LivenessAnalysisPass::visit(MemberAccessExpression& node) {
    if (node.object_expr) node.object_expr->accept(*this);
}

void LivenessAnalysisPass::visit(SuperMethodCallExpression& node) {
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(NewExpression& node) {
    for (const auto& arg : node.constructor_arguments) {
        if (arg) arg->accept(*this);
    }
}

void LivenessAnalysisPass::visit(ListExpression& node) {
    for (const auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void LivenessAnalysisPass::visit(VecInitializerExpression& node) {
    for (const auto& init : node.initializers) {
        if (init) init->accept(*this);
    }
}

void LivenessAnalysisPass::visit(BitfieldAccessExpression& node) {
    if (node.base_expr) node.base_expr->accept(*this);
    if (node.start_bit_expr) node.start_bit_expr->accept(*this);
    if (node.width_expr) node.width_expr->accept(*this);
}

void LivenessAnalysisPass::visit(FVecAllocationExpression& node) {
    if (node.size_expr) node.size_expr->accept(*this);
}

// Print results for debugging

#include <algorithm>
#include <map>
#include <string>

std::map<std::string, int> LivenessAnalysisPass::calculate_register_pressure() const {
    std::map<std::string, int> pressure_map;

    // Iterate over each function's Control Flow Graph
    for (const auto& cfg_pair : cfgs_) {
        const std::string& func_name = cfg_pair.first;
        const auto& cfg = cfg_pair.second;
        int max_pressure = 0;

        // Iterate over each basic block in the current function's CFG
        for (const auto& block_pair : cfg->get_blocks()) {
            BasicBlock* block = block_pair.second.get();

            // Find the size of the in-set and out-set for the current block
            size_t in_size = get_in_set(block).size();
            size_t out_size = get_out_set(block).size();

            // The register pressure at this block's boundaries is the max of the two
            int current_max = std::max(in_size, out_size);

            // Keep track of the highest pressure seen so far in this function
            if (current_max > max_pressure) {
                max_pressure = current_max;
            }
        }
        pressure_map[func_name] = max_pressure;
    }
    return pressure_map;
}

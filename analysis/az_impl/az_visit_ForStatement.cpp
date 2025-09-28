#include "ASTAnalyzer.h"
#include <iostream>
#include <map>
#include <string>

/**
 * @brief Visits a ForStatement node.
 * Handles unique naming for loop variables, manages scope, and traverses the loop body.
 * OPTIMIZATION: Detects constant expressions for end and step values to avoid unnecessary backing variables.
 */
void ASTAnalyzer::visit(ForStatement& node) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Visiting ForStatement for variable: " << node.loop_variable << std::endl;

    // Push FOR loop context to prevent state leakage to other loop types
    loop_context_stack_.push(LoopContext::FOR_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed FOR loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    // --- Determine the unique name for the loop variable itself ---
    std::string canonical_for_var_name;
    if (for_variable_unique_aliases_.count(node.loop_variable)) {
        canonical_for_var_name = for_variable_unique_aliases_[node.loop_variable];
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE]   Reusing unique loop var '" << canonical_for_var_name
                      << "' for original '" << node.loop_variable << "' (previously declared FOR var)." << std::endl;
        }
    } else {
        canonical_for_var_name = node.loop_variable + "_for_var_" + std::to_string(for_loop_var_counter_++);
        for_variable_unique_aliases_[node.loop_variable] = canonical_for_var_name;
        if (current_function_scope_ != "Global") {
            variable_definitions_[canonical_for_var_name] = current_function_scope_;
            function_metrics_[current_function_scope_].num_variables++;
            // Register the type of the new loop variable. It's always an integer.
            function_metrics_[current_function_scope_].variable_types[canonical_for_var_name] = VarType::INTEGER;
            if (trace_enabled_) {
                std::cout << "[ANALYZER TRACE]   Created NEW unique loop var '" << canonical_for_var_name
                          << "' for original '" << node.loop_variable << "'. Defined for stack space. Incremented var count." << std::endl;
            }
        } else {
            variable_definitions_[canonical_for_var_name] = "Global";
            if (trace_enabled_) {
                std::cout << "[ANALYZER TRACE]   Created NEW unique global loop var '" << canonical_for_var_name
                          << "' for original '" << node.loop_variable << "'. Defined as global." << std::endl;
            }
        }
    }
    node.unique_loop_variable_name = canonical_for_var_name;

    // --- Register the loop variable in the symbol table ---
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Attempting to register loop variable '" << canonical_for_var_name 
                  << "' in symbol table. symbol_table_=" << (symbol_table_ ? "valid" : "null") 
                  << ", current_function_scope_='" << current_function_scope_ << "'" << std::endl;
    }
    
    if (symbol_table_ && current_function_scope_ != "Global") {
        Symbol loop_var_symbol(
            canonical_for_var_name,
            SymbolKind::LOCAL_VAR,
            VarType::INTEGER,
            symbol_table_->currentScopeLevel(),
            current_function_scope_
        );
        bool added = symbol_table_->addSymbol(loop_var_symbol);
        if (trace_enabled_) {
            if (added) {
                std::cout << "[ANALYZER TRACE] Successfully added loop variable '" << canonical_for_var_name << "' to symbol table." << std::endl;
            } else {
                std::cout << "[ANALYZER TRACE] Note: loop variable '" << canonical_for_var_name << "' already in symbol table." << std::endl;
            }
        }
    } else if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] Skipping symbol table registration for loop variable '" << canonical_for_var_name 
                  << "' (either no symbol table or global scope)" << std::endl;
    }

    // --- OPTIMIZATION: Analyze end and step expressions for constants ---
    bool end_is_constant = false;
    int64_t constant_end_val = 0;
    if (node.end_expr) {
        constant_end_val = evaluate_constant_expression(node.end_expr.get(), &end_is_constant);
        node.is_end_expr_constant = end_is_constant;
        node.constant_end_value = constant_end_val;
        if (trace_enabled_) {
            if (end_is_constant) {
                std::cout << "[ANALYZER TRACE] OPTIMIZATION: End expression is constant: " << constant_end_val << std::endl;
            } else {
                std::cout << "[ANALYZER TRACE] End expression is variable, will create backing variable." << std::endl;
            }
        }
    }

    bool step_is_constant = false;
    int64_t constant_step_val = 1; // Default step is 1
    if (node.step_expr) {
        constant_step_val = evaluate_constant_expression(node.step_expr.get(), &step_is_constant);
    } else {
        step_is_constant = true; // No step expression means step is 1 (constant)
    }
    node.is_step_expr_constant = step_is_constant;
    node.constant_step_value = constant_step_val;
    if (trace_enabled_) {
        if (step_is_constant) {
            std::cout << "[ANALYZER TRACE] OPTIMIZATION: Step expression is constant: " << constant_step_val << std::endl;
        } else {
            std::cout << "[ANALYZER TRACE] Step expression is variable, will create backing variable." << std::endl;
        }
    }

    // --- Manage active_for_loop_scopes_ for nested FOR loops ---
    std::map<std::string, std::string> current_for_scope_map;
    current_for_scope_map[node.loop_variable] = node.unique_loop_variable_name;
    active_for_loop_scopes_.push(current_for_scope_map);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed FOR loop scope for '" << node.loop_variable << "' -> '" << node.unique_loop_variable_name << "'. Stack size: " << active_for_loop_scopes_.size() << std::endl;

    // --- Conditionally create backing variables for non-constant expressions ---
    
    // Only create step backing variable if step is not constant
    if (!step_is_constant) {
        node.unique_step_variable_name = node.unique_loop_variable_name + "_step_inst_" + std::to_string(for_loop_instance_suffix_counter);
        variable_definitions_[node.unique_step_variable_name] = current_function_scope_;
        if (current_function_scope_ != "Global") {
            function_metrics_[current_function_scope_].num_variables++;
        }
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE]   Created backing var for step: '" << node.unique_step_variable_name << "'" << std::endl;
        }
    } else {
        // Clear the step variable name since we won't need it
        node.unique_step_variable_name.clear();
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] OPTIMIZATION: Skipped creating step backing variable (constant: " << constant_step_val << ")" << std::endl;
        }
    }

    // Only create end backing variable if end is not constant
    if (!end_is_constant) {
        node.unique_end_variable_name = node.unique_loop_variable_name + "_end_inst_" + std::to_string(for_loop_instance_suffix_counter);
        variable_definitions_[node.unique_end_variable_name] = current_function_scope_;
        if (current_function_scope_ != "Global") {
            function_metrics_[current_function_scope_].num_variables++;
        }
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE]   Created backing var for hoisted end value: '" << node.unique_end_variable_name << "'" << std::endl;
        }
    } else {
        // Clear the end variable name since we won't need it
        node.unique_end_variable_name.clear();
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] OPTIMIZATION: Skipped creating end backing variable (constant: " << constant_end_val << ")" << std::endl;
        }
    }

    for_loop_instance_suffix_counter++;
    for_statements_[node.unique_loop_variable_name] = &node;

    // Visit expressions (even if constant, they might contain nested function calls)
    if (node.start_expr) node.start_expr->accept(*this);
    if (node.end_expr && !end_is_constant) node.end_expr->accept(*this); // Only visit if not constant
    if (node.step_expr && !step_is_constant) node.step_expr->accept(*this); // Only visit if not constant

    // Visit the body after pushing the scope
    if (node.body) node.body->accept(*this);

    // Pop the scope after visiting the body
    active_for_loop_scopes_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped FOR loop scope. Stack size: " << active_for_loop_scopes_.size() << std::endl;

    // Pop FOR loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped FOR loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    // Print optimization summary
    if (trace_enabled_) {
        int vars_saved = 0;
        if (end_is_constant) vars_saved++;
        if (step_is_constant) vars_saved++;
        if (vars_saved > 0) {
            std::cout << "[ANALYZER TRACE] OPTIMIZATION SUMMARY: Saved " << vars_saved 
                      << " backing variables for FOR loop '" << node.loop_variable << "'" << std::endl;
        }
    }
}
#include "../ASTAnalyzer.h"
#include "../../RuntimeManager.h"
#include <iostream>

/**
 * @brief Visits a FunctionCall node.
 * Tracks local and runtime function calls, and traverses arguments.
 */
void ASTAnalyzer::visit(FunctionCall& node) {
    // Track the approximate instruction point for this function call
    // Estimate based on metrics in the current function
    if (!current_function_scope_.empty()) {
        // Generate a synthetic instruction index for this call point
        int call_point = function_metrics_[current_function_scope_].instruction_count++;
        
        // Add this call site to the function's list
        function_call_sites_[current_function_scope_].push_back(call_point);

        // --- LEAF DETECTION (GENERIC) ---
        // A function that calls another function is not a leaf.
        function_metrics_[current_function_scope_].is_leaf = false;
        // --- END LEAF DETECTION ---
    }



    if (auto* var_access = dynamic_cast<VariableAccess*>(node.function_expr.get())) {
        if (is_local_function(var_access->name)) {
            function_metrics_[current_function_scope_].num_local_function_calls++;
            // Register in call graph for heap allocation propagation
            register_function_call(current_function_scope_, var_access->name);
            if (trace_enabled_) std::cout << "[ANALYZER TRACE]   Detected call to local function: " << var_access->name << std::endl;
        } else if (RuntimeManager::instance().is_function_registered(var_access->name)) {
            function_metrics_[current_function_scope_].num_runtime_calls++;
            function_metrics_[current_function_scope_].accesses_globals = true; // <-- Flag global access for runtime call
            // Runtime functions don't need to be in call graph as they don't perform heap allocation
            if (trace_enabled_) std::cout << "[ANALYZER TRACE]   Detected call to runtime function: " << var_access->name << ", Type: " << (get_runtime_function_type(var_access->name) == FunctionType::FLOAT ? "FLOAT" : "INTEGER") << std::endl;
        }
    }
    for (const auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }

    // --- NEW: Ownership Check for RELEASE Method Calls ---
    if (auto* member_access = dynamic_cast<MemberAccessExpression*>(node.function_expr.get())) {
        // Check if the method being called is RELEASE.
        if (member_access->member_name == "RELEASE") {
            // Check if the object being released is a simple variable.
            if (auto* var_access = dynamic_cast<VariableAccess*>(member_access->object_expr.get())) {
                const std::string& var_name = var_access->name;
                Symbol symbol;
                // Look up the variable in the symbol table.
                if (symbol_table_ && symbol_table_->lookup(var_name, symbol)) {
                    // *** THE CORE CHECK ***
                    // If the symbol does not own its heap memory, this is an error.
                    if (!symbol.owns_heap_memory) {
                        std::string error_msg = "Invalid call to RELEASE on variable '" + var_name +
                                                "' which does not own its heap memory (was it RETAINed or not heap-allocated?).";
                        std::cerr << "[SEMANTIC ERROR] " << error_msg << std::endl;
                        semantic_errors_.push_back(error_msg);
                    }
                }
            }
        }
    }
    // --- END NEW LOGIC ---


    // Increment the approximate instruction counter to account for the call instructions
    if (!current_function_scope_.empty()) {
        // Approximate the number of instructions generated for this call
        // Each argument + the call itself + possible return value handling
        function_metrics_[current_function_scope_].instruction_count += node.arguments.size() + 2;
    }
}

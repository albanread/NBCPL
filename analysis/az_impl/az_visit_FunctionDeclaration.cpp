#include "ASTAnalyzer.h"
#include "../../NameMangler.h"
#include <iostream>

void ASTAnalyzer::visit(FunctionDeclaration& node) {
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Visiting FunctionDeclaration: " << node.name << std::endl;

    // Synchronize SymbolTable scope
    if (symbol_table_) {
        symbol_table_->enterScope();
    }

    // --- START OF FIX ---
    // Reset FOR loop state for the new function scope.
    for_variable_unique_aliases_.clear();
    while (!active_for_loop_scopes_.empty()) {
        active_for_loop_scopes_.pop();
    }
    // --- END OF FIX ---

    // Save previous scopes to handle nested functions correctly.
    std::string previous_function_scope = current_function_scope_;
    std::string previous_lexical_scope = current_lexical_scope_;

    // --- FIX: Use the mangled name as the scope if in class context ---
    std::string mangled_name = node.name;
    if (!current_class_name_.empty()) {
        mangled_name = NameMangler::mangleMethod(current_class_name_, node.name);
        
        // This is a method. Prepend the implicit '_this' parameter to the AST node only if not already present.
        if (std::find(node.parameters.begin(), node.parameters.end(), "_this") == node.parameters.end()) {
            node.parameters.insert(node.parameters.begin(), "_this");
            if (trace_enabled_) {
                std::cout << "\n[ANALYZER IMPORTANT] === Adding _this parameter to method: " << mangled_name << " ===\n";
                std::cout << "[ANALYZER TRACE]   Injected '_this' parameter for method: " << mangled_name << std::endl;
            }
        }
    }
    current_function_scope_ = mangled_name;
    current_lexical_scope_ = mangled_name;

    // Update parameter count (will include _this if it was injected)
    function_metrics_[mangled_name].num_parameters = node.parameters.size();
    for (size_t i = 0; i < node.parameters.size(); ++i) {
        const auto& param = node.parameters[i];
        variable_definitions_[param] = mangled_name; // Defined in this function's scope.
        function_metrics_[mangled_name].parameter_indices[param] = i;

        // Handle parameters, with special case for '_this'
        if (param == "_this" && !current_class_name_.empty()) {
            // This is our injected parameter. Give it the correct type.
            function_metrics_[mangled_name].variable_types[param] = VarType::POINTER_TO_OBJECT;
            if (trace_enabled_) {
                std::cout << "[ANALYZER IMPORTANT] Setting _this parameter type to POINTER_TO_OBJECT for " << mangled_name << std::endl;
            }
            // Also update the symbol table with the full type information.
            if (symbol_table_) {
                Symbol this_symbol(
                    param,
                    SymbolKind::PARAMETER,
                    VarType::POINTER_TO_OBJECT,
                    symbol_table_->currentScopeLevel(),
                    mangled_name
                );
                // Associate the '_this' pointer with its class.
                this_symbol.class_name = current_class_name_;
                symbol_table_->addSymbol(this_symbol);
                if (trace_enabled_) {
                    std::cout << "[ANALYZER IMPORTANT] Added _this symbol to symbol table with class_name: " << current_class_name_ << std::endl;
                }
            }
        } else {
            // Check if parameter type was already set by signature analysis
            auto& metrics = function_metrics_[mangled_name];
            auto existing_param_it = metrics.parameter_types.find(param);
            
            if (existing_param_it != metrics.parameter_types.end() && 
                existing_param_it->second != VarType::UNKNOWN) {
                // Parameter type already set by signature analysis, keep it
                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] Preserving existing parameter type for " << param 
                              << " in " << mangled_name << " (type: " << static_cast<int>(existing_param_it->second) << ")" << std::endl;
                }
                metrics.variable_types[param] = existing_param_it->second;
            } else {
                // Regular parameter - get type from symbol table
                VarType param_type = VarType::INTEGER; // Default fallback
                if (symbol_table_) {
                    Symbol param_symbol;
                    if (trace_enabled_) {
                        std::cout << "[ANALYZER DEBUG] Looking up parameter '" << param 
                                  << "' in function context '" << mangled_name << "'" << std::endl;
                    }
                    if (symbol_table_->lookup(param, mangled_name, param_symbol)) {
                        param_type = param_symbol.type;
                        if (trace_enabled_) {
                            std::cout << "[ANALYZER DEBUG] Found parameter '" << param 
                                      << "' with type " << static_cast<int>(param_type) << std::endl;
                        }
                    } else {
                        // Parameter not found in symbol table, use default INTEGER
                        param_type = VarType::INTEGER;
                        if (trace_enabled_) {
                            std::cout << "[ANALYZER DEBUG] Parameter '" << param 
                                      << "' NOT found in symbol table, using default INTEGER" << std::endl;
                        }
                    }
                }
                metrics.variable_types[param] = param_type;
                set_parameter_type_safe(mangled_name, param, param_type);
                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] Early parameter type for " << param << " in " << mangled_name 
                              << " set to " << static_cast<int>(param_type) << std::endl;
                }
            }
        }
    }

    // --- PARAMETER TYPE INFERENCE ---
    // Before visiting the function body, infer parameter types from their usage
    infer_parameter_types(mangled_name, node.parameters, node.body.get());
    
    // Update parameter types in function metrics from symbol table after inference
    // Use the safe setter method to preserve SignatureAnalysisVisitor results
    for (const auto& param : node.parameters) {
        if (symbol_table_) {
            Symbol param_symbol;
            if (symbol_table_->lookup(param, mangled_name, param_symbol)) {
                set_parameter_type_safe(mangled_name, param, param_symbol.type);
                function_metrics_[mangled_name].variable_types[param] = param_symbol.type;
                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] Updated parameter type for " << param << " in " << mangled_name 
                              << " to " << static_cast<int>(param_symbol.type) << " after inference" << std::endl;
                }
            }
        }
    }

    if (node.body) {
        node.body->accept(*this);
    }
    


    // --- TRIVIAL ACCESSOR DETECTION (TARGETED) ---
    if (!current_class_name_.empty()) {
        // Optimization requires the function to be a leaf.
        if (function_metrics_[mangled_name].is_leaf) {
            std::string member_name = identify_trivial_accessor(node.body.get(), current_class_name_);
            if (!member_name.empty()) {
                auto& metrics = function_metrics_[mangled_name];
                metrics.is_trivial_accessor = true;
                metrics.accessed_member_name = member_name;
                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] Detected trivial accessor method: " << mangled_name << " (accesses: " << member_name << ")" << std::endl;
                }
            }
        }
    }
    // --- END TRIVIAL ACCESSOR DETECTION ---

    // Restore previous scopes upon exit.
    current_function_scope_ = previous_function_scope;
    current_lexical_scope_ = previous_lexical_scope;

    // Synchronize SymbolTable scope
    if (symbol_table_) {
        symbol_table_->exitScope();
    }
}

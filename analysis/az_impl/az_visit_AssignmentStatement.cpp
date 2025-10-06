#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../Reducer.h"

// Visitor implementation for AssignmentStatement nodes
void ASTAnalyzer::visit(AssignmentStatement& node) {
    if (trace_enabled_) {
        std::cerr << "[ASSIGNMENT VISITOR] Processing AssignmentStatement with " 
                  << node.lhs.size() << " LHS variables" << std::endl;
    }
    
    // Check for single assignment with reducer function: result := REDUCER_FUNC(...)
    if (node.lhs.size() == 1 && node.rhs.size() == 1) {
        auto* lhs_var = dynamic_cast<VariableAccess*>(node.lhs[0].get());
        auto* func_call = dynamic_cast<FunctionCall*>(node.rhs[0].get());

        if (lhs_var && func_call) {
            if (auto* func_var = dynamic_cast<VariableAccess*>(func_call->function_expr.get())) {
                const std::string& func_name = func_var->name;

                // Check for PAIRWISE_MIN and other reducer functions
                if (func_name == "PAIRWISE_MIN" && func_call->arguments.size() >= 1) {
                    if (trace_enabled_) {
                        std::cerr << "[ASSIGNMENT VISITOR] Detected PAIRWISE_MIN assignment - converting to ReductionStatement" << std::endl;
                    }
                    
                    // Note: In a real implementation, you would need to transform the AST
                    // For now, we'll just mark that this should be handled as a reduction
                    // and process it normally, but with special awareness
                    
                    // Ensure the LHS variable is in the symbol table
                    if (symbol_table_) {
                        Symbol existing_symbol;
                        if (!symbol_table_->lookup(lhs_var->name, existing_symbol)) {
                            // Add the variable to symbol table as it will receive the reduction result
                            Symbol new_symbol(
                                lhs_var->name,
                                SymbolKind::LOCAL_VAR,
                                VarType::POINTER_TO_FLOAT_VEC, // PAIRWISE_MIN typically returns a vector
                                symbol_table_->currentScopeLevel(),
                                current_function_scope_
                            );
                            symbol_table_->addSymbol(new_symbol);
                            
                            // Update function metrics
                            if (current_function_scope_ != "Global") {
                                auto& metrics = function_metrics_[current_function_scope_];
                                metrics.variable_types[lhs_var->name] = VarType::POINTER_TO_FLOAT_VEC;
                                metrics.num_variables++;
                            }
                            
                            if (trace_enabled_) {
                                std::cerr << "[ASSIGNMENT VISITOR] Added PAIRWISE_MIN result variable '" 
                                         << lhs_var->name << "' to symbol table" << std::endl;
                            }
                        }
                    }
                }
                // Add other reducer functions here as needed
                else if (func_name == "PAIRWISE_MAX" && func_call->arguments.size() >= 1) {
                    // Similar handling for PAIRWISE_MAX
                    if (symbol_table_) {
                        Symbol existing_symbol;
                        if (!symbol_table_->lookup(lhs_var->name, existing_symbol)) {
                            Symbol new_symbol(
                                lhs_var->name,
                                SymbolKind::LOCAL_VAR,
                                VarType::POINTER_TO_FLOAT_VEC,
                                symbol_table_->currentScopeLevel(),
                                current_function_scope_
                            );
                            symbol_table_->addSymbol(new_symbol);
                            
                            if (current_function_scope_ != "Global") {
                                auto& metrics = function_metrics_[current_function_scope_];
                                metrics.variable_types[lhs_var->name] = VarType::POINTER_TO_FLOAT_VEC;
                                metrics.num_variables++;
                            }
                        }
                    }
                }
                else if (func_name == "PAIRWISE_ADD" && func_call->arguments.size() >= 1) {
                    // Similar handling for PAIRWISE_ADD
                    if (symbol_table_) {
                        Symbol existing_symbol;
                        if (!symbol_table_->lookup(lhs_var->name, existing_symbol)) {
                            Symbol new_symbol(
                                lhs_var->name,
                                SymbolKind::LOCAL_VAR,
                                VarType::POINTER_TO_FLOAT_VEC,
                                symbol_table_->currentScopeLevel(),
                                current_function_scope_
                            );
                            symbol_table_->addSymbol(new_symbol);
                            
                            if (current_function_scope_ != "Global") {
                                auto& metrics = function_metrics_[current_function_scope_];
                                metrics.variable_types[lhs_var->name] = VarType::POINTER_TO_FLOAT_VEC;
                                metrics.num_variables++;
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Visit all RHS expressions first (to analyze subexpressions)
    for (const auto& rhs : node.rhs) {
        if (rhs) rhs->accept(*this);
    }

    // Check for destructuring assignment pattern
    if ((node.lhs.size() == 2 && node.rhs.size() == 1) || 
        (node.lhs.size() == 4 && node.rhs.size() == 1)) {
        // This is potentially a destructuring assignment
        VarType rhs_type = infer_expression_type(node.rhs[0].get());
        
        if (rhs_type == VarType::PAIR || rhs_type == VarType::FPAIR || rhs_type == VarType::QUAD) {
            if (trace_enabled_) {
                std::string type_name = (rhs_type == VarType::PAIR) ? "PAIR" : 
                                       (rhs_type == VarType::FPAIR) ? "FPAIR" : "QUAD";
                std::cerr << "[ASSIGNMENT VISITOR] Processing destructuring assignment for " << type_name << std::endl;
            }
            
            // Handle destructuring: LHS variables get the component type
            VarType component_type = (rhs_type == VarType::PAIR) ? VarType::INTEGER : 
                                   (rhs_type == VarType::FPAIR) ? VarType::FLOAT : VarType::INTEGER;
            size_t num_components = (rhs_type == VarType::QUAD) ? 4 : 2;
            
            for (size_t i = 0; i < num_components; ++i) {
                auto* var = dynamic_cast<VariableAccess*>(node.lhs[i].get());
                if (var) {
                    // Check if this is a class member
                    bool is_class_member = false;
                    if (!current_class_name_.empty() && class_table_) {
                        const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
                        if (class_entry && class_entry->member_variables.count(var->name) > 0) {
                            is_class_member = true;
                        }
                    }
                    
                    if (!is_class_member) {
                        // Update the variable type in function_metrics_
                        if (!current_function_scope_.empty()) {
                            function_metrics_[current_function_scope_].variable_types[var->name] = component_type;
                        }
                        
                        // Update symbol table
                        if (symbol_table_) {
                            Symbol symbol;
                            if (symbol_table_->lookup(var->name, symbol)) {
                                symbol.type = component_type;
                                symbol.owns_heap_memory = false; // Components don't own heap memory
                                symbol.contains_literals = false;
                                symbol_table_->updateSymbol(var->name, symbol);
                            }
                        }
                        
                        // Attribute variable to current function if not already present
                        if (variable_definitions_.find(var->name) == variable_definitions_.end()) {
                            variable_definitions_[var->name] = current_function_scope_;
                            if (current_function_scope_ != "Global") {
                                function_metrics_[current_function_scope_].num_variables++;
                            }
                        }
                    }
                }
                
                // Visit the LHS expression
                if (node.lhs[i]) node.lhs[i]->accept(*this);
            }
            return; // Early return for destructuring case
        } else {
            // Invalid destructuring: RHS is not a PAIR/FPAIR/QUAD
            std::string error_msg = "Invalid destructuring assignment: RHS must be PAIR, FPAIR, or QUAD type, got " + 
                                  var_type_to_string(rhs_type);
            std::cerr << "[SEMANTIC ERROR] " << error_msg << std::endl;
            semantic_errors_.push_back(error_msg);
        }
    }

    // For regular assignment, process normally
    size_t count = std::min(node.lhs.size(), node.rhs.size());
    for (size_t i = 0; i < count; ++i) {
        auto* var = dynamic_cast<VariableAccess*>(node.lhs[i].get());
        if (var) {
            // --- START OF FIX ---
            // Check if this is an assignment to a class member.
            bool is_class_member = false;
            if (!current_class_name_.empty() && class_table_) {
                const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
                if (class_entry && class_entry->member_variables.count(var->name) > 0) {
                    is_class_member = true;
                }
            }
            // If it's a member, skip adding it as a local variable.
            if (is_class_member) {
                continue;
            }
            // --- END OF FIX ---

            // --- START OF MEMORY LEAK DETECTION ---
            // Check if this variable already owns heap memory before reassignment
            if (trace_enabled_) {
                std::cerr << "[DEBUG] Processing assignment to variable: " << var->name << std::endl;
            }
            if (symbol_table_) {
                Symbol existing_symbol;
                bool symbol_found = symbol_table_->lookup(var->name, existing_symbol);
                if (trace_enabled_) {
                    std::cerr << "[DEBUG] Symbol lookup for '" << var->name << "': " 
                             << (symbol_found ? "FOUND" : "NOT FOUND") << std::endl;
                }
                
                if (symbol_found) {
                    if (trace_enabled_) {
                        std::cerr << "[DEBUG] Symbol '" << var->name << "' owns_heap_memory: " 
                                 << (existing_symbol.owns_heap_memory ? "TRUE" : "FALSE") << std::endl;
                    }
                    
                    if (existing_symbol.owns_heap_memory) {
                        // Check if the RHS is a heap allocation (would cause a leak)
                        bool rhs_allocates_memory = dynamic_cast<NewExpression*>(node.rhs[i].get()) ||
                                                  dynamic_cast<ListExpression*>(node.rhs[i].get()) ||
                                                  dynamic_cast<VecAllocationExpression*>(node.rhs[i].get()) ||
                                                  dynamic_cast<StringAllocationExpression*>(node.rhs[i].get());
                        
                        if (trace_enabled_) {
                            std::cerr << "[DEBUG] RHS allocates memory: " 
                                     << (rhs_allocates_memory ? "TRUE" : "FALSE") << std::endl;
                        }
                        
                        if (rhs_allocates_memory) {
                            std::string error_msg = "Potential memory leak: Variable '" + var->name +
                                                  "' is being reassigned while it still owns heap memory. " +
                                                  "Consider calling RELEASE first or use a different variable.";
                            std::cerr << "[MEMORY LEAK WARNING] " << error_msg << std::endl;
                            semantic_errors_.push_back(error_msg);
                        }
                    }
                }
            }
            // --- END OF MEMORY LEAK DETECTION ---

            VarType rhs_type = infer_expression_type(node.rhs[i].get());
            
            // Check if variable already exists in symbol table with a declared type
            VarType variable_type = VarType::UNKNOWN;
            bool variable_exists = false;
            if (symbol_table_) {
                Symbol existing_symbol;
                if (symbol_table_->lookup(var->name, existing_symbol)) {
                    variable_exists = true;
                    // Use type priority: if inferred type is more specific, use it
                    if (should_update_type(existing_symbol.type, rhs_type)) {
                        variable_type = rhs_type;
                    } else {
                        variable_type = existing_symbol.type;
                    }
                } else {
                    // New variable - infer type from expression
                    variable_type = rhs_type;
                }
            } else {
                // No symbol table - infer from expression  
                variable_type = rhs_type;
            }
            
            // Update the variable type in function_metrics_
            if (!current_function_scope_.empty()) {
                function_metrics_[current_function_scope_].variable_types[var->name] = variable_type;
            }
            
            // Ownership-flagging logic for heap-allocating assignments
            if (symbol_table_) {
                Symbol symbol;
                if (symbol_table_->lookup(var->name, symbol)) {
                    // Update type using priority system - more specific types win
                    symbol.type = variable_type;
                    if (auto* new_expr = dynamic_cast<NewExpression*>(node.rhs[i].get())) {
                        symbol.class_name = new_expr->class_name;
                    }

                    // 2. Correctly set the ownership flag based on the RHS expression type.
                    if (dynamic_cast<NewExpression*>(node.rhs[i].get()) ||
                        dynamic_cast<ListExpression*>(node.rhs[i].get()) ||
                        dynamic_cast<VecAllocationExpression*>(node.rhs[i].get()) ||
                        dynamic_cast<StringAllocationExpression*>(node.rhs[i].get())) {
                        symbol.owns_heap_memory = true;
                    } else {
                        symbol.owns_heap_memory = false;
                    }

                    // 3. Check if this is a list assignment and mark if it contains literals
                    if (auto* list_expr = dynamic_cast<ListExpression*>(node.rhs[i].get())) {
                        symbol.contains_literals = list_expr->contains_literals;
                    } else {
                        symbol.contains_literals = false;
                    }

                    // 4. Update the symbol in the table with all new information.
                    symbol_table_->updateSymbol(var->name, symbol);

                    // Debug output for variable type using vartype_to_string
                    if (trace_enabled_) {
                        std::cerr << "[DEBUG] Updated variable '" << var->name << "' type to " << vartype_to_string(symbol.type) << std::endl;
                    }
                }
            }
            // Attribute variable to current function/routine if not already present
            if (variable_definitions_.find(var->name) == variable_definitions_.end()) {
                variable_definitions_[var->name] = current_function_scope_;
                if (current_function_scope_ != "Global") {
                    function_metrics_[current_function_scope_].num_variables++;
                }
            }
        }

        // --- NEW: Prevent assignment to HD/TL of MANIFESTLIST (const list types) ---
        if (auto* un_op = dynamic_cast<UnaryOp*>(node.lhs[i].get())) {
            if (un_op->op == UnaryOp::Operator::HeadOf ||
                un_op->op == UnaryOp::Operator::TailOf) {
                VarType list_type = infer_expression_type(un_op->operand.get());
                if (is_const_list_type(list_type)) {
                    std::cerr << "Semantic Error: Cannot assign to a part of a read-only MANIFESTLIST." << std::endl;
                    // Optionally: set an error flag or throw
                }
            }
        }

        if (node.lhs[i]) node.lhs[i]->accept(*this);
    }

    // Visit any remaining LHS expressions (in case of destructuring or side effects)
    for (size_t i = count; i < node.lhs.size(); ++i) {
        if (node.lhs[i]) node.lhs[i]->accept(*this);
    }
}

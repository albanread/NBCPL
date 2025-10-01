//
// File: az_visit_LetDeclaration.cpp
// Description: This file implements the `visit` method for `LetDeclaration` nodes
// in the ASTAnalyzer. Its primary role is to perform semantic analysis for `let`
// variable declarations. This includes type inference for the declared variables,
// updating the symbol table with their names and types, and gathering metrics
// about variable counts within function scopes.
//

#include "../ASTAnalyzer.h"
#include "../../SymbolTable.h"

// RAII helper class for managing function scope changes
class ScopeGuard {
private:
    std::string& scope_ref_;
    std::string previous_scope_;
    
public:
    ScopeGuard(std::string& scope_ref, const std::string& new_scope) 
        : scope_ref_(scope_ref), previous_scope_(scope_ref) {
        scope_ref_ = new_scope;
    }
    
    ~ScopeGuard() {
        scope_ref_ = previous_scope_;
    }
    
    // Delete copy constructor and assignment operator to prevent misuse
    ScopeGuard(const ScopeGuard&) = delete;
    ScopeGuard& operator=(const ScopeGuard&) = delete;
};

void ASTAnalyzer::visit(LetDeclaration& node) {
    if (trace_enabled_) {
        std::cerr << "DEBUG: ASTAnalyzer::visit(LetDeclaration) called with " << node.names.size() << " variables" << std::endl;
        std::cerr << "DEBUG: LetDeclaration has " << node.initializers.size() << " initializers" << std::endl;
    }
    
    // Check for destructuring LET declaration pattern
    if (node.names.size() == 2 && node.initializers.size() == 1) {
        // This is potentially a destructuring LET declaration
        VarType initializer_type = infer_expression_type(node.initializers[0].get());
        
        if (initializer_type == VarType::PAIR || initializer_type == VarType::FPAIR) {
            if (trace_enabled_) {
                std::cerr << "[LET DECLARATION VISITOR] Processing destructuring declaration for " 
                          << (initializer_type == VarType::PAIR ? "PAIR" : "FPAIR") << std::endl;
            }
            
            // Handle destructuring: both variables get the component type
            VarType component_type = (initializer_type == VarType::PAIR) ? VarType::INTEGER : VarType::FLOAT;
            
            for (size_t i = 0; i < 2; ++i) {
                const std::string& name = node.names[i];
                
                // Check if we're in a class method context and this is a class member
                bool is_class_member = false;
                if (!current_class_name_.empty() && class_table_) {
                    const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
                    if (class_entry && class_entry->member_variables.count(name) > 0) {
                        is_class_member = true;
                    }
                }
                
                if (!is_class_member && current_function_scope_ != "Global") {
                    // Update the metrics map for the current function scope
                    auto& metrics = function_metrics_[current_function_scope_];
                    metrics.variable_types[name] = component_type;
                    
                    if (component_type == VarType::FLOAT) {
                        metrics.num_float_variables++;
                    } else {
                        metrics.num_variables++;
                    }
                    
                    // Update symbol table
                    if (symbol_table_) {
                        Symbol updated_symbol(
                            name,
                            SymbolKind::LOCAL_VAR,
                            component_type,
                            symbol_table_->currentScopeLevel(),
                            current_function_scope_
                        );
                        updated_symbol.owns_heap_memory = false; // Components don't own heap memory
                        updated_symbol.contains_literals = false;
                        
                        Symbol existing_symbol;
                        if (symbol_table_->lookup(name, existing_symbol)) {
                            symbol_table_->updateSymbol(name, updated_symbol);
                        } else {
                            symbol_table_->addSymbol(updated_symbol);
                        }
                    }
                }
            }
            
            // Visit the initializer expression
            if (node.initializers[0]) {
                node.initializers[0]->accept(*this);
            }
            return; // Early return for destructuring case
        } else {
            // Invalid destructuring: initializer is not a PAIR/FPAIR
            std::string error_msg = "Invalid destructuring LET declaration: initializer must be PAIR or FPAIR type, got " + 
                                  vartype_to_string(initializer_type);
            std::cerr << "[SEMANTIC ERROR] " << error_msg << std::endl;
            semantic_errors_.push_back(error_msg);
        }
    }
    
    // For regular LET declarations, process normally
    for (size_t i = 0; i < node.names.size(); ++i) {
        const std::string& name = node.names[i];
        if (trace_enabled_) {
            std::cerr << "DEBUG: Processing variable '" << name << "' in ASTAnalyzer" << std::endl;
        }
        Expression* initializer = (i < node.initializers.size()) ? node.initializers[i].get() : nullptr;
        if (trace_enabled_) {
            std::cerr << "DEBUG: Variable '" << name << "' initializer is " << (initializer ? "present" : "null") << std::endl;
            if (initializer) {
                std::cerr << "DEBUG: Variable '" << name << "' initializer type: " << vartype_to_string(infer_expression_type(initializer)) << std::endl;
            }
            std::cerr << "DEBUG: Variable '" << name << "' current_function_scope_='" << current_function_scope_ << "'" << std::endl;
        }

        // Check if we're in a class method context and this is a class member
        bool is_class_member = false;
        if (!current_class_name_.empty() && class_table_) {
            // We are in a class method context
            const ClassTableEntry* class_entry = class_table_->get_class(current_class_name_);
            if (class_entry && class_entry->member_variables.count(name) > 0) {
                is_class_member = true;
                // Skip processing class member variables in method declarations 
                // since they should be accessed from the object instance
                if (initializer) {
                    initializer->accept(*this);
                }
                continue;
            }
        }
    
        // Semantic analysis and type checking are only performed for local function scopes.
        // Global declarations are handled by a different mechanism.
        if (trace_enabled_) {
            std::cerr << "DEBUG: Variable '" << name << "' current_function_scope_='" 
                      << current_function_scope_ << "'" << std::endl;
        }
        
        // --- IMPROVED FIX START ---
        // Check if this declaration is a function-like VALOF/FVALOF expression.
        bool is_function_like = initializer && (dynamic_cast<ValofExpression*>(initializer) || dynamic_cast<FloatValofExpression*>(initializer));

        // If it's a function-like declaration, we must set the correct scope before visiting its body.
        if (is_function_like) {
            if (trace_enabled_) {
                std::cerr << "DEBUG: Detected function-like LET declaration: " << name << std::endl;
            }
            // Use RAII to ensure scope is always restored, even if an exception occurs
            ScopeGuard scope_guard(current_function_scope_, name);

            if (initializer) {
                initializer->accept(*this); // Visit the VALOF body with the correct scope
            }

            continue; // Skip the rest of the loop; this was a function, not a variable.
        }
        // --- END IMPROVED FIX ---
        
        if (current_function_scope_ == "Global") {
            if (trace_enabled_) {
                std::cerr << "DEBUG: Skipping global scope variable '" << name << "'" << std::endl;
            }
            if (initializer) {
                initializer->accept(*this);
            }
            continue;
        }

        // 1. Determine the variable's final type and class name.
        VarType determined_type = VarType::UNKNOWN;
        std::string determined_class_name = "";

        // --- PRIORITIZE EXPLICIT TYPE ANNOTATION ---
        if (node.explicit_type != VarType::UNKNOWN) {
            determined_type = node.explicit_type;
            std::cerr << "DEBUG: Variable '" << name << "' using explicit type annotation: " 
                      << vartype_to_string(determined_type) << std::endl;
        }
        // --- HANDLE NEW expressions and CAPTURE class_name ---
        else if (initializer && dynamic_cast<NewExpression*>(initializer)) {
            auto* new_expr = static_cast<NewExpression*>(initializer);
            determined_type = VarType::POINTER_TO_OBJECT;
            determined_class_name = new_expr->class_name;
            std::cerr << "DEBUG: Variable '" << name << "' is NewExpression for class: " << determined_class_name << " with type: " << vartype_to_string(determined_type) << std::endl;
        }
        else if (initializer) {
            // First, check for explicit vector allocations which have unambiguous types.
            if (dynamic_cast<FVecAllocationExpression*>(initializer)) {
                determined_type = VarType::POINTER_TO_FLOAT_VEC;
                std::cerr << "DEBUG: Variable '" << name << "' is FVecAllocation with type: " << vartype_to_string(determined_type) << std::endl;
            } else if (dynamic_cast<VecAllocationExpression*>(initializer)) {
                determined_type = VarType::POINTER_TO_INT_VEC;
                std::cerr << "DEBUG: Variable '" << name << "' is VecAllocation with type: " << vartype_to_string(determined_type) << std::endl;
            } else {
                // For all other expressions, use the primary type inferencer.
                VarType inferred_type = infer_expression_type(initializer);
                std::cerr << "DEBUG: Variable '" << name << "' inference returned type: " 
                          << vartype_to_string(inferred_type) << std::endl;

                // PRIORITIZE INFERENCE FROM INITIALIZER:
                // If the inferencer returns any concrete type (including complex types), use it directly.
                if (inferred_type != VarType::UNKNOWN) {
                    determined_type = inferred_type;
                    std::cerr << "DEBUG: Variable '" << name << "' type from inference: " 
                              << vartype_to_string(determined_type) << " (inference-based)" << std::endl;
                } else {
                    // SCOPING FIX: Don't rely on symbol table lookup during ASTAnalyzer traversal
                    // SymbolDiscoveryPass already populated the symbol table correctly
                    // Use declaration type directly when inference fails
                    determined_type = node.is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
                    std::cerr << "DEBUG: Variable '" << name << "' using declaration type (no lookup): " 
                              << vartype_to_string(determined_type) << " (declaration-based)" << std::endl;
                }
            }
        } else {
            // No initializer; the type is determined solely by the declaration keyword.
            determined_type = node.is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
            if (trace_enabled_) {
                std::cerr << "DEBUG: Variable '" << name << "' has no initializer, using declaration type: " 
                          << static_cast<int>(node.explicit_type) << std::endl;
            }
        }

        if (trace_enabled_) {
            std::cerr << "DEBUG: Variable '" << name << "' final determined_type: " 
                      << vartype_to_string(determined_type) << " before symbol table update" << std::endl;
        }

        // --- Set owns_heap_memory flag if initializer is a heap allocation ---
        bool owns_heap_memory = false;
        bool contains_literals = false;
        if (initializer) {
            if (dynamic_cast<NewExpression*>(initializer) ||
                dynamic_cast<ListExpression*>(initializer) ||
                dynamic_cast<VecAllocationExpression*>(initializer) ||
                dynamic_cast<StringAllocationExpression*>(initializer)) {
                owns_heap_memory = true;
            }
            
            // Check if this is a list containing literals
            if (auto* list_expr = dynamic_cast<ListExpression*>(initializer)) {
                contains_literals = list_expr->contains_literals;
            }
        }

        // 2. If the variable is a float, transform `HD` operator to its float-specific variant.
        if (determined_type == VarType::FLOAT && initializer) {
            if (auto* un_op = dynamic_cast<UnaryOp*>(initializer)) {
                if (un_op->op == UnaryOp::Operator::HeadOf) {
                    un_op->op = UnaryOp::Operator::HeadOfAsFloat;
                }
            }
        }

        // 3. Update the metrics map for the current function scope.
        auto& metrics = function_metrics_[current_function_scope_];
        metrics.variable_types[name] = determined_type;

        if (determined_type == VarType::FLOAT || determined_type == VarType::POINTER_TO_FLOAT_VEC) {
            metrics.num_float_variables++;
        } else {
            metrics.num_variables++;
        }

        // 4. Update the symbol table, making it the canonical source of type information.
        if (symbol_table_) {
            Symbol symbol;
            bool symbol_exists = symbol_table_->lookup(name, symbol);

            // Create a new, fully-informed symbol object regardless.
            Symbol updated_symbol(
                name,
                SymbolKind::LOCAL_VAR,
                determined_type,
                symbol_table_->currentScopeLevel(),
                current_function_scope_ // Pass the current function context
            );
            if (!determined_class_name.empty()) {
                updated_symbol.class_name = determined_class_name;
            }
            updated_symbol.owns_heap_memory = owns_heap_memory;
            updated_symbol.contains_literals = contains_literals;

            // --- NEW LOGIC: Handle RETAIN ... = ... syntax ---
            if (node.is_retained) {
                if (updated_symbol.owns_heap_memory) {
                    updated_symbol.owns_heap_memory = false;
                } else {
                    // This is a warning, not an error.
                    std::cerr << "[SEMANTIC WARNING] RETAIN used on variable '" << name 
                              << "' which was not initialized with heap memory." << std::endl;
                }
            }
            // --- END OF NEW LOGIC ---

            if (symbol_exists) {
                // If it already exists (e.g., from a forward reference), update it.
                symbol_table_->updateSymbol(name, updated_symbol);
            } else {
                // Otherwise, add the new, complete symbol to the table.
                symbol_table_->addSymbol(updated_symbol);
            }
        }

        // 5. Recursively visit the initializer's AST to continue analysis.
        if (initializer) {
            initializer->accept(*this);
        }
    }
}
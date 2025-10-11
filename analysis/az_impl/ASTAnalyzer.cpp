#include "../../AST.h"
#include "../ASTAnalyzer.h"
#include "../../DataTypes.h"
#include "../../RuntimeManager.h"
#include "../../SymbolTable.h"
#include "../../ClassTable.h"
#include "../../NameMangler.h"
#include <iostream>

// --- RETAIN statement semantic handler ---
void ASTAnalyzer::visit(RetainStatement& node) {
    if (!symbol_table_) return;

    for (const auto& var_name : node.variable_names) {
        Symbol symbol;
        // Use a scope-aware lookup if available
        if (symbol_table_->lookup(var_name, symbol)) {
            if (symbol.owns_heap_memory) {
                symbol.owns_heap_memory = false;
                symbol_table_->updateSymbol(var_name, symbol);

                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] RETAIN on '" << var_name 
                              << "'. Automatic cleanup disabled." << std::endl;
                }
            } else {
                // This is a warning, not an error. Retaining a non-owning variable is a no-op.
                std::cerr << "[SEMANTIC WARNING] RETAIN used on variable '" << var_name 
                          << "' which does not own heap memory." << std::endl;
            }
        }
    }
}

void ASTAnalyzer::visit(RemanageStatement& node) {
    if (!symbol_table_) return;

    for (const auto& var_name : node.variable_names) {
        Symbol symbol;
        // Use a scope-aware lookup if available
        if (symbol_table_->lookup(var_name, symbol)) {
            if (!symbol.owns_heap_memory) {
                symbol.owns_heap_memory = true;
                symbol_table_->updateSymbol(var_name, symbol);

                if (trace_enabled_) {
                    std::cout << "[ANALYZER TRACE] REMANAGE on '" << var_name 
                              << "'. Automatic cleanup re-enabled." << std::endl;
                }
            } else {
                // This is a warning, not an error. Re-managing an already owning variable is a no-op.
                std::cerr << "[SEMANTIC WARNING] REMANAGE used on variable '" << var_name 
                          << "' which already has automatic cleanup enabled." << std::endl;
            }
        } else {
            std::cerr << "[SEMANTIC ERROR] REMANAGE used on undefined variable '" << var_name << "'." << std::endl;
        }
    }
}

// This file contains implementations that aren't split into az_*.cpp files
// All duplicated functions have been removed to prevent linker errors

// Return a list of instruction indices where function calls occur in the given function
const std::vector<int>& ASTAnalyzer::get_call_sites_for(const std::string& function_name) const {
    auto it = function_call_sites_.find(function_name);
    if (it != function_call_sites_.end()) {
        return it->second;
    }
    return empty_call_sites_;
}

// Register a call site for injected function calls (e.g., SAMM scope calls)
void ASTAnalyzer::register_call_site(const std::string& function_name, int instruction_index) {
    function_call_sites_[function_name].push_back(instruction_index);
    if (trace_enabled_) {
        std::cout << "[ANALYZER DEBUG] Registered call site at instruction " << instruction_index 
                  << " for function '" << function_name << "'" << std::endl;
    }
}

void ASTAnalyzer::register_function_call(const std::string& caller, const std::string& callee) {
    call_graph_[caller].insert(callee);
    if (trace_enabled_) {
        std::cout << "[CALL GRAPH] " << caller << " calls " << callee << std::endl;
    }
}

const std::set<std::string>& ASTAnalyzer::get_callees_for(const std::string& function_name) const {
    auto it = call_graph_.find(function_name);
    if (it != call_graph_.end()) {
        return it->second;
    }
    return empty_callees_;
}

void ASTAnalyzer::propagate_heap_allocation_info() {
    if (trace_enabled_) {
        std::cout << "[SAMM OPTIMIZATION] Starting heap allocation propagation..." << std::endl;
    }
    
    bool changed = true;
    int iteration = 0;
    
    while (changed) {
        changed = false;
        iteration++;
        
        if (trace_enabled_) {
            std::cout << "[SAMM OPTIMIZATION] Propagation iteration " << iteration << std::endl;
        }
        
        for (auto& caller_pair : function_metrics_) {
            const std::string& caller_name = caller_pair.first;
            FunctionMetrics& caller_metrics = caller_pair.second;

            if (caller_metrics.performs_heap_allocation) {
                continue; // Already marked as performing allocation
            }

            // Check if any callee performs heap allocation
            const auto& callees = get_callees_for(caller_name);
            for (const std::string& callee_name : callees) {
                auto callee_it = function_metrics_.find(callee_name);
                if (callee_it != function_metrics_.end() && callee_it->second.performs_heap_allocation) {
                    caller_metrics.performs_heap_allocation = true;
                    changed = true;
                    
                    if (trace_enabled_) {
                        std::cout << "[SAMM OPTIMIZATION] " << caller_name 
                                  << " now marked as heap-allocating due to call to " << callee_name << std::endl;
                    }
                    break;
                }
            }
        }
    }
    
    if (trace_enabled_) {
        std::cout << "[SAMM OPTIMIZATION] Propagation completed after " << iteration << " iterations" << std::endl;
        
        // Report optimization results
        int total_functions = function_metrics_.size();
        int allocation_free_functions = 0;
        int allocation_free_leaf_functions = 0;
        
        for (const auto& pair : function_metrics_) {
            const auto& metrics = pair.second;
            if (!metrics.performs_heap_allocation) {
                allocation_free_functions++;
                if (metrics.is_leaf) {
                    allocation_free_leaf_functions++;
                }
            }
        }
        
        std::cout << "[SAMM OPTIMIZATION] Results:" << std::endl;
        std::cout << "  Total functions: " << total_functions << std::endl;
        std::cout << "  Allocation-free functions: " << allocation_free_functions << std::endl;
        std::cout << "  Allocation-free leaf functions: " << allocation_free_leaf_functions << std::endl;
        std::cout << "  Allocation-free non-leaf functions: " << (allocation_free_functions - allocation_free_leaf_functions) << std::endl;
    }
}

VarType ASTAnalyzer::get_variable_type(const std::string& function_name, const std::string& var_name) const {
    if (trace_enabled_) {
        std::cout << "[DEBUG get_variable_type] Looking for '" << var_name << "' in function '" << function_name << "'" << std::endl;
    }
    
    auto it = function_metrics_.find(function_name);
    if (it != function_metrics_.end()) {
        if (trace_enabled_) {
            std::cout << "[DEBUG get_variable_type] Found function metrics for '" << function_name << "'" << std::endl;
            std::cout << "[DEBUG get_variable_type] Parameter types count: " << it->second.parameter_types.size() << std::endl;
            std::cout << "[DEBUG get_variable_type] Variable types count: " << it->second.variable_types.size() << std::endl;
        }
        
        // First check parameter_types
        const auto& parameter_types = it->second.parameter_types;
        auto param_it = parameter_types.find(var_name);
        if (param_it != parameter_types.end()) {
            if (trace_enabled_) {
                std::cout << "[DEBUG get_variable_type] Found in parameter_types: " << var_name << " -> " << static_cast<int>(param_it->second) << std::endl;
            }
            return param_it->second;
        }
        
        // Then check variable_types
        const auto& variable_types = it->second.variable_types;
        auto var_it = variable_types.find(var_name);
        if (var_it != variable_types.end()) {
            if (trace_enabled_) {
                std::cout << "[DEBUG get_variable_type] Found in variable_types: " << var_name << " -> " << static_cast<int>(var_it->second) << std::endl;
            }
            return var_it->second;
        }
        
        if (trace_enabled_) {
            std::cout << "[DEBUG get_variable_type] Variable '" << var_name << "' not found in function metrics" << std::endl;
        }
    } else {
        if (trace_enabled_) {
            std::cout << "[DEBUG get_variable_type] No function metrics found for '" << function_name << "'" << std::endl;
        }
    }

    // --- FIX START ---
    // If not found in the function's metrics, check the symbol table using the correct function context.
    if (symbol_table_) {
        Symbol symbol;
        if (symbol_table_->lookup(var_name, function_name, symbol)) { // Pass function_name as context
            if (trace_enabled_) {
                std::cout << "[DEBUG get_variable_type] Found in symbol table: " << var_name << " -> " << static_cast<int>(symbol.type) << std::endl;
            }
            return symbol.type;
        }
    }
    // --- FIX END ---

    // Add trace message on failure
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] get_variable_type FAILED for '" << var_name << "' in function '" << function_name << "'" << std::endl;
    }
    return VarType::UNKNOWN;
}

std::string ASTAnalyzer::infer_object_class_name(const Expression* expr) const {
    if (!expr) {
        return "";
    }

    // Case 1: The expression is a variable (e.g., `p` in `p.x`).
    if (const auto* var_access = dynamic_cast<const VariableAccess*>(expr)) {
        if (var_access->name == "_this") {
            return current_class_name_; // Correct use of current class context.
        }
        Symbol symbol;
        // Use the symbol table to find the variable's true class type.
        if (symbol_table_ && symbol_table_->lookup(var_access->name, symbol)) {
            return symbol.class_name; // Return the stored class name.
        }
    }

    // Case 2: The expression is a function call (e.g., `getPoint().x`).
    if (const auto* func_call = dynamic_cast<const FunctionCall*>(expr)) {
        if (const auto* func_var = dynamic_cast<const VariableAccess*>(func_call->function_expr.get())) {
            Symbol func_symbol;
            // Look up the function in the symbol table to get its return type info.
            if (symbol_table_ && symbol_table_->lookup(func_var->name, func_symbol)) {
                // Assuming the function symbol stores the class name of the object it returns.
                return func_symbol.class_name;
            }
        }
    }

    // Case 3: The expression is a direct instantiation (e.g., `(NEW Point()).x`).
    if (const auto* new_expr = dynamic_cast<const NewExpression*>(expr)) {
        return new_expr->class_name;
    }

    // Fallback: if the type cannot be determined, return an empty string.
    return "";
}

VarType ASTAnalyzer::get_class_member_type(const MemberAccessExpression* member_access) const {
    if (!member_access || !class_table_ || !symbol_table_) {
        return VarType::UNKNOWN;
    }

    // --- START OF FIX ---

    // 1. Reliably determine the class name of the object being accessed.
    std::string object_class_name = infer_object_class_name(member_access->object_expr.get());

    if (object_class_name.empty()) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] get_class_member_type: Could not determine class for member '"
                      << member_access->member_name << "'" << std::endl;
        }
        return VarType::UNKNOWN;
    }

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] get_class_member_type: Determined class '" << object_class_name
                  << "' for member '" << member_access->member_name << "'" << std::endl;
    }

    // 2. Look up the member within that specific class in the ClassTable.
    const ClassTableEntry* class_entry = class_table_->get_class(object_class_name);
    if (!class_entry) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] get_class_member_type: Class '" << object_class_name << "' not found in ClassTable." << std::endl;
        }
        return VarType::UNKNOWN;
    }

    auto member_it = class_entry->member_variables.find(member_access->member_name);
    if (member_it != class_entry->member_variables.end()) {
        // Found a data member. Return its type.
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] get_class_member_type: Found member '" << member_access->member_name
                      << "' with type " << static_cast<int>(member_it->second.type) << std::endl;
        }
        return member_it->second.type;
    }
    
    // NOTE: This function only seems to look for member variables. If it should also handle
    // method types, that logic would be added here by looking in `class_entry->member_methods`.

    // --- END OF FIX ---

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] get_class_member_type: Member '" << member_access->member_name
                  << "' not found in class '" << object_class_name << "'" << std::endl;
    }
    return VarType::UNKNOWN;
}

std::string ASTAnalyzer::lookup_variable_class_name(const std::string& var_name) const {
    if (!symbol_table_) {
        return "";
    }
    
    Symbol symbol;
    if (symbol_table_->lookup(var_name, symbol)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] lookup_variable_class_name: Found symbol '" << var_name 
                      << "' with class_name '" << symbol.class_name << "'" << std::endl;
        }
        return symbol.class_name;
    }
    
    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] lookup_variable_class_name: Symbol '" << var_name 
                  << "' not found in symbol table" << std::endl;
    }
    return "";
}

void ASTAnalyzer::visit(FloatValofExpression& node) {
    // Visit the body of the FloatValofExpression if present
    if (node.body) {
        node.body->accept(*this);
    }
    // Additional logic can be added here if needed for FloatValofExpression
}




bool ASTAnalyzer::list_contains_only_literals(const ListExpression& node) const {
    for (const auto& expr : node.initializers) {
        if (!expr) continue;
        
        // Check if this expression is a literal
        if (dynamic_cast<const StringLiteral*>(expr.get()) ||
            dynamic_cast<const NumberLiteral*>(expr.get()) ||
            dynamic_cast<const CharLiteral*>(expr.get()) ||
            dynamic_cast<const BooleanLiteral*>(expr.get())) {
            continue; // This is a literal, check next
        }
        
        // Check if this is a nested list that also contains only literals
        if (const auto* nested_list = dynamic_cast<const ListExpression*>(expr.get())) {
            if (list_contains_only_literals(*nested_list)) {
                continue; // Nested list is all literals, check next
            }
        }
        
        // If we get here, this expression is not a literal
        return false;
    }
    return true; // All expressions are literals
}

void ASTAnalyzer::visit(ListExpression& node) {
    // Mark that the current function/routine performs heap allocation
    if (current_function_scope_ != "Global") {
        // SAMM Optimization: Mark that this function performs heap allocation
        function_metrics_[current_function_scope_].performs_heap_allocation = true;
    }
    
    bool needs_temp = false;
    bool contains_literals = list_contains_only_literals(node);
    
    // Debug output to verify literal detection
    if (trace_enabled_) {
        std::cout << "[DEBUG] ListExpression: contains_literals = " << (contains_literals ? "true" : "false") 
                  << ", num_initializers = " << node.initializers.size() << std::endl;
    }
    
    for (auto& expr : node.initializers) {
        if (expr) {
            expr->accept(*this);
            // If any initializer is not a literal, we need a callee-saved temp
            if (!expr->is_literal()) {
                needs_temp = true;
            }
        }
    }
    
    // Store whether this list contains literals for later use
    node.contains_literals = contains_literals;
    
    // Track nested temporary values
    if (needs_temp && !current_function_scope_.empty()) {
        function_metrics_[current_function_scope_].required_callee_saved_temps += 1;
    }
    // Remove reference to current_scope_stack_ (not defined)
}

void ASTAnalyzer::visit(DeferStatement& node) {
    // No semantic analysis needed for DEFER in the new system.
}

// --- Semantic analysis for SUPER.method() calls ---
void ASTAnalyzer::visit(SuperMethodCallExpression& node) {
    // --- LEAF DETECTION (GENERIC) ---
    if (!current_function_scope_.empty()) {
        // A SUPER call is a call; this method is not a leaf.
        function_metrics_[current_function_scope_].is_leaf = false;
    }
    // --- END LEAF DETECTION ---

    // Check context: must be inside a class method
    if (current_class_name_.empty()) {
        std::cerr << "[SEMANTIC ERROR] SUPER used outside of class method context." << std::endl;
        semantic_errors_.push_back("SUPER used outside of class method context.");
        return;
    }

    const ClassTableEntry* class_entry = class_table_ ? class_table_->get_class(current_class_name_) : nullptr;
    if (!class_entry) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Current class '" << current_class_name_ << "' not found in ClassTable." << std::endl;
        semantic_errors_.push_back("SUPER: Current class '" + current_class_name_ + "' not found in ClassTable.");
        return;
    }

    if (class_entry->parent_name.empty()) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Class '" << current_class_name_ << "' has no parent class." << std::endl;
        semantic_errors_.push_back("SUPER: Class '" + current_class_name_ + "' has no parent class.");
        return;
    }

    const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
    if (!parent_entry) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Parent class '" << class_entry->parent_name << "' not found in ClassTable." << std::endl;
        semantic_errors_.push_back("SUPER: Parent class '" + class_entry->parent_name + "' not found in ClassTable.");
        return;
    }

    auto method_it = parent_entry->member_methods.find(node.member_name);
    if (method_it == parent_entry->member_methods.end()) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Method '" << node.member_name << "' not found in parent class '" << class_entry->parent_name << "'." << std::endl;
        semantic_errors_.push_back("SUPER: Method '" + node.member_name + "' not found in parent class '" + class_entry->parent_name + "'.");
        return;
    }

    // Check visibility: SUPER cannot access private methods
    const ClassMethodInfo& method_info = method_it->second;
    if (method_info.visibility == Visibility::Private) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Cannot access private method '" << node.member_name << "' in parent class '" << class_entry->parent_name << "'." << std::endl;
        semantic_errors_.push_back("SUPER: Cannot access private method '" + node.member_name + "' in parent class '" + class_entry->parent_name + "'.");
        return;
    }

    // Visit arguments for further semantic analysis
    for (auto& arg : node.arguments) {
        if (arg) arg->accept(*this);
    }

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] SUPER call to '" << node.member_name << "' validated in parent class '" << class_entry->parent_name << "'." << std::endl;
    }
}

void ASTAnalyzer::visit(SuperMethodAccessExpression& node) {
    // Check context: must be inside a class method
    if (current_class_name_.empty()) {
        std::cerr << "[SEMANTIC ERROR] SUPER used outside of class method context." << std::endl;
        semantic_errors_.push_back("SUPER used outside of class method context.");
        return;
    }

    const ClassTableEntry* class_entry = class_table_ ? class_table_->get_class(current_class_name_) : nullptr;
    if (!class_entry) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Current class '" << current_class_name_ << "' not found in ClassTable." << std::endl;
        semantic_errors_.push_back("SUPER: Current class '" + current_class_name_ + "' not found in ClassTable.");
        return;
    }

    if (class_entry->parent_name.empty()) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Class '" << current_class_name_ << "' has no parent class." << std::endl;
        semantic_errors_.push_back("SUPER: Class '" + current_class_name_ + "' has no parent class.");
        return;
    }

    const ClassTableEntry* parent_entry = class_table_->get_class(class_entry->parent_name);
    if (!parent_entry) {
        std::cerr << "[SEMANTIC ERROR] SUPER: Parent class '" << class_entry->parent_name << "' not found in ClassTable." << std::endl;
        semantic_errors_.push_back("SUPER: Parent class '" + class_entry->parent_name + "' not found in ClassTable.");
        return;
    }

    if (trace_enabled_) {
        std::cout << "[ANALYZER TRACE] SUPER call to '" << node.member_name << "' validated in parent class '" << class_entry->parent_name << "'." << std::endl;
    }
}




// --- Update symbol table type on assignment ---


void ASTAnalyzer::visit(VecInitializerExpression& node) {
    // No-op: type inference is handled in infer_expression_type.
}



void ASTAnalyzer::visit(CharLiteral& node) {}
void ASTAnalyzer::visit(BrkStatement& node) {}
void ASTAnalyzer::visit(GotoStatement& node) {}
void ASTAnalyzer::visit(LoopStatement& node) {}
void ASTAnalyzer::visit(NumberLiteral& node) {}
void ASTAnalyzer::visit(StringLiteral& node) {}
void ASTAnalyzer::visit(BooleanLiteral& node) {}
void ASTAnalyzer::visit(BreakStatement& node) {}
void ASTAnalyzer::visit(FinishStatement& node) {}
void ASTAnalyzer::visit(ReturnStatement& node) {}
void ASTAnalyzer::visit(TableExpression& node) {}
void ASTAnalyzer::visit(EndcaseStatement& node) {}
void ASTAnalyzer::visit(LabelDeclaration& node) {}
void ASTAnalyzer::visit(StaticDeclaration& node) {}

void ASTAnalyzer::visit(ForEachStatement& node) {
    // Push FOREACH loop context to prevent FOR loop state interference
    loop_context_stack_.push(LoopContext::FOREACH_LOOP);
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Pushed FOREACH loop context. Context stack size: " << loop_context_stack_.size() << std::endl;

    if (node.collection_expression) node.collection_expression->accept(*this);

    VarType collection_type = infer_expression_type(node.collection_expression.get());

    // --- NEW: Handle destructuring FOREACH (X, Y) IN list_of_pairs ---
    if (node.is_destructuring) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER TRACE] Processing destructuring FOREACH (X, Y) pattern" << std::endl;
        }
        
        // For destructuring, we expect a list of PAIR/FPAIR elements
        // The collection should be a list type
        bool is_list = (static_cast<int64_t>(collection_type) & (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST)))
                       == (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST));
        
        if (!is_list) {
            std::string error_msg = "Destructuring FOREACH requires a list collection, got " + vartype_to_string(collection_type);
            std::cerr << "[SEMANTIC ERROR] " << error_msg << std::endl;
            semantic_errors_.push_back(error_msg);
        }
        
        // For now, assume the list contains PAIR elements (INTEGER components)
        // TODO: In the future, this should be inferred from the list's element type
        VarType component_type = VarType::INTEGER; // Assume PAIR for now
        
        // Register both destructuring variables with component type
        if (!current_function_scope_.empty()) {
            auto& metrics = function_metrics_[current_function_scope_];
            metrics.variable_types[node.loop_variable_name] = component_type;  // X
            metrics.variable_types[node.type_variable_name] = component_type;  // Y
            
            // Also update symbol table
            if (symbol_table_) {
                symbol_table_->updateSymbolType(node.loop_variable_name, component_type);
                symbol_table_->updateSymbolType(node.type_variable_name, component_type);
            }
        }
        
        // Set inferred element type to PAIR (the packed type we're destructuring)
        node.inferred_element_type = VarType::PAIR;
        
        // Pop context and return early
        loop_context_stack_.pop();
        if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped FOREACH loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
        return;
    }
    // --- END DESTRUCTURING LOGIC ---

    // --- Centralized element type inference for all collection types ---
    VarType element_type = VarType::ANY; // Default for ANY_LIST

    // --- REFACTORED LOGIC: Use bitwise checks for list types ---
    // 1. First, check if the collection is a pointer to a list using bit flags.
    bool is_list = (static_cast<int64_t>(collection_type) & (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST)))
                   == (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST));

    if (is_list) {
        // 2. Isolate the base type by masking out container and modifier flags.
        VarType base_type = static_cast<VarType>(
            static_cast<int64_t>(collection_type) &
            ~(static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST) | static_cast<int64_t>(VarType::CONST))
        );

        // 3. Determine the element type based on the isolated base type flag.
        if (base_type == VarType::INTEGER) {
            element_type = VarType::INTEGER;
        } else if (base_type == VarType::FLOAT) {
            element_type = VarType::FLOAT;
        } else if (base_type == VarType::STRING) {
            element_type = VarType::POINTER_TO_STRING;
        } else {
            // Default for ANY_LIST or other combinations
            element_type = VarType::ANY;
        }
    } else {
        // This part of the logic handles non-list types like VEC and STRING
        switch(collection_type) {
            case VarType::POINTER_TO_INT_VEC:
                element_type = VarType::INTEGER;
                break;
            case VarType::POINTER_TO_FLOAT_VEC:
                element_type = VarType::FLOAT;
                break;
            case VarType::POINTER_TO_STRING:
                element_type = VarType::INTEGER; // Characters are integers
                break;
            // ... add other non-list cases as needed ...
            default:
                // For unknown types, keep as ANY
                break;
        }
    }

    // Register loop variables in the current function's scope with correct types
    if (!current_function_scope_.empty()) {
        auto& metrics = function_metrics_[current_function_scope_];

        if (!node.type_variable_name.empty()) { // Two-variable form: FOREACH T, V
            metrics.variable_types[node.type_variable_name] = VarType::INTEGER;

            // Use the new bitwise logic for is_list
            bool is_list = (static_cast<int64_t>(collection_type) & (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST)))
                           == (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST));

            if (is_list) {
                metrics.variable_types[node.loop_variable_name] = VarType::ANY; // V is the node pointer
                node.filter_type = VarType::ANY;
            } else {
                metrics.variable_types[node.loop_variable_name] = element_type; // V is the element value
            }
        } else { // One-variable form
            if ((static_cast<int64_t>(collection_type) & (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST) | static_cast<int64_t>(VarType::ANY)))
                == (static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::LIST) | static_cast<int64_t>(VarType::ANY))) {
                element_type = node.filter_type; // Refine type for ANY_LIST if a filter exists
            }
            metrics.variable_types[node.loop_variable_name] = element_type;

            if (trace_enabled_) {
                std::cout << "[DEBUG FOREACH] Set variable_types[" << node.loop_variable_name << "] = " << static_cast<int>(element_type)
                          << " (" << vartype_to_string(element_type) << ") in function " << current_function_scope_ << std::endl;
            }

            // --- FIX IS HERE ---
            // Also update the symbol table with the correct inferred type.
            if (symbol_table_) {
                symbol_table_->updateSymbolType(node.loop_variable_name, element_type);
            }
            // --- END OF FIX ---
        }
    }
    node.inferred_element_type = element_type;

    if (node.body) node.body->accept(*this);

    // Pop FOREACH loop context
    loop_context_stack_.pop();
    if (trace_enabled_) std::cout << "[ANALYZER TRACE] Popped FOREACH loop context. Context stack size: " << loop_context_stack_.size() << std::endl;
}
  // --- BitfieldAccessExpression visitor ---
  void ASTAnalyzer::visit(BitfieldAccessExpression& node) {
      if (node.base_expr) node.base_expr->accept(*this);
      if (node.start_bit_expr) node.start_bit_expr->accept(*this);
      if (node.width_expr) node.width_expr->accept(*this);
  }



ASTAnalyzer& ASTAnalyzer::getInstance() {
    static ASTAnalyzer instance;
    return instance;
}

ASTAnalyzer::ASTAnalyzer() {
    reset_state();
}

// --- Type inference for expressions ---
// --- Modular Type Inference Implementation ---
// This replaces the monolithic infer_expression_type function

VarType ASTAnalyzer::infer_expression_type(const Expression* expr) const {
    if (!expr) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: infer_expression_type called with null expr" << std::endl;
        }
        return VarType::INTEGER;
    }
    
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_expression_type called on expression type: " 
                  << static_cast<int>(expr->getType()) << std::endl;
        std::cerr << "DEBUG: current_function_scope_: '" << current_function_scope_ << "'" << std::endl;
        std::cerr << "DEBUG: current_class_name_: '" << current_class_name_ << "'" << std::endl;
    }

    try {
        // Dispatch to appropriate specialized method based on expression type
        switch (expr->getType()) {
            case ASTNode::NodeType::NumberLit:
            case ASTNode::NodeType::StringLit:
            case ASTNode::NodeType::BooleanLit:
            case ASTNode::NodeType::CharLit:
            case ASTNode::NodeType::NullLit:
                return infer_literal_type(expr);
                
            case ASTNode::NodeType::VariableAccessExpr:
                return infer_variable_access_type(static_cast<const VariableAccess*>(expr));
                
            case ASTNode::NodeType::FunctionCallExpr:
                return infer_function_call_type(static_cast<const FunctionCall*>(expr));
                
            case ASTNode::NodeType::BinaryOpExpr:
                return infer_binary_op_type(static_cast<const BinaryOp*>(expr));
                
            case ASTNode::NodeType::UnaryOpExpr:
                // Handle TYPE(expr) macro: statically infer type and replace with StringLiteral node
                {
                    const UnaryOp* unary = static_cast<const UnaryOp*>(expr);
                    if (unary->op == UnaryOp::Operator::TypeAsString) {
                        VarType inferred_type = infer_expression_type(unary->operand.get());
                        std::string type_str = vartype_to_string(inferred_type);

                        // Replace the TYPE(expr) node with a StringLiteral node in the AST
                        // This is a semantic transformation: the caller should replace the node,
                        // but for type inference, we return STRING.
                        // If you want to actually replace the node, do it in the AST transformation pass.
                        // For now, just return STRING type.
                        // Optionally, you could log or mark for replacement.
                        return VarType::STRING;
                    }
                    return infer_unary_op_type(unary);
                }
                
            case ASTNode::NodeType::ListExpr:
            case ASTNode::NodeType::VecInitializerExpr:
                return infer_collection_type(expr);
                
            case ASTNode::NodeType::MemberAccessExpr:
            case ASTNode::NodeType::VectorAccessExpr:
            case ASTNode::NodeType::CharIndirectionExpr:
            case ASTNode::NodeType::FloatVectorIndirectionExpr:
                return infer_access_type(expr);
                
            case ASTNode::NodeType::VecAllocationExpr:
            case ASTNode::NodeType::FVecAllocationExpr:
            case ASTNode::NodeType::PairsAllocationExpr:
            case ASTNode::NodeType::FPairsAllocationExpr:
            case ASTNode::NodeType::StringAllocationExpr:
                return infer_allocation_type(expr);
                
            case ASTNode::NodeType::ConditionalExpr:
                return infer_conditional_type(static_cast<const ConditionalExpression*>(expr));
                
            case ASTNode::NodeType::ValofExpr:
            case ASTNode::NodeType::FloatValofExpr:
                return infer_valof_type(expr);
                
            case ASTNode::NodeType::TableExpr:
                return VarType::POINTER_TO_TABLE;
                
            case ASTNode::NodeType::NewExpr:
                return VarType::POINTER_TO_OBJECT;
                
            case ASTNode::NodeType::PairExpr:
                return VarType::PAIR;
                
            case ASTNode::NodeType::FPairExpr:
                return VarType::FPAIR;
                
            case ASTNode::NodeType::QuadExpr:
                return VarType::QUAD;
                
            case ASTNode::NodeType::FQuadExpr:
                return VarType::FQUAD;
                
            case ASTNode::NodeType::OctExpr:
                return VarType::OCT;
                
            case ASTNode::NodeType::FOctExpr:
                return VarType::FOCT;
                
            case ASTNode::NodeType::LaneAccessExpr:
                {
                    // For lane access, return the element type of the vector
                    auto* lane_access = static_cast<const LaneAccessExpression*>(expr);
                    VarType vector_type = infer_expression_type(lane_access->vector_expr.get());
                    
                    if (vector_type == VarType::FPAIR || vector_type == VarType::FOCT) {
                        return VarType::FLOAT;
                    } else {
                        return VarType::INTEGER;
                    }
                }
                
            case ASTNode::NodeType::PairAccessExpr:
                return VarType::INTEGER;  // .first and .second return integers
                
            case ASTNode::NodeType::FPairAccessExpr:
                return VarType::FLOAT;  // .first and .second return floats for FPAIR
                
            case ASTNode::NodeType::QuadAccessExpr:
                return VarType::INTEGER;  // .first/.second/.third/.fourth return integers for QUAD
                
            case ASTNode::NodeType::FQuadAccessExpr:
                return VarType::FLOAT;  // .first/.second/.third/.fourth return floats for FQUAD
                
            default:
                if (trace_enabled_) {
                    std::cerr << "DEBUG: Unknown expression type in infer_expression_type: " 
                              << static_cast<int>(expr->getType()) << std::endl;
                }
                return VarType::UNKNOWN;
        }
    } catch (const std::out_of_range& e) {
        std::cerr << "ERROR: map::at key not found in infer_expression_type!" << std::endl;
        std::cerr << "  Expression type: " << static_cast<int>(expr->getType()) << std::endl;
        std::cerr << "  Current function scope: '" << current_function_scope_ << "'" << std::endl;
        std::cerr << "  Current class name: '" << current_class_name_ << "'" << std::endl;
        std::cerr << "  Exception details: " << e.what() << std::endl;
        throw;
    } catch (const std::invalid_argument& e) {
        std::cerr << "ERROR: std::invalid_argument in infer_expression_type!" << std::endl;
        std::cerr << "  Expression type: " << static_cast<int>(expr->getType()) << std::endl;
        std::cerr << "  Current function scope: '" << current_function_scope_ << "'" << std::endl;
        std::cerr << "  Exception message: " << e.what() << std::endl;
        if (std::string(e.what()).find("Register string cannot be empty") != std::string::npos) {
            std::cerr << "  *** THIS IS THE REGISTER STRING ERROR! ***" << std::endl;
            std::cerr << "  *** Error occurred during TYPE ANALYSIS, not code generation! ***" << std::endl;
        }
        throw;
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception in infer_expression_type!" << std::endl;
        std::cerr << "  Expression type: " << static_cast<int>(expr->getType()) << std::endl;
        std::cerr << "  Current function scope: '" << current_function_scope_ << "'" << std::endl;
        std::cerr << "  Exception: " << e.what() << std::endl;
        if (std::string(e.what()).find("Register string cannot be empty") != std::string::npos) {
            std::cerr << "  *** THIS IS THE REGISTER STRING ERROR! ***" << std::endl;
            std::cerr << "  *** Error occurred during TYPE ANALYSIS, not code generation! ***" << std::endl;
        }
        throw;
    }
}

VarType ASTAnalyzer::infer_literal_type(const Expression* expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_literal_type called" << std::endl;
    }
    
    if (auto* lit = dynamic_cast<const NumberLiteral*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: NumberLiteral dynamic_cast succeeded" << std::endl;
            std::cerr << "DEBUG: literal_type = " << static_cast<int>(lit->literal_type) << std::endl;
        }
        return (lit->literal_type == NumberLiteral::LiteralType::Float) ? VarType::FLOAT : VarType::INTEGER;
    }
    
    if (dynamic_cast<const StringLiteral*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: StringLiteral detected" << std::endl;
        }
        return VarType::POINTER_TO_STRING;
    }
    
    if (dynamic_cast<const NullLiteral*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: NullLiteral detected" << std::endl;
        }
        return VarType::INTEGER;
    }
    
    if (dynamic_cast<const BooleanLiteral*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: BooleanLiteral detected" << std::endl;
        }
        return VarType::INTEGER;
    }
    
    if (dynamic_cast<const CharLiteral*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: CharLiteral detected" << std::endl;
        }
        return VarType::INTEGER;
    }
    
    return VarType::UNKNOWN;
}

VarType ASTAnalyzer::infer_variable_access_type(const VariableAccess* var_access) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_variable_access_type for '" << var_access->name << "'" << std::endl;
    }
    
    if (!symbol_table_) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: No symbol table available" << std::endl;
        }
        return VarType::INTEGER;
    }

    // Try to get variable type from symbol table
    VarType var_type = get_variable_type(current_function_scope_, var_access->name);
    if (var_type != VarType::UNKNOWN) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: Found variable '" << var_access->name << "' with type " 
                      << static_cast<int>(var_type) << std::endl;
        }
        return var_type;
    }
    
    // Check if it's a global variable or manifest using lookup
    Symbol symbol;
    if (symbol_table_->lookup(var_access->name, symbol)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: Found symbol '" << var_access->name << "' with type " 
                      << static_cast<int>(symbol.type) << std::endl;
        }
        return symbol.type;
    }
    
    if (trace_enabled_) {
        std::cerr << "DEBUG: Variable '" << var_access->name << "' not found, defaulting to INTEGER" << std::endl;
    }
    return VarType::INTEGER;
}

VarType ASTAnalyzer::infer_function_call_type(const FunctionCall* func_call) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_function_call_type" << std::endl;
    }
    
    // Extract function name from function_expr (should be a VariableAccess)
    std::string func_name;
    if (auto* var_access = dynamic_cast<const VariableAccess*>(func_call->function_expr.get())) {
        func_name = var_access->name;
        if (trace_enabled_) {
            std::cerr << "DEBUG: Function name: '" << func_name << "'" << std::endl;
        }
    } else {
        if (trace_enabled_) {
            std::cerr << "DEBUG: Function call is not a simple variable access" << std::endl;
        }
        return VarType::INTEGER; // Default for complex function calls
    }
    
    // Check if it's a local function with known return type
    auto it = function_return_types_.find(func_name);
    if (it != function_return_types_.end()) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: Found local function '" << func_name << "' with return type " 
                      << static_cast<int>(it->second) << std::endl;
        }
        return it->second;
    }
    
    // Handle MAX and MIN - generic: return same type as arguments
    if (func_name == "MAX" || func_name == "MIN") {
        if (func_call->arguments.size() >= 2) {
            VarType arg_type = infer_expression_type(func_call->arguments[0].get());
            if (trace_enabled_) {
                std::cerr << "DEBUG: " << func_name << "() inferred return type: " 
                          << static_cast<int>(arg_type) << " (same as argument type)" << std::endl;
            }
            return arg_type; // Generic: MAX/MIN return same type as input
        }
        return VarType::UNKNOWN;
    }
    
    // Handle SUM - generic: returns element type of vector arguments
    if (func_name == "SUM") {
        if (func_call->arguments.size() >= 2) {
            VarType arg_type = infer_expression_type(func_call->arguments[0].get());
            
            // If arguments are PAIRS vectors, return PAIR (single element)
            if (arg_type == VarType::PAIRS || 
                (static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::PAIRS)) != 0) {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: SUM() inferred return type: PAIR (element of PAIRS)" << std::endl;
                }
                return VarType::PAIR;
            }
            
            // If arguments are FPAIRS vectors, return FPAIR (single element)
            if (arg_type == VarType::FPAIRS || 
                (static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::FPAIRS)) != 0) {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: SUM() inferred return type: FPAIR (element of FPAIRS)" << std::endl;
                }
                return VarType::FPAIR;
            }
            // If arguments are VEC OF INTEGER, return INTEGER
            else if ((static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::VEC)) != 0 &&
                     (static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::INTEGER)) != 0) {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: SUM() inferred return type: INTEGER (element of VEC OF INTEGER)" << std::endl;
                }
                return VarType::INTEGER;
            }
            // If arguments are VEC OF FLOAT, return FLOAT
            else if ((static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::VEC)) != 0 &&
                     (static_cast<int64_t>(arg_type) & static_cast<int64_t>(VarType::FLOAT)) != 0) {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: SUM() inferred return type: FLOAT (element of VEC OF FLOAT)" << std::endl;
                }
                return VarType::FLOAT;
            }
            // For scalar types, return the same type
            else {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: SUM() inferred return type: " << static_cast<int>(arg_type) 
                              << " (same as scalar argument)" << std::endl;
                }
                return arg_type;
            }
        }
        return VarType::UNKNOWN;
    }

    // Handle special runtime functions
    if (func_name == "CONCAT" || func_name == "BCPL_CONCAT_LISTS") {
        if (func_call->arguments.size() == 2) {
            VarType list1_type = infer_expression_type(func_call->arguments[0].get());
            VarType list2_type = infer_expression_type(func_call->arguments[1].get());
            if (list1_type == list2_type) {
                return list1_type;
            }
        }
        return VarType::POINTER_TO_ANY_LIST;
    }
    
    // Handle modifying list functions
    if (func_name == "APND" || func_name == "LPND" || func_name == "SPND" || func_name == "FPND") {
        if (!func_call->arguments.empty()) {
            VarType list_arg_type = infer_expression_type(func_call->arguments[0].get());
            if (is_const_list_type(list_arg_type)) {
                std::cerr << "Semantic Error: Cannot use modifying function '"
                          << func_name
                          << "' on a read-only MANIFESTLIST." << std::endl;
            }
        }
        return VarType::POINTER_TO_ANY_LIST;
    }
    
    // Check runtime functions
    if (RuntimeManager::instance().is_function_registered(func_name)) {
        const RuntimeFunction& runtime_func = RuntimeManager::instance().get_function(func_name);
        
        if (trace_enabled_) {
            std::cerr << "DEBUG: Runtime function '" << func_name << "' found" << std::endl;
            std::cerr << "DEBUG: runtime_func.return_type = " << static_cast<int>(runtime_func.return_type) << std::endl;
        }
        
        // Use the registered return type if it's not UNKNOWN, otherwise fall back to legacy logic
        if (runtime_func.return_type != VarType::UNKNOWN) {
            if (trace_enabled_) {
                std::cerr << "DEBUG: Returning registered return type: " << static_cast<int>(runtime_func.return_type) << std::endl;
            }
            return runtime_func.return_type;
        }
        
        if (trace_enabled_) {
            std::cerr << "DEBUG: runtime_func.return_type == UNKNOWN, falling back to legacy logic" << std::endl;
        }
        
        // Legacy fallback for functions not yet updated with return types
        if (runtime_func.type == FunctionType::FLOAT) {
            return VarType::FLOAT;
        }
        
        // Handle specific runtime function return types (legacy fallback)
        if (func_name == "GETVEC") return VarType::POINTER_TO_INT_VEC;
        if (func_name == "FGETVEC") return VarType::POINTER_TO_FLOAT_VEC;
        if (func_name == "STRLEN") return VarType::INTEGER;
        if (func_name == "PACKSTRING" || func_name == "UNPACKSTRING") return VarType::POINTER_TO_STRING;
        if (func_name == "SLURP") return VarType::POINTER_TO_STRING;
        if (func_name.find("BCPL_LIST") != std::string::npos) {
            if (func_name == "BCPL_LIST_GET_HEAD_AS_INT" || func_name == "BCPL_LIST_GET_NTH") return VarType::INTEGER;
            if (func_name == "BCPL_LIST_GET_HEAD_AS_FLOAT") return VarType::FLOAT;
            if (func_name == "BCPL_LIST_GET_TAIL" || func_name == "BCPL_LIST_GET_REST") return VarType::POINTER_TO_ANY_LIST;
            if (func_name == "BCPL_LIST_CREATE_EMPTY") return VarType::POINTER_TO_ANY_LIST;
        }
        
        return VarType::INTEGER; // Default for runtime functions
    }
    
    // Default to INTEGER for unknown functions
    if (trace_enabled_) {
        std::cerr << "DEBUG: Unknown function '" << func_name << "', defaulting to INTEGER" << std::endl;
    }
    return VarType::INTEGER;
}

VarType ASTAnalyzer::infer_binary_op_type(const BinaryOp* bin_op) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_binary_op_type" << std::endl;
    }
    
    VarType left_type = infer_expression_type(bin_op->left.get());
    VarType right_type = infer_expression_type(bin_op->right.get());
    
    if (trace_enabled_) {
        std::cerr << "DEBUG: Binary op - left type: " << static_cast<int>(left_type) 
                  << ", right type: " << static_cast<int>(right_type) << std::endl;
    }
    
    // Handle equality comparison operators with type validation
    if (bin_op->op == BinaryOp::Operator::Equal ||
        bin_op->op == BinaryOp::Operator::NotEqual) {
        
        // For PAIR types, both operands must be PAIR
        if (left_type == VarType::PAIR && right_type == VarType::PAIR) {
            return VarType::INTEGER;
        }
        
        // For QUAD types, both operands must be QUAD
        if (left_type == VarType::QUAD && right_type == VarType::QUAD) {
            return VarType::INTEGER;
        }
        
        // For FQUAD types, both operands must be FQUAD
        if (left_type == VarType::FQUAD && right_type == VarType::FQUAD) {
            return VarType::INTEGER;
        }
        
        // For scalar types, allow standard comparisons
        if ((left_type == VarType::INTEGER || left_type == VarType::FLOAT) &&
            (right_type == VarType::INTEGER || right_type == VarType::FLOAT)) {
            return VarType::INTEGER;
        }
        
        // For other compatible types (STRING, etc.)
        if (left_type == right_type) {
            return VarType::INTEGER;
        }
        
        // Incompatible types for equality comparison
        throw std::runtime_error("Cannot compare " + vartype_to_string(left_type) + 
                                " with " + vartype_to_string(right_type) + " using equality operators");
    }
    
    // Other comparison operators (ordering) - don't allow PAIR types
    if (bin_op->op == BinaryOp::Operator::Less ||
        bin_op->op == BinaryOp::Operator::LessEqual ||
        bin_op->op == BinaryOp::Operator::Greater ||
        bin_op->op == BinaryOp::Operator::GreaterEqual) {
        
        // PAIR, QUAD, and FQUAD types don't support ordering comparisons
        if (left_type == VarType::PAIR || right_type == VarType::PAIR || 
            left_type == VarType::QUAD || right_type == VarType::QUAD ||
            left_type == VarType::FQUAD || right_type == VarType::FQUAD) {
            throw std::runtime_error("PAIR, QUAD, and FQUAD types do not support ordering comparisons (<, <=, >, >=)");
        }
        
        return VarType::INTEGER;
    }
    
    // Logical operators return INTEGER
    if (bin_op->op == BinaryOp::Operator::LogicalAnd ||
        bin_op->op == BinaryOp::Operator::LogicalOr) {
        return VarType::INTEGER;
    }
    
    // For arithmetic operations with PAIR types: PAIR OP PAIR = PAIR
    if (left_type == VarType::PAIR && right_type == VarType::PAIR) {
        // Only allow arithmetic operations on PAIRs
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::PAIR;
        }
    }
    
    // For arithmetic operations with QUAD types: QUAD OP QUAD = QUAD
    if (left_type == VarType::QUAD && right_type == VarType::QUAD) {
        // Only allow arithmetic operations on QUADs
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::QUAD;
        }
    }
    
    // For arithmetic operations with FPAIR types: FPAIR OP FPAIR = FPAIR
    if (left_type == VarType::FPAIR && right_type == VarType::FPAIR) {
        // Only allow arithmetic operations on FPAIRs
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FPAIR;
        }
    }
    
    // For arithmetic operations with FQUAD types: FQUAD OP FQUAD = FQUAD
    if (left_type == VarType::FQUAD && right_type == VarType::FQUAD) {
        // Only allow arithmetic operations on FQUADs
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FQUAD;
        }
    }
    
    // For scalar-PAIR operations: PAIR OP INTEGER = PAIR, INTEGER OP PAIR = PAIR
    if ((left_type == VarType::PAIR && right_type == VarType::INTEGER) ||
        (left_type == VarType::INTEGER && right_type == VarType::PAIR)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::PAIR;
        }
    }
    
    // For scalar-QUAD operations: QUAD OP INTEGER = QUAD, INTEGER OP QUAD = QUAD
    if ((left_type == VarType::QUAD && right_type == VarType::INTEGER) ||
        (left_type == VarType::INTEGER && right_type == VarType::QUAD)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::QUAD;
        }
    }
    
    // For scalar-FPAIR operations: FPAIR OP FLOAT = FPAIR, FLOAT OP FPAIR = FPAIR
    if ((left_type == VarType::FPAIR && right_type == VarType::FLOAT) ||
        (left_type == VarType::FLOAT && right_type == VarType::FPAIR)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FPAIR;
        }
    }
    
    // For scalar-FQUAD operations: FQUAD OP FLOAT = FQUAD, FLOAT OP FQUAD = FQUAD
    if ((left_type == VarType::FQUAD && right_type == VarType::FLOAT) ||
        (left_type == VarType::FLOAT && right_type == VarType::FQUAD)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FQUAD;
        }
    }
    
    // For scalar-FQUAD operations: FQUAD OP INTEGER = FQUAD, INTEGER OP FQUAD = FQUAD
    if ((left_type == VarType::FQUAD && right_type == VarType::INTEGER) ||
        (left_type == VarType::INTEGER && right_type == VarType::FQUAD)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FQUAD;
        }
    }

    // For mixed scalar-PAIR operations: PAIR OP FLOAT = FPAIR, FLOAT OP PAIR = FPAIR  
    if ((left_type == VarType::PAIR && right_type == VarType::FLOAT) ||
        (left_type == VarType::FLOAT && right_type == VarType::PAIR)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FPAIR;  // Result is FPAIR when mixing PAIR with FLOAT
        }
    }
    
    // For mixed scalar-FPAIR operations: FPAIR OP INTEGER = FPAIR, INTEGER OP FPAIR = FPAIR
    if ((left_type == VarType::FPAIR && right_type == VarType::INTEGER) ||
        (left_type == VarType::INTEGER && right_type == VarType::FPAIR)) {
        if (bin_op->op == BinaryOp::Operator::Add ||
            bin_op->op == BinaryOp::Operator::Subtract ||
            bin_op->op == BinaryOp::Operator::Multiply ||
            bin_op->op == BinaryOp::Operator::Divide) {
            return VarType::FPAIR;  // Result is FPAIR when mixing FPAIR with INTEGER
        }
    }
    
    // For arithmetic operations, if either operand is FLOAT, result is FLOAT
    if (left_type == VarType::FLOAT || right_type == VarType::FLOAT) {
        return VarType::FLOAT;
    }
    
    // Otherwise, result is INTEGER
    return VarType::INTEGER;
}

VarType ASTAnalyzer::infer_unary_op_type(const UnaryOp* un_op) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_unary_op_type" << std::endl;
    }
    
    VarType operand_type = infer_expression_type(un_op->operand.get());
    
    switch (un_op->op) {
        case UnaryOp::Operator::AddressOf:
            if (operand_type == VarType::FLOAT) return VarType::POINTER_TO_FLOAT;
            if (operand_type == VarType::INTEGER) return VarType::POINTER_TO_INT;
            return VarType::INTEGER;
            
        case UnaryOp::Operator::Indirection:
            if (operand_type == VarType::POINTER_TO_FLOAT) return VarType::FLOAT;
            if (operand_type == VarType::POINTER_TO_INT) return VarType::INTEGER;
            if (operand_type == VarType::POINTER_TO_FLOAT_VEC) return VarType::FLOAT;
            return VarType::INTEGER;
            
        case UnaryOp::Operator::HeadOf:
            // HD returns the element type of the list
            if (operand_type == VarType::POINTER_TO_INT_LIST || operand_type == VarType::POINTER_TO_LIST_NODE) {
                return VarType::INTEGER;
            }
            if (operand_type == VarType::POINTER_TO_FLOAT_LIST) {
                return VarType::FLOAT;
            }
            if (operand_type == VarType::POINTER_TO_ANY_LIST) {
                return VarType::INTEGER; // Default to INTEGER for generic lists
            }
            return VarType::UNKNOWN;
            
        case UnaryOp::Operator::TailOf:
        case UnaryOp::Operator::TailOfNonDestructive:
            // TL returns the same list type as its operand
            if (operand_type == VarType::POINTER_TO_INT_LIST ||
                operand_type == VarType::POINTER_TO_FLOAT_LIST ||
                operand_type == VarType::POINTER_TO_ANY_LIST) {
                return operand_type;
            }
            return VarType::UNKNOWN;
            
        case UnaryOp::Operator::LengthOf:
            // LEN always returns INTEGER
            if (operand_type == VarType::POINTER_TO_INT_VEC ||
                operand_type == VarType::POINTER_TO_FLOAT_VEC ||
                operand_type == VarType::POINTER_TO_STRING ||
                operand_type == VarType::POINTER_TO_TABLE ||
                operand_type == VarType::POINTER_TO_INT_LIST ||
                operand_type == VarType::POINTER_TO_FLOAT_LIST ||
                operand_type == VarType::POINTER_TO_ANY_LIST) {
                return VarType::INTEGER;
            }
            return VarType::INTEGER; // Default
            
        case UnaryOp::Operator::FloatConvert:
            return VarType::FLOAT;
            
        case UnaryOp::Operator::Negate:
            return operand_type; // Preserve the operand type
            
        case UnaryOp::Operator::LogicalNot:
        case UnaryOp::Operator::BitwiseNot:
            return VarType::INTEGER;
            
        default:
            return VarType::INTEGER;
    }
}

VarType ASTAnalyzer::infer_collection_type(const Expression* expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_collection_type" << std::endl;
    }
    
    if (auto* list_expr = dynamic_cast<const ListExpression*>(expr)) {
        if (trace_enabled_) {
            std::cerr << "DEBUG: ListExpression with " << list_expr->initializers.size() << " elements" << std::endl;
        }
        
        if (list_expr->initializers.empty()) {
            return VarType::POINTER_TO_INT_LIST; // Empty list defaults to integer list
        }
        
        // Infer type from first element
        VarType first_element_type = infer_expression_type(list_expr->initializers[0].get());
        if (trace_enabled_) {
            std::cerr << "DEBUG: First element type: " << static_cast<int>(first_element_type) << std::endl;
        }
        
        // Check if all elements have the same type
        bool all_match = true;
        for (size_t i = 1; i < list_expr->initializers.size(); ++i) {
            VarType element_type = infer_expression_type(list_expr->initializers[i].get());
            if (element_type != first_element_type) {
                if (trace_enabled_) {
                    std::cerr << "DEBUG: Element " << i << " has different type: " << static_cast<int>(element_type) << std::endl;
                }
                all_match = false;
                break;
            }
        }
        
        if (all_match) {
            VarType inferred_type;
            if (first_element_type == VarType::FLOAT) {
                inferred_type = VarType::POINTER_TO_FLOAT_LIST;
            } else if (first_element_type == VarType::STRING || first_element_type == VarType::POINTER_TO_STRING) {
                inferred_type = VarType::POINTER_TO_STRING_LIST;
            } else {
                inferred_type = VarType::POINTER_TO_INT_LIST;
            }
            if (trace_enabled_) {
                std::cerr << "DEBUG: infer_collection_type inferred list type: " << static_cast<int>(inferred_type) << std::endl;
            }
            return inferred_type;
        } else {
            if (trace_enabled_) {
                std::cerr << "DEBUG: infer_collection_type inferred list type: POINTER_TO_ANY_LIST (mixed types)" << std::endl;
            }
            return VarType::POINTER_TO_ANY_LIST; // Mixed types
        }
    }
    
    if (auto* vec_init = dynamic_cast<const VecInitializerExpression*>(expr)) {
        if (vec_init->initializers.empty()) {
            return VarType::POINTER_TO_INT_VEC; // Empty vector defaults to integer vector
        }
        
        // Infer type from first element
        VarType first_element_type = infer_expression_type(vec_init->initializers[0].get());
        
        // Check if all elements have the same type
        for (size_t i = 1; i < vec_init->initializers.size(); ++i) {
            if (infer_expression_type(vec_init->initializers[i].get()) != first_element_type) {
                std::cerr << "Semantic Error: VEC initializers must all be of the same type (either all integer or all float)." << std::endl;
                return VarType::UNKNOWN;
            }
        }
        
        // Return appropriate vector type
        if (first_element_type == VarType::FLOAT) {
            return VarType::POINTER_TO_FLOAT_VEC;
        } else {
            return VarType::POINTER_TO_INT_VEC;
        }
    }
    
    return VarType::UNKNOWN;
}

VarType ASTAnalyzer::infer_access_type(const Expression* expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_access_type" << std::endl;
    }
    
    if (auto* member_access = dynamic_cast<const MemberAccessExpression*>(expr)) {
        return get_class_member_type(member_access);
    }
    
    if (auto* vector_access = dynamic_cast<const VectorAccess*>(expr)) {
        VarType vector_type = infer_expression_type(vector_access->vector_expr.get());
        if (vector_type == VarType::POINTER_TO_INT_VEC) return VarType::INTEGER;
        if (vector_type == VarType::POINTER_TO_FLOAT_VEC) return VarType::FLOAT;
        if (vector_type == VarType::POINTER_TO_STRING) return VarType::INTEGER; // Character access
        
        // Handle PAIRS vector access - return PAIR for each element
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::PAIRS)) != 0) {
            return VarType::PAIR;
        }
        
        // Handle FPAIRS vector access - return FPAIR for each element
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::FPAIRS)) != 0) {
            return VarType::FPAIR;
        }
        
        // Handle QUAD vector access - return QUAD for each element
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::QUAD)) != 0) {
            return VarType::QUAD;
        }
        
        // Handle OCT vector access - return OCT for each element
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::OCT)) != 0) {
            return VarType::OCT;
        }
        
        // Handle FOCT vector access - return FOCT for each element
        if ((static_cast<int64_t>(vector_type) & static_cast<int64_t>(VarType::FOCT)) != 0) {
            return VarType::FOCT;
        }
        
        return VarType::INTEGER; // Default
    }
    
    if (auto* char_indir = dynamic_cast<const CharIndirection*>(expr)) {
        return VarType::INTEGER; // Character indirection always returns INTEGER
    }
    
    if (auto* float_vec_indir = dynamic_cast<const FloatVectorIndirection*>(expr)) {
        return VarType::FLOAT; // Float vector indirection always returns FLOAT
    }
    
    return VarType::UNKNOWN;
}

VarType ASTAnalyzer::infer_allocation_type(const Expression* expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_allocation_type" << std::endl;
    }
    
    if (dynamic_cast<const VecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_INT_VEC;
    }
    
    if (dynamic_cast<const FVecAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_FLOAT_VEC;
    }
    
    if (dynamic_cast<const PairsAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_PAIRS;
    }
    
    if (dynamic_cast<const FPairsAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_FPAIRS;
    }
    
    if (dynamic_cast<const StringAllocationExpression*>(expr)) {
        return VarType::POINTER_TO_STRING;
    }
    
    return VarType::UNKNOWN;
}

VarType ASTAnalyzer::infer_conditional_type(const ConditionalExpression* cond_expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_conditional_type" << std::endl;
    }
    
    VarType true_type = infer_expression_type(cond_expr->true_expr.get());
    VarType false_type = infer_expression_type(cond_expr->false_expr.get());
    
    // If both branches have the same type, return that type
    if (true_type == false_type) {
        return true_type;
    }
    
    // If one is FLOAT and the other is INTEGER, promote to FLOAT
    if ((true_type == VarType::FLOAT && false_type == VarType::INTEGER) ||
        (true_type == VarType::INTEGER && false_type == VarType::FLOAT)) {
        return VarType::FLOAT;
    }
    
    // Otherwise, default to INTEGER
    return VarType::INTEGER;
}

VarType ASTAnalyzer::infer_valof_type(const Expression* expr) const {
    if (trace_enabled_) {
        std::cerr << "DEBUG: infer_valof_type" << std::endl;
    }
    
    if (dynamic_cast<const FloatValofExpression*>(expr)) {
        return VarType::FLOAT;
    }
    
    if (dynamic_cast<const ValofExpression*>(expr)) {
        return VarType::INTEGER;
    }
    
    return VarType::UNKNOWN;
}

// Helper function to check if a list type is const/read-only
bool ASTAnalyzer::is_const_list_type(VarType type) const {
    // Add logic to determine if a list type is const/read-only
    // This would depend on how const lists are represented in your type system
    return false; // Placeholder implementation
}
FunctionType ASTAnalyzer::get_runtime_function_type(const std::string& name) const {
    if (RuntimeManager::instance().is_function_registered(name)) {
        return RuntimeManager::instance().get_function(name).type;
    }
    return FunctionType::STANDARD;
}

// Helper to evaluate an expression to a constant integer value.
// Sets has_value to false if the expression is not a compile-time integer constant.
int64_t ASTAnalyzer::evaluate_constant_expression(Expression* expr, bool* has_value) const {
    if (!expr) {
        *has_value = false; // Null expression is not a constant
        return 0;
    }

    // Case 1: Number Literal
    if (auto* number_literal = dynamic_cast<NumberLiteral*>(expr)) {
        if (number_literal->literal_type == NumberLiteral::LiteralType::Integer) {
            *has_value = true;
            return number_literal->int_value;
        }
        // Float literals are not valid integer constants for CASE
        *has_value = false;
        return 0;
    }

    // Case 2: Variable Access (check for manifest constants)
    if (auto* var_access = dynamic_cast<VariableAccess*>(expr)) {
        // Check if this is a manifest constant by looking it up in the symbol table
        Symbol symbol;
        if (symbol_table_ && symbol_table_->lookup(var_access->name, symbol)) {
            if (symbol.kind == SymbolKind::MANIFEST) {
                *has_value = true;
                return symbol.location.absolute_value;
            }
        }
        // Not a manifest constant, so not a compile-time constant
        *has_value = false;
        return 0;
    }

    // Case 3 (Optional): Simple Constant Folding for Binary/Unary Operations
    // You can extend this to recursively evaluate simple arithmetic operations
    // if all their operands are also compile-time constants.
    // Example for addition:
    // --- Bitwise OR support for type constants ---
    if (auto* bin_op = dynamic_cast<BinaryOp*>(expr)) {
        bool left_has_value, right_has_value;
        int64_t left_val = evaluate_constant_expression(bin_op->left.get(), &left_has_value);
        int64_t right_val = evaluate_constant_expression(bin_op->right.get(), &right_has_value);
        if (left_has_value && right_has_value) {
            if (bin_op->op == BinaryOp::Operator::BitwiseOr) { // bitwise OR operator
                *has_value = true;
                return left_val | right_val;
            }
            if (bin_op->op == BinaryOp::Operator::LogicalOr) { // logical OR operator
                *has_value = true;
                return (left_val != 0 || right_val != 0) ? static_cast<int64_t>(-1) : static_cast<int64_t>(0);
            }
            // Add support for basic arithmetic operators for constant folding
            switch (bin_op->op) {
                case BinaryOp::Operator::Add:
                    *has_value = true;
                    return left_val + right_val;
                case BinaryOp::Operator::Subtract:
                    *has_value = true;
                    return left_val - right_val;
                case BinaryOp::Operator::Multiply:
                    *has_value = true;
                    return left_val * right_val;
                case BinaryOp::Operator::Divide:
                    if (right_val != 0) {
                        *has_value = true;
                        return left_val / right_val;
                    }
                    break;
                default: 
                    break;
            }
        }
    }

    // If it's none of the above (e.g., function call, complex expression, non-integer literal),
    // it's not a compile-time integer constant for a CASE.
    *has_value = false;
    return 0;
}

void ASTAnalyzer::visit(GlobalVariableDeclaration& node) {
    // This is a declaration in the "Global" scope.
    current_function_scope_ = "Global";

    // For each variable in this declaration (e.g., LET G, H = 1, 2)
    for (const auto& name : node.names) {
        // Correctly register this variable's defining scope as "Global".
        variable_definitions_[name] = "Global";
    }
}

bool ASTAnalyzer::is_local_float_function(const std::string& name) const {
    return local_float_function_names_.find(name) != local_float_function_names_.end();
}

void ASTAnalyzer::infer_parameter_types(const std::string& function_name, const std::vector<std::string>& parameters, ASTNode* body) {
    if (!body) return;

    if (trace_enabled_) {
        std::cout << "[ANALYZER] Starting parameter type inference for function: " << function_name << std::endl;
    }

    // For each parameter, analyze how it's used in the function body
    for (const std::string& param_name : parameters) {
        // Skip _this parameter - it's already handled
        if (param_name == "_this") continue;

        if (trace_enabled_) {
            std::cout << "[ANALYZER] Analyzing parameter: " << param_name << std::endl;
        }

        // --- FIX START ---
        // Check if a type for this parameter already exists from Pass 1 (SignatureAnalysisVisitor).
        // This is a defensive check to prevent overwriting parameter types set by the first pass.
        auto function_it = function_metrics_.find(function_name);
        if (function_it != function_metrics_.end()) {
            const auto& parameter_types = function_it->second.parameter_types;
            auto param_it = parameter_types.find(param_name);
            if (param_it != parameter_types.end()) {
                // A type is already set by Pass 1, so do nothing. Do not overwrite it.
                if (trace_enabled_) {
                    std::cout << "[ANALYZER] Parameter '" << param_name << "' already has type from Pass 1: " 
                             << static_cast<int>(param_it->second) << ". Preserving it." << std::endl;
                }
                continue;
            }
        }
        // --- FIX END ---

        VarType current_type = get_variable_type(function_name, param_name);
        if (current_type != VarType::UNKNOWN) {
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Parameter " << param_name << " already has type: " << static_cast<int>(current_type) << std::endl;
            }
            continue; // Already has a determined type
        }

        // First, check if the parameter is used at all
        bool is_used = uses_parameter(body, param_name);
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Parameter " << param_name << " is used: " << (is_used ? "YES" : "NO") << std::endl;
        }

        if (!is_used) {
            // Parameter is not used - mark as NOTUSED
            function_metrics_[function_name].variable_types[param_name] = VarType::NOTUSED;
            
            // Also update the symbol table
            if (symbol_table_) {
                symbol_table_->updateSymbolType(param_name, VarType::NOTUSED);
            }
            
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Parameter not used: " << param_name 
                         << " in " << function_name << " -> NOTUSED" << std::endl;
            }
            continue;
        }

        // Analyze usage patterns to infer type
        VarType inferred_type = analyze_parameter_usage(body, param_name, function_name);

        if (trace_enabled_) {
            std::cout << "[ANALYZER] Type inference result for " << param_name << ": " << static_cast<int>(inferred_type) << std::endl;
        }

        if (inferred_type != VarType::UNKNOWN) {
            // Update the parameter type in function metrics
            function_metrics_[function_name].variable_types[param_name] = inferred_type;
    
            // Update the function symbol's parameter types in the symbol table
            if (symbol_table_) {
                // Find the parameter index
                auto param_it = std::find(parameters.begin(), parameters.end(), param_name);
                if (param_it != parameters.end()) {
                    size_t param_index = std::distance(parameters.begin(), param_it);
                    symbol_table_->updateFunctionParameterType(function_name, param_index, inferred_type);
                }
                
                // Also update the parameter as a local variable
                symbol_table_->updateSymbolType(param_name, inferred_type);
            }
    
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Inferred parameter type: " << param_name 
                         << " in " << function_name << " -> " 
                         << static_cast<int>(inferred_type) << std::endl;
            }
        } else {
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Could not infer type for parameter: " << param_name 
                         << " in " << function_name << " - leaving as UNKNOWN" << std::endl;
            }
        }
    }
}

void ASTAnalyzer::set_parameter_type_safe(const std::string& function_name, const std::string& param_name, VarType new_type) {
    auto function_it = function_metrics_.find(function_name);
    if (function_it == function_metrics_.end()) {
        // Function doesn't exist yet, create it and set the type
        function_metrics_[function_name].parameter_types[param_name] = new_type;
        function_metrics_[function_name].variable_types[param_name] = new_type;
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Created new function metrics and set parameter type: " 
                     << param_name << " in " << function_name << " to " << static_cast<int>(new_type) << std::endl;
        }
        return;
    }

    // Check if parameter already has a type from SignatureAnalysisVisitor
    const auto& parameter_types = function_it->second.parameter_types;
    auto param_it = parameter_types.find(param_name);
    
    if (param_it != parameter_types.end() && param_it->second != VarType::UNKNOWN) {
        // Parameter already has a valid type from Pass 1 (SignatureAnalysisVisitor), preserve it
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Preserving existing parameter type from Pass 1: " 
                     << param_name << " in " << function_name << " (type: " << static_cast<int>(param_it->second) 
                     << ") - ignoring new type: " << static_cast<int>(new_type) << std::endl;
        }
        return;
    }

    // Parameter doesn't exist or has UNKNOWN type, safe to set
    function_metrics_[function_name].parameter_types[param_name] = new_type;
    function_metrics_[function_name].variable_types[param_name] = new_type;
    if (trace_enabled_) {
        std::cout << "[ANALYZER] Set parameter type: " << param_name << " in " << function_name 
                 << " to " << static_cast<int>(new_type) << std::endl;
    }
}

VarType ASTAnalyzer::analyze_parameter_usage(ASTNode* node, const std::string& param_name, const std::string& function_name) {
    if (!node) return VarType::UNKNOWN;

    if (trace_enabled_) {
        std::cout << "[ANALYZER] analyze_parameter_usage: checking node type " << static_cast<int>(node->getType()) << " for param " << param_name << std::endl;
        
        // Add specific node type identification
        if (dynamic_cast<BinaryOp*>(node)) {
            std::cout << "[ANALYZER] Node is BinaryOp" << std::endl;
        } else if (dynamic_cast<ResultisStatement*>(node)) {
            std::cout << "[ANALYZER] Node is ResultisStatement" << std::endl;
        } else if (dynamic_cast<ValofExpression*>(node)) {
            std::cout << "[ANALYZER] Node is ValofExpression" << std::endl;
        } else if (dynamic_cast<FloatValofExpression*>(node)) {
            std::cout << "[ANALYZER] Node is FloatValofExpression" << std::endl;
        } else if (dynamic_cast<CompoundStatement*>(node)) {
            std::cout << "[ANALYZER] Node is CompoundStatement" << std::endl;
        } else if (dynamic_cast<BlockStatement*>(node)) {
            std::cout << "[ANALYZER] Node is BlockStatement" << std::endl;
        } else if (dynamic_cast<RetainStatement*>(node)) {
            std::cout << "[ANALYZER] Node is RetainStatement (type 58)" << std::endl;
        } else {
            std::cout << "[ANALYZER] Node type " << static_cast<int>(node->getType()) << " not specifically handled in inference" << std::endl;
        }
    }

    // Check if this node directly uses the parameter
    if (auto* var_access = dynamic_cast<VariableAccess*>(node)) {
        if (var_access->name == param_name) {
            if (trace_enabled_) {
                std::cout << "[ANALYZER] Found direct usage of parameter: " << param_name << std::endl;
            }
            
            // NEW: Check if we can infer type from function return type context
            Symbol func_symbol;
            if (symbol_table_ && symbol_table_->lookup(function_name, func_symbol)) {
                if (trace_enabled_) {
                    std::cout << "[ANALYZER] Function " << function_name << " return type: " << static_cast<int>(func_symbol.type) << std::endl;
                }
                // If this is a direct parameter return (RESULTIS x), use function return type
                if (func_symbol.type == VarType::FLOAT || func_symbol.type == VarType::INTEGER) {
                    if (trace_enabled_) {
                        std::cout << "[ANALYZER] Inferring parameter type from function return type: " << static_cast<int>(func_symbol.type) << std::endl;
                    }
                    return func_symbol.type;
                }
            }
            
            // Fallback: Found usage, but we need context to infer type
            return VarType::UNKNOWN; // Will be inferred from parent context
        }
    }

    // Check binary operations - this is key for IntFunc(a, b) with "a + b"
    if (auto* bin_op = dynamic_cast<BinaryOp*>(node)) {
        bool left_uses_param = uses_parameter(bin_op->left.get(), param_name);
        bool right_uses_param = uses_parameter(bin_op->right.get(), param_name);

        if (trace_enabled_) {
            std::cout << "[ANALYZER] Binary op: left_uses=" << left_uses_param << ", right_uses=" << right_uses_param << std::endl;
        }

        if (left_uses_param || right_uses_param) {
            // Parameter is used in arithmetic operation
            if (bin_op->op == BinaryOp::Operator::Add ||
                bin_op->op == BinaryOp::Operator::Subtract ||
                bin_op->op == BinaryOp::Operator::Multiply ||
                bin_op->op == BinaryOp::Operator::Divide ||
                bin_op->op == BinaryOp::Operator::Remainder) {
        
                // First priority: Check the function's return type for context
                Symbol func_symbol;
                if (symbol_table_ && symbol_table_->lookup(function_name, func_symbol)) {
                    if (trace_enabled_) {
                        std::cout << "[ANALYZER] Function " << function_name << " return type: " << static_cast<int>(func_symbol.type) << std::endl;
                    }
                    if (func_symbol.type == VarType::FLOAT) {
                        // Float function - parameters in arithmetic should be float
                        if (trace_enabled_) {
                            std::cout << "[ANALYZER] Inferring FLOAT type from function return type" << std::endl;
                        }
                        return VarType::FLOAT;
                    } else if (func_symbol.type == VarType::INTEGER) {
                        // Integer function - parameters in arithmetic should be integer
                        if (trace_enabled_) {
                            std::cout << "[ANALYZER] Inferring INTEGER type from function return type" << std::endl;
                        }
                        return VarType::INTEGER;
                    }
                }
        
                // Second priority: Check if this is in an integer or float context
                VarType other_operand_type = VarType::UNKNOWN;
                if (left_uses_param && bin_op->right) {
                    other_operand_type = infer_expression_type(bin_op->right.get());
                } else if (right_uses_param && bin_op->left) {
                    other_operand_type = infer_expression_type(bin_op->left.get());
                }
        
                // If the other operand is a literal or known type, use that
                if (other_operand_type == VarType::INTEGER) {
                    return VarType::INTEGER;
                } else if (other_operand_type == VarType::FLOAT) {
                    return VarType::FLOAT;
                }
            }
        }
    }

    // Check function calls where parameter is used as argument
    if (auto* func_call = dynamic_cast<FunctionCall*>(node)) {
        for (size_t i = 0; i < func_call->arguments.size(); ++i) {
            if (uses_parameter(func_call->arguments[i].get(), param_name)) {
                // Parameter is being passed to another function
                // We could potentially infer type from the called function's signature
                // For now, let's be conservative
                return VarType::UNKNOWN;
            }
        }
    }

    // Recursively check child nodes
    VarType inferred_type = VarType::UNKNOWN;

    // Visit all child nodes (this is a simplified traversal)
    if (auto* compound = dynamic_cast<CompoundStatement*>(node)) {
        for (auto& stmt : compound->statements) {
            VarType child_result = analyze_parameter_usage(stmt.get(), param_name, function_name);
            if (child_result != VarType::UNKNOWN) {
                inferred_type = child_result;
                break; // Use first concrete inference
            }
        }
    } else if (auto* valof = dynamic_cast<ValofExpression*>(node)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Recursing into ValofExpression body" << std::endl;
        }
        inferred_type = analyze_parameter_usage(valof->body.get(), param_name, function_name);
    } else if (auto* float_valof = dynamic_cast<FloatValofExpression*>(node)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Recursing into FloatValofExpression body" << std::endl;
        }
        inferred_type = analyze_parameter_usage(float_valof->body.get(), param_name, function_name);
    } else if (auto* resultis = dynamic_cast<ResultisStatement*>(node)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Found RESULTIS, analyzing expression..." << std::endl;
            if (resultis->expression) {
                std::cout << "[ANALYZER] RESULTIS expression type: " << static_cast<int>(resultis->expression->getType()) << std::endl;
            } else {
                std::cout << "[ANALYZER] RESULTIS expression is NULL!" << std::endl;
            }
        }
        // Recursively analyze the RESULTIS expression to find parameter usage
        return analyze_parameter_usage(resultis->expression.get(), param_name, function_name);
    } else if (auto* block_stmt = dynamic_cast<BlockStatement*>(node)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Found BlockStatement, analyzing statements..." << std::endl;
        }
        for (const auto& stmt : block_stmt->statements) {
            VarType child_result = analyze_parameter_usage(stmt.get(), param_name, function_name);
            if (child_result != VarType::UNKNOWN) {
                return child_result;
            }
        }
    } else if (auto* assign_stmt = dynamic_cast<AssignmentStatement*>(node)) {
        if (trace_enabled_) {
            std::cout << "[ANALYZER] Found AssignmentStatement, analyzing RHS..." << std::endl;
        }
        // Check if the parameter is used in any RHS expression
        for (const auto& rhs_expr : assign_stmt->rhs) {
            if (rhs_expr && uses_parameter(rhs_expr.get(), param_name)) {
                // Analyze the RHS expression to infer parameter type
                VarType rhs_result = analyze_parameter_usage(rhs_expr.get(), param_name, function_name);
                if (rhs_result != VarType::UNKNOWN) {
                    return rhs_result;
                }
                
                // If we can't infer from the RHS directly, check the LHS type context
                if (!assign_stmt->lhs.empty() && assign_stmt->lhs[0]) {
                    if (auto* lhs_var = dynamic_cast<VariableAccess*>(assign_stmt->lhs[0].get())) {
                        // Check if the LHS variable has a known type that can give us context
                        VarType lhs_type = get_variable_type(function_name, lhs_var->name);
                        if (lhs_type == VarType::FLOAT) {
                            if (trace_enabled_) {
                                std::cout << "[ANALYZER] Inferring FLOAT from assignment to float variable" << std::endl;
                            }
                            return VarType::FLOAT;
                        } else if (lhs_type == VarType::INTEGER) {
                            if (trace_enabled_) {
                                std::cout << "[ANALYZER] Inferring INTEGER from assignment to integer variable" << std::endl;
                            }
                            return VarType::INTEGER;
                        }
                    }
                }
            }
        }
    }

    return inferred_type;
}

bool ASTAnalyzer::uses_parameter(ASTNode* node, const std::string& param_name) {
    if (!node) return false;

    if (auto* var_access = dynamic_cast<VariableAccess*>(node)) {
        return var_access->name == param_name;
    }

    if (auto* bin_op = dynamic_cast<BinaryOp*>(node)) {
        return uses_parameter(bin_op->left.get(), param_name) || 
               uses_parameter(bin_op->right.get(), param_name);
    }

    if (auto* un_op = dynamic_cast<UnaryOp*>(node)) {
        return uses_parameter(un_op->operand.get(), param_name);
    }

    if (auto* func_call = dynamic_cast<FunctionCall*>(node)) {
        for (const auto& arg : func_call->arguments) {
            if (uses_parameter(arg.get(), param_name)) {
                return true;
            }
        }
    }

    if (auto* assignment = dynamic_cast<AssignmentStatement*>(node)) {
        for (const auto& lhs_expr : assignment->lhs) {
            if (uses_parameter(lhs_expr.get(), param_name)) {
                return true;
            }
        }
        for (const auto& rhs_expr : assignment->rhs) {
            if (uses_parameter(rhs_expr.get(), param_name)) {
                return true;
            }
        }
        return false;
    }

    if (auto* compound = dynamic_cast<CompoundStatement*>(node)) {
        for (const auto& stmt : compound->statements) {
            if (uses_parameter(stmt.get(), param_name)) {
                return true;
            }
        }
        return false;
    }

    if (auto* valof = dynamic_cast<ValofExpression*>(node)) {
        return uses_parameter(valof->body.get(), param_name);
    }

    if (auto* float_valof = dynamic_cast<FloatValofExpression*>(node)) {
        return uses_parameter(float_valof->body.get(), param_name);
    }

    if (auto* resultis = dynamic_cast<ResultisStatement*>(node)) {
        return uses_parameter(resultis->expression.get(), param_name);
    }

    if (auto* if_stmt = dynamic_cast<IfStatement*>(node)) {
        return uses_parameter(if_stmt->condition.get(), param_name) ||
               uses_parameter(if_stmt->then_branch.get(), param_name);
    }

    if (auto* while_stmt = dynamic_cast<WhileStatement*>(node)) {
        return uses_parameter(while_stmt->condition.get(), param_name) ||
               uses_parameter(while_stmt->body.get(), param_name);
    }

    if (auto* for_stmt = dynamic_cast<ForStatement*>(node)) {
        return uses_parameter(for_stmt->start_expr.get(), param_name) ||
               uses_parameter(for_stmt->end_expr.get(), param_name) ||
               uses_parameter(for_stmt->body.get(), param_name);
    }

    if (auto* test_stmt = dynamic_cast<TestStatement*>(node)) {
        return uses_parameter(test_stmt->condition.get(), param_name);
    }

    if (auto* unless_stmt = dynamic_cast<UnlessStatement*>(node)) {
        return uses_parameter(unless_stmt->condition.get(), param_name) ||
               uses_parameter(unless_stmt->then_branch.get(), param_name);
    }

    if (auto* repeat_stmt = dynamic_cast<RepeatStatement*>(node)) {
        return uses_parameter(repeat_stmt->body.get(), param_name);
    }

    if (auto* until_stmt = dynamic_cast<UntilStatement*>(node)) {
        return uses_parameter(until_stmt->condition.get(), param_name) ||
               uses_parameter(until_stmt->body.get(), param_name);
    }

    if (auto* block_stmt = dynamic_cast<BlockStatement*>(node)) {
        for (const auto& stmt : block_stmt->statements) {
            if (uses_parameter(stmt.get(), param_name)) {
                return true;
            }
        }
        return false;
    }

    // Add more node types as needed
    return false;
}

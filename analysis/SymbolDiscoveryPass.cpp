#include "SymbolDiscoveryPass.h"
#include "../AST.h"
#include <cassert>
#include <iostream>
#include <utility>
#include <unordered_set>
#include <algorithm>

void SymbolDiscoveryPass::visit(SuperMethodAccessExpression& node) {
    // No symbol discovery action needed for this node.
}

SymbolDiscoveryPass::SymbolDiscoveryPass(bool enable_tracing)
    : symbol_table_(nullptr), class_table_(nullptr), enable_tracing_(enable_tracing) {}

void SymbolDiscoveryPass::build_into(Program& program, SymbolTable& symbol_table, ClassTable& class_table) {
    // In this pass, we only read the class table, not modify it
    // We're building the symbol table from scratch
    symbol_table_ = &symbol_table;
    class_table_ = &class_table; // Used for lookup only, not modification
    visit(program);
}

void SymbolDiscoveryPass::visit(Program& node) {
    trace("Entering global scope");

    // Process all declarations first to ensure forward references work
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }

    // Then process statements
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(LetDeclaration& node) {
    trace("Processing let declaration");

    for (size_t i = 0; i < node.names.size(); ++i) {
        const std::string& name = node.names[i];
        
        // Check if this is a function-like declaration (has VALOF/FVALOF initializer)
        bool is_function_like = false;
        if (i < node.initializers.size() && node.initializers[i]) {
            ExprPtr& initializer = node.initializers[i];
            is_function_like = (dynamic_cast<ValofExpression*>(initializer.get()) != nullptr ||
                               dynamic_cast<FloatValofExpression*>(initializer.get()) != nullptr);
        }
        
        if (is_function_like) {
            // This is a function declaration like: FLET FloatFunc(x,y) = VALOF {...}
            trace("Detected function-like declaration: " + name);
            
            VarType return_type = node.is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
            std::string type_source = node.is_float_declaration ? " (FLET function)" : " (LET function)";
            
            if (enable_tracing_) {
                std::cerr << "DEBUG SymbolDiscovery: Function '" << name 
                          << "' is_float_declaration=" << (node.is_float_declaration ? "true" : "false")
                          << " return_type=" << static_cast<int>(return_type) << std::endl;
            }
            
            Symbol new_symbol(
                name,
                SymbolKind::FUNCTION,
                return_type,
                symbol_table_->currentScopeLevel(),
                current_function_name_
            );
            
            // Extract parameters from VALOF expression if possible
            // For now, mark parameters as UNKNOWN - they'll be inferred later
            if (i < node.initializers.size() && node.initializers[i]) {
                // Parameters will be inferred by ASTAnalyzer
                trace("Function-like declaration will have parameters inferred later");
            }
            
            if (!symbol_table_->addSymbol(new_symbol)) {
                report_duplicate_symbol(name);
            } else {
                trace("Added function: " + name + " as " + vartype_to_string(return_type) + type_source);
            }
        } else {
            // Regular variable declaration
            VarType determined_type;
            std::string type_source_str;

            // DEBUG: Trace the declaration type detection
            if (enable_tracing_) {
                std::cerr << "DEBUG SymbolDiscovery: Variable '" << name 
                          << "' is_float_declaration=" << (node.is_float_declaration ? "true" : "false") << std::endl;
            }

            // Priority 1: Use the explicit 'AS' type annotation if present.
            if (node.explicit_type != VarType::UNKNOWN) {
                determined_type = node.explicit_type;
                type_source_str = " (from AS annotation)";
            } else if (node.is_float_declaration) {
                determined_type = VarType::FLOAT;
                type_source_str = " (from FLET)";
            } else {
                // Priority 2: Check if LET variable is initialized with a float literal
                bool has_float_initializer = false;
                if (i < node.initializers.size() && node.initializers[i]) {
                    if (enable_tracing_) {
                        std::cerr << "DEBUG SymbolDiscovery: Checking initializer for '" << name << "'" << std::endl;
                    }
                    
                    // Check if the initializer is a float literal
                    if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.initializers[i].get())) {
                        if (enable_tracing_) {
                            std::cerr << "DEBUG SymbolDiscovery: Found NumberLiteral for '" << name 
                                      << "', literal_type=" << static_cast<int>(num_lit->literal_type) << std::endl;
                        }
                        if (num_lit->literal_type == NumberLiteral::LiteralType::Float) {
                            has_float_initializer = true;
                            if (enable_tracing_) {
                                std::cerr << "DEBUG SymbolDiscovery: Detected float literal for '" << name << "'" << std::endl;
                            }
                        }
                    } else {
                        if (enable_tracing_) {
                            std::cerr << "DEBUG SymbolDiscovery: Initializer for '" << name 
                                      << "' is not a NumberLiteral (node type: " << static_cast<int>(node.initializers[i]->getType()) << ")" << std::endl;
                        }
                    }
                }
                
                if (has_float_initializer) {
                    determined_type = VarType::FLOAT;
                    type_source_str = " (inferred from float literal)";
                } else {
                    determined_type = VarType::INTEGER;
                    type_source_str = " (default INTEGER)";
                }
            }

            if (enable_tracing_) {
                std::cerr << "DEBUG SymbolDiscovery: Variable '" << name 
                          << "' determined_type=" << static_cast<int>(determined_type) << type_source_str << std::endl;
            }

            Symbol new_symbol(
                name,
                SymbolKind::LOCAL_VAR,
                determined_type,
                symbol_table_->currentScopeLevel(),
                current_function_name_
            );
            if (!symbol_table_->addSymbol(new_symbol)) {
                report_duplicate_symbol(name);
            } else {
                trace("Added let variable: " + name + " as " + vartype_to_string(determined_type) + type_source_str);
            }
        }
    }
}

void SymbolDiscoveryPass::visit(ManifestDeclaration& node) {
    trace("Processing manifest declaration: " + node.name);

    Symbol new_symbol(
        node.name,
        SymbolKind::MANIFEST,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        symbol_table_->setSymbolAbsoluteValue(node.name, node.value);
        trace("Added manifest constant: " + node.name + " = " + std::to_string(node.value));
    }
}

void SymbolDiscoveryPass::visit(StaticDeclaration& node) {
    trace("Processing static declaration: " + node.name);

    VarType type = VarType::INTEGER;
    if (node.is_float_declaration) {
        type = VarType::FLOAT;
    } else if (node.initializer && node.initializer->getType() == ASTNode::NodeType::NumberLit) {
        // Could add more sophisticated type inference here
        type = VarType::INTEGER;
    }

    Symbol new_symbol(
        node.name,
        SymbolKind::STATIC_VAR,
        type,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added static variable: " + node.name + " as " + vartype_to_string(type));
    }
}

void SymbolDiscoveryPass::visit(GlobalDeclaration& node) {
    trace("Processing global declaration");

    for (auto& global : node.globals) {
        const std::string& name = global.first;
        VarType type = VarType::INTEGER;

        Symbol new_symbol(
            name,
            SymbolKind::GLOBAL_VAR,
            type,
            symbol_table_->currentScopeLevel(),
            "Global"  // Global variables must always use "Global" as function context
        );
        if (!symbol_table_->addSymbol(new_symbol)) {
            report_duplicate_symbol(name);
        } else {
            trace("Added global variable: " + name + " as " + vartype_to_string(type));
        }
    }
}

void SymbolDiscoveryPass::visit(GlobalVariableDeclaration& node) {
    trace("Processing global variable declaration");

    for (size_t i = 0; i < node.names.size(); i++) {
        const std::string& name = node.names[i];
        VarType type = node.is_float_declaration ? VarType::FLOAT : VarType::INTEGER;

        Symbol new_symbol(
            name,
            SymbolKind::GLOBAL_VAR,
            type,
            symbol_table_->currentScopeLevel(),
            "Global"  // Global variables must always use "Global" as function context
        );
        if (!symbol_table_->addSymbol(new_symbol)) {
            report_duplicate_symbol(name);
        } else {
            trace("Added global variable: " + name + " as " + vartype_to_string(type));
        }
    }
}

void SymbolDiscoveryPass::visit(FunctionDeclaration& node) {
    trace("Processing function declaration: " + node.name);
    current_function_name_ = node.name; // Set the current function context

    SymbolKind kind = SymbolKind::FUNCTION;
    VarType return_type = VarType::INTEGER;
    
    // TEMPORARY FIX: Use naming convention to detect float functions
    // TODO: Fix parser to properly set is_float_function field
    bool is_float_function = node.is_float_function || 
                            node.name.find("Float") != std::string::npos ||
                            node.name.find("FLOAT") != std::string::npos;
    
    if (is_float_function) {
        return_type = VarType::FLOAT;
    }

    Symbol new_symbol(
        node.name,
        kind,
        return_type,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    
    // Always treat parameter types as unknown at this stage
    for (const std::string& param_name : node.parameters) {
        Symbol::ParameterInfo param_info;
        param_info.type = VarType::UNKNOWN;
        param_info.is_optional = false;
        new_symbol.parameters.push_back(param_info);
        trace("Added parameter '" + param_name + "' as " + vartype_to_string(param_info.type) + 
              " (parameter type will be determined at call sites)");
    }
    
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added function: " + node.name + " returns " + vartype_to_string(return_type) + 
              " with " + std::to_string(node.parameters.size()) + " parameters");
    }

    // Enter a new scope for the function body
    symbol_table_->enterScope();
    
    // Register function parameters as parameters
    for (const std::string& param_name : node.parameters) {
        Symbol param_symbol(
            param_name,
            SymbolKind::PARAMETER,
            VarType::UNKNOWN, // Will be inferred later
            symbol_table_->currentScopeLevel(),
            current_function_name_
        );
        if (!symbol_table_->addSymbol(param_symbol)) {
            report_duplicate_symbol(param_name);
        } else {
            trace("Added parameter variable: " + param_name + " in function " + current_function_name_);
        }
    }
    
    // Process the function body to register local variables
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the function scope
    symbol_table_->exitScope();
    current_function_name_ = ""; // Reset function context
}

void SymbolDiscoveryPass::visit(RoutineDeclaration& node) {
    trace("Processing routine declaration: " + node.name);
    current_function_name_ = node.name; // Set the current routine context

    Symbol new_symbol(
        node.name,
        SymbolKind::ROUTINE,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added routine: " + node.name);
    }

    // Enter a new scope for the routine body
    symbol_table_->enterScope();
    
    // Register routine parameters as parameters
    for (const std::string& param_name : node.parameters) {
        Symbol param_symbol(
            param_name,
            SymbolKind::PARAMETER,
            VarType::UNKNOWN, // Will be inferred later
            symbol_table_->currentScopeLevel(),
            current_function_name_
        );
        if (!symbol_table_->addSymbol(param_symbol)) {
            report_duplicate_symbol(param_name);
        } else {
            trace("Added parameter variable: " + param_name + " in routine " + current_function_name_);
        }
    }
    
    // Process the routine body to register local variables
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the routine scope
    symbol_table_->exitScope();
    current_function_name_ = ""; // Reset function context
}



void SymbolDiscoveryPass::visit(LabelDeclaration& node) {
    trace("Processing label declaration: " + node.name);

    Symbol new_symbol(
        node.name,
        SymbolKind::LABEL,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        symbol_table_->setSymbolAbsoluteValue(node.name, 0);
        trace("Added label: " + node.name);
    }
}

void SymbolDiscoveryPass::visit(BlockStatement& node) {
    trace("Entering block scope");
    symbol_table_->enterScope();

    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }

    symbol_table_->exitScope();
}

void SymbolDiscoveryPass::visit(ForStatement& node) {
    trace("Processing for statement with loop variable: " + node.loop_variable);

    Symbol new_symbol(
        node.loop_variable,
        SymbolKind::LOCAL_VAR,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.loop_variable);
    } else {
        trace("Added for-loop variable: " + node.loop_variable);
    }

    if (node.body) node.body->accept(*this);
}

void SymbolDiscoveryPass::visit(ForEachStatement& node) {
    trace("Processing foreach statement, loop variable: " + node.loop_variable_name);

    // Register the main loop variable (e.g., 'item' in FOREACH item IN list)
    Symbol loop_var_symbol(
        node.loop_variable_name,
        SymbolKind::LOCAL_VAR,
        VarType::UNKNOWN, // Type will be properly inferred later by ASTAnalyzer
        symbol_table_->currentScopeLevel(),
        current_function_name_
    );
    if (!symbol_table_->addSymbol(loop_var_symbol)) {
        report_duplicate_symbol(node.loop_variable_name);
    } else {
        trace("Added foreach loop variable: " + node.loop_variable_name);
    }

    // Register the type variable if it exists (e.g., 'T' in FOREACH T, V IN list)
    if (!node.type_variable_name.empty()) {
        Symbol type_var_symbol(
            node.type_variable_name,
            SymbolKind::LOCAL_VAR,
            VarType::INTEGER, // Type variable is always an integer tag
            symbol_table_->currentScopeLevel(),
            current_function_name_
        );
        if (!symbol_table_->addSymbol(type_var_symbol)) {
            report_duplicate_symbol(node.type_variable_name);
        } else {
            trace("Added foreach type variable: " + node.type_variable_name);
        }
    }

    // Recursively visit the body of the loop
    if (node.body) {
        node.body->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(FloatValofExpression& node) {
    trace("Entering FloatValofExpression block");
    symbol_table_->enterScope();
    if (node.body) {
        node.body->accept(*this);
    }
    symbol_table_->exitScope();
}

void SymbolDiscoveryPass::visit(ValofExpression& node) {
    trace("Entering ValofExpression block");
    symbol_table_->enterScope();
    if (node.body) {
        node.body->accept(*this);
    }
    symbol_table_->exitScope();
}

void SymbolDiscoveryPass::visit(IfStatement& node) {
    trace("Processing if statement");
    
    // Visit condition
    if (node.condition) {
        node.condition->accept(*this);
    }
    
    // Visit then block
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(WhileStatement& node) {
    trace("Processing while statement");
    
    // Visit condition
    if (node.condition) {
        node.condition->accept(*this);
    }
    
    // Visit body
    if (node.body) {
        node.body->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(UntilStatement& node) {
    trace("Processing until statement");
    
    // Visit body first (until executes body before checking condition)
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Visit condition
    if (node.condition) {
        node.condition->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(RepeatStatement& node) {
    trace("Processing repeat statement");
    
    // Visit body
    if (node.body) {
        node.body->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(TestStatement& node) {
    trace("Processing test statement");
    
    // Visit condition
    if (node.condition) {
        node.condition->accept(*this);
    }
    
    // Visit then block
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
    
    // Visit else block if present
    if (node.else_branch) {
        node.else_branch->accept(*this);
    }
}

void SymbolDiscoveryPass::visit(UnlessStatement& node) {
    trace("Processing unless statement");
    
    // Visit condition
    if (node.condition) {
        node.condition->accept(*this);
    }
    
    // Visit then block
    if (node.then_branch) {
        node.then_branch->accept(*this);
    }
}



void SymbolDiscoveryPass::trace(const std::string& message) const {
    if (enable_tracing_) {
        std::cout << "[SymbolDiscoveryPass] " << message << std::endl;
    }
}

void SymbolDiscoveryPass::processInheritedMembers(const std::string& class_name, const std::string& parent_name) {
    // Skip if no parent class
    if (parent_name.empty()) {
        return;
    }

    // Get the class entries
    ClassTableEntry* child_entry = class_table_->get_class(class_name);
    ClassTableEntry* parent_entry = class_table_->get_class(parent_name);
    
    // If parent doesn't exist yet, we have a problem
    if (!parent_entry) {
        std::cerr << "Error: Parent class '" << parent_name << "' not found for class '" << class_name << "'" << std::endl;
        return;
    }

    // Detect cyclic inheritance using a set to track resolution path
    std::unordered_set<std::string> resolution_path;
    if (!ensureParentProcessed(parent_name, resolution_path)) {
        std::cerr << "Error: Circular inheritance detected involving class: " << parent_name << std::endl;
        return;
    }
    
    // Now parent is guaranteed to be processed, copy its members to child
    
    // We should NOT copy inherited member variables in this pass
    // This should be handled exclusively by ClassPass
    // SymbolDiscoveryPass should only read class information, not modify it
    
    // Copy inherited methods
    for (const auto& [method_qual_name, method_info] : parent_entry->member_methods) {
        // Check if this method has already been overridden in child
        bool is_overridden = false;
        for (const auto& [child_qual_name, child_method] : child_entry->member_methods) {
            if (child_method.name == method_info.name) {
                is_overridden = true;
                break;
            }
        }
        
        // If not overridden, add this inherited method to the child's symbol table
        if (!is_overridden) {
            // Generate the new qualified name for the inherited method
            std::string child_qual_name = class_name + "::" + method_info.name;
            
            // Create a new symbol for the inherited method in the child class
            SymbolKind kind = method_qual_name.find("::VALOF") != std::string::npos ?
                SymbolKind::FUNCTION : SymbolKind::ROUTINE;
                
            // Determine return type based on original method
            Symbol parent_symbol;
            VarType return_type = VarType::INTEGER;
            if (symbol_table_->lookup(method_qual_name, parent_symbol)) {
                return_type = parent_symbol.type;
            }
            
            Symbol new_symbol(
                child_qual_name,
                kind,
                return_type,
                symbol_table_->currentScopeLevel(),
                current_function_name_
            );
            
            if (!symbol_table_->addSymbol(new_symbol)) {
                trace("Note: Inherited method already exists in symbol table: " + child_qual_name);
            } else {
                trace("Added inherited method to symbol table: " + child_qual_name + 
                      " (inherited from " + method_qual_name + ")");
            }
            
            // Add a copy of the method info to the child's class table entry
            ClassMethodInfo inherited_method_info = method_info;
            inherited_method_info.qualified_name = child_qual_name;
            // Keep the original vtable slot, it will be properly assigned by ClassPass
            child_entry->add_member_method(inherited_method_info);
        }
    }
    
    // Copy parent's simple_name_to_method mappings for methods that weren't overridden
    for (const auto& [simple_name, method_ptr] : parent_entry->simple_name_to_method) {
        if (child_entry->simple_name_to_method.count(simple_name) == 0) {
            // Find the corresponding method in the child's member_methods
            std::string child_qual_name = class_name + "::" + simple_name;
            if (child_entry->member_methods.count(child_qual_name) > 0) {
                child_entry->simple_name_to_method[simple_name] = &child_entry->member_methods[child_qual_name];
            }
        }
    }
}

bool SymbolDiscoveryPass::ensureParentProcessed(const std::string& class_name, 
                                                std::unordered_set<std::string>& resolution_path) {
    // Detect cycles
    if (resolution_path.count(class_name) > 0) {
        return false;
    }
    
    resolution_path.insert(class_name);
    
    // Get the class entry
    ClassTableEntry* entry = class_table_->get_class(class_name);
    if (!entry) {
        return false;
    }
    
    // If parent exists, process it first
    if (!entry->parent_name.empty()) {
        if (!ensureParentProcessed(entry->parent_name, resolution_path)) {
            return false;
        }
    }
    
    resolution_path.erase(class_name);
    return true;
}

void SymbolDiscoveryPass::report_duplicate_symbol(const std::string& name) const {
    std::cerr << "Warning: Duplicate symbol declaration: " << name << std::endl;
}



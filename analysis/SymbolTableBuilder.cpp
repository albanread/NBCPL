#include "SymbolTableBuilder.h"
#include "AST.h"
#include <iostream>
#include "analysis/ASTAnalyzer.h"



SymbolTableBuilder::SymbolTableBuilder(bool enable_tracing)
    : enable_tracing_(enable_tracing) {
    // Create a new, empty symbol table
    symbol_table_ = std::make_unique<SymbolTable>();
}

std::unique_ptr<SymbolTable> SymbolTableBuilder::build(Program& program) {
    trace("Building symbol table...");
    
    // Visit the AST to populate the symbol table
    visit(program);
    
    if (enable_tracing_) {
        trace("Symbol table construction complete. Symbol table contents:");
        symbol_table_->dumpTable();
    }
    
    // Return the populated symbol table
    return std::move(symbol_table_);
}

void SymbolTableBuilder::visit(Program& node) {
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

void SymbolTableBuilder::visit(LetDeclaration& node) {
    trace("Processing let declaration");

    for (size_t i = 0; i < node.names.size(); i++) {
        const std::string& name = node.names[i];
        VarType determined_type;
        std::string type_source_str;
        std::string class_name;

        // Priority 1: Use the explicit 'AS' type annotation if present.
        if (node.explicit_type != VarType::UNKNOWN) {
            determined_type = node.explicit_type;
            type_source_str = " (from AS annotation)";
        }
        // Priority 2: Fall back to the 'FLET' keyword for floats.
        else if (node.is_float_declaration) {
            determined_type = VarType::FLOAT;
            type_source_str = " (from FLET keyword)";
        }
        // Priority 3: Default to INTEGER for standard 'LET' declarations.
        else {
            determined_type = VarType::INTEGER;
            type_source_str = " (default to INTEGER)";
        }

        // Check if this variable has an initializer with a NEW expression
        if (i < node.initializers.size() && node.initializers[i]) {
            std::string extracted_class = extract_class_name_from_expression(node.initializers[i].get());
            if (!extracted_class.empty()) {
                class_name = extracted_class;
                // If we found a NEW expression, this should be a pointer to object
                determined_type = static_cast<VarType>(static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::OBJECT));
                type_source_str = " (POINTER_TO_OBJECT from NEW " + class_name + ")";
            }
        }

        // Add the symbol with the correctly determined type.
        Symbol new_symbol(
            name,
            SymbolKind::LOCAL_VAR,
            determined_type,
            symbol_table_->currentScopeLevel(),
            symbol_table_->getCurrentFunction()
        );
        
        // Set the class name if we found one
        if (!class_name.empty()) {
            new_symbol.class_name = class_name;
        }
        
        if (!symbol_table_->addSymbol(new_symbol)) {
            report_duplicate_symbol(name);
        } else {
            trace("Added local variable: " + name + type_source_str);
        }
    }
}

void SymbolTableBuilder::visit(ManifestDeclaration& node) {
    trace("Processing manifest declaration: " + node.name);
    
    // Add the manifest constant to the symbol table
    Symbol new_symbol(
        node.name,
        SymbolKind::MANIFEST,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        symbol_table_->getCurrentFunction()
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        // Set the absolute value for the manifest constant
        symbol_table_->setSymbolAbsoluteValue(node.name, node.value);
        trace("Added manifest constant: " + node.name + " = " + std::to_string(node.value));
    }
}

void SymbolTableBuilder::visit(StaticDeclaration& node) {
    trace("Processing static declaration: " + node.name);

    // Add the static variable to the symbol table (at global scope)
    // Even though static variables have local scope visibility, they live in global storage

    // Determine the type: use explicit FSTATIC if present, otherwise infer from initializer
    VarType type = VarType::INTEGER;
    if (node.is_float_declaration) {
        type = VarType::FLOAT;
    } else if (node.initializer && node.initializer->getType() == ASTNode::NodeType::NumberLit) {
        auto* num_lit = static_cast<NumberLiteral*>(node.initializer.get());
        if (num_lit->literal_type == NumberLiteral::LiteralType::Float) {
            type = VarType::FLOAT;
        }
    }
    
    Symbol new_symbol(
        node.name,
        SymbolKind::STATIC_VAR,
        type,
        symbol_table_->currentScopeLevel(),
        symbol_table_->getCurrentFunction()
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added static variable: " + node.name);
    }
}

void SymbolTableBuilder::visit(GlobalDeclaration& node) {
    trace("Processing global declaration");
    
    // Add each global to the symbol table
    for (auto& global : node.globals) {
        const std::string& name = global.first;
        VarType type = VarType::INTEGER; // Default to INTEGER type
        
        Symbol new_symbol(
            name,
            SymbolKind::GLOBAL_VAR,
            type,
            symbol_table_->currentScopeLevel(),
            symbol_table_->getCurrentFunction()
        );
        if (!symbol_table_->addSymbol(new_symbol)) {
            report_duplicate_symbol(name);
        } else {
            trace("Added global variable: " + name);
        }
    }
}

void SymbolTableBuilder::visit(GlobalVariableDeclaration& node) {
    trace("Processing global variable declaration");
    
    // Process each global variable
    for (size_t i = 0; i < node.names.size(); i++) {
        const std::string& name = node.names[i];
        
        // Determine type based on declaration
        VarType type = node.is_float_declaration ? VarType::FLOAT : VarType::INTEGER;
        
        Symbol new_symbol(
            name,
            SymbolKind::GLOBAL_VAR,
            type,
            symbol_table_->currentScopeLevel(),
            symbol_table_->getCurrentFunction()
        );
        if (!symbol_table_->addSymbol(new_symbol)) {
            report_duplicate_symbol(name);
        } else {
            trace("Added global variable: " + name + 
                  (node.is_float_declaration ? " (float)" : " (int)"));
        }
    }
}

void SymbolTableBuilder::visit(FunctionDeclaration& node) {
    trace("Processing function declaration: " + node.name);
    
    // Add function to the symbol table
    SymbolKind kind = SymbolKind::FUNCTION;
    
    // --- CORRECTED LOGIC ---
    // Determine the function's return type by inspecting its body.
    VarType return_type = VarType::INTEGER; // Default to INTEGER
    if (node.body && dynamic_cast<FloatValofExpression*>(node.body.get())) {
        // If the body is a FloatValofExpression, it returns a float.
        return_type = VarType::FLOAT;
        kind = SymbolKind::FLOAT_FUNCTION; // Use a more specific kind for clarity
    }
    // --- END CORRECTION ---
    
    // Add the function to the symbol table with the correct return type.
    Symbol new_symbol(
        node.name,
        kind,
        return_type,
        symbol_table_->currentScopeLevel(),
        node.name // function name context
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added function: " + node.name + (return_type == VarType::FLOAT ? " (float)" : " (int)"));
    }
    
    // Enter a new scope for the function body
    symbol_table_->enterScope();
    // Pre-allocate _temp0 through _temp3 as ANY
    for (int i = 0; i < 4; ++i) {
        Symbol temp_symbol(
            "_temp" + std::to_string(i),
            SymbolKind::LOCAL_VAR,
            VarType::ANY,
            symbol_table_->currentScopeLevel(),
            node.name
        );
        symbol_table_->addSymbol(temp_symbol);
    }
    symbol_table_->setCurrentFunction(node.name);
    
    // Add parameters to the function's scope
    for (const auto& param : node.parameters) {
        // Default to INTEGER parameters
        VarType param_type = VarType::INTEGER;
        Symbol param_symbol(
            param,
            SymbolKind::PARAMETER,
            param_type,
            symbol_table_->currentScopeLevel(),
            node.name
        );
        symbol_table_->addSymbol(param_symbol);
        trace("Added parameter: " + param);
    }
    
    // Visit the function body
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the function scope
    symbol_table_->setCurrentFunction("");
    symbol_table_->exitScope();
}

void SymbolTableBuilder::visit(RoutineDeclaration& node) {
    trace("Processing routine declaration: " + node.name);
    
    // Add routine to the symbol table
    Symbol new_symbol(
        node.name,
        SymbolKind::ROUTINE,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        node.name // function context
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        trace("Added routine: " + node.name);
    }
    
    // Enter a new scope for the routine body
    symbol_table_->enterScope();
    // Pre-allocate _temp0 through _temp3 as ANY
    for (int i = 0; i < 4; ++i) {
        Symbol temp_symbol(
            "_temp" + std::to_string(i),
            SymbolKind::LOCAL_VAR,
            VarType::ANY,
            symbol_table_->currentScopeLevel(),
            node.name
        );
        symbol_table_->addSymbol(temp_symbol);
    }
    symbol_table_->setCurrentFunction(node.name);
    
    // Add parameters to the routine's scope
    for (const auto& param : node.parameters) {
        // Default to INTEGER parameters
        VarType param_type = VarType::INTEGER;
        Symbol param_symbol(
            param,
            SymbolKind::PARAMETER,
            param_type,
            symbol_table_->currentScopeLevel(),
            node.name
        );
        symbol_table_->addSymbol(param_symbol);
        trace("Added parameter: " + param);
    }
    
    // Visit the routine body
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the routine scope
    symbol_table_->setCurrentFunction("");
    symbol_table_->exitScope();
}

void SymbolTableBuilder::visit(LabelDeclaration& node) {
    trace("Processing label declaration: " + node.name);
    
    // Add label to the symbol table
    Symbol new_symbol(
        node.name,
        SymbolKind::LABEL,
        VarType::INTEGER,
        symbol_table_->currentScopeLevel(),
        symbol_table_->getCurrentFunction()
    );
    if (!symbol_table_->addSymbol(new_symbol)) {
        report_duplicate_symbol(node.name);
    } else {
        // Set the label location
        symbol_table_->setSymbolAbsoluteValue(node.name, 0); // Address will be resolved later
        trace("Added label: " + node.name);
    }
    
    // Visit the label's command
    if (node.command) {
        node.command->accept(*this);
    }
}

void SymbolTableBuilder::visit(BlockStatement& node) {
    trace("Entering block scope");
    
    // Enter a new scope for the block
    symbol_table_->enterScope();
    
    // Process declarations in the block
    for (auto& decl : node.declarations) {
        if (decl) decl->accept(*this);
    }
    
    // Process statements in the block
    for (auto& stmt : node.statements) {
        if (stmt) stmt->accept(*this);
    }
    
    // Exit the block scope
    symbol_table_->exitScope();
    
    trace("Exited block scope");
}

void SymbolTableBuilder::visit(ForStatement& node) {
    trace("Processing for statement with loop variable: " + node.loop_variable);
    
    // Add the loop variable to the current scope
    Symbol loop_var_symbol(
        node.loop_variable,
        SymbolKind::LOCAL_VAR,
        VarType::INTEGER,
        symbol_table_->getCurrentScopeLevel(),
        symbol_table_->getCurrentFunction()
    );
    if (!symbol_table_->addSymbol(loop_var_symbol)) {
        report_duplicate_symbol(node.loop_variable);
    } else {
        trace("Added for-loop variable: " + node.loop_variable);
    }
    
    // Visit the for-loop body
    if (node.body) {
        node.body->accept(*this);
    }
}

void SymbolTableBuilder::visit(FloatValofExpression& node) {
    trace("Entering FloatValofExpression block");
    
    // Enter a new scope for the valof block
    symbol_table_->enterScope();
    
    // Visit the valof body
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the valof scope
    symbol_table_->exitScope();
    
    trace("Exited FloatValofExpression block");
}

void SymbolTableBuilder::visit(ValofExpression& node) {
    trace("Entering ValofExpression block");
    
    // Enter a new scope for the valof block
    symbol_table_->enterScope();
    
    // Visit the valof body
    if (node.body) {
        node.body->accept(*this);
    }
    
    // Exit the valof scope
    symbol_table_->exitScope();
    
    trace("Exited ValofExpression block");
}

void SymbolTableBuilder::trace(const std::string& message) const {
    if (enable_tracing_) {
        std::cout << "[SymbolTableBuilder] " << message << std::endl;
    }
}

void SymbolTableBuilder::report_duplicate_symbol(const std::string& name) const {
    std::cerr << "Warning: Duplicate symbol declaration: " << name << std::endl;
}

std::string SymbolTableBuilder::extract_class_name_from_expression(Expression* expr) const {
    if (!expr) {
        return "";
    }
    
    // Check if this is a NewExpression
    if (auto* new_expr = dynamic_cast<NewExpression*>(expr)) {
        trace("Found NEW expression with class: " + new_expr->class_name);
        return new_expr->class_name;
    }
    
    // Could extend this to handle other cases like function calls that return objects
    // For now, we only handle NEW expressions
    return "";
}

void SymbolTableBuilder::visit(AssignmentStatement& node) {
    // Check if this is a simple assignment (one lhs, one rhs) to a variable
    if (node.lhs.size() == 1 && node.rhs.size() == 1) {
        if (auto* var_access = dynamic_cast<VariableAccess*>(node.lhs[0].get())) {
            // Check for NEW expression
            std::string class_name = extract_class_name_from_expression(node.rhs[0].get());
            if (!class_name.empty()) {
                // Look up the symbol and update its class name
                Symbol symbol;
                if (symbol_table_->lookup(var_access->name, symbol)) {
                    symbol.class_name = class_name;
                    // Update type to POINTER_TO_OBJECT
                    symbol.type = static_cast<VarType>(static_cast<int64_t>(VarType::POINTER_TO) | static_cast<int64_t>(VarType::OBJECT));
                    symbol_table_->updateSymbol(var_access->name, symbol);
                    trace("Updated symbol '" + var_access->name + "' with class_name '" + class_name + "' from assignment");
                }
            }
            
            // Check for vector allocations and set the variable name
            if (auto* vec_alloc = dynamic_cast<VecAllocationExpression*>(node.rhs[0].get())) {
                vec_alloc->variable_name = var_access->name;
                trace("Set variable name for VecAllocationExpression: " + var_access->name);
            } else if (auto* fvec_alloc = dynamic_cast<FVecAllocationExpression*>(node.rhs[0].get())) {
                fvec_alloc->variable_name = var_access->name;
                trace("Set variable name for FVecAllocationExpression: " + var_access->name);
            } else if (auto* pairs_alloc = dynamic_cast<PairsAllocationExpression*>(node.rhs[0].get())) {
                pairs_alloc->variable_name = var_access->name;
                trace("Set variable name for PairsAllocationExpression: " + var_access->name);
            } else if (auto* fpairs_alloc = dynamic_cast<FPairsAllocationExpression*>(node.rhs[0].get())) {
                fpairs_alloc->variable_name = var_access->name;
                trace("Set variable name for FPairsAllocationExpression: " + var_access->name);
            }
        }
    }
    
    // Continue with normal assignment processing - visit all RHS expressions
    for (auto& expr : node.rhs) {
        if (expr) {
            expr->accept(*this);
        }
    }
}

void SymbolTableBuilder::visit(VecAllocationExpression& node) {
    trace("Processing VecAllocationExpression");
    
    // Check if size is a constant
    if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.size_expr.get())) {
        int64_t size = num_lit->int_value;
        
        // If this vector allocation is being assigned to a variable, update its symbol
        if (!node.variable_name.empty()) {
            Symbol symbol;
            if (symbol_table_->lookup(node.variable_name, symbol)) {
                symbol.type = VarType::POINTER_TO_INT_VEC;
                symbol.size = static_cast<size_t>(size);
                symbol.has_size = true;
                symbol_table_->updateSymbol(node.variable_name, symbol);
                trace("Updated vector variable: " + node.variable_name + " with size " + std::to_string(size));
            }
        }
    }
    
    // Visit the size expression
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}

void SymbolTableBuilder::visit(FVecAllocationExpression& node) {
    trace("Processing FVecAllocationExpression");
    
    // Check if size is a constant
    if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.size_expr.get())) {
        int64_t size = num_lit->int_value;
        
        // If this vector allocation is being assigned to a variable, update its symbol
        if (!node.variable_name.empty()) {
            Symbol symbol;
            if (symbol_table_->lookup(node.variable_name, symbol)) {
                symbol.type = VarType::POINTER_TO_FLOAT_VEC;
                symbol.size = static_cast<size_t>(size);
                symbol.has_size = true;
                symbol_table_->updateSymbol(node.variable_name, symbol);
                trace("Updated float vector variable: " + node.variable_name + " with size " + std::to_string(size));
            }
        }
    }
    
    // Visit the size expression
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}

void SymbolTableBuilder::visit(PairsAllocationExpression& node) {
    trace("Processing PairsAllocationExpression");
    
    // Check if size is a constant
    if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.size_expr.get())) {
        int64_t size = num_lit->int_value;
        
        // If this pairs allocation is being assigned to a variable, update its symbol
        if (!node.variable_name.empty()) {
            Symbol symbol;
            if (symbol_table_->lookup(node.variable_name, symbol)) {
                symbol.type = VarType::POINTER_TO_PAIRS;
                symbol.size = static_cast<size_t>(size);
                symbol.has_size = true;
                symbol_table_->updateSymbol(node.variable_name, symbol);
                trace("Updated PAIRS variable: " + node.variable_name + " with size " + std::to_string(size));
            }
        }
    }
    
    // Visit the size expression
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}

void SymbolTableBuilder::visit(FPairsAllocationExpression& node) {
    trace("Processing FPairsAllocationExpression");
    
    // Check if size is a constant
    if (auto* num_lit = dynamic_cast<NumberLiteral*>(node.size_expr.get())) {
        int64_t size = num_lit->int_value;
        
        // If this fpairs allocation is being assigned to a variable, update its symbol
        if (!node.variable_name.empty()) {
            Symbol symbol;
            if (symbol_table_->lookup(node.variable_name, symbol)) {
                symbol.type = VarType::POINTER_TO_FPAIRS;
                symbol.size = static_cast<size_t>(size);
                symbol.has_size = true;
                symbol_table_->updateSymbol(node.variable_name, symbol);
                trace("Updated FPAIRS variable: " + node.variable_name + " with size " + std::to_string(size));
            }
        }
    }
    
    // Visit the size expression
    if (node.size_expr) {
        node.size_expr->accept(*this);
    }
}




